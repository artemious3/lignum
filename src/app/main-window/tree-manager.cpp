#include "tree-manager.h"
#include "DB.h"
#include "SqlDB.h"
#include "abstract-family-connector.h"
#include "datamodel.h"

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

	family_tree_item->render();
}


void TreeManager::addParent(const Person &person, id_t child){

	// -- add to DB --
	mftb::DB* db =  mftb::SqlDB::getInstance();
	id_t couple_id;
	id_t parent_id = db->addParent(child, person, &couple_id);

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
	} else {
		// inserted parent is 2nd parent
                family->setParent2(parent_item);
        }

	family_tree_item->render();
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
		// no family => no children were specified 
		family_tree_item->addFamily(couple_id, Couple{partner1, partner2}, {});
	} else {
		family->setParent2(partner2_item);
	}
	

        family_tree_item->render();
}


