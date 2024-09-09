#pragma once
#include <QApplication>
#include <QGraphicsItem>
#include <QPalette>
#include <QPen>
#include <set>
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

  bool hasParent(PersonItem* item);
  bool hasChild(PersonItem* item);

  std::pair<PersonItem*, PersonItem*> getParents();
  const std::set<PersonItem*>& getChildren();

  void addChild(PersonItem* child);

  bool setEmptyParent(PersonItem* person);
  bool isSingleParent() const;
  bool isEmpty();


  void setFamilyConnectionPointX(qreal x);
  void setFamilyLineYBias(qreal y);

private:
  PersonItem *parent1, *parent2;
  std::set<PersonItem*> children;

  PeopleConnectorItem *parents_connector = nullptr;
  QList<PeopleConnectorItem *> children_connectors;


  std::optional<qreal> family_connection_point_x;
  std::optional<qreal> family_line_y_bias;

  static const inline qreal INITIAL_FAMILY_LINE_BIAS = 20;

  const QPen pen;
};