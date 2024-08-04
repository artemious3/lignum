/*
 * File: /src/core/DB.h
 * Project: MFTB
 * File Created: Saturday, 20th July 2024 10:02:06 pm
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

#include <QHash>
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <qlist.h>

#include "Person.h"
#include "Relationship.h"

namespace mftb {
class DB {
  
    using db_mutex_t =  std::recursive_mutex;

  /*---------------Singletone------------------*/
private:
  DB() = default;

public:
  static DB &getInstance();

  /*-------------------------------------------*/

public:
  uint32_t createPerson();
  uint32_t _createPerson(bool gender, QString firsrt, QString second);
  void insertPerson(std::shared_ptr<Person> person);
  bool removePerson(uint32_t id);

  uint32_t createRelationship(uint32_t id1, uint32_t id2, Relationship::Type type);
  void insertRelationship(std::shared_ptr<Relationship> rel);

  QList<std::shared_ptr<Relationship>> getRelationships();
  QList<std::shared_ptr<Person>> getPersons();

  std::weak_ptr<Person> getPersonById(uint32_t id);

  ~DB() = default;

private:
  db_mutex_t mutex;
  uint32_t getNewId();
  
  std::atomic_uint32_t last_id = 1;

  QHash<uint32_t, std::shared_ptr<Person>>       persons;
  QHash<uint32_t, std::shared_ptr<Relationship>> relationships;
};
} // namespace mftb
