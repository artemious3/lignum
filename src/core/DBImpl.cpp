#include "DBImpl.h"
#include <QDir>
#include <QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <filesystem>
#include <qcoreapplication.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qhashfunctions.h>
#include <qlogging.h>
#include <qsqldatabase.h>
#include <qsqlerror.h>
#include <qsqlquery.h>
#include <stdexcept>

namespace mftb {
SqlDB *SqlDB::getInstance() {
  static SqlDB db_instance{};
  return &db_instance;
}

SqlDB::SqlDB() : db_filename(getTemporaryDbName()) {

  auto db = QSqlDatabase::addDatabase("QSQLITE");

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

void SqlDB::insertPerson(const Person &pers) {
  auto db = QSqlDatabase::database();

  static const QString INSERT_QUERY =
      R"sql(
      
  INSERT INTO persons 
    (gender, first_name, middle_name, last_name, birth_date, death_date, parents_couple_id)
  VALUES
    (:gender, :first_name, :middle_name, :last_name, :birth_date, :death_date, 0);

  )sql";

  QSqlQuery query(db);
  query.prepare(INSERT_QUERY);
  query.bindValue(":gender", pers.gender);
  query.bindValue(":first_name", pers.first_name);
  query.bindValue(":middle_name", pers.middle_name);
  query.bindValue(":last_name", pers.last_name);
  query.bindValue(":birth_date", pers.birth_date.toJulianDay());
  query.bindValue(":death_date", pers.death_date.toJulianDay());

  if (!query.exec()) {
    qDebug() << "Error inserting person: "
             << (qUtf8Printable(query.lastError().text()));
    throw std::runtime_error("Error inserting person");
  }
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

SqlDB::~SqlDB() {
  QFile db_file(db_filename);
  if (db_file.exists()) {
    db_file.remove();
  }
}

QString SqlDB::getTemporaryDbName() {
  std::string temp_path = std::filesystem::temp_directory_path() / ".mftbdb";
  return QString::fromStdString(temp_path);
}

} // namespace mftb
