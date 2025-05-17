#include "lignum-window.h"
#include "FamilyTreeModel.h"
#include "FamilyTreeSqlModel.h"
#include "entities.h"
#include "family-tree-view.h"
#include "qcontainerfwd.h"
#include "qinputdialog.h"
#include "spdlog/spdlog.h"
#include "ui_lignum-window.h"
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
#include "person-view.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <ranges>

#include "Config.h"
#include "renderer.h"
#include "GedcomLoader.h"

static const QList<int> SplitterWidgetsRelativeSizes = {1000,1};


LignumWindow::LignumWindow() : ui(new Ui::LignumWindow) {
  ui->setupUi(this);


  initialize_actions();

  family_tree = new FamilyTreeView();
  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(family_tree);
  ui->familyTreeView->setScene(scene);

  treeManager = std::make_unique<TreeManager>(family_tree);

  treeManager->buildDefault();

  ui->splitter->setSizes(SplitterWidgetsRelativeSizes);

  connect(family_tree, &FamilyTreeView::personSelected, 
          this, &LignumWindow::show_selected_person);

  connect(ui->personEditor, &PersonView::personChanged,
		  this, &LignumWindow::person_changed);

}



void LignumWindow::on_actionAddFather_triggered(){
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
void LignumWindow::on_actionAddMother_triggered(){
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


void LignumWindow::initialize_actions(){
	auto * toolbar = ui->toolBar;
	toolbar->addAction(ui->actionAddSon);
	toolbar->addAction(ui->actionAddDaughter);
	toolbar->addAction(ui->actionAddMother);
	toolbar->addAction(ui->actionAddFather);
	toolbar->addAction(ui->actionRemove);
}


bool LignumWindow::on_actionSave_triggered(){
	auto save_name = QFileDialog::getSaveFileName(this, tr("Save"), 
			                 qApp->applicationDirPath(), tr("Lignum database (*.lgn)"));
	if(!save_name.isEmpty()){
		mftb::FamilyTreeSqlModel::getInstance()->Save(save_name);
		db_changed = false;
		return true;
	}
	return false;

}





bool LignumWindow::on_actionOpen_triggered(){

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


void LignumWindow::person_changed(id_t id){
	auto * db = mftb::FamilyTreeSqlModel::getInstance();
	family_tree->getPerson(id)->setPerson(id, db->getPersonById(id).value());
	family_tree->reselectItem();
}


void LignumWindow::update_actions_availability(id_t target_person){
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

void LignumWindow::show_selected_person(id_t id){
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
void LignumWindow::on_actionAddPartner_triggered() {

  auto selected_id = family_tree->getSelectedItemId();
  if(selected_id.id != 0){
    treeManager->addPartner(DefaultInsertedPerson, selected_id.id);
    treeManager->render();
  }
}

void LignumWindow::on_actionAddSon_triggered(){
	add_child_action(DefaultInsertedMale);
}

void LignumWindow::on_actionAddDaughter_triggered(){
	add_child_action(DefaultInsertedFemale);
}

void LignumWindow::add_child_action(const Person& person) {
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

void LignumWindow::add_parent(const Person& person) {

  auto selected_id = family_tree->getSelectedItemId();

  if (selected_id.id != 0) {
    treeManager->addParent(DefaultInsertedPerson, selected_id.id);
    treeManager->render();
  }
}


void LignumWindow::on_actionRemove_triggered(){

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

void LignumWindow::on_actionSwitchGender_triggered(){	
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
  

void LignumWindow::on_actionLoadGedcom_triggered(){
	auto* db = mftb::FamilyTreeSqlModel::getInstance();
	auto path = QFileDialog::getOpenFileName(this, "GEDCOM");
	std::ifstream ifs{path.toStdString()};

	db->dropData();
	GedcomLoader::load(db, ifs);



	auto ids = db->getPeopleIds();
	QStringList ids_strings;
	for(id_t id : ids) {
		ids_strings.push_back(QString::number(id) + " " +  db->getPersonById(id)->fullName());
	}
	QString root_person_string  = QInputDialog::getItem(this, "Select root person", "Select root person", ids_strings);
	id_t id = root_person_string.split(" ")[0].toLong();
	// id_t id = root_person_string.toLong();

	auto couple_id = db->getPersonCouplesId(id);

	// TODO : make it adequate
	if(couple_id.empty()) {
		QMessageBox::warning(this, "No couples", "No couples found for this person");
		return;
	}
	
	db->setRenderData(RenderData{
			.center_couple = couple_id.front()
	});
	treeManager->buildFromScratch();
	treeManager->render();
}

