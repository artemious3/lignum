/*
 * File: /src/core/Family.h
 * Project: MFTB
 * File Created: Tuesday, 30th July 2024 6:47:21 pm
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

#include "Person.h"
#include <QList>
#include <QPair>

class Family {
public:
    Family(Person* a_parent1, Person* a_parent2);
    
    QPair<Person*, Person*> getParents() const;
    const QList<Person*>& getChildren() const;

    void addChild(Person* child);

private:
  Person *const parent1, *const parent2;
  QList<Person*> children;
};