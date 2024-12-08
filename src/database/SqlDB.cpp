/*
 * File: /src/database/SqlDB.cpp
 * Project: MFTB
 * File Created: Saturday, 17th August 2024 3:32:15 pm
 * Author: Artsiom Padhaiski (artempodgaisky@gmail.com)
 * Copyright 2024 - 2024 Artsiom Padhaiski
 *
 * ______________________________________________________________
 *
 * This file is part of MFTB.
 *
 * MFTB is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later
 *  version.
 *
 * MFTB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with MFTB. If not, see <https: //www.gnu.org/licenses/>.
 */

#include "SqlDB.h"
#include "spdlog/spdlog.h"
#include <QDate>
#include <QDir>
#include <QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>
#include <filesystem>
#include <qdebug.h>
#include <qsqldatabase.h>
#include <qsqlquery.h>
#include <qvariant.h>
#include <stdexcept>
#include <vector>

namespace mftb {

SqlDB *SqlDB::getInstance() {
  static SqlDB db_instance{};
  return &db_instance;
}



QString SqlDB::getTemporaryDbName() {
  std::string temp_path =
      std::filesystem::temp_directory_path() / TEMP_FILENAME;
  return QString::fromStdString(temp_path);
}

QSqlQuery
SqlDB::executeQuery(QString query_text,
                    std::vector<std::pair<QString, QVariant>> bindings) {
  auto db = QSqlDatabase::database();
  QSqlQuery query(db);
  query.prepare(query_text);

  for (const auto & binding : bindings) {
    query.bindValue(binding.first, binding.second);
  }
  if (!query.exec()) {
    qDebug() << "Query " << query.lastQuery() << " was not executed";
    qDebug() << "Error: " << query.lastError().text();
  }
  return query;
}

void 
SqlDB::executePreparedQuery(QSqlQuery& prepared_query,std::vector<std::pair<QString, QVariant>> bindings){
	
  for (const auto &binding : bindings) {
    prepared_query.bindValue(binding.first, binding.second);
  }
  if (!prepared_query.exec()) {
    qDebug() << "Query " << prepared_query.lastQuery() << " was not executed";
    qDebug() << "Error: " << prepared_query.lastError().text();
  }

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

id_t SqlDB::convertToId(QVariant variant) { return variant.toLongLong(); }

SqlDB::SqlDB()
    : db_filename(getTemporaryDbName()),
      db(QSqlDatabase::addDatabase(DB_DRIVER)), q_insertPerson(db),
      q_insertPersonWithParentsCoupleId(db), q_getPerson(db),
      q_getCoupleChildren(db), q_getPersonCouplesId(db), q_getPersonQuery(db),
      q_addNewCouple(db), q_addPartner(db), q_addChild(db), q_getPersonById(db),
      q_getParentsCoupleId(db), q_getCoupleById(db), q_getParents(db),
      q_getCoupleIdByPersons(db), q_getPartners(db), q_getPersonChildren(db),
      q_getParentsChildren(db), q_addParent(db), q_setSecondParent(db),
      q_setParentCoupleId(db), q_updatePerson(db) {

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
    
    )sql",

      R"sql(

    CREATE INDEX 
      parents_couple_index 
    ON 
      persons(parents_couple_id);
    
    )sql"};

  db.setDatabaseName(db_filename);
  if (!db.open()) {
    throw std::runtime_error(qUtf8Printable(db.lastError().text()));
  }

  QSqlQuery init_query(db);

  for (const auto &query : INIT_QUERIES) {
    if (!init_query.exec(query)) {
      qDebug() << init_query.lastError();
      qFatal("Unable to create database");
    }
  }

