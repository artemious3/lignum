#pragma once
#include "abstract-person-item.h"

/*
 * This QGraphicsObject contains all connectors that cover
 * all parent-child and parent-parent relationships of the
 * family.
 */

class AbstractFamilyConnector : public QGraphicsObject {

public:
  AbstractFamilyConnector(QGraphicsObject *obj) : QGraphicsObject(obj) {}

  virtual ~AbstractFamilyConnector() {}

  QRectF boundingRect() const = 0;

  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) = 0;

  virtual bool hasParent(const AbstractPersonItem *item) const = 0;

  virtual bool hasChild(const AbstractPersonItem *item) const = 0;

  virtual void addChild(const AbstractPersonItem *item) = 0;

  virtual void removeChild(const AbstractPersonItem* item) = 0;

  virtual void setParent1(const AbstractPersonItem *item) = 0;

  virtual void setParent2(const AbstractPersonItem *item) = 0;

  virtual std::pair<const AbstractPersonItem*, const AbstractPersonItem*> getParents() const = 0;

  virtual const QList<const AbstractPersonItem*>& getChildren() const = 0;

  virtual void setFamilyLineYBias(qreal bias) = 0;

  virtual void setChildrenConnectionPointX(qreal x) = 0;

  virtual void renderConnections() = 0;

};