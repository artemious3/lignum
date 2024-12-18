#include "mftbwindow.h"
#include "DB.h"
#include "SqlDB.h"
#include "datamodel.h"
#include "family-tree-item.h"
#include "spdlog/spdlog.h"
#include "ui_mftbwindow.h"
#include <qaction.h>
#include <qforeach.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
#include "person-editor-widget.h"
#include <QSizeGrip>
#include "family-tree-builder.h"

#include <QMessageBox>


static Person DefaultInsertedPerson {
	.gender = 'U',
	.first_name = "Name", 
	.middle_name = "",
	.last_name = "Surname",
	.birth_date = QDate(),
	.death_date = QDate()
};


MFTBWindow::MFTBWindow() : ui(new Ui::MFTBWindow) {
  ui->setupUi(this);


  initialize_actions();

  family_tree = new FamilyTreeItem();
  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(family_tree);
  ui->familyTreeView->setScene(scene);



  auto * db = mftb::SqlDB::getInstance();
  auto p1 = db->insertPerson(DefaultInsertedPerson);
  auto p2 = db->addPartner(DefaultInsertedPerson, p1);
  FamilyTreeBuilder fb{family_tree, db};
  fb.build_tree_from(1);
  family_tree->render();

  treeManager = std::make_unique<TreeManager>(family_tree);

  connect(family_tree, &FamilyTreeItem::personSelected, 
          this, &MFTBWindow::show_selected_person);

  connect(ui->personEditor, &PersonEditorWidget::personChanged,
		  this, &MFTBWindow::person_changed);

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
  connect(add_child, &QAction::triggered,
        this, &MFTBWindow::add_child_action);
  ui->toolBar->addAction(add_child);


  QAction *add_parent = new QAction("Add parent", this);
  connect(add_parent, &QAction::triggered,
        this, &MFTBWindow::add_parent_action);
  ui->toolBar->addAction(add_parent);
  

  QAction* remove_person = new QAction("Remove", this);
  connect(remove_person, &QAction::triggered, 
		  this, &MFTBWindow::remove_person_action);
  ui->toolBar->addAction(remove_person);
}


void MFTBWindow::person_changed(id_t id){
	auto db = mftb::SqlDB::getInstance();
	family_tree->getPerson(id)->setPerson(id, db->getPersonById(id).value());
}

void MFTBWindow::show_selected_person(id_t id){
  mftb::DB* db = mftb::SqlDB::getInstance();
  auto person_data = db->getPersonById(id);

  if(person_data.has_value()){
    ui->statusBar->showMessage(
        QString("%1 selected").arg(person_data->fullName()));
    if(ui->personEditor->ConnectedPerson() != 0){
	    ui->personEditor->ApplyChanges();
    }
    ui->personEditor->ConnectToPerson(id);
  } else {
    spdlog::error("Selected id is not in DB");
    ui->statusBar->clearMessage();
  }



}
void MFTBWindow::add_partner_action() {

  mftb::DB* db = mftb::SqlDB::getInstance();

  auto selected_id = family_tree->getSelectedItemId();
  if(selected_id.id != 0){
	  treeManager->addPartner(DefaultInsertedPerson, selected_id.id);
  }
}

void MFTBWindow::add_child_action() {
  mftb::DB* db = mftb::SqlDB::getInstance();
  auto selected_id = family_tree->getSelectedItemId();


  if(selected_id.id != 0){
    auto partners = db->getPersonPartners(selected_id.id);
    id_t second_parent = 0;
    if(!partners.empty()){
      second_parent = partners.front();
    }

    treeManager->addChild(DefaultInsertedPerson, selected_id.id, second_parent);
  }
}

void MFTBWindow::add_parent_action() {
  mftb::DB* db = mftb::SqlDB::getInstance();

  auto selected_id = family_tree->getSelectedItemId();

  if(selected_id.id != 0){
	  treeManager->addParent(DefaultInsertedPerson, selected_id.id);
  }

}


void MFTBWindow::remove_person_action(){

  mftb::DB* db = mftb::SqlDB::getInstance();

  auto selected_id = family_tree->getSelectedItemId().id;

  if(selected_id != 0){
	  bool was_removed = treeManager->removePerson(selected_id);
	  if(!was_removed){
		  QMessageBox::information(this, "Removing",  "Person can not be removed until if it has both descendants and ancestors and has more than 1 partner"); 
		  return;
	  }
	  ui->personEditor->ConnectToPerson(0);


  }

}