  q_insertPersonWithParentsCoupleId.prepare(
      R"sql(
      
  INSERT INTO persons 
    (gender, first_name, middle_name, last_name, birth_date, death_date, parents_couple_id)
  VALUES
    (:gender, :first_name, :middle_name, :last_name, :birth_date, :death_date, :couple_id);

  )sql"
		  );


  q_getPersonCouplesId.prepare(
      R"sql(
  
  SELECT id FROM couples WHERE person1_id = :id OR person2_id = :id;
  
  )sql"
		  );

  q_getCoupleChildren.prepare(
      R"sql(
  
  SELECT id FROM persons WHERE parents_couple_id = :id;
  
  )sql");

  q_getCoupleIdByPersons.prepare(

      R"sql(

  SELECT id FROM couples
     WHERE (person1_id = :id1 AND person2_id = :id2)
            OR
            (person1_id = :id2 AND person2_id = :id1);
  
  )sql");

  q_getPersonById.prepare(
      R"sql(

  SELECT * FROM persons WHERE id=:id
  
  )sql");

  q_addNewCouple.prepare(
      R"sql(

  INSERT INTO couples 
    (person1_id, person2_id)
  VALUES
    (:parent1, :parent2)

  )sql");
  q_addPartner.prepare(
      R"sql(

    INSERT INTO couples
       (person1_id, person2_id)
    VALUES 
      (:id1, :id2);

  )sql");
  q_getPersonById.prepare(
      R"sql(

  SELECT * FROM persons WHERE id=:id
  
  )sql");

  q_getParentsCoupleId.prepare(
      R"sql(

  SELECT parents_couple_id FROM persons WHERE id=:id

  )sql");
  q_getCoupleById.prepare(
      R"sql( 
    SELECT * FROM couples WHERE id=:id 
  )sql"

  );
  q_getParents.prepare(
      R"sql(

  SELECT person1_id, person2_id FROM couples
    INNER JOIN persons ON couples.id = persons.parents_couple_id
    WHERE persons.id=:id;

  )sql");
  q_getPartners.prepare(
      R"sql(

    SELECT
      person1_id, person2_id
    FROM couples
      WHERE person1_id = :id OR person2_id = :id;

  )sql");
  q_getPersonChildren.prepare(
      R"sql(

  SELECT persons.id FROM persons
    INNER JOIN couples ON
     persons.parents_couple_id = couples.id
    WHERE 
    couples.person1_id = :parent_id
    OR 
    couples.person2_id = :parent_id;
    
  )sql");

  q_getParentsChildren.prepare(
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

  )sql");
  q_addParent.prepare(

      R"sql(

  INSERT INTO couples(person1_id, person2_id) VALUES (:parent_id, 0);

  )sql");

  q_setSecondParent.prepare(
      R"sql(

  UPDATE couples SET person2_id=:parent_id WHERE id=:couple_id;

  )sql");

  q_setParentCoupleId.prepare(
      R"sql(

  UPDATE persons SET parents_couple_id=:pcouple_id WHERE id=:id;

  )sql");


  q_updatePerson.prepare(
      R"sql(
      
  UPDATE persons 
   SET gender=:gender,
       first_name=:first_name, 
       middle_name=:middle_name,
       last_name=:last_name,
       birth_date=:birth_date,
       death_date=:death_date
  WHERE id=:id

  )sql"
		  );
}

id_t SqlDB::insertPersonWithParentsCoupleId(const Person &pers,
                                            id_t couple_id) {
  auto db = QSqlDatabase::database();

  executePreparedQuery(
      q_insertPersonWithParentsCoupleId, {{":gender", pers.gender},
                     {":first_name", pers.first_name},
                     {":middle_name", pers.middle_name},
                     {":last_name", pers.last_name},
                     {":birth_date", pers.birth_date.toJulianDay()},
                     {":death_date", pers.death_date.toJulianDay()},
                     {":couple_id", couple_id}});

  return convertToId(q_insertPersonWithParentsCoupleId.lastInsertId());
}

std::vector<id_t> SqlDB::getPersonCouplesId(id_t id) const {

  executePreparedQuery(q_getPersonCouplesId, {{":id", id}});

  std::vector<id_t> couples;
  while (q_getPersonCouplesId.next()) {
    couples.push_back(convertToId(q_getPersonCouplesId.value(0)));
  }

  q_getPersonCouplesId.finish();

  return couples;
}

