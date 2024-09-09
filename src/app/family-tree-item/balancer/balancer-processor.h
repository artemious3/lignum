#include "DB.h"
#include "balancer-preprocessor.h"
#include "datamodel.h"
#include "family-tree-item.h"

class FamilyTreeBalancer {

public:



public:
  FamilyTreeBalancer(mftb::DB *db, FamilyTreeItem* item);

  void balance_from_couple_id(id_t);

  mftb::DB *const db;
  FamilyTreeItem* const ftree;

  static inline int DISTANCE_BETWEEN_TREE_LEAVES = 100;
  static inline int DISTANCE_BETWEEN_GENERATIONS = 200;
  static inline int DISTANCE_BETWEEN_FAMILY_LINES = 40;

};