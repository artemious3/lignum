#include "render-manager.h"
#include "SqlDB.h"
#include "family-tree-builder.h"

void RenderManager::prepare(){
	tree_item->clear();
	mftb::DB * db = mftb::SqlDB::getInstance();
	FamilyTreeBuilder builder(tree_item, db);
	builder.build_tree_from(1);
}


void RenderManager::prepare_add_child_to_family(id_t couple_id, const Person& child){
}