std::vector<id_t> SqlDB::getCoupleChildren(id_t id) const {
executePreparedQuery(q_getCoupleChildren, {{":id", id}});

  std::vector<id_t> chidlren;
  while (q_getCoupleChildren.next()) {
    chidlren.push_back(convertToId(q_getCoupleChildren.value(0)));
  }

  q_getCoupleChildren.finish();

  return chidlren;
}

id_t SqlDB::addChild(const Person &person, id_t parent1, id_t parent2, id_t* out_couple_id) {
  executePreparedQuery(q_getCoupleIdByPersons,
                   {{":id1", parent1}, {":id2", parent2}});

  id_t couple_id;
  if (!q_getCoupleIdByPersons.next()) {
    executePreparedQuery(
        q_addNewCouple, {{":parent1", parent1}, {":parent2", parent2}});
    couple_id = convertToId(q_addNewCouple.lastInsertId());
  } else {
    couple_id = convertToId(q_getCoupleIdByPersons.value("id"));
  }

  if(out_couple_id != nullptr){
	  *out_couple_id = couple_id;
  }

  q_getCoupleIdByPersons.finish();
  q_addNewCouple.finish();

  return insertPersonWithParentsCoupleId(person, couple_id);
}

id_t SqlDB::addPartner(const Person &person, id_t partner_id, id_t* out_couple_id) {
  auto inserted_person_id = insertPerson(person);

  executePreparedQuery(
      q_addPartner, {{":id1", inserted_person_id}, {":id2", partner_id}});

  if(out_couple_id != nullptr){
	  *out_couple_id = convertToId(q_addPartner.lastInsertId());
  }


  q_addPartner.finish();

  return inserted_person_id;
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

  const QString &query_text =
      max_amount == -1 ? GET_ALL_QUERY : GET_ALL_WITH_LIMIT_QUERY;

  auto executed_query = executeQuery(query_text, {{":top", max_amount}});

  std::vector<Person> persons;

  while (executed_query.next()) {
    auto person_record = executed_query.record();
    auto person = extractPersonFromRecord(person_record);
    persons.push_back(person);
  }

  return persons;
}

std::vector<id_t> SqlDB::getPeopleIds(int max_amount) const {
  static const QString GET_ALL_WITH_LIMIT_QUERY =
      R"sql(
    SELECT id FROM persons LIMIT :top;
  )sql";

  static const QString GET_ALL_QUERY =
      R"sql(
    SELECT id FROM persons;
  )sql";

  const QString &query_text =
      max_amount == -1 ? GET_ALL_QUERY : GET_ALL_WITH_LIMIT_QUERY;

  auto executed_query = executeQuery(query_text, {{":top", max_amount}});

  std::vector<id_t> person_ids;

  while (executed_query.next()) {
    person_ids.push_back(convertToId(executed_query.value(0)));
  }

  return person_ids;
}

std::optional<Person> SqlDB::getPersonById(id_t id) const {



  executePreparedQuery(q_getPersonById, {{":id", id}});

  if (!q_getPersonById.next()) {
    return {};
  }

  auto person = extractPersonFromRecord(q_getPersonById.record());

  q_getPersonById.finish();

  return person;
}

std::optional<id_t> SqlDB::getParentsCoupleId(id_t id) const {

executePreparedQuery(q_getParentsCoupleId, {{":id", id}});

if (!q_getParentsCoupleId.next()) {
  return {};
}

auto couple_id = convertToId(q_getParentsCoupleId.value(0));
q_getParentsCoupleId.finish();

return {couple_id};
}

std::optional<Couple> SqlDB::getCoupleById(id_t id) const {


  executePreparedQuery(q_getCoupleById, {{":id", id}});

  if (!q_getCoupleById.next()) {
    return {};
  }

  return extractCoupleFromRecord(q_getCoupleById.record());
}

std::pair<id_t, id_t> SqlDB::getPersonParentsById(id_t id) const {


  executePreparedQuery(q_getParents, {{":id", id}});

  if (!q_getParents.next()) {
    return {0, 0};
  }

  return {convertToId(q_getParents.value("person1_id")),
          convertToId(q_getParents.value("person2_id"))};
}

