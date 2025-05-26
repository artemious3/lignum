
#include "simple-person-item.h"
#include <QApplication>
#include <QTextDocument>
#include <QWidget>
#include <cstdint>
#include <qapplication.h>
#include <qassert.h>
#include <qgraphicsitem.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpair.h>
#include <qpalette.h>
#include <qtextoption.h>
#include "ColorManager.h"
#include "Config.h"

#include "abstract-person-item.h"
#include "simple-family-connector.h"


static const double ICON_PADDING = 0.04;

SimplePersonItem::SimplePersonItem(id_t id_, const Person &person, QGraphicsObject *parent)
    : AbstractPersonItem(parent),
      TEXT_BACKGROUND_COLOR(ColorManager::BackgroundColor()),
      TEXT_STYLESHEET("background-color: " + TEXT_BACKGROUND_COLOR.name()) {
  setPerson(id_, person);
  setZValue(4.0);
}

void SimplePersonItem::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) {
  ;
}

QRectF SimplePersonItem::boundingRect() const { return childrenBoundingRect(); }

QPointF SimplePersonItem::getConnectionPoint(Side side) const {

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

void SimplePersonItem::addIcon() {

  delete icon;
  const auto icon_size = Config::PersonItemConfig().icon_size;

  if (person_data.gender == 'M') {
    icon = new QGraphicsRectItem(0, 0, icon_size, icon_size, this);
    icon->moveBy(-icon_size / 2, -icon_size / 2);
  } else if (person_data.gender == 'F') {
    icon = new QGraphicsEllipseItem(0, 0, icon_size, icon_size, this);
    icon->moveBy(-icon_size / 2, -icon_size / 2);
  } else {
    const qreal half_diagonal = icon_size * sqrt(2) / 2.0;
    icon = new QGraphicsRectItem(0, 0, half_diagonal, half_diagonal, this);
    icon->moveBy(-half_diagonal/2.0, -half_diagonal/2.0);
    icon->setTransformOriginPoint(half_diagonal/2.0,  half_diagonal/2.0);
    icon->setRotation(45);
  }

  
  icon->setZValue(2.0);
  icon->setPen(QPen(ColorManager::TextColor(), 2));

}

QString SimplePersonItem::getFormattedName() {
  return QString("<div style='%1'>%2</div>")
      .arg(TEXT_STYLESHEET,
           QStringList{person_data.first_name, person_data.middle_name,
                       person_data.last_name}
               .join(' '));
}

void SimplePersonItem::addName() {
  delete text;

  QTextOption opt;
  const auto icon_size = Config::PersonItemConfig().icon_size;
  opt.setAlignment(Config::PersonItemConfig().text_alignment);

  text = new QGraphicsTextItem(this);
  qDebug() << getFormattedName();
  text->setHtml(getFormattedName());
  text->setTextWidth(Config::PersonItemConfig().text_width);
  text->moveBy(-text->boundingRect().width() / 2, (1 + ICON_PADDING) * icon_size / 2);
  text->document()->setDefaultTextOption(opt);
}

void SimplePersonItem::setPerson(id_t id_, const Person &person) {
  id = id_;
  this->person_data = person;
  addIcon();
  addName();
}

void SimplePersonItem::toggleSelected(bool is_selected) {
  if (is_selected) {
    icon->setPen(QPen(ColorManager::AccentColor(), 3));
  } else {
    icon->setPen(QPen(ColorManager::TextColor(), 2));
  }
}

renderer_flags_t& SimplePersonItem::rendererFlags(){
	return m_rendererData;
}

id_t SimplePersonItem::getId() const {
  return id;    
}
