#include "tree-manager.h"
#include "DB.h"
#include "renderer.h"
#include "SqlDB.h"
#include "abstract-family-connector.h"
#include "datamodel.h"
#include "family-tree-builder.h"
#include "spdlog/spdlog.h"


TreeManager::TreeManager(FamilyTreeItem *tree) : family_tree_item(tree) {}

void TreeManager::addChild(const Person &person, id_t parent1, id_t parent2){
	// -- add to DB --
	
	mftb::DB* db =  mftb::SqlDB::getInstance();
	id_t parents_couple_id;
	id_t child_id = db->addChild(person, parent1, parent2, &parents_couple_id);

	// -- add to FamilyTreeItem -- 
	auto*  child_item = family_tree_item->addPerson(child_id, person);
	auto*  family = family_tree_item->getFamily(parents_couple_id);
        if (family == nullptr) {
          family = family_tree_item->addFamily(parents_couple_id,
                                               Couple{parent1, parent2}, {});
        }
        family->addChild(child_item);

}


void TreeManager::addParent(const Person &person, id_t child){

	// -- add to DB --
	mftb::DB* db =  mftb::SqlDB::getInstance();
	id_t couple_id;
	id_t parent_id = db->addParent(child, person, &couple_id);
	if(parent_id == 0){
		return;
	}

	// -- add to FamilyTreeItem -- 
        auto *parent_item =
                    family_tree_item->addPerson(parent_id, person);
	auto * child_item = family_tree_item->getPerson(child);
	auto * family = family_tree_item->getFamily(couple_id);
	if(family == nullptr){
		// we can assume that this node had no siblings and 
		// other parents specified  
	        auto parents_couple = Couple{parent_id, 0}; 
		family = family_tree_item->addFamily(couple_id, parents_couple, {});
		family->addChild(child_item);	
	} else if(family->getParents().second == nullptr) {
		// inserted parent is 2nd parent
                family->setParent2(parent_item);
        } 
	// else both parents are already specified

}

void TreeManager::addPartner(const Person& person, id_t partner1){
	// -- add to DB --	
	mftb::DB* db =  mftb::SqlDB::getInstance();
	id_t couple_id;
	id_t partner2 = db->addPartner(person, partner1, &couple_id);

	// -- add to FamilyTreeItem --
	auto* partner2_item = family_tree_item->addPerson(partner2, person);
	auto* family = family_tree_item->getFamily(couple_id);
	if(family == nullptr){ 
		family_tree_item->addFamily(couple_id, Couple{partner1, partner2}, {});
	} else {
		family->setParent2(partner2_item);
	}
	

}


bool TreeManager::removePerson(id_t person_id){
	mftb::DB* db = mftb::SqlDB::getInstance();

	if(!db->isRemovable(person_id)){
		SPDLOG_DEBUG("PERSON IS NOT REMOVABLE");
		return false;
	}
	
	// -- remove from FamilyTreeItem --
	// if person has no partners, only the person node is removed  
	// if he is one of two partners, he is removed from couple
	// else the whole couple is removed
	auto * removed_person_item = family_tree_item->getPerson(person_id);
	auto couples = db->getPersonCouplesId(person_id);
	if(!couples.empty()){
		auto couple_id = couples.front();
		SPDLOG_DEBUG("PERSON IS MEMBER OF COUPLE {}", couple_id);
		auto * family = family_tree_item->getFamily(couple_id);
		auto  parents = family->getParents();


		if(parents.first != nullptr && parents.second != nullptr){
			SPDLOG_DEBUG("REMOVED PARENT {} FROM COUPLE {}", person_id, couple_id);
                        family->removeParent(removed_person_item);
		} else {
			SPDLOG_DEBUG("REMOVED COUPLE {}", couple_id);
			family_tree_item->removeFamily(couple_id);
                }

	}

	auto parents_couple = db->getParentsCoupleId(person_id).value();
	if(parents_couple != 0){
		auto * family = family_tree_item->getFamily(parents_couple);
		family->removeChild(removed_person_item);
	}

	family_tree_item->removePerson(person_id);


	//  -- remove from DB --
	db->removePerson(person_id);
	family_tree_item->clear_selection();
	family_tree_item->renderConnections();

	return true;


}




void TreeManager::render(){
  mftb::DB* db = mftb::SqlDB::getInstance();

  Renderer renderer(db, family_tree_item);
  renderer.balance_from_couple_id(db->getRenderData().center_couple);

  family_tree_item->reselectItem();
  family_tree_item->renderConnections();

}

void TreeManager::buildFromScratch(){
	auto * db = mftb::SqlDB::getInstance();	
	FamilyTreeBuilder fb(family_tree_item, db);


	family_tree_item->clear();

	auto one_person_vec = db->getPeopleIds(1);
	if(!one_person_vec.empty()){
		fb.build_tree_from(one_person_vec[0]);
	} else {
		spdlog::error("DB IS EMPTY!");
	}
}
