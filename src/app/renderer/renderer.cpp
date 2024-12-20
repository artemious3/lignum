#include "renderer.h"
#include "family-tree-item.h"
#include "cluster.h"
#include <qgraphicsitem.h>
#include "Config.h"
#include "spdlog/spdlog.h"

#include "valgrind/callgrind.h"

Renderer::Renderer(mftb::DB *db_, FamilyTreeItem* item) : db(db_), ftree(item) {}

void Renderer::balance_from_couple_id(id_t id) {


	SPDLOG_DEBUG("--------- START RENDERING ---------");

  auto cfg = Config::BalancerConfig(); 

  RenderPreprocessor::data preprocessor_data;

  // TODO : make it async
  {
    RenderPreprocessor preprocessor(db);
    preprocessor_data = preprocessor.preprocess_from_id(db->getCoupleById(id)->person1_id);
  }


  FamilyTreeCluster cluster = FamilyTreeCluster::fromCouple(db, preprocessor_data, id);
  auto cluster_data = cluster.getPlacementData();

  for(const auto& person : cluster_data.first){
    auto* item = ftree->getPerson(person.first);
    auto x = person.second.x * cfg.DISTANCE_BETWEEN_TREE_LEAVES;
    auto y = preprocessor_data.person_data[person.first].relative_generation * -cfg.DISTANCE_BETWEEN_GENERATIONS;
    item->setPos(x,y);
    item->show();
  }

  for(const auto& couple : cluster_data.second){
    auto * item = ftree->getFamily(couple.first);
    item->setFamilyLineYBias(cfg.FIRST_FAMILY_LINE_BIAS + couple.second.family_line_y_bias * cfg.DISTANCE_BETWEEN_FAMILY_LINES);
    item->setChildrenConnectionPointX(couple.second.family_line_connection_point_x * cfg.DISTANCE_BETWEEN_TREE_LEAVES);
    item->show();
  }
	SPDLOG_DEBUG("--------- FINISH RENDERING ---------");
}
