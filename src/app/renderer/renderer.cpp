#include "renderer.h"
#include "cluster.h"
#include <qgraphicsitem.h>
#include "Config.h"
#include "spdlog/spdlog.h"


Renderer::Renderer(mftb::FamilyTreeModel *db_) : db(db_) {}

Renderer::Result Renderer::render(id_t id) {


	SPDLOG_DEBUG("--------- START RENDERING ---------");

  auto cfg = Config::BalancerConfig(); 

  RenderPreprocessor::data preprocessor_data;

  // TODO : make it async
  {
    RenderPreprocessor preprocessor(db);
    preprocessor_data = preprocessor.preprocess_from_id(db->getCoupleById(id)->person1_id);
  }


  FamilyTreeCluster cluster = FamilyTreeCluster::fromCouple(db, preprocessor_data, id);
  return cluster.getPlacementData();

	SPDLOG_DEBUG("--------- FINISH RENDERING ---------");
}
