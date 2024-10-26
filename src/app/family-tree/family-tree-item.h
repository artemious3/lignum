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

#include "DB.h"
#include "datamodel.h"
#include "family-connector.h"
#include "individual-item.h"
#include <QHash>
#include <cstdint>
#include <functional>
#include <memory>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qhash.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class FamilyTreeItem : public QGraphicsObject {

  Q_OBJECT

public:
  FamilyTreeItem(QGraphicsObject *parent = nullptr);

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;

  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

  PersonItem *addPersonWithId(id_t id, const Person &person);
  FamilyConnector *addFamilyWithCoupleId(id_t id, Couple couple,
                                         std::vector<id_t> children);

  void renderConnections();
  void refresh();
  void clear();

  PersonItem *getPersonItemById(uint32_t id) const;
  FamilyConnector *getFamilyWithCoupleId(id_t id) const;
  std::pair<IdType, id_t> getSelectedItemId() const;

  static constexpr qreal CONNECTORS_Z_VALUE = -1.0;

signals:
  void personSelected(id_t id);

private:
  QHash<uint32_t, PersonItem *> person_map;
  QHash<uint32_t, FamilyConnector *> couple_id_to_family_map;

  void reselectItem();

  id_t selected_item_id = 0;
  IdType selected_item_type;

};