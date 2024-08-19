/*
 * File: /src/database/SqlDB.h
 * Project: MFTB
 * File Created: Saturday, 17th August 2024 10:46:16 am
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

#pragma once
#include "DB.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/qsqlrecord.h>
#include <optional>
#include <qcontainerfwd.h>
#include <qsqlquery.h>
#include <qvariant.h>

namespace mftb {

class SqlDB : public DB {

public:
  static SqlDB *getInstance();

private:
  SqlDB();
  ~SqlDB();
  static Person extractPersonFromRecord(const QSqlRecord &);
  static Couple extractCoupleFromRecord(const QSqlRecord &);
  static QString getTemporaryDbName();
  id_t insertPersonWithParentsCoupleId(const Person &pers, id_t couple_id);
  static id_t convertToId(QVariant variand); 
  static QSqlQuery executeQuery(QString query, std::vector<std::pair<QString, QVariant>> bindings = {});

public:
  virtual std::optional<Person> getPersonById(id_t) const override;
  virtual std::optional<Couple> getCoupleById(id_t) const override;

  virtual std::pair<id_t, id_t> getPersonParentsById(id_t) const override;
  virtual std::vector<id_t> getPersonPartners(id_t ids) const override;

  virtual std::vector<id_t> getPersonChildren(id_t parent1_id) const override;

  virtual std::vector<id_t>
  getParentsChildren(id_t parent1, id_t parent2) const override;

  virtual id_t insertPerson(const Person &pers) override;
  virtual id_t addChild(const Person &person, id_t parent1,
                        id_t parent2 = 0) override;
  virtual id_t addPartner(const Person &person, id_t partner) override;
  // virtual void removePerson(id_t) override;
  virtual std::vector<Person> getPeople(int max_amount) const override;
  virtual std::vector<id_t> getPeopleIds(int max_amount = -1) const override; 
  void dropData() override;

private:
  static const inline QString DB_DRIVER = "QSQLITE";
  static const inline std::string TEMP_FILENAME = ".mftbdb";
  QString db_filename;
};

} // namespace mftb
