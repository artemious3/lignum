/*
 * File: /src/database/DB.h
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


#pragma once
#include "datamodel.h"
#include <vector>

namespace mftb {

class DB {
public:
	virtual void Save(const QString& path) = 0;
	virtual void Load(const QString& path) = 0;

  virtual std::optional<Person> getPersonById(id_t) const = 0;
  virtual std::optional<Couple> getCoupleById(id_t) const = 0;

  virtual std::pair<id_t, id_t> getPersonParentsById(id_t) const = 0;
  virtual std::optional<id_t> getParentsCoupleId(id_t) const = 0;
  virtual std::vector<id_t> getPersonPartners(id_t ids) const = 0;
  virtual std::optional<id_t> getCoupleIdByPersons(id_t, id_t) const = 0;

  virtual std::vector<id_t> getPersonChildren(id_t parent1_id) const = 0;
  virtual std::vector<id_t>
  getParentsChildren(id_t parent1, id_t parent2) const = 0;
  
  virtual std::vector<id_t> getPersonCouplesId(id_t) const = 0;

  virtual std::vector<id_t> getCoupleChildren(id_t) const = 0;
  // virtual bool hasPersonWithId(id_t id) const = 0;

  virtual std::vector<Person> getPeople(int max_amount = -1) const = 0;
  virtual std::vector<id_t> getPeopleIds(int max_amount = -1) const = 0; 

  // virtual void loadFromFile(const QFile& file) = 0;
  virtual id_t insertPerson(const Person& person) = 0;
  virtual id_t addChild(const Person &person, id_t parent1,
                        id_t parent2 = 0, id_t* couple_id = nullptr) = 0;
  virtual id_t addPartner(const Person &person, id_t partner, id_t* couple_id = nullptr) = 0;
  virtual id_t addParent(id_t child, const Person& person, id_t* couple_id = nullptr) = 0;
  
  // Remove person from DB only if it is a leaf node
  // that is:
  //  	- has no ancestors
  //  	- has no more than one partner
  //  	- has no descendants
  //
  // return value : was the person actually removed
  virtual bool removePerson(id_t person) = 0;

  virtual bool isRemovable(id_t person) = 0;

  virtual void  updatePerson(const Person& person, id_t id) = 0;

  virtual void dropData() = 0;


  virtual void setRenderData(const RenderData& renderData) = 0;
  virtual RenderData getRenderData()const  = 0 ;


  virtual ~DB() = default;
};

} // namespace mftb
