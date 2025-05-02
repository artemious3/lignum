#include "renderer.h"
#include "family-tree-item.h"
#include "cluster.h"
#include <qgraphicsitem.h>
#include "Config.h"
#include "spdlog/spdlog.h"


Renderer::Renderer(mftb::FamilyTreeModel *db_, FamilyTreeView* item) : db(db_), ftree(item) {}

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

  for(const auto& [id, person] : cluster_data.first){
    auto* item = ftree->getPerson(id);
    auto x = person.x * cfg.DISTANCE_BETWEEN_TREE_LEAVES;
    auto y = preprocessor_data.person_data[id].relative_generation * -cfg.DISTANCE_BETWEEN_GENERATIONS;
    item->setPos(x,y);
    item->show();
    
    item->rendererFlags() = 0;
    if (person.is_secondary_to_this_cluster)
      item->rendererFlags() |= RENDERER_IS_SECONDARY;
    if (person.is_anccestor)
      item->rendererFlags() |= RENDERER_IS_ANCESTOR;
    if (person.is_descendant)
      item->rendererFlags() |= RENDERER_IS_DESCENDANT;
  }

  for(const auto& [id, couple] : cluster_data.second){
    auto * item = ftree->getFamily(id);
    item->setFamilyLineYBias(cfg.FIRST_FAMILY_LINE_BIAS + couple.family_line_y_bias * cfg.DISTANCE_BETWEEN_FAMILY_LINES);
    if(couple.family_line_connection_point_x.has_value()){
      item->setChildrenConnectionPointX(
          couple.family_line_connection_point_x.value() *
          cfg.DISTANCE_BETWEEN_TREE_LEAVES);
    } else {
	    item->setDefaultChildrenConnectionPointX();
    }
    item->show();
  }
	SPDLOG_DEBUG("--------- FINISH RENDERING ---------");
}
