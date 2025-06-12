#pragma once
#include <QApplication>
#include <QGraphicsItem>
#include <QPen>
#include "abstract-connector.h"
#include "abstract-person-item.h"
#include "abstract-family-connector.h"
#include "qpalette.h"

class SimplePersonItem;
class PeopleConnectorItem;



// TODO: replace raw pointers with shared;

class SimpleFamilyConnector : public AbstractFamilyConnector {

public:
  SimpleFamilyConnector(AbstractPersonItem *parent1, AbstractPersonItem *parent2 = nullptr,
                  QGraphicsObject *parent = nullptr);

  SimpleFamilyConnector(const SimpleFamilyConnector&) = delete;
  SimpleFamilyConnector operator=(SimpleFamilyConnector) = delete;

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;


  void renderConnections() override;
  void renderCoupleConnection();
  void renderParentChildConnections();

  bool hasParent(const AbstractPersonItem* item) const override;
  bool hasChild(const AbstractPersonItem* item) const override;

  std::pair<const AbstractPersonItem*, const AbstractPersonItem*> getParents() const override;
  const QList<const AbstractPersonItem*>& getChildren() const override;

  void addChild(const AbstractPersonItem* child) override;
  void removeChild(const AbstractPersonItem* child) override; 
  void removeParent(const AbstractPersonItem* parent) override;

  void setParent1(const AbstractPersonItem* p) override;
  void setParent2(const AbstractPersonItem* p) override;

  bool setEmptyParent(AbstractPersonItem* person);
  bool isSingleParent() const;
  bool isEmpty();

  void setChildrenConnectionPointX(qreal x) override;
  void setDefaultChildrenConnectionPointX() override;
  void setFamilyLineYBias(qreal y) override;
	void recolor(const QPalette& palette) override;

private:
	QPalette palette;
  QPen pen;

  const AbstractPersonItem *parent1 = nullptr, *parent2 = nullptr;
  QList<const AbstractPersonItem*> children;

  AbstractConnector *parents_connector = nullptr;
  QList<const AbstractConnector*> children_connectors;

  std::optional<qreal> family_connection_point_x;
  std::optional<qreal> family_line_y_bias;



  static const inline qreal INITIAL_FAMILY_LINE_BIAS = 20;

};
