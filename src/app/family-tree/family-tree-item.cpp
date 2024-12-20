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
#include "abstract-person-item.h"
#include "datamodel.h"
#include "family-connector.h"
#include "family-tree-builder.h"
#include "person-item.h"
#include <QApplication>
#include <cassert>
#include <cstdint>
#include <qalgorithms.h>
#include <qgraphicsitem.h>
#include <QGraphicsScene>
#include <stack>
#include <stdexcept>
#include <QGraphicsSceneMouseEvent>

FamilyTreeItem::FamilyTreeItem(QGraphicsObject *parent)
    : QGraphicsObject(parent) {

  using namespace mftb;

  // DB *db = mftb::SqlDB::getInstance();      
  //
  // auto p00 = db->insertPerson({'M', "Greg", "Doe"}); //1
  // auto p0 = db->addChild({'M', "John", "Doe"}, p00);  //2
  // auto p1 = db->addPartner({'F', "Anna", "Doe"}, p0); //3
  // auto p2 = db->addPartner({'F', "Selena", "Jackson"}, p0); //4
  // auto p3 = db->addChild({'M', "Lois", "Doe"}, p0, p1); //5
  // auto p4 = db->addChild({'M', "Max", "Doe"}, p0, p1); //6
  // auto p5 = db->addChild({'F', "Michael", "Jackson"}, p0, p2); //7
  // auto p6 = db->addPartner({'M', "Jane", "Jackson"}, p5); //8
  //
  // auto p000 = db->addParent(p00, {'M', "Alan", "Doe"});
  // auto p01 = db->addChild({'F', "Helena", "Doe"}, p000);
  //
  // FamilyTreeBuilder fb{this, db};
  // fb.build_tree_from(1);

  // render();
}

QRectF FamilyTreeItem::boundingRect() const { return childrenBoundingRect(); }

void FamilyTreeItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget) {
  ;
}

AbstractPersonItem *FamilyTreeItem::addPerson(id_t id, const Person &person) {
  AbstractPersonItem *person_item = new PersonItem(id, person, this);
  person_item->hide();
  person_map[id] = person_item;
  return person_item;
}


void FamilyTreeItem::removePerson(id_t id){
	auto * target = person_map[id];
	delete target;
	person_map.remove(id);
}


void FamilyTreeItem::removeFamily(id_t id){
	auto* target  = couple_id_to_family_map[id];
	delete target;
	couple_id_to_family_map.remove(id);
}

AbstractPersonItem *FamilyTreeItem::getPerson(uint32_t id) const {
  return person_map[id];
}

FamilyConnector *
FamilyTreeItem::addFamily(id_t id, Couple couple,
                                      std::vector<id_t> children) {
  auto *person_item1 = getPerson(couple.person1_id);
  auto *person_item2 = getPerson(couple.person2_id);

  assert(person_item1 != nullptr);

  FamilyConnector *fc = new FamilyConnector(person_item1, person_item2, this);
  for (auto child_id : children) {
    auto *child_item = getPerson(child_id);
    fc->addChild(child_item);
  }

  couple_id_to_family_map[id] = fc;

  return fc;
}

AbstractFamilyConnector *FamilyTreeItem::getFamily(id_t id) const {
  return couple_id_to_family_map[id];
}


void FamilyTreeItem::renderConnections(id_t family ) {
  if (family == 0) {
    for (auto family_id : couple_id_to_family_map) {
      family_id->renderConnections();
    }
  } else {
	  getFamily(family)->renderConnections();
  }
}

void FamilyTreeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {    

  if(event->button() == Qt::MouseButton::LeftButton){
    auto pos = event->scenePos();
    auto items_list_at_pos = scene()->items(pos);

    AbstractPersonItem * new_selected_item = nullptr;

    for(auto * item : items_list_at_pos){
      auto * maybe_person_item = dynamic_cast<AbstractPersonItem*>(item);
      
      if(maybe_person_item  != nullptr){
        new_selected_item = maybe_person_item;
        break;
      }
    }

    if(new_selected_item != nullptr){
      if(selected_item_id != 0){
        getPerson(selected_item_id)->toggleSelected(false);
      }

      new_selected_item->toggleSelected(true);
      selected_item_id = new_selected_item->getId();
      emit personSelected(selected_item_id);
    }
  }
}


void FamilyTreeItem::clear_selection(){
	selected_item_id = 0;
}

item_selection FamilyTreeItem::getSelectedItemId() const {
  //user will be able to choose couple/family in the future
  return {IdType::Person, selected_item_id};    
}


void FamilyTreeItem::clear() {
    qDeleteAll(childItems());
    person_map.clear();
    couple_id_to_family_map.clear();
}

void FamilyTreeItem::reselectItem() {
  if(selected_item_id != 0){
    auto person_item = getPerson(selected_item_id);
    if(person_item != nullptr){
      person_item->toggleSelected(true);
    }
  }    
}
