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
#include "datamodel.h"
#include "family-connector.h"
#include "individual-item.h"
#include <QApplication>
#include <cstdint>
#include <stack>
#include "SqlDB.h"
#include <qlogging.h>
#include <qobject.h>
#include <stdexcept>


FamilyTreeItem::FamilyTreeItem(QGraphicsObject *parent)  : QGraphicsObject(parent)  {}

QRectF FamilyTreeItem::boundingRect() const{
  return childrenBoundingRect();
}

void FamilyTreeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
                      ;
                     }

PersonItem* FamilyTreeItem::addPersonWithId(id_t id, const Person& person) {
  PersonItem *person_item = new PersonItem(person, this);
  person_map[id] = person_item;
  return person_item;
} 

PersonItem* FamilyTreeItem::getPersonItemById(uint32_t id) const {
  return person_map[id];   
}

FamilyConnector* FamilyTreeItem::addFamilyWithCoupleId(id_t id, Couple couple, std::vector<id_t> children) {
  auto* person_item1 = getPersonItemById(couple.person1_id);
  auto* person_item2 = getPersonItemById(couple.person2_id);

  if(person_item1 == nullptr){
    throw std::runtime_error("The first parent in the family must not be nullptr");
  }

  FamilyConnector* fc = new FamilyConnector(person_item1, person_item2, this);
  for( auto child_id : children){
    auto* child_item = getPersonItemById(child_id);
    fc->addChild(child_item);
  }

  couple_id_to_family_map[id] = fc;
  qDebug() << couple_id_to_family_map.keys();
  return fc;
}

FamilyConnector* FamilyTreeItem::getFamilyWithCoupleId(id_t id) const {
  return couple_id_to_family_map[id];
}


void FamilyTreeItem::renderFamilies() {
   
}


