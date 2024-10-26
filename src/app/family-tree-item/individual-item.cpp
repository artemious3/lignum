
#include "individual-item.h"
#include "SqlDB.h"
#include <QApplication>
#include <QTextDocument>
#include <QWidget>
#include <qapplication.h>
#include <qassert.h>
#include <qgraphicsitem.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpair.h>
#include <qpalette.h>
#include <qtextoption.h>
#include <stdexcept>

#include "family-connector.h"

PersonItem::PersonItem(id_t id_, const Person &person, QGraphicsObject *parent)
    : QGraphicsObject(parent),
    id(id_),
      TEXT_BACKGROUND_COLOR(QApplication::palette().base().color().name()),
      TEXT_STYLESHEET("background-color: " + TEXT_BACKGROUND_COLOR.name()) {
  refresh(person);
  setZValue(4.0);
  qDebug() << TEXT_STYLESHEET;
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
  icon->setZValue(2.0);
  icon->setPen(QPen(QApplication::palette().text().color(), 2));
}

QString PersonItem::getFormattedName() {
  return QString("<div style='%1'>%2</div>")
      .arg(TEXT_STYLESHEET)
      .arg(QStringList{QString::number(id), person_data.first_name, person_data.middle_name,
                          person_data.last_name}
               .join(' '));
}

void PersonItem::addName() {
  delete text;

  QTextOption opt;
  opt.setAlignment(ALIGNMENT);

  text = new QGraphicsTextItem(this);
  qDebug() << getFormattedName();
  text->setHtml(getFormattedName());
  text->setTextWidth(TEXT_WIDTH);
  text->moveBy(-text->boundingRect().width() / 2, 1.02 * PICTURE_SIDE / 2);
  text->document()->setDefaultTextOption(opt);
}

void PersonItem::refresh(const Person &person) {
  this->person_data = person;
  addIcon();
  addName();
}

void PersonItem::toggleSelected(bool is_selected) {
  if (is_selected) {
    icon->setPen(QPen(QApplication::palette().accent().color(), 3));
  } else {
    icon->setPen(QPen(QApplication::palette().text().color(), 2));
  }
}

id_t PersonItem::getId() const {
  return id;    
}
