#include "family-connector.h"
#include "connector.h"
#include "individual-item.h"
#include "people-connector.h"
#include "family-tree-item.h"
#include <algorithm>
#include <qassert.h>
#include <qgraphicsitem.h>
#include <qobject.h>
#include <qsharedpointer.h>

FamilyConnector::FamilyConnector(PersonItem *a_parent1, PersonItem *a_parent2,
                                 QGraphicsItem *parent_item)
    : pen(QPen(/*############*/)), parent1(a_parent1),
      parent2(a_parent2), QGraphicsItem(parent_item),
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

void FamilyConnector::addChild(PersonItem *child) {
  Q_ASSERT(child != nullptr);
  children.insert(child);
}

bool FamilyConnector::isSingleParent() const { return parent2 == nullptr; }

bool FamilyConnector::FamilyConnector::FamilyConnector::isEmpty() {
  return parent1 == nullptr && parent2 == nullptr && children.empty();
}

std::pair<PersonItem *, PersonItem *> FamilyConnector::getParents() {
  return {parent1, parent2};
}

bool FamilyConnector::hasParent(PersonItem* item) {
  return (parent1 == item) ||
         (parent2 == item);
}

bool FamilyConnector::hasChild(PersonItem* item) {
  return children.contains(item);
}

bool FamilyConnector::setEmptyParent(PersonItem *person) {
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

  for (auto child : children) {
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
    PeopleConnectorItem *child_connector;
    if (isSingleParent()) {
      child_connector = new PeopleConnectorItem(parent1, Side::Bottom, child,
                                                Side::Top, Axis::Y, this);

    } else {
      QPointF parents_connector_center = parents_connector->getMidlineCenter();
      child_connector = PeopleConnectorItem::PointToPerson(
          parents_connector_center, child, Side::Top, Axis::X, this);
    }

    child_connector->setPen(pen);
    children_connectors.push_back(child_connector);
  }
}

void FamilyConnector::renderCoupleConnection() {
  auto *ftree = qobject_cast<FamilyTreeItem*>( parentObject() );

  if (parent2 != nullptr) {
    delete parents_connector;
    parents_connector = new PeopleConnectorItem(parent1, Side::Bottom, parent2,
                                                Side::Bottom, Axis::X, this);

                                                //TODO: get amounts of families and set biasr
    parents_connector->setBias(
        BIAS_PER_COUPLE);
    parents_connector->setPen(pen);
  }
}

const std::set<PersonItem*>& FamilyConnector::getChildren() {
  return children;
}
