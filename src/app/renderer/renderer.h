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

};