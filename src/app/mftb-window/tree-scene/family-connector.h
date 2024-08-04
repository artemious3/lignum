#pragma once
#include <QApplication>
#include <QGraphicsItem>
#include <QPalette>
#include <QPen>
#include <cstdint>
#include <qapplication.h>
#include <qlist.h>

class PersonItem;
class PeopleConnectorItem;

// TODO: replace raw pointers with shared;

class FamilyConnector : public QGraphicsItem {

public:
  FamilyConnector(PersonItem *parent1, PersonItem *parent2 = nullptr,
                  QGraphicsItem *parent = nullptr);

  FamilyConnector(const FamilyConnector&) = delete;
  FamilyConnector operator=(FamilyConnector) = delete;

  QRectF boundingRect() const override;
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                     QWidget *widget = nullptr) override;


  void renderConnections();
  void renderCoupleConnection();
  void renderParentChildConnections();

  bool hasParent(uint32_t id);
  bool hasChild(uint32_t id);

  std::pair<PersonItem*, PersonItem*> getParents();
  const QSet<PersonItem*>& getChildren();

  void addChild(PersonItem* child);

  bool setEmptyParent(PersonItem* person);
  bool isSingleParent() const;
  bool isEmpty();

private:
  PersonItem *parent1, *parent2;
  QSet<PersonItem *> children;

  PeopleConnectorItem *parents_connector = nullptr;
  QList<PeopleConnectorItem *> children_connectors;
  qreal connection_point_x_bias = 0;

  const QPen pen;
  static constexpr qreal BIAS_PER_COUPLE = 20;
};