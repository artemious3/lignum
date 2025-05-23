#include "FamilyTreeModel.h"
#include "cluster.h"
#include "entities.h"
#include <cstdint>

const uint16_t RENDERER_IS_SECONDARY = 1 << 0;
const uint16_t RENDERER_IS_ANCESTOR = 1 << 1;
const uint16_t RENDERER_IS_DESCENDANT = 1 << 2;

class Renderer {


public:
  Renderer(mftb::FamilyTreeModel *db);

  using Result = FamilyTreeCluster::ClusterPlacement;

  Result render(id_t);

  mftb::FamilyTreeModel *const db;

};
