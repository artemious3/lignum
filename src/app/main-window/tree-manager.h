#include "datamodel.h"
#include "family-tree-item.h"


enum class RemoveStatus{
	Ok,
	NotLeaf,
	AttemptToRemoveCenterCouple
};

class TreeManager{
public:
  TreeManager(FamilyTreeItem *tree);
  void addChild(const Person &person, id_t parent1, id_t parent2);
  void addPartner(const Person &person, id_t partner1);
  void addParent(const Person &person, id_t child); 
  RemoveStatus removePerson(id_t id);
  void render();
  void buildFromScratch();


private:
  FamilyTreeItem *const family_tree_item;
};

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
