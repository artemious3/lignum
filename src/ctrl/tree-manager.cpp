#include "tree-manager.h"
#include "FamilyTreeModel.h"
#include "renderer.h"
#include "FamilyTreeSqlModel.h"
#include "abstract-family-connector.h"
#include "entities.h"
#include "family-tree-builder.h"
#include "spdlog/spdlog.h"


TreeManager::TreeManager(FamilyTreeView *tree) : family_tree_item(tree) {}

void TreeManager::addChild(const Person &person, id_t parent1, id_t parent2){
	// -- add to DB --
	
	mftb::FamilyTreeModel* db =  mftb::FamilyTreeSqlModel::getInstance();
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
	mftb::FamilyTreeModel* db =  mftb::FamilyTreeSqlModel::getInstance();
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
	mftb::FamilyTreeModel* db =  mftb::FamilyTreeSqlModel::getInstance();
	id_t couple_id, partner2;


	// TODO: maybe this should be done on DB side
	// this implementation does not look beautiful
	std::vector<id_t> partners = db->getPersonPartners(partner1);
        if(!partners.empty() && partners[0] == 0){
		couple_id = db->getCoupleIdByPersons(partner1, 0).value();

		assert(partners.size() == 1);
		auto children = db->getPersonChildren(partner1);
                // tree should not contain couples (person, 0) without children
                assert(children.size() > 0);
		auto any_child = children[0];

		partner2 = db->addParent(any_child, person);
		
	} else {
		partner2 = db->addPartner(person, partner1, &couple_id);
	}

	// -- add to FamilyTreeItem --
	auto* partner2_item = family_tree_item->addPerson(partner2, person);
	auto* family = family_tree_item->getFamily(couple_id);
	if(family == nullptr){ 
		family_tree_item->addFamily(couple_id, Couple{partner1, partner2}, {});
	} else {
		family->setParent2(partner2_item);
	}
	

}


RemoveStatus TreeManager::removePerson(id_t person_id){
	mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();

	if(!db->isRemovable(person_id)){
		SPDLOG_DEBUG("PERSON IS NOT REMOVABLE");
		return RemoveStatus::NotLeaf;
	}
	
	// -- remove from FamilyTreeItem --
	auto * removed_person_item = family_tree_item->getPerson(person_id);
	auto couples = db->getPersonCouplesId(person_id);
	if(couples.size() == 1){
		auto couple_id = couples.front();
		SPDLOG_DEBUG("PERSON IS MEMBER OF COUPLE {}", couple_id);

		if(couple_id == db->getRenderData().center_couple){
			return RemoveStatus::AttemptToRemoveCenterCouple;
		}

		auto * family = family_tree_item->getFamily(couple_id);
		auto  parents = family->getParents();
		auto children = family->getChildren();
                const auto *partner = parents.second == removed_person_item
                                          ? parents.first
                                          : parents.second;

                // see SqlDB.cpp : bool removePerson()
		if(partner != nullptr && children.size() != 0){
			family->removeParent(removed_person_item);
		} else {
			family_tree_item->removeFamily(couple_id);
			family = nullptr;
		}
	}

	auto parents_couple = db->getParentsCoupleId(person_id).value();
	if(parents_couple != 0){
		auto * family = family_tree_item->getFamily(parents_couple);
		family->removeChild(removed_person_item);

                if (family->getParents().second == nullptr &&
                    family->getChildren().size() == 0) {
                  family_tree_item->removeFamily(parents_couple);
		  family = nullptr;
                }
        }

	family_tree_item->removePerson(person_id);

	//  -- remove from DB --
	db->removePerson(person_id);
	family_tree_item->clear_selection();
	render();

	return RemoveStatus::Ok;
}




void TreeManager::render(){
  mftb::FamilyTreeModel* db = mftb::FamilyTreeSqlModel::getInstance();

  Renderer renderer(db);
  Renderer::Result result = renderer.render(db->getRenderData().center_couple);

  for(const auto& [id, person] : result.persons_placement){
    auto* item = family_tree_item->getPerson(id);
    item->setPos(person.x,person.y);
    item->show();
    
    //TODO : use the same metainfo format inside rendered and in PersonItem
    item->rendererFlags() = 0;
    if (person.is_secondary_to_this_cluster)
      item->rendererFlags() |= RENDERER_IS_SECONDARY;
    if (person.is_anccestor)
      item->rendererFlags() |= RENDERER_IS_ANCESTOR;
    if (person.is_descendant)
      item->rendererFlags() |= RENDERER_IS_DESCENDANT;
  }

  for(const auto& [id, couple] : result.couple_placement){
    auto * item = family_tree_item->getFamily(id);
    item->setFamilyLineYBias(couple.family_line_y_bias);
    if(couple.family_line_connection_point_x.has_value()){
      item->setChildrenConnectionPointX(*couple.family_line_connection_point_x);
    } else {
	    item->setDefaultChildrenConnectionPointX();
    }
    item->show();
  }

  family_tree_item->reselectItem();
  family_tree_item->renderConnections();

}

void TreeManager::buildFromScratch(){
	auto * db = mftb::FamilyTreeSqlModel::getInstance();	
	FamilyTreeBuilder fb(family_tree_item, db);


	family_tree_item->clear();

	auto one_person_vec = db->getPeopleIds(1);
	if(!one_person_vec.empty()){
		fb.build_tree_from(one_person_vec[0]);
	} else {
		spdlog::error("DB IS EMPTY!");
	}
}


void TreeManager::buildDefault(){
  auto * db = mftb::FamilyTreeSqlModel::getInstance();
  auto p1 = db->insertPerson(DefaultInsertedMale);
  db->addPartner(DefaultInsertedFemale, p1);
  this->buildFromScratch();
  this->render();
}
