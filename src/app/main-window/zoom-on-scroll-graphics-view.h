/*
 * File: /src/app/mftb-window/zoom-on-scroll-graphics-view.h
 * Project: MFTB
 * File Created: Tuesday, 30th July 2024 1:16:26 pm
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
#include <qgraphicsview.h>
#include <qwidget.h>

class ZoomOnScrollGraphicsView : public QGraphicsView {
  Q_OBJECT 
public:
  ZoomOnScrollGraphicsView(QWidget *parent = nullptr);

protected:
  void wheelEvent(QWheelEvent *event) override;

private:
  //TODO: should be somehow achieved from system settings
  static constexpr qreal WheelSensitivity = 0.002;
  static constexpr qreal MaxZoom = 5.0;
  static constexpr qreal MinZoom = 1.0/2;

  qreal relative_scale = 1.0;
  bool isAtMaxZoom() const;
  bool isAtMinZoom() const;
};
