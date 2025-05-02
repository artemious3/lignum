/*
 * File: /src/app/mftb-window/tree-scene/individual-item.h
 * Project: MFTB
 * File Created: Tuesday, 16th July 2024 9:44:31 pm
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
#include "family-tree-view.h"
#include <QGraphicsObject>
#include <cstdint>
#include <qgraphicsitem.h>
#include <QPainter>

class PeopleConnectorItem;
class FamilyConnector;


class PersonItem : public AbstractPersonItem {

  Q_OBJECT

public:
  PersonItem(id_t id, const Person& person, QGraphicsObject *parent);

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget = nullptr) override;

  QPointF getConnectionPoint(Side side) const override;
  void setPerson(id_t id, const Person& person) override final;

  void toggleSelected(bool is_selected) override;

  id_t getId() const override;

  uint16_t& rendererFlags() override;

private:
  void addIcon();
  void addName();
  QString getFormattedName();

  Person person_data;
  id_t id;
  uint16_t m_rendererData;

  QAbstractGraphicsShapeItem *icon = nullptr;
  QGraphicsTextItem *text = nullptr;

  const QColor TEXT_BACKGROUND_COLOR;
  const QString TEXT_STYLESHEET = "";
};
