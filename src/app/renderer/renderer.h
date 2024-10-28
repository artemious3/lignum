#include "DB.h"
#include "render-preprocessor.h"
#include "datamodel.h"
#include "family-tree-item.h"

class Renderer {

public:



public:
  Renderer(mftb::DB *db, FamilyTreeItem* item);

  void balance_from_couple_id(id_t);

  mftb::DB *const db;
  FamilyTreeItem* const ftree;

  static inline int FIRST_FAMILY_LINE_BIAS = 35;
  static inline int DISTANCE_BETWEEN_TREE_LEAVES = 100;
  static inline int DISTANCE_BETWEEN_GENERATIONS = 200;
  static inline int DISTANCE_BETWEEN_FAMILY_LINES = 7;

};