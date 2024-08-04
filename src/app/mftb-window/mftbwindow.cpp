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

  using namespace mftb;

  DB& db = DB::getInstance();

  uint32_t dad_id = db._createPerson(true, "John", "Doe");
  uint32_t mum_id = db._createPerson(false, "Anna", "Doe");
  uint32_t son_id = db._createPerson(true, "Lois", "Doe");

  uint32_t rel1 = db.createRelationship(dad_id, son_id, Relationship::Type::ParentChild);
   uint32_t rel2 = db.createRelationship(mum_id, son_id, Relationship::Type::ParentChild);

  auto dad = db.getPersonById(dad_id);
  auto mum = db.getPersonById(mum_id);
  auto son = db.getPersonById(son_id);

  FamilyTreeItem *ftree = new FamilyTreeItem(nullptr);

  ftree->addPerson(dad.lock());
  ftree->addPerson(mum.lock());
  ftree->addPerson(son.lock());

  auto relatipships = db.getRelationships();
  foreach(auto rel, relatipships){
    ftree->addRelationship(rel);
  }

  ftree->renderFamilies();


  QGraphicsScene* scene = new QGraphicsScene(ui->familyTreeView);
  scene->addItem(ftree);
  ui->familyTreeView->setScene(scene);


  QAction *mouse_act = new QAction(QIcon(":/icons/cursor-svgrepo-com.svg"), "Mouse", this);
  QAction *hand_act = new QAction(QIcon(":/icons/hand-svgrepo-com.svg"), "Hand", this);
  ui->toolBar->addAction(mouse_act);
  ui->toolBar->addAction(hand_act);
}