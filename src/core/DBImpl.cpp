#include "DBImpl.h"
#include <QDir>
#include <QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>
#include <filesystem>
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qhashfunctions.h>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>

namespace mftb {

SqlDB *SqlDB::getInstance() {
  static SqlDB db_instance{};
  return &db_instance;
}

SqlDB::SqlDB() : db_filename(getTemporaryDbName()) {

  auto db = QSqlDatabase::addDatabase(DB_DRIVER);

  static const QString INIT_QUERIES[] = {
      R"sql(

    CREATE TABLE persons(
        id                INTEGER PRIMARY KEY,
        gender            CHAR(1) NOT NULL,
        first_name        TEXT NOT NULL,
        middle_name       TEXT,
        last_name         TEXT,
        birth_date        INT,
        death_date        INT,
        parents_couple_id INT
    );

    )sql",
      R"sql(

    CREATE TABLE couples(
        id         INTEGER PRIMARY KEY,
        person1_id INTEGER NOT NULL,
        person2_id INTEGER
    );

    )sql",
      R"sql(

    CREATE INDEX
       person1_couples_index 
    ON 
      couples(person1_id);

    )sql",
      R"sql(

    CREATE INDEX 
      person2_couples_index 
    ON 
      couples(person2_id);
    
    )sql"};

  db.setDatabaseName(db_filename);
  if (!db.open()) {
    throw std::runtime_error(qUtf8Printable(db.lastError().text()));
  }

  QSqlQuery init_query(db);

  for (const auto &query : INIT_QUERIES) {
    if (!init_query.exec(query)) {
      qFatal("Unable to create database");
    }
  }
}

id_t SqlDB::insertPersonWithParentsCoupleId(const Person &pers,
                                            id_t couple_id) {
  auto db = QSqlDatabase::database();

  static const QString INSERT_QUERY =
      R"sql(
      
  INSERT INTO persons 
    (gender, first_name, middle_name, last_name, birth_date, death_date, parents_couple_id)
  VALUES
    (:gender, :first_name, :middle_name, :last_name, :birth_date, :death_date, :couple_id);

  )sql";

  QSqlQuery query(db);
  query.prepare(INSERT_QUERY);
  query.bindValue(":gender", pers.gender);
  query.bindValue(":first_name", pers.first_name);
  query.bindValue(":middle_name", pers.middle_name);
  query.bindValue(":last_name", pers.last_name);
  query.bindValue(":birth_date", pers.birth_date.toJulianDay());
  query.bindValue(":death_date", pers.death_date.toJulianDay());
  query.bindValue(":couple_id", couple_id);

  if (!query.exec()) {
    qDebug() << "Error inserting person: "
             << (qUtf8Printable(query.lastError().text()));
    throw std::runtime_error("Error inserting person");
  }

  return convertToId(query.lastInsertId());
}

id_t SqlDB::insertPerson(const Person &pers) {
  return insertPersonWithParentsCoupleId(pers, 0);
}

std::vector<Person> SqlDB::getPeople(int max_amount) const {
  static const QString GET_ALL_WITH_LIMIT_QUERY =
      R"sql(
    SELECT * FROM persons LIMIT :top;
  )sql";

  static const QString GET_ALL_QUERY =
      R"sql(
    SELECT * FROM persons;
  )sql";

  auto db = QSqlDatabase::database();
  std::vector<Person> persons;
  QSqlQuery query(db);

  query.prepare(max_amount == -1 ? GET_ALL_QUERY : GET_ALL_WITH_LIMIT_QUERY);
  query.bindValue(":top", max_amount);
  if (!query.exec()) {
    qDebug() << "Error obtaining peoople: "
             << qUtf8Printable(query.lastError().text());
    throw std::runtime_error("Could not get all people");
  }

  while (query.next()) {
    auto person_record = query.record();
    auto person = extractPersonFromRecord(person_record);
    persons.push_back(person);
  }

  return persons;
}

Person SqlDB::extractPersonFromRecord(const QSqlRecord &record) {

  Person person;
  person.gender = record.value("gender").toString()[0];
  person.first_name = record.value("first_name").toString();
  person.last_name = record.value("last_name").toString();
  person.middle_name = record.value("middle_name").toString();
  person.birth_date =
      QDate::fromJulianDay(record.value("birth_date").toLongLong());
  person.death_date =
      QDate::fromJulianDay(record.value("death_date").toLongLong());

  return person;
}

Couple SqlDB::extractCoupleFromRecord(const QSqlRecord &record) {
  Couple couple;
  couple.person1_id = convertToId(record.value("person1_id"));
  couple.person2_id = convertToId(record.value("person2_id"));
  return couple;
}

std::optional<Person> SqlDB::getPersonById(id_t id) const {

  static const QString GET_PERSON_BY_ID_QUERY =
      R"sql(

  SELECT * FROM persons WHERE id=:id
  
  )sql";

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);

  query.prepare(GET_PERSON_BY_ID_QUERY);
  query.bindValue(":id", id);
  query.exec();
  if (!query.next()) {
    return {};
  }

  return extractPersonFromRecord(query.record());
}

std::optional<Couple> SqlDB::getCoupleById(id_t id) const {

  static const QString GET_COUPLE_BY_ID_QUERY =
      R"sql( 
    SELECT * FROM couples WHERE id=:id 
  )sql";

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  query.prepare(GET_COUPLE_BY_ID_QUERY);
  query.bindValue(":id", id);
  query.exec();
  if (!query.next()) {
    return {};
  }

  return extractCoupleFromRecord(query.record());
}

