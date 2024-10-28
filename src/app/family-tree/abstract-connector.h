
#pragma once
#include <qgraphicsitem.h>
#include <qpoint.h>

class AbstractConnector : public QGraphicsObject {

public:
    AbstractConnector(QGraphicsObject* obj) : QGraphicsObject(obj) {}

    virtual ~AbstractConnector() {}

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget = nullptr)         = 0;

    virtual QRectF boundingRect() const                   = 0;

    virtual void setStart(const QPointF&)                 = 0;

    virtual void setEnd(const QPointF&)                   = 0;

    virtual void setBias(qreal bias)                      = 0;

    virtual void setPen(QPen pen) = 0;

    virtual QPointF getConnectionPoint(float ratio = 0.5) const = 0;


};