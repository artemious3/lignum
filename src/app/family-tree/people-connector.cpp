/*
 * File: /src/app/mftb-window/tree-scene/people-connector.cpp
 * Project: MFTB
 * File Created: Sunday, 28th July 2024 11:22:12 pm
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

#include "people-connector.h"
#include "individual-item.h"
#include <qgraphicsitem.h>
#include <qpoint.h>

/* Constructs connector between two people, with points of connections
 * on specified sides. */
PeopleConnectorItem::PeopleConnectorItem(PersonItem *p1, Side s1,
                                         PersonItem *p2, Side s2, Axis axis,
                                         QGraphicsItem *parent)
    : ConnectorItem(axis, parent), person1(p1), person2(p2)

{
  QPointF start, end;
  start = this->person1->getConnectionPoint(s1);
  end = this->person2->getConnectionPoint(s2);
  setEndPoints(start, end);
  int x;
}

PeopleConnectorItem *PeopleConnectorItem::PointToPerson(
    QPointF start_point, PersonItem *person, Side side, Axis axis,
    QGraphicsItem *parent_item) {
  return new PeopleConnectorItem(start_point, person, side, axis, parent_item);
}

/* Constructs connector between a point on a scene and person. */
PeopleConnectorItem::PeopleConnectorItem(QPointF start, PersonItem *person,
                                         Side side, Axis axis,
                                         QGraphicsItem *parent)
    : ConnectorItem(axis, parent), person1(person), person2(nullptr) {

  QPointF end = this->person1->getConnectionPoint(side);
  setEndPoints(start, end);
}