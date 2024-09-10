#include "mftbwindow.h"
#include "DB.h"
#include "SqlDB.h"
#include "datamodel.h"
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

  family_tree = new FamilyTreeItem();
  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(family_tree);
  ui->familyTreeView->setScene(scene);


  connect(family_tree, &FamilyTreeItem::personSelected, 
          this, &MFTBWindow::show_selected_person);

}


void MFTBWindow::initialize_actions(){
  QAction *mouse_act = new QAction(QIcon(":/icons/cursor-svgrepo-com.svg"), "Mouse", this);
  connect(mouse_act, &QAction::triggered, ui->familyTreeView, &mftb::FamilyTreeView::toggle_mouse_mode);
  ui->toolBar->addAction(mouse_act);

  QAction *hand_act = new QAction(QIcon(":/icons/hand-svgrepo-com.svg"), "Hand", this);
  connect(hand_act, &QAction::triggered,
          ui->familyTreeView, &mftb::FamilyTreeView::toggle_hand_mode);
  ui->toolBar->addAction(hand_act);

  ui->toolBar->addSeparator();

  QAction *add_partner = new QAction("Add partner", this);
  connect(add_partner, &QAction::triggered,
        this, &MFTBWindow::add_partner_action);
  ui->toolBar->addAction(add_partner);


  QAction *add_child = new QAction("Add child", this);
  ui->toolBar->addAction(add_child);
}

void MFTBWindow::show_selected_person(id_t id){

  mftb::DB* db = mftb::SqlDB::getInstance();
  auto person_data = db->getPersonById(id);

  if(person_data.has_value()){
    ui->statusBar->showMessage(QString("%1 selected").arg(person_data->fullName()));
  } else {
    qDebug() << "Given id is not in DB";
    ui->statusBar->clearMessage();
  }

}
void MFTBWindow::add_partner_action() {

  static int _temp_counter = 0;

  mftb::DB* db = mftb::SqlDB::getInstance();

  auto selected_id = family_tree->getSelectedItemId();
  if(selected_id.second != 0){
    db->addPartner({'F', QString::number(_temp_counter)}, selected_id.second);
    family_tree->refresh();
  }
}
