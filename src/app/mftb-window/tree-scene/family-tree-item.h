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
#include "Person.h"
#include "Relationship.h"
#include "individual-item.h"
#include <cstdint>
#include <memory>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qhash.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include "family-connectors-db.h"
#include <QHash>

class FamilyTreeItem : public QGraphicsObject {

  Q_OBJECT
private:
 

public:
  FamilyTreeItem(QGraphicsObject  *parent = nullptr);

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;

  void addPerson(std::shared_ptr<Person> pers);
  void addRelationship(std::shared_ptr<Relationship> relationship);
  void renderFamilies();
  int getAmountOfFamilies(uint32_t id);
  
  QGraphicsObject *getObjectById(uint32_t id) const;
  PersonItem* getPersonById(uint32_t id)const ;

  static constexpr qreal CONNECTORS_Z_VALUE = -1.0;

private:
  QHash<uint32_t, QGraphicsObject *> object_map;
  FamilyConnectorsDB family_connectors_db;
 
};