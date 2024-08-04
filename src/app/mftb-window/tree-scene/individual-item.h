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

#include "Person.h"
#include <QGraphicsObject>
#include <cstdint>
#include <memory>
#include <qgraphicsitem.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <optional>

class PeopleConnectorItem;
class FamilyConnector;

enum class Side { Top, Bottom, Left, Right };

class PersonItem : public QGraphicsObject {

  Q_OBJECT

public:
  PersonItem(std::shared_ptr<const Person>, QGraphicsObject *parent);

  QRectF boundingRect() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
             QWidget *widget = nullptr) override;

  QPointF getConnectionPoint(Side side) const;

  uint32_t getId();

private:
  void addIcon();
  void addName();
  QString getFormattedName();
  std::shared_ptr<const Person> getBackend();

private:
  const std::weak_ptr<const Person> backend;
  std::optional<uint32_t> optional_id;

  QAbstractGraphicsShapeItem *icon;
  QGraphicsTextItem *text;

  static constexpr float PICTURE_SIDE = 40;
  static constexpr float TEXT_WIDTH = 2 * PICTURE_SIDE;
  static constexpr Qt::Alignment ALIGNMENT = Qt::AlignCenter;
  const QColor TEXT_BACKGROUND_COLOR;
  const QString TEXT_STYLESHEET = "";
};
