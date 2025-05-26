#pragma once
#include "FamilyTreeModel.h"
#include "cluster.h"
#include "entities.h"
#include <cstdint>
#include "renderer-flags.h"

class Renderer {


public:
  Renderer(mftb::FamilyTreeModel *db);

  using Result = FamilyTreeCluster::ClusterPlacement;

  Result render(id_t);

  mftb::FamilyTreeModel *const db;

};
