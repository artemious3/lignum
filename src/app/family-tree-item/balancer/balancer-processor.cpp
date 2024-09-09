#include "balancer-processor.h"
#include "family-tree-item.h"
#include "cluster.h"
#include <qgraphicsitem.h>

FamilyTreeBalancer::FamilyTreeBalancer(mftb::DB *db_, FamilyTreeItem* item) : db(db_), ftree(item) {}

void FamilyTreeBalancer::balance_from_couple_id(id_t id) {

  FamilyTreeBalancerPreprocessor::data preprocessor_data;

  // TODO : make it async
  {
    FamilyTreeBalancerPreprocessor preprocessor(db);
    preprocessor_data = preprocessor.preprocess_from_id(id);
  }


  FamilyTreeCluster cluster = FamilyTreeCluster::fromCouple(db, preprocessor_data, id);
  auto cluster_data = cluster.getPlacementData();

  for(const auto& person : cluster_data.first){
    auto* item = ftree->getPersonItemById(person.first);
    auto x = person.second.x * DISTANCE_BETWEEN_TREE_LEAVES;
    auto y = preprocessor_data.person_data[person.first].relative_generation * DISTANCE_BETWEEN_GENERATIONS;
    item->setPos(x,y);
  }

  for(const auto& couple : cluster_data.second){
    auto * item = ftree->getFamilyWithCoupleId(couple.first);
    //item->setFamilyConnectionPointX(couple.second.family_line_connection_point_x);
    item->setFamilyLineYBias(couple.second.family_line_y_bias * DISTANCE_BETWEEN_FAMILY_LINES);
  }
}
