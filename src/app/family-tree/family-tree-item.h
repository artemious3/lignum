/*
 * File: /src/app/tree-wizard/tree-scene/treescene.h
 * Project: MFTB
 * File Created: Tuesday, 16th July 2024 12:32:22 pm
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

#include "entities.h"
#include "family-connector.h"
#include "person-item.h"
#include <QHash>
#include <cstdint>
#include "abstract-person-item.h"
#include <QWidget>
#include <QGraphicsItem>

enum class IdType{
  Person, 
  Couple
};

struct item_selection{
	IdType idType;
	id_t id;
};

class FamilyTreeView : public QGraphicsObject {

  Q_OBJECT

public:
  FamilyTreeView(QGraphicsObject *parent = nullptr);

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  AbstractPersonItem *addPerson(id_t id, const Person &person);
  FamilyConnector *addFamily(id_t id, Couple couple,
                                         std::vector<id_t> children);
  void removePerson(id_t id);
  void removeFamily(id_t id);

  void clear_selection();

  // Renders all connections, if couple=0 
  // Else renders connections only in specified family
  void renderConnections(id_t family = 0);
  void clear();

  void reselectItem();

  AbstractPersonItem *getPerson(uint32_t id) const;
  AbstractFamilyConnector *getFamily(id_t id) const;
  item_selection getSelectedItemId() const;

  static constexpr qreal CONNECTORS_Z_VALUE = -1.0;

signals:
  void personSelected(id_t id);

private:
  QHash< uint32_t, AbstractPersonItem *> person_map;
  QHash< uint32_t, AbstractFamilyConnector *> couple_id_to_family_map;


  id_t selected_item_id = 0;
  IdType selected_item_type;

};
