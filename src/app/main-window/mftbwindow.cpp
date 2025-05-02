#include "mftbwindow.h"
#include "DB.h"
#include "SqlDB.h"
#include "datamodel.h"
#include "family-tree-item.h"
#include "spdlog/spdlog.h"
#include "ui_mftbwindow.h"
#include <climits>
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

#include "Config.h"
#include "renderer.h"

static const QList<int> SplitterWidgetsRelativeSizes = {1000,1};

static Person DefaultInsertedPerson {
	.gender = 'U',
	.first_name = "Name", 
	.middle_name = "",
	.last_name = "Surname",
	.birth_date = QDate(),
	.death_date = QDate()
};

static Person DefaultInsertedMale {
	.gender = 'M',
	.first_name = "Name", 
	.middle_name = "",
	.last_name = "Surname",
	.birth_date = QDate(),
	.death_date = QDate()
};

static Person DefaultInsertedFemale {
	.gender = 'F',
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

  treeManager = std::make_unique<TreeManager>(family_tree);

  create_default_tree();

  ui->splitter->setSizes(SplitterWidgetsRelativeSizes);

  connect(family_tree, &FamilyTreeItem::personSelected, 
          this, &MFTBWindow::show_selected_person);

  connect(ui->personEditor, &PersonEditorWidget::personChanged,
		  this, &MFTBWindow::person_changed);

}


void MFTBWindow::create_default_tree(){
  auto * db = mftb::FamilyTreeSqlModel::getInstance();
  auto p1 = db->insertPerson(DefaultInsertedMale);
  db->addPartner(DefaultInsertedFemale, p1);
  treeManager->buildFromScratch();
  treeManager->render();
}

void MFTBWindow::on_actionAddFather_triggered(){
	mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();
	auto selected_id = family_tree->getSelectedItemId().id;
	if(selected_id != 0){
		auto parents = db->getPersonParentsById(selected_id);
		if((parents.first != 0 && db->getPersonById(parents.first).value().gender == 'M' )||
		   (parents.second != 0 && db->getPersonById(parents.second).value().gender == 'M')){
			return;
		}
                treeManager->addParent(DefaultInsertedMale, selected_id);
                treeManager->render();
        }
}
void MFTBWindow::on_actionAddMother_triggered(){
	mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();
	auto selected_id = family_tree->getSelectedItemId().id;
	if(selected_id != 0){
		auto parents = db->getPersonParentsById(selected_id);
		if((parents.first != 0 && db->getPersonById(parents.first).value().gender == 'F')||
		   (parents.second != 0 && db->getPersonById(parents.second).value().gender == 'F')){
			return;
		}
	}
        treeManager->addParent(DefaultInsertedFemale, selected_id);
        treeManager->render();
}


void MFTBWindow::initialize_actions(){

auto * toolbar = ui->toolBar;
toolbar->addAction(ui->actionAddSon);
toolbar->addAction(ui->actionAddDaughter);
toolbar->addAction(ui->actionAddMother);
toolbar->addAction(ui->actionAddFather);
toolbar->addAction(ui->actionRemove);

  // auto Keys = Config::KeysConfig();
  //
  // QAction *mouse_act = new QAction(QIcon(":/icons/cursor-svgrepo-com.svg"), "Mouse", this);
  // connect(mouse_act, &QAction::triggered, ui->familyTreeView, &mftb::FamilyTreeView::toggle_mouse_mode);
  // ui->toolBar->addAction(mouse_act);
  //
  // QAction *hand_act = new QAction(QIcon(":/icons/hand-svgrepo-com.svg"), "Hand", this);
  // connect(hand_act, &QAction::triggered,
  //         ui->familyTreeView, &mftb::FamilyTreeView::toggle_hand_mode);
  // ui->toolBar->addAction(hand_act);
  //
  // ui->toolBar->addSeparator();
  //
  //
  // QMenu* edit_menu = new QMenu("Edit", ui->menuBar);
  //
  // QAction *add_partner = new QAction("Add partner", this);
  // add_partner->setShortcut(QKeySequence(Keys.ADD_PARTNER));
  // connect(add_partner, &QAction::triggered,
  //       this, &MFTBWindow::add_partner_action);
  // ui->toolBar->addAction(add_partner);
  // edit_menu->addAction(add_partner);
  //
  //
  // QAction *add_child = new QAction("Add child", this);
  // add_child->setShortcut(QKeySequence(Keys.ADD_CHILD));
  // connect(add_child, &QAction::triggered,
  //       this, &MFTBWindow::add_child_action);
  // ui->toolBar->addAction(add_child);
  // edit_menu->addAction(add_child);
  //
  //
  // QAction *add_parent = new QAction("Add parent", this);
  // add_parent->setShortcut(QKeySequence(Keys.ADD_PARENT));
  // connect(add_parent, &QAction::triggered,
  //       this, &MFTBWindow::add_parent_action);
  // ui->toolBar->addAction(add_parent);
  // edit_menu->addAction(add_parent);
  // 
  //
  // QAction* remove_person = new QAction("Remove", this);
  // remove_person->setShortcut(QKeySequence(Keys.REMOVE));
  // connect(remove_person, &QAction::triggered, 
		//   this, &MFTBWindow::remove_person_action);
  // ui->toolBar->addAction(remove_person);
  // edit_menu->addAction(remove_person);
  //
  //
  // QMenu* file_menu = new QMenu("File", ui->menuBar);
  //
  // QAction* save_action = new QAction("Save", this);
  // save_action->setShortcut(QKeySequence(Keys.SAVE));
  // connect(save_action ,&QAction::triggered,
		//   this, &MFTBWindow::save_action);
  // file_menu->addAction(save_action);
  //
  // QAction* open_action = new QAction("Open", this);
  // open_action->setShortcut(QKeySequence(Keys.OPEN));
  // connect(open_action ,&QAction::triggered,
		//   this, &MFTBWindow::load_action);
  // file_menu->addAction(open_action);
  // ui->menuBar->addMenu(file_menu);
  // ui->menuBar->addMenu(edit_menu);
}


