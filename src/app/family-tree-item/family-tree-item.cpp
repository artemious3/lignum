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
#include "DB.h"
#include "SqlDB.h"
#include "family-connector.h"
#include "family-tree-builder.h"
#include "balancer-preprocessor.h"
#include "individual-item.h"
#include <QApplication>
#include <cstdint>
#include <qgraphicsitem.h>
#include <qgraphicssceneevent.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qtransform.h>
#include <stack>
#include <stdexcept>
#include "balancer-processor.h"
#include <QGraphicsSceneMouseEvent>

FamilyTreeItem::FamilyTreeItem(QGraphicsObject *parent)
    : QGraphicsObject(parent) {

  using namespace mftb;

  DB *db = mftb::SqlDB::getInstance();      
  auto p00 = db->insertPerson({'M', "Greg", "Doe"}); //1
  auto p0 = db->addChild({'M', "John", "Doe"}, p00);  //2
  auto p1 = db->addPartner({'F', "Anna", "Doe"}, p0); //3
  auto p2 = db->addPartner({'F', "Selena", "Jackson"}, p0); //4
  auto p3 = db->addChild({'M', "Lois", "Doe"}, p0, p1); //5
  auto p4 = db->addChild({'M', "Max", "Doe"}, p0, p1); //6
  auto p5 = db->addChild({'F', "Michael", "Jackson"}, p0, p2); //7
  auto p6 = db->addPartner({'M', "Jane", "Jackson"}, p5); //8

  FamilyTreeBuilder builder(this, db);
  builder.build_tree_from(p0);

  FamilyTreeBalancer balancer(db, this);
  balancer.balance_from_couple_id(db->getPersonCouplesId(p00).front());

  renderConnections();
}

QRectF FamilyTreeItem::boundingRect() const { return childrenBoundingRect(); }

void FamilyTreeItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget) {
  ;
}

PersonItem *FamilyTreeItem::addPersonWithId(id_t id, const Person &person) {
  PersonItem *person_item = new PersonItem(person, this);
  person_map[id] = person_item;
  return person_item;
}

PersonItem *FamilyTreeItem::getPersonItemById(uint32_t id) const {
  return person_map[id];
}

FamilyConnector *
FamilyTreeItem::addFamilyWithCoupleId(id_t id, Couple couple,
                                      std::vector<id_t> children) {
  auto *person_item1 = getPersonItemById(couple.person1_id);
  auto *person_item2 = getPersonItemById(couple.person2_id);

  if (person_item1 == nullptr) {
    throw std::runtime_error(
        "The first parent in the family must not be nullptr");
  }

  FamilyConnector *fc = new FamilyConnector(person_item1, person_item2, this);

  
  for (auto child_id : children) {
    auto *child_item = getPersonItemById(child_id);
    fc->addChild(child_item);
  }

  couple_id_to_family_map[id] = fc;
  qDebug() << couple_id_to_family_map.keys();

  return fc;
}

FamilyConnector *FamilyTreeItem::getFamilyWithCoupleId(id_t id) const {
  return couple_id_to_family_map[id];
}

void FamilyTreeItem::renderConnections() {
  for(auto family_id : couple_id_to_family_map){
    family_id->renderConnections();
  }
}

void FamilyTreeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {    

  if(event->button() == Qt::MouseButton::LeftButton){
    auto pos = event->scenePos();
    auto items_list_at_pos = scene()->items(pos);

    PersonItem* new_selected_item = nullptr;

    for(auto * item : items_list_at_pos){
      auto * maybe_person_item = dynamic_cast<PersonItem*>(item);
      if(maybe_person_item  != nullptr){
        new_selected_item = maybe_person_item;
      }
    }

    if(new_selected_item != nullptr){
      qDebug() << "non nullptr PersonItem: " << new_selected_item;
      if(selected_item != nullptr){
        selected_item->toggleSelected(false);
      }
      new_selected_item->toggleSelected(true);
      selected_item = new_selected_item;
      
    }
  }
}
