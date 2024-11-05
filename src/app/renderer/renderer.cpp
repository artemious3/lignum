#include "renderer.h"
#include "family-tree-item.h"
#include "cluster.h"
#include <qgraphicsitem.h>

Renderer::Renderer(mftb::DB *db_, FamilyTreeItem* item) : db(db_), ftree(item) {}

void Renderer::balance_from_couple_id(id_t id) {

  RenderPreprocessor::data preprocessor_data;

  // TODO : make it async
  {
    RenderPreprocessor preprocessor(db);
    preprocessor_data = preprocessor.preprocess_from_id(id);
  }


  FamilyTreeCluster cluster = FamilyTreeCluster::fromCouple(db, preprocessor_data, id);
  auto cluster_data = cluster.getPlacementData();

  for(const auto& person : cluster_data.first){
    auto* item = ftree->getPersonItemById(person.first);
    auto x = person.second.x * DISTANCE_BETWEEN_TREE_LEAVES;
    auto y = preprocessor_data.person_data[person.first].relative_generation * -DISTANCE_BETWEEN_GENERATIONS;
    item->setPos(x,y);
    item->show();
  }

  for(const auto& couple : cluster_data.second){
    auto * item = ftree->getFamilyWithCoupleId(couple.first);
    item->setFamilyLineYBias(FIRST_FAMILY_LINE_BIAS + couple.second.family_line_y_bias * DISTANCE_BETWEEN_FAMILY_LINES);
    item->setChildrenConnectionPointX(couple.second.family_line_connection_point_x * DISTANCE_BETWEEN_TREE_LEAVES);
    item->show();
  }
}
