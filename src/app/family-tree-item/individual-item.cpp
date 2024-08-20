
#include "individual-item.h"
#include "SqlDB.h"
#include <QApplication>
#include <QTextDocument>
#include <QWidget>
#include <qapplication.h>
#include <qassert.h>
#include <qgraphicsitem.h>
#include <qlogging.h>
#include <qobject.h>
#include <qpair.h>
#include <qpalette.h>
#include <qtextoption.h>
#include <stdexcept>

#include "family-connector.h"

PersonItem::PersonItem(const Person& person,
                       QGraphicsObject *parent)
    : QGraphicsObject(parent),
      TEXT_BACKGROUND_COLOR(/*#######*/),
      TEXT_STYLESHEET("background-color: " + TEXT_BACKGROUND_COLOR.name()) {
  refresh(person);
}

void PersonItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  ;
}

QRectF PersonItem::boundingRect() const { return childrenBoundingRect(); }

QPointF PersonItem::getConnectionPoint(Side side) const {

  const QRectF picture_rect = icon->sceneBoundingRect();

  QPointF connection_point;
  switch (side) {
  case Side::Left:
    connection_point = (picture_rect.topLeft() + picture_rect.bottomLeft()) / 2;
    break;
  case Side::Right:
    connection_point =
        (picture_rect.topRight() + picture_rect.bottomRight()) / 2;
    break;

  case Side::Top:
    connection_point = (picture_rect.topRight() + picture_rect.topLeft()) / 2;
    break;

  case Side::Bottom:
    connection_point =
        (picture_rect.bottomRight() + picture_rect.bottomLeft()) / 2.0;
    break;
  }

  return connection_point;
}

void PersonItem::addIcon() {

  delete icon;

  if (person_data.gender == 'M') {
    icon = new QGraphicsRectItem(0, 0, PICTURE_SIDE, PICTURE_SIDE, this);
  } else if (person_data.gender == 'F') {
    icon = new QGraphicsEllipseItem(0, 0, PICTURE_SIDE, PICTURE_SIDE, this);
  } else {
    const qreal half_diagnoal = PICTURE_SIDE * sqrt(2) / 2.0;
    icon = new QGraphicsRectItem(0, 0, half_diagnoal, half_diagnoal, this);
    icon->setRotation(45);
  }

  icon->moveBy(-PICTURE_SIDE / 2, -PICTURE_SIDE / 2);
  icon->setPen(QPen(/*#######*/));
}

QString PersonItem::getFormattedName() {
  return QString("<div style='%1'>%2</div>")
      .arg(TEXT_STYLESHEET)
      .arg(QStringList{person_data.first_name, person_data.middle_name, person_data.last_name}.join(' '));
}

void PersonItem::addName() {
  delete text;

  QTextOption opt;
  opt.setAlignment(ALIGNMENT);

  text = new QGraphicsTextItem(this);
  text->setHtml(getFormattedName());
  text->setTextWidth(TEXT_WIDTH);
  text->moveBy(-text->boundingRect().width() / 2, 1.02 * PICTURE_SIDE / 2);
  text->document()->setDefaultTextOption(opt);
}


void PersonItem::refresh(const Person& person) {
  this->person_data = person;
  // addIcon();
  // addName();
}