bool MFTBWindow::on_actionSave_triggered(){
	auto save_name = QFileDialog::getSaveFileName(this, tr("Save"), 
			                 qApp->applicationDirPath(), tr("Lignum database (*.lgn)"));
	if(!save_name.isEmpty()){
		mftb::FamilyTreeSqlModel::getInstance()->Save(save_name);
		db_changed = false;
		return true;
	}
	return false;

}





bool MFTBWindow::on_actionOpen_triggered(){

	if(db_changed){
		auto answer = QMessageBox::question(this, tr("Unsaved changes"),
				tr("Do you wish to save current family tree?"), 
				QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);

		switch(answer){
			case QMessageBox::Cancel:
				return false;
			case QMessageBox::Yes:{
				if(!on_actionSave_triggered()){
					return false;
				}
			}
			break;
			default:
				break;
		}
		
	}

	auto load_name = QFileDialog::getOpenFileName(this, 
			tr("Load"), qApp->applicationDirPath(), tr("Lignum database (*.lgn)"));

	if(!load_name.isEmpty()){
		auto * db = mftb::FamilyTreeSqlModel::getInstance();
		db->Load(load_name);
		treeManager->buildFromScratch();
		treeManager->render();
		return true;
	}

	return false;
}


void MFTBWindow::person_changed(id_t id){
	auto * db = mftb::FamilyTreeSqlModel::getInstance();
	family_tree->getPerson(id)->setPerson(id, db->getPersonById(id).value());
	family_tree->reselectItem();
}


void MFTBWindow::update_actions_availability(id_t target_person){
  auto renderer_data = family_tree->getPerson(target_person)->rendererFlags();
  bool is_secondary = (bool)(renderer_data & RENDERER_IS_SECONDARY);
  bool is_descendant = (bool)(renderer_data & RENDERER_IS_DESCENDANT);
  bool is_ancestor = (bool)(renderer_data & RENDERER_IS_ANCESTOR);

  ui->actionAddPartner->setDisabled(is_secondary);
  ui->actionAddMother->setDisabled(is_secondary && is_descendant);
  ui->actionAddFather->setDisabled(is_secondary && is_descendant);
  ui->actionAddSon->setDisabled(is_secondary && is_ancestor);
  ui->actionAddDaughter->setDisabled(is_secondary && is_ancestor);
}

void MFTBWindow::show_selected_person(id_t id){
  mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();
  auto person_data = db->getPersonById(id);

  if(person_data.has_value()){
    ui->statusBar->showMessage(
        QString("%1 selected").arg(person_data->fullName()));
    if(ui->personEditor->ConnectedPerson() != 0){
	    ui->personEditor->ApplyChanges();
    }
    ui->personEditor->ConnectToPerson(id);
    update_actions_availability(id);
    SPDLOG_DEBUG("ID {} SELECTED", id);


  } else {
    spdlog::error("Selected id is not in DB");
    ui->statusBar->clearMessage();
  }

}
void MFTBWindow::on_actionAddPartner_triggered() {

  auto selected_id = family_tree->getSelectedItemId();
  if(selected_id.id != 0){
    treeManager->addPartner(DefaultInsertedPerson, selected_id.id);
    treeManager->render();
  }
}

void MFTBWindow::on_actionAddSon_triggered(){
	add_child_action(DefaultInsertedMale);
}

void MFTBWindow::on_actionAddDaughter_triggered(){
	add_child_action(DefaultInsertedFemale);
}

void MFTBWindow::add_child_action(const Person& person) {
  mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();
  auto selected_id = family_tree->getSelectedItemId();


  if(selected_id.id != 0){
    auto partners = db->getPersonPartners(selected_id.id);
    id_t second_parent = 0;
    if(!partners.empty()){
      second_parent = partners.front();
    }

    treeManager->addChild(person, selected_id.id, second_parent);
    treeManager->render();
  }
}

void MFTBWindow::add_parent(const Person& person) {

  auto selected_id = family_tree->getSelectedItemId();

  if (selected_id.id != 0) {
    treeManager->addParent(DefaultInsertedPerson, selected_id.id);
    treeManager->render();
  }
}


void MFTBWindow::on_actionRemove_triggered(){

  auto selected_id = family_tree->getSelectedItemId().id;

  if(selected_id != 0){
	  RemoveStatus status = treeManager->removePerson(selected_id);
	  switch(status){

		  case RemoveStatus::NotLeaf:
                    QMessageBox::information(
                        this, "Removing",
			"Can not remove: the tree will be split up.");
		    return;
                  case RemoveStatus::AttemptToRemoveCenterCouple:
                    QMessageBox::information(
                        this, "Removing",
                        "Can not remove: rendering is centered at selected person.");
                    return;

                  default:
                    break;
          }
	  ui->personEditor->ConnectToPerson(0);
  }

}

void MFTBWindow::on_actionSwitchGender_triggered(){	
  mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();
  auto selected_id = family_tree->getSelectedItemId().id;
  if(selected_id != 0){
	  auto person = db->getPersonById(selected_id).value();
	  if(person.gender == 'M'){
		  person.gender = 'F';
	  } else {
		  person.gender = 'M';
	  }
	  db->updatePerson(person, selected_id);
	  family_tree->getPerson(selected_id)
		  ->setPerson(selected_id, person);
	  ui->personEditor->ConnectToPerson(selected_id);

  }

}
