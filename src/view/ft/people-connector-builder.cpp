#include "people-connector-builder.h"
#include <qpoint.h>
#include "Config.h"
#include "ColorManager.h"
#include <QPen>

PeopleConnectorBuilder::PeopleConnectorBuilder(AbstractConnector *conn)
    : connector(conn) {
      // conn->setPen(QPen(ColorManager::TextColor(), Config::ConnectorConfig().pen_width));
    }

PeopleConnectorBuilder &
PeopleConnectorBuilder::SetPerson1(const AbstractPersonItem *p, Side s) {
  connector->setStart(p->getConnectionPoint(s));
  return *this;
}

PeopleConnectorBuilder &
PeopleConnectorBuilder::SetPerson2(const AbstractPersonItem *p, Side s) {
  connector->setEnd(p->getConnectionPoint(s));
  return *this;
}

PeopleConnectorBuilder &PeopleConnectorBuilder::SetEndPoint1(QPointF p) {
  connector->setStart(p);
  return *this;
}

PeopleConnectorBuilder &PeopleConnectorBuilder::SetEndPoint2(QPointF p) {
  connector->setEnd(p);
  return *this;
}

PeopleConnectorBuilder& PeopleConnectorBuilder::SetPen(QPen pen){
	connector->setPen(pen);
	return *this;
}

AbstractConnector *PeopleConnectorBuilder::Result() { return connector; }
