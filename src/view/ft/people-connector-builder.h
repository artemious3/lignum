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

#include <qgraphicsitem.h>
#include <qpoint.h>
#include "abstract-person-item.h"
#include "abstract-connector.h"

class PeopleConnectorBuilder {
public:
  PeopleConnectorBuilder(AbstractConnector* conn);
  PeopleConnectorBuilder& SetPerson1(const AbstractPersonItem* p, Side s);
  PeopleConnectorBuilder& SetPerson2(const AbstractPersonItem* p, Side s);
  PeopleConnectorBuilder& SetEndPoint1(QPointF);
  PeopleConnectorBuilder& SetEndPoint2(QPointF);

  AbstractConnector* Result();

protected:
  AbstractConnector* const connector;
};