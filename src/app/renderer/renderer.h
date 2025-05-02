#include "DB.h"
#include "render-preprocessor.h"
#include "datamodel.h"
#include "family-tree-item.h"
#include <cstdint>

const uint16_t RENDERER_IS_SECONDARY = 1 << 0;
const uint16_t RENDERER_IS_ANCESTOR = 1 << 1;
const uint16_t RENDERER_IS_DESCENDANT = 1 << 2;

class Renderer {


public:
  Renderer(mftb::FamilyTreeModel *db, FamilyTreeItem* item);

  void balance_from_couple_id(id_t);

  mftb::FamilyTreeModel *const db;
  FamilyTreeItem* const ftree;

};
