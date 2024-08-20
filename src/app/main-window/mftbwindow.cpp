#include "mftbwindow.h"
#include "DB.h"
#include "Person.h"
#include "family-connector.h"
#include "family-tree-item.h"
#include "ui_mftbwindow.h"
#include <cstdint>
#include <qaction.h>
#include <qforeach.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qnamespace.h>
#include <qsharedpointer.h>

MFTBWindow::MFTBWindow() : ui(new Ui::MFTBWindow) {
  ui->setupUi(this);


  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(ftree);
  ui->familyTreeView->setScene(scene);

  QAction *mouse_act = new QAction(QIcon(":/icons/cursor-svgrepo-com.svg"), "Mouse", this);
  QAction *hand_act = new QAction(QIcon(":/icons/hand-svgrepo-com.svg"), "Hand", this);
  ui->toolBar->addAction(mouse_act);
  ui->toolBar->addAction(hand_act);
}