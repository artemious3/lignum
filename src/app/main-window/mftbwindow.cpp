#include "mftbwindow.h"
#include "DB.h"
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

  initialize_actions();


  FamilyTreeItem* ftree = new FamilyTreeItem();
  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(ftree);
  ui->familyTreeView->setScene(scene);
}


void MFTBWindow::initialize_actions(){
  QAction *mouse_act = new QAction(QIcon(":/icons/cursor-svgrepo-com.svg"), "Mouse", this);
  connect(mouse_act, &QAction::triggered, ui->familyTreeView, &mftb::FamilyTreeView::toggle_mouse_mode);
  ui->toolBar->addAction(mouse_act);


  QAction *hand_act = new QAction(QIcon(":/icons/hand-svgrepo-com.svg"), "Hand", this);
   connect(hand_act, &QAction::triggered, ui->familyTreeView, &mftb::FamilyTreeView::toggle_hand_mode);
  ui->toolBar->addAction(hand_act);

  ui->toolBar->addSeparator();

  QAction *add_partner = new QAction("Add partner", this);
  ui->toolBar->addAction(add_partner);
  QAction *add_child = new QAction("Add child", this);
  ui->toolBar->addAction(add_child);

}