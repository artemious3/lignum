#include "family-connector.h"
#include "connector.h"
#include "people-connector-builder.h"
#include "abstract-person-item.h"
#include "family-tree-item.h"
#include <algorithm>
#include <qalgorithms.h>
#include <qapplication.h>
#include <qassert.h>
#include <qgraphicsitem.h>
#include <qobject.h>
#include <qsharedpointer.h>

FamilyConnector::FamilyConnector(AbstractPersonItem *a_parent1, AbstractPersonItem *a_parent2,
                                 QGraphicsObject *parent_item)
    : pen(QPen(QApplication::palette().text().color(), 1)), parent1(a_parent1),
      parent2(a_parent2), AbstractFamilyConnector(parent_item),
      parents_connector(nullptr) {

  Q_ASSERT(parent1 != nullptr);
  parents_connector = nullptr;
}

void FamilyConnector::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget) {
  ; // do nothing, since only children should be drawn.
}

QRectF FamilyConnector::boundingRect() const { return childrenBoundingRect(); }

void FamilyConnector::addChild(const AbstractPersonItem *child) {
  Q_ASSERT(child != nullptr);
  children.push_back(child);
}

bool FamilyConnector::isSingleParent() const { return parent2 == nullptr; }

bool FamilyConnector::FamilyConnector::FamilyConnector::isEmpty() {
  return parent1 == nullptr && parent2 == nullptr && children.empty();
}

std::pair<const AbstractPersonItem *, const AbstractPersonItem *> FamilyConnector::getParents() const{
  return {parent1, parent2};
}

bool FamilyConnector::hasParent(const AbstractPersonItem* item) const {
  return (parent1 == item) ||
         (parent2 == item);
}

bool FamilyConnector::hasChild(const AbstractPersonItem* item) const {
  return std::find(children.cbegin(), children.cend(), item) != children.end();
}

bool FamilyConnector::setEmptyParent(AbstractPersonItem *person) {
  if (parent1 == nullptr) {
    parent1 = person;
    return true;
  }

  if (parent2 == nullptr) {
    parent2 = person;
    return true;
  }

  return false;
}

void FamilyConnector::renderConnections() {
  Q_ASSERT(parentObject() != nullptr);
  renderCoupleConnection();
  renderParentChildConnections();
}

void FamilyConnector::renderParentChildConnections() {
  qDeleteAll(children_connectors);
  children_connectors.clear();
  for (auto * child : children) {
    /* If family is single-parent, create straight connection from parent to
     * child:
     *
     *   o
     *   |
     *   o
     *
     * Otherwise connect the child to the center of couple connector:
     *
     *   o   o
     *   |___|
     *     |
     *     o
     */
    AbstractConnector *child_connector;
    if (isSingleParent()) {
      child_connector = PeopleConnectorBuilder(new ConnectorItem(Axis::X, this))
      .SetPerson1(parent1, Side::Bottom)
      .SetPerson2(child,   Side::Top)
      .Result();

    } else {

      QPointF parents_connector_center = parents_connector->getConnectionPoint(0.5);
      if(family_connection_point_x.has_value()){
        parents_connector_center.setX(family_connection_point_x.value());
      }

      child_connector = PeopleConnectorBuilder(new ConnectorItem(Axis::X, this))
              .SetEndPoint1(parents_connector_center)
              .SetPerson2(child, Side::Top)
              .Result();
    }
    children_connectors.push_back(child_connector);
  }
}

void FamilyConnector::renderCoupleConnection() {
  auto *ftree = qobject_cast<FamilyTreeItem*>( parentObject() );

  if (parent2 != nullptr) {
    delete parents_connector;
    parents_connector = PeopleConnectorBuilder(new ConnectorItem(Axis::X, this))
                        .SetPerson1(parent1, Side::Bottom)
                        .SetPerson2(parent2, Side::Bottom)
                        .Result();

    parents_connector->setBias(family_line_y_bias.value_or(INITIAL_FAMILY_LINE_BIAS));
  }
}

const QList<const AbstractPersonItem*>& FamilyConnector::getChildren() const {
  return children;
}

void FamilyConnector::setChildrenConnectionPointX(qreal x) {
  family_connection_point_x = x;
}

void FamilyConnector::setFamilyLineYBias(qreal y) {
  family_line_y_bias = y;
}

void FamilyConnector::removeChild(const AbstractPersonItem* child) {
  std::remove(children.begin(), children.end(), child);
}

void FamilyConnector::setParent1(const AbstractPersonItem* p) {
  parent1 = p;
}

void FamilyConnector::setParent2(const AbstractPersonItem* p) {
  parent2 = p;
}
