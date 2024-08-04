/*
 * File: /src/app/tree-wizard/tree-scene/treescene.cpp
 * Project: MFTB
 * File Created: Tuesday, 16th July 2024 12:32:29 pm
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

#include "family-tree-item.h"
#include "family-connector.h"
#include "individual-item.h"
#include <QApplication>
#include <cstdint>
#include <memory>
#include <qlogging.h>
#include <qobject.h>
#include <sys/types.h>

QGraphicsObject *FamilyTreeItem::getObjectById(u_int32_t id) const {
  return object_map[id];
}



FamilyTreeItem::FamilyTreeItem(QGraphicsObject *parent)  : QGraphicsObject(parent), family_connectors_db(*this) {}

QRectF FamilyTreeItem::boundingRect() const{
  return childrenBoundingRect();
}

void FamilyTreeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
                      ;
                     }

void FamilyTreeItem::addPerson(std::shared_ptr<Person> person) {
  static const qreal moveby = 100;
  static int counter = 0;

  PersonItem *person_item = new PersonItem(person, this);
  person_item->moveBy(moveby * counter, 0);

  // FIXME : identifier should start with '#', need separate conversion function
  object_map[person->identifier.toUInt()] = person_item;
  ++counter;
}


PersonItem* FamilyTreeItem::getPersonById(uint32_t id) const {
  return qobject_cast<PersonItem*>(getObjectById(id));    
}

void FamilyTreeItem::addRelationship(std::shared_ptr<Relationship> relationship) {
    family_connectors_db.addRelationship(relationship);
}


int FamilyTreeItem::getAmountOfFamilies(uint32_t id) {
  return family_connectors_db.getAmountOfFamilies(id);    
}

void FamilyTreeItem::renderFamilies() {
  auto families = family_connectors_db.getFamilies();
  foreach(auto family, families){
    family->setParentItem(this);
    family->renderConnections();
  }    
}