id_t SqlDB::addChild(const Person &person, id_t parent1, id_t parent2) {
  static const QString GET_EXISTING_COUPLE_QUERY =
      R"sql(

  SELECT id FROM couples WHERE person1_id = :parent1 and person2_id = :parent2;
  
  )sql";

  static const QString ADD_NEW_COUPLE_QUERY =
      R"sql(

  INSERT INTO couples 
    (person1_id, person2_id)
  VALUES
    (:parent1, :parent2)

  )sql";

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  query.prepare(GET_EXISTING_COUPLE_QUERY);
  query.bindValue(":parent1", parent1);
  query.bindValue(":parent2", parent2);

  id_t couple_id;
  query.exec();
  if (!query.next()) {
    query.prepare(ADD_NEW_COUPLE_QUERY);
    query.bindValue(":parent1", parent1);
    query.bindValue(":parent2", parent2);
    query.exec();
    couple_id = convertToId(query.lastInsertId());
  } else {
    couple_id = convertToId(query.value("id"));
  }

  return insertPersonWithParentsCoupleId(person, couple_id);
}

std::pair<id_t, id_t> SqlDB::getPersonParentsById(id_t id) const {
  static const QString GET_PARENTS_QUERY =
      R"sql(

  SELECT person1_id, person2_id FROM couples
    INNER JOIN persons ON couples.id = persons.parents_couple_id
    WHERE persons.id=:id;

  )sql";

  auto db = QSqlDatabase::database();

  QSqlQuery query(db);
  query.prepare(GET_PARENTS_QUERY);
  query.bindValue(":id", id);
  bool executed = query.exec();
  if (!executed) {
    qDebug() << qUtf8Printable(query.lastError().text());
  }
  if (!query.next()) {
    return {0, 0};
  }

  return {convertToId(query.value("person1_id")),
          convertToId(query.value("person2_id"))};
}

id_t SqlDB::addPartner(const Person &person, id_t partner_id) {

  static const QString ADD_PARTNER_QUERY =
      R"sql(

    INSERT INTO couples
       (person1_id, person2_id)
    VALUES 
      (:id1, :id2);

  )sql";

  auto inserted_person_id = insertPerson(person);
  auto db = QSqlDatabase::database();
  QSqlQuery query{db};

  query.prepare(ADD_PARTNER_QUERY);
  query.bindValue(":id1", inserted_person_id);
  query.bindValue(":id2", partner_id);
  query.exec();

  return inserted_person_id;
}

std::vector<id_t> SqlDB::getPersonPartners(id_t target_id) const {

  static const QString GET_PARTNERS_QUERY =
      R"sql(

    SELECT
      person1_id, person2_id
    FROM couples
      WHERE person1_id = :id OR person2_id = :id;

  )sql";

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  std::vector<id_t> partners;

  query.prepare(GET_PARTNERS_QUERY);
  query.bindValue(":id", target_id);
  query.exec();

  while (query.next()) {

    auto id1 = convertToId(query.value(0));
    auto id2 = convertToId(query.value(1));

    if (id1 == target_id) {
      partners.push_back(id2);
    } else {
      partners.push_back(id1);
    }
  }

  return partners;
}

std::vector<id_t> SqlDB::getPersonChildren(id_t parent_id) const {

  static const QString GET_CHILDREN_QUERY =
      R"sql(

  SELECT persons.id FROM persons
    INNER JOIN couples ON
     persons.parents_couple_id = couples.id
    WHERE 
    couples.person1_id = :parent_id
    OR 
    couples.person2_id = :parent_id;
    
  )sql";

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  std::vector<id_t> children;

  query.prepare(GET_CHILDREN_QUERY);
  query.bindValue(":parent_id", parent_id);
  query.exec();

  while (query.next()) {
    children.push_back(convertToId(query.value(0)));
  }

  return children;
}

SqlDB::~SqlDB() {
  QFile db_file(db_filename);
  if (db_file.exists()) {
    db_file.remove();
  }
}

QString SqlDB::getTemporaryDbName() {
  std::string temp_path =
      std::filesystem::temp_directory_path() / TEMP_FILENAME;
  return QString::fromStdString(temp_path);
}

std::vector<id_t> SqlDB::getParentsChildren(id_t parent1, id_t parent2) const {
  static const QString GET_PARENTS_CHILDREN_QUERY = 
  R"sql(

   SELECT persons.id FROM persons
    INNER JOIN couples ON
     persons.parents_couple_id = couples.id
    WHERE 

    (couples.person1_id = :parent1_id
    AND
    couples.person2_id = :parent2_id)
    OR 
    (couples.person1_id = :parent2_id
    AND
    couples.person2_id = :parent1_id)

  )sql";


  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  std::vector<id_t> children;

  query.prepare(GET_PARENTS_CHILDREN_QUERY);
  query.bindValue(":parent1_id", parent1);
  query.bindValue(":parent2_id", parent2);
  query.exec();

  while(query.next()){
    children.push_back(convertToId(query.value(0)));
  }

  return children;
}

void SqlDB::dropData() {

  static const QString DROP_QUERIES[] = {
      R"sql(DELETE FROM couples; )sql",
      R"sql(DELETE FROM persons; )sql",
  };

  auto db = QSqlDatabase::database();
  QSqlQuery query(db);

  for (auto drop_query : DROP_QUERIES) {
    if (!query.exec(drop_query)) {
      qDebug() << query.lastError().text();
    }
  }
}

id_t SqlDB::convertToId(QVariant variant) { return variant.toLongLong(); }

} // namespace mftb
