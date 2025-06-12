#include "simple-family-connector.h"
#include "qalgorithms.h"
#include "qcoreapplication.h"
#include "qgraphicsitem.h"
#include "qpalette.h"
#include "simple-connector.h"
#include "people-connector-builder.h"
#include "abstract-person-item.h"
#include "spdlog/spdlog.h"
#include <QPalette>
#include <algorithm>
#include <QObject>
#include <QtAlgorithms>
#include <QGraphicsItem>
#include "Config.h"


SimpleFamilyConnector::SimpleFamilyConnector(AbstractPersonItem *a_parent1, AbstractPersonItem *a_parent2,
                                 QGraphicsObject *parent_item)
    : AbstractFamilyConnector(parent_item),
			palette(qApp ? qApp->palette() : QPalette{}),
			pen(QPen(palette.text(), Config::ConnectorConfig().pen_width)), parent1(a_parent1),
      parent2(a_parent2), 
      parents_connector(nullptr) {

  Q_ASSERT(parent1 != nullptr);
}

void SimpleFamilyConnector::paint(QPainter *painter,
                            const QStyleOptionGraphicsItem *option,
                            QWidget *widget) {
  ; // do nothing, since only children should be drawn.
}

QRectF SimpleFamilyConnector::boundingRect() const { return childrenBoundingRect(); }

void SimpleFamilyConnector::addChild(const AbstractPersonItem *child) {
  Q_ASSERT(child != nullptr);
  children.push_back(child);
}

bool SimpleFamilyConnector::isSingleParent() const { return parent2 == nullptr; }

bool SimpleFamilyConnector::isEmpty() {
  return parent1 == nullptr && parent2 == nullptr && children.empty();
}

std::pair<const AbstractPersonItem *, const AbstractPersonItem *> SimpleFamilyConnector::getParents() const{
  return {parent1, parent2};
}

bool SimpleFamilyConnector::hasParent(const AbstractPersonItem* item) const {
  return (parent1 == item) ||
         (parent2 == item);
}

bool SimpleFamilyConnector::hasChild(const AbstractPersonItem* item) const {
  return std::find(children.cbegin(), children.cend(), item) != children.end();
}

bool SimpleFamilyConnector::setEmptyParent(AbstractPersonItem *person) {
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

void SimpleFamilyConnector::renderConnections() {
  Q_ASSERT(parentObject() != nullptr);
  renderCoupleConnection();
  renderParentChildConnections();
}

void SimpleFamilyConnector::renderParentChildConnections() {
  qDeleteAll(children_connectors);
  children_connectors.clear();
  foreach (const auto *child , children) {
    if (!child->isVisible()) {
	    //here should be some routine, showing that node has invisible children
      continue;
    }
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
      child_connector = PeopleConnectorBuilder(new SimpleConnectorItem(Axis::X, this))
			.SetPen(pen)
      .SetPerson1(parent1, Side::Bottom)
      .SetPerson2(child,   Side::Top)
      .Result();

    } else {

      QPointF parents_connector_center = parents_connector->getConnectionPoint(0.5);
      if(family_connection_point_x.has_value()){
        parents_connector_center.setX(family_connection_point_x.value());
      }

      child_connector = PeopleConnectorBuilder(new SimpleConnectorItem(Axis::X, this))
							.SetPen(pen)
              .SetEndPoint1(parents_connector_center)
              .SetPerson2(child, Side::Top)
              .Result();
    }
    children_connectors.push_back(child_connector);
  }
}

void SimpleFamilyConnector::renderCoupleConnection() {

  delete parents_connector;
  if (parent2 != nullptr) {
			parents_connector = PeopleConnectorBuilder(new SimpleConnectorItem(Axis::X, this))
												.SetPen(pen)
                        .SetPerson1(parent1, Side::Bottom)
                        .SetPerson2(parent2, Side::Bottom)
                        .Result();

    parents_connector->setBias(family_line_y_bias.value_or(INITIAL_FAMILY_LINE_BIAS));
  } else {
	  parents_connector = nullptr;
  }
}

const QList<const AbstractPersonItem*>& SimpleFamilyConnector::getChildren() const {
  return children;
}

void SimpleFamilyConnector::setChildrenConnectionPointX(qreal x) {
  family_connection_point_x = x;
}
void SimpleFamilyConnector::setDefaultChildrenConnectionPointX() {
	family_connection_point_x = {};
}

void SimpleFamilyConnector::setFamilyLineYBias(qreal y) {
  family_line_y_bias = y;
}

void SimpleFamilyConnector::removeChild(const AbstractPersonItem* child) {
	children.erase(std::find(children.begin(), children.end(), child));
}

void SimpleFamilyConnector::removeParent(const AbstractPersonItem* parent){
	if(parent1 == parent){
		parent1 = parent2;
		parent2 = nullptr;
	} else if (parent2 == parent){
		parent2 = nullptr;
	}
}

void SimpleFamilyConnector::setParent1(const AbstractPersonItem* p) {
  parent1 = p;
}

void SimpleFamilyConnector::setParent2(const AbstractPersonItem* p) {
  parent2 = p;
}


void SimpleFamilyConnector::recolor(const QPalette& palette) {
	this->palette = palette;
	this->pen = QPen(palette.text(), 1);
	qDeleteAll(((QGraphicsObject*)this)->children());
	this->renderConnections();
}
