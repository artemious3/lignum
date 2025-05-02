#pragma once
#include <qgraphicsview.h>
#include <qwidget.h>

namespace mftb {

class FamilyTreeGraphicsView : public QGraphicsView {
  Q_OBJECT 
public:
  FamilyTreeGraphicsView(QWidget *parent = nullptr);

protected:
  void wheelEvent(QWheelEvent *event) override;

private:
  //TODO: should be somehow achieved from system settings
  static constexpr qreal WheelSensitivity = 0.002;
  static constexpr qreal MaxZoom = 5.0;
  static constexpr qreal MinZoom = 1.0/2;

  qreal relative_scale = 1.0;
  bool isAtMaxZoom() const;
  bool isAtMinZoom() const;
};

}


