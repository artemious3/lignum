/*
 * File: /src/app/tree-wizard/tree-scene/connector.h
 * Project: MFTB
 * File Created: Saturday, 27th July 2024 7:43:36 pm
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

/* --- Description ---
 * ConnectorItem is a  shape that connects
 * two points on a scene with a polyline,
 * having segments parallel to axes.
 * ConnectorItem has at most 3 segments,
 * the 2nd segments is called midline. */

#pragma once
#include <QGraphicsItem>
#include <QPen>
#include <climits>
#include <qline.h>
#include <qpoint.h>
#include "abstract-connector.h"

enum class Axis { X, Y };

class ConnectorItem : public AbstractConnector {

public:
  ConnectorItem(Axis ax, QGraphicsObject *par = nullptr);

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;

  void setStart(const QPointF &p) override;
  void setEnd(const QPointF &p) override;
  void setBias(qreal bias) override;

  QPointF getConnectionPoint(float ratio) const override;

  void setPen(QPen pen) override;
  QPen getPen() const;


private:
  QPointF start, end;
  QLineF midline;
  qreal midline_bias = 0;
  QPen pen;

  void process_midline();

protected:
  const Axis middle_axis;
};
