
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
#include "Config.h"

#include "abstract-person-item.h"
#include "qalgorithms.h"
#include "qfontmetrics.h"
#include "renderer-flags.h"
#include "simple-family-connector.h"
#include "spdlog/spdlog.h"



SimplePersonItem::SimplePersonItem(id_t id_, const Person &person, QGraphicsObject *parent)
    : AbstractPersonItem(parent),
			palette(qApp ? qApp->palette() : QPalette{}),
      TEXT_BACKGROUND_COLOR(palette.base().color()),
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
	const auto padding = Config::PersonItemConfig().icon_padding;

  if (person_data.gender == 'M') {
    icon = new QGraphicsRectItem(0, 0, icon_size, icon_size, this);
		icon->setPos(padding,padding);
  } else if (person_data.gender == 'F') {
    icon = new QGraphicsEllipseItem(0, 0, icon_size, icon_size, this);
		icon->setPos(padding,padding);
  } else {
    const qreal half_diagonal = icon_size * sqrt(2) / 2.0;
    icon = new QGraphicsRectItem(0, 0, half_diagonal, half_diagonal, this);
    // icon->moveBy(-half_diagonal/2.0, -half_diagonal/2.0);
		
    icon->setTransformOriginPoint(half_diagonal/2.0,  half_diagonal/2.0);
    icon->setRotation(45);
		icon->setPos(padding + half_diagonal/4,padding + half_diagonal/4);

  }

  
  icon->setZValue(2.0);
  icon->setPen(QPen(palette.text(), 2));

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
	const auto padding = Config::PersonItemConfig().icon_padding;
  opt.setAlignment(Config::PersonItemConfig().text_alignment);

  text = new QGraphicsTextItem(this);
  qDebug() << getFormattedName();
  text->setHtml(getFormattedName());
  text->setTextWidth(Config::PersonItemConfig().text_width);
  // text->moveBy(-text->boundingRect().width() / 2, (1 + ICON_PADDING) * icon_size / 2);
	text->setPos((icon_size + 2*padding -  text->textWidth())/2.0, icon_size+2*padding);
  text->document()->setDefaultTextOption(opt);
}


void SimplePersonItem::addFlags(){

	const bool is_secondary = (bool)(rendererFlags() & RENDERER_IS_SECONDARY);
	const bool is_descendant = (bool)(rendererFlags() & RENDERER_IS_DESCENDANT);
	const bool is_ancestor = (bool)(rendererFlags() & RENDERER_IS_ANCESTOR);
	const bool has_descendants = (bool)(rendererFlags() & RENDERER_HAS_DESCENDANTS);
	const bool has_ancestors = (bool)(rendererFlags() & RENDERER_HAS_ANCESTORS);

	SPDLOG_DEBUG("flags for id {} : {:b}", id, rendererFlags() );

  const auto icon_size = Config::PersonItemConfig().icon_size;
	const auto padding = Config::PersonItemConfig().icon_padding;

	delete flagItem;

	if(is_secondary && is_ancestor && has_descendants){
		flagItem = new QGraphicsTextItem(this);
		flagItem->setHtml(
				QString("<div style='background:rgba(255, 255, 255, 0%);'>...</div>"));
		
		QFontMetrics fm {flagItem->font()};
		flagItem->setPos((icon_size + 2*padding - flagItem->boundingRect().width())/2.0, icon_size+padding);
		// flagItem->moveBy(0, icon_size/2);
	}

	if(is_secondary && is_descendant && has_ancestors){
			flagItem = new QGraphicsTextItem(this);
			flagItem->setHtml(
					QString("<div style='background:rgba(255, 255, 255, 0%);'>...</div>"));
		QFontMetrics fm {flagItem->font()};
		flagItem->setPos((icon_size+2*padding - flagItem->boundingRect().width())/2.0, -0.75*fm.height());
	}
}

void SimplePersonItem::setPerson(id_t id_, const Person &person) {
  id = id_;
  this->person_data = person;
  addIcon();
  addName();
	addFlags();
}

void SimplePersonItem::toggleSelected(bool is_selected) {
  if (is_selected) {
    icon->setPen(QPen(palette.accent(), 3));
  } else {
    icon->setPen(QPen(palette.text(), 2));
  }
}

renderer_flags_t SimplePersonItem::rendererFlags() const{
	return m_rendererData;
}

void SimplePersonItem::setRendererFlags(renderer_flags_t flags){
	m_rendererData = flags;
	addFlags();
}

id_t SimplePersonItem::getId() const {
  return id;    
}



	void SimplePersonItem::recolor(const QPalette& palette) {
		this->palette = palette;
		TEXT_BACKGROUND_COLOR = palette.base().color();
    TEXT_STYLESHEET = "background-color: " + TEXT_BACKGROUND_COLOR.name() + ';' +
			                 "color:" + palette.text().color().name() + ';';
		qDeleteAll(this->children());
		addIcon();
		addName();
		addFlags();
	}
