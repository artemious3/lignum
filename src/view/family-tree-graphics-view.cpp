#include "family-tree-graphics-view.h"
#include "family-tree-view.h"
#include "qmargins.h"
#include <QWheelEvent>
#include <algorithm>
#include <qdebug.h>
#include <qgraphicsitem.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpolygon.h>
#include <qwidget.h>


static qreal bound(qreal val, qreal min, qreal max){
	return std::max(std::min(val, max), min);
}

namespace mftb {

FamilyTreeGraphicsView::FamilyTreeGraphicsView(QWidget *parent)
    : QGraphicsView(parent) {
			setRenderHint(QPainter::Antialiasing);
			setRenderHint(QPainter::SmoothPixmapTransform);
			setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		}

void FamilyTreeGraphicsView::wheelEvent(QWheelEvent *event) {
  qreal rotation_angle = (qreal)event->angleDelta().y();
  qreal koef = 1.0 + bound(rotation_angle * WheelSensitivity, -0.08, 0.08);

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
	QPolygonF scene_polygon = this->mapFromScene( this->scene()->sceneRect() );
	QRectF viewport_rect = this->viewport()->rect().toRectF();
	// we want to allow some free space around the tree
	QRectF viewport_small_rect = viewport_rect.marginsRemoved(QMarginsF{
			viewport_rect.height()*0.25,
			viewport_rect.width()*0.25,
			viewport_rect.height()*0.25,
			viewport_rect.width()*0.25,
			});
	return viewport_small_rect.contains(scene_polygon.boundingRect());
}

}
