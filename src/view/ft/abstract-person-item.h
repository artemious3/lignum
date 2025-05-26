#pragma once
#include <QGraphicsObject>
#include <qgraphicsitem.h>
#include "entities.h"
#include "renderer-flags.h"


enum class Side { Top, Bottom, Left, Right };

class AbstractPersonItem : public QGraphicsObject
{
public:
  AbstractPersonItem(QGraphicsObject* obj) : QGraphicsObject(obj) {}

  virtual ~AbstractPersonItem() {}

  virtual QRectF boundingRect() const                 = 0;

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                    QWidget *widget = nullptr)        = 0;

  virtual QPointF getConnectionPoint(Side side) const = 0;


  virtual void setPerson(id_t id, const Person& person)= 0;

  virtual void toggleSelected(bool is_selected)       = 0;

  virtual id_t getId() const                         = 0;

  virtual renderer_flags_t& rendererFlags()		     = 0;

};
