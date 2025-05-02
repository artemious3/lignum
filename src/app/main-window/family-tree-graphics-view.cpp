#include "family-tree-graphics-view.h"
#include <QWheelEvent>
#include <qdebug.h>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpolygon.h>
#include <qwidget.h>

namespace mftb {

FamilyTreeGraphicsView::FamilyTreeGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {}

void FamilyTreeGraphicsView::wheelEvent(QWheelEvent *event) {
  qreal rotation_angle = (qreal)event->angleDelta().y();
  qreal koef = 1.0 + rotation_angle * WheelSensitivity;

  if (koef > 1.0 && isAtMaxZoom() || koef < 1.0 && isAtMinZoom()) {
    return;
  }

  scale(koef, koef);
  relative_scale *= koef;
}

bool FamilyTreeGraphicsView::isAtMaxZoom() const {
  return relative_scale >= MaxZoom;
}

bool FamilyTreeGraphicsView::isAtMinZoom() const {
  return relative_scale <= MinZoom;
}

}
