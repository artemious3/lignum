/*
 * File: /src/app/mftb-window/tree-scene/people-connector.h
 * Project: MFTB
 * File Created: Sunday, 28th July 2024 11:20:41 pm
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

#include "connector.h"
#include "individual-item.h"
#include <qgraphicsitem.h>
#include <qpoint.h>

class PeopleConnectorItem : public ConnectorItem {
public:
  PeopleConnectorItem(PersonItem *, Side, PersonItem *, Side, Axis,
                      QGraphicsItem *parent);

  static PeopleConnectorItem *PointToPerson(QPointF start,
                                                     PersonItem *, Side, Axis,
                                                     QGraphicsItem *parent);

private:
  PeopleConnectorItem(QPointF start_point, PersonItem *, Side, Axis,
                      QGraphicsItem *parent);

protected:
  PersonItem *const person1, *const person2;
};