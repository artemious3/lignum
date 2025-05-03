/*
 * File: /src/app/tree-wizard/tree-scene/connector.cpp
 * Project: MFTB
 * File Created: Saturday, 27th July 2024 8:44:56 pm
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
 
#include "simple-connector.h"
#include <QPainter>
#include <qgraphicsitem.h>
#include <qline.h>
#include <qobject.h>
#include <qpoint.h>

SimpleConnectorItem::SimpleConnectorItem(Axis ax, QGraphicsObject *par)
    : AbstractConnector(par), middle_axis(ax) {

      
}

void SimpleConnectorItem::setStart(const QPointF &p) {
  this->start = p;
  process_midline();
}

void SimpleConnectorItem::setEnd(const QPointF &p) {
  this->end = p;
  process_midline();
}

void SimpleConnectorItem::setBias(qreal bias) {
  this->midline_bias = bias;
  process_midline();
}

void SimpleConnectorItem::setPen(QPen pen_) { pen = pen_; }

QPen SimpleConnectorItem::getPen() const { return pen; }

void SimpleConnectorItem::paint(QPainter *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget *widget) {
  painter->save();
  painter->setPen(pen);
  painter->drawLine(start, midline.p1());
  painter->drawLine(midline);
  painter->drawLine(midline.p2(), end);
  painter->restore();
}

QRectF SimpleConnectorItem::boundingRect() const {
  QRectF start_end_rect = QRectF(start, end).normalized();
  QRectF midline_rect = QRectF(midline.p1(), midline.p2()).normalized();
  return start_end_rect | midline_rect;
}

void SimpleConnectorItem::process_midline() {
  QPointF midline_start, midline_end;

  if (middle_axis == Axis::X) {
    const qreal y_center = (start.y() + end.y()) / 2.0;
    midline_start.setX(start.x());
    midline_start.setY(y_center + midline_bias);
    midline_end.setX(end.x());
    midline_end.setY(y_center + midline_bias);

  } else if (middle_axis == Axis::Y) {
    const qreal x_center = (start.x() + end.x()) / 2.0;
    midline_start.setX(x_center + midline_bias);
    midline_start.setY(start.y());
    midline_end.setX(x_center + midline_bias);
    midline_end.setY(end.y());
  }
  
  this->midline = {midline_start, midline_end};
}

QPointF SimpleConnectorItem::getConnectionPoint(float ratio) const {
  return midline.pointAt(ratio);
}