std::optional<id_t> SqlDB::getCoupleIdByPersons(id_t id1, id_t id2) const {

	 

      executePreparedQuery(q_getCoupleIdByPersons, {{":id1", id1}, {":id2", id2}});

  if (!q_getCoupleIdByPersons.next()) {
    return {};
  }

  return convertToId(q_getCoupleIdByPersons.value(0));
}

std::vector<id_t> SqlDB::getPersonPartners(id_t target_id) const {




executePreparedQuery(q_getPartners, {{":id", target_id}});

  std::vector<id_t> partners;
  while (q_getPartners.next()) {

    auto id1 = convertToId(q_getPartners.value(0));
    auto id2 = convertToId(q_getPartners.value(1));

    if (id1 == target_id) {
      partners.push_back(id2);
    } else {
      partners.push_back(id1);
    }
  }

  q_getPartners.finish();

  return partners;
}

std::vector<id_t> SqlDB::getPersonChildren(id_t parent_id) const {



      executePreparedQuery(q_getPersonChildren, {{":parent_id", parent_id}});

  std::vector<id_t> children;
  while (q_getPersonChildren.next()) {
    children.push_back(convertToId(q_getPersonChildren.value(0)));
  }

  q_getPersonChildren.finish();

  return children;
}

std::vector<id_t> SqlDB::getParentsChildren(id_t parent1, id_t parent2) const {



executePreparedQuery(q_getParentsChildren,
                     {{":parent1_id", parent1}, {":parent2_id", parent2}});

std::vector<id_t> children;
while (q_getParentsChildren.next()) {
  children.push_back(convertToId(q_getParentsChildren.value(0)));
}

  return children;
}

SqlDB::~SqlDB() {
  QFile db_file(db_filename);
  if (db_file.exists()) {
    db_file.remove();
  }
}


void SqlDB::updatePerson(const Person& pers, id_t id) {
  executePreparedQuery(
      q_updatePerson, {{":id", id},
                     {":gender", pers.gender},
                     {":first_name", pers.first_name},
                     {":middle_name", pers.middle_name},
                     {":last_name", pers.last_name},
                     {":birth_date", pers.birth_date.toJulianDay()},
                     {":death_date", pers.death_date.toJulianDay()}});
}

void SqlDB::dropData() {

  static const QString DROP_QUERIES[] = {
      R"sql(DELETE FROM couples; )sql",
      R"sql(DELETE FROM persons; )sql",
  };

  for (auto drop_query : DROP_QUERIES) {
    executeQuery(drop_query);
  }
}

id_t SqlDB::addParent(id_t child, const Person &person, id_t* out_couple_id) {

  //TODO: maybe rewrite it as whole sql code

  id_t inserted_parent_id;

  auto parents_couple_id = getParentsCoupleId(child);
  if (parents_couple_id.value() == 0) {
    inserted_parent_id = insertPerson(person);
    executePreparedQuery(q_addParent, {{":parent_id", QVariant(inserted_parent_id)}});
    auto couple_id = q_addParent.lastInsertId();
    if(out_couple_id){
	    *out_couple_id = convertToId(couple_id);
    }
    executePreparedQuery(q_setParentCoupleId, {{":pcouple_id", couple_id}, {":id", child}});

  } else {
    if (out_couple_id) {
      *out_couple_id = parents_couple_id.value();
    }
    auto couple = getCoupleById(parents_couple_id.value());
    if(couple->person2_id != 0){
      SPDLOG_INFO("Both parents are already set for person {}", child);
      return 0;
    }
    inserted_parent_id = insertPerson(person);
    executePreparedQuery(q_setSecondParent, {{":parent_id", inserted_parent_id}, {":couple_id", parents_couple_id.value()}});
  }


  q_setSecondParent.finish();
  q_addParent.finish();
  q_setParentCoupleId.finish();

  return inserted_parent_id;
}

} // namespace mftb
