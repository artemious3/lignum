/*
 * File: /src/database/datamodel.h
 * Project: MFTB
 * File Created: Saturday, 17th August 2024 10:28:15 am
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
#include <QDate>
#include <QString>
#include <cstdint>

typedef uint32_t id_t;

struct Person {
  QChar gender;

  QString first_name;
  QString middle_name;
  QString last_name;

  QDate birth_date;
  QDate death_date;

  bool operator==(const Person& pers) const = default;
};

struct Couple {
  id_t person1_id;
  id_t person2_id;
  QDate start_date;

  id_t getAnotherPerson(id_t id){
    return person1_id == id ? person2_id : person1_id;
  }

};

struct IdentifiedObject {
  id_t id_;
};
