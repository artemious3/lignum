#include "mftbwindow.h"
#include "DB.h"
#include "SqlDB.h"
#include "datamodel.h"
#include "family-tree-item.h"
#include "spdlog/spdlog.h"
#include "ui_mftbwindow.h"
#include <qaction.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qforeach.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qmenu.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
#include "person-editor-widget.h"

#include <QFileDialog>
#include <QMessageBox>
#include "family-tree-builder.h"


static Person DefaultInsetedPerson {
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

  // auto db = mftb::SqlDB::getInstance();
  // auto person = db->insertPerson({'M', "Name", "", "Surname"});
  // auto partner = db->addPartner({'F', "Name", "", "Surname"}, person);
  // FamilyTreeBuilder fb(family_tree, db);
  // fb.build_tree_from(1);
  // family_tree->render();
  

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


  QMenu* file_menu = new QMenu("File", ui->menuBar);

  QAction* save_action = new QAction("Save", this);
  connect(save_action ,&QAction::triggered,
		  this, &MFTBWindow::save_action);
  file_menu->addAction(save_action);

  QAction* open_action = new QAction("Open", this);
  connect(open_action ,&QAction::triggered,
		  this, &MFTBWindow::load_action);
  file_menu->addAction(open_action);
  ui->menuBar->addMenu(file_menu);
}


bool MFTBWindow::save_action(){
	auto save_name = QFileDialog::getSaveFileName(this, tr("Save"), 
			                 qApp->applicationDirPath(), tr("Lignum database (*.lgn)"));
	if(!save_name.isEmpty()){
		mftb::SqlDB::getInstance()->Save(save_name);
		db_changed = false;
		return true;
	}
	return false;

}


bool MFTBWindow::load_action(){

	if(db_changed){
		bool do_save;
		auto answer = QMessageBox::question(this, tr("Unsaved changes"),
				tr("Do you wish to save current family tree?"), 
				QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		switch(answer){
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Yes:{
				if(!save_action()){
					return false;
				}
			}
			break;
			case QMessageBox::No:
				break;
		}
		
	}

	auto load_name = QFileDialog::getOpenFileName(this, 
			tr("Load"), qApp->applicationDirPath(), tr("Lignum database (*.lgn)"));

	if(!load_name.isEmpty()){
		mftb::SqlDB::getInstance()->Load(load_name);
		family_tree->clear();

		FamilyTreeBuilder fb(family_tree, mftb::SqlDB::getInstance());
		// FIXME : id 1 can be removed from DB 
		fb.build_tree_from(1);
		family_tree->render();

		return true;
	}

	return false;
}


void MFTBWindow::person_changed(id_t id){
	auto db = mftb::SqlDB::getInstance();
	family_tree->getPerson(id)->setPerson(id, db->getPersonById(id).value());
}

void MFTBWindow::show_selected_person(id_t id){
  mftb::DB* db = mftb::SqlDB::getInstance();
  auto person_data = db->getPersonById(id);

  if(person_data.has_value()){
    ui->statusBar->showMessage(QString("%1 selected").arg(person_data->fullName()));
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
	  treeManager->addPartner(DefaultInsetedPerson, selected_id.id);
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

    treeManager->addChild(DefaultInsetedPerson, selected_id.id, second_parent);
  }
}

void MFTBWindow::add_parent_action() {
  mftb::DB* db = mftb::SqlDB::getInstance();

  auto selected_id = family_tree->getSelectedItemId();

  if(selected_id.id != 0){
	  treeManager->addParent(DefaultInsetedPerson, selected_id.id);
  }

}
