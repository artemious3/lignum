#include "cluster.h"
#include "entities.h"
#include "descendants-node-placer.h"
#include "renderer-flags.h"
#include "renderer.h"
#include "spdlog/spdlog.h"
#include "tree-traversal.h"
#include <QtLogging>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <optional>
#include <utility>
#include "ancestors-node-placer.h"

FamilyTreeCluster::FamilyTreeCluster(mftb::FamilyTreeModel *db_,
                                     const RenderPreprocessor::data &data)
    :  preprocessor_data(data), db(db_){}

FamilyTreeCluster
FamilyTreeCluster::fromCouple(FamilyTreeModel *db, const RenderPreprocessor::data &data,
                              id_t id) {

  auto person_data = data.person_data;
  auto couple_data = data.couple_data;
  FamilyTreeCluster cluster(db, data);

  auto center = 0;
  SPDLOG_DEBUG("CLUSTER CENTER IS {}", center);

  cluster.place_couple_descendants(id, (double)center-couple_data[id].hourglass_descendants_width/2.0);

  auto couple = db->getCoupleById(id);
  auto p1 = couple->person1_id;
  auto p2 = couple->person2_id;
  auto p1_parents = db->getParentsCoupleId(p1);
  auto p2_parents = db->getParentsCoupleId(p2);
  if (p1 != 0 && p1_parents != 0 && p2 != 0 && p2_parents != 0) {
	  // CASE 1 : both couple members have ancestors

    auto couple_ancestor_width = (person_data[p1].ancestors_and_siblings_width + 
		    		  person_data[p2].ancestors_and_siblings_width);

    cluster.place_persons_ancestors(
        p1, center - couple_ancestor_width/2.0 - 0.5, p2);
    cluster.place_persons_ancestors(
        p2, center - couple_ancestor_width/2.0 - 0.5 + person_data[p1].ancestors_and_siblings_width, p1);
  } else {

    if (p1_parents.has_value() && p1_parents != 0) {
      cluster.place_persons_ancestors(
          p1, center - person_data[p1].ancestors_and_siblings_width / 2.0 - 0.5,
          0);
    } else if (p2_parents.has_value() && p2_parents != 0) {
      cluster.place_persons_ancestors(
          p2, center - person_data[p2].ancestors_and_siblings_width / 2.0 - 0.5,
          0);
    } else {
      double p1pos = (p2 != 0) ? center - 0.5 : center;
      if(p1  != 0) {
	      cluster.place_person(p1, p1pos, 0);
      }
      double p2pos = (p1 != 0) ? center + 0.5 : center;
      if(p2 != 0) {
	      cluster.place_person(p2, p2pos, 0);
      }
      cluster.couple_placement[id].family_line_connection_point_x = center;
    }
  }


  // AncestorNodePlacer could mark p1 or p2 secondary,
  // so we make sure they are not
  if(p1 != 0){
	  // cluster.persons_placement[p1].is_secondary_to_this_cluster = false;
		cluster.persons_placement[p1].flags &= ~RENDERER_IS_SECONDARY;

  }
  if(p2 != 0){
	  // cluster.persons_placement[p2].is_secondary_to_this_cluster = false;
		cluster.persons_placement[p2].flags &= ~RENDERER_IS_SECONDARY;
  }
  return cluster;
}

double FamilyTreeCluster::place_person(id_t id, double pos, renderer_flags_t flags) {
  auto cfg = Config::BalancerConfig();
  persons_placement[id].x = pos * cfg.DISTANCE_BETWEEN_TREE_LEAVES;
  persons_placement[id].y = -preprocessor_data.person_data.at(id).relative_generation * cfg.DISTANCE_BETWEEN_GENERATIONS;
  persons_placement[id].processed = true;
	persons_placement[id].flags = flags;
	if(!db->getPersonChildren(id).empty()){
		persons_placement[id].flags |= RENDERER_HAS_DESCENDANTS;
	}
	if(db->getParentsCoupleId(id).value() != 0){
		persons_placement[id].flags |= RENDERER_HAS_ANCESTORS;
	}
  SPDLOG_DEBUG("placed person {} at relative pos {}", id, pos);
  return pos;
}


void FamilyTreeCluster::place_couple(id_t couple, std::optional<double> connector_pos, int bias){
	auto cfg = Config::BalancerConfig();
	if(connector_pos.has_value()) {
		couple_placement[couple].family_line_connection_point_x = *connector_pos * cfg.DISTANCE_BETWEEN_TREE_LEAVES;
    // if (no_parents_partners_couples.empty()) {
	}
	couple_placement[couple].family_line_y_bias = cfg.FIRST_FAMILY_LINE_BIAS + bias * cfg.DISTANCE_BETWEEN_FAMILY_LINES;

}

void FamilyTreeCluster::place_couple_descendants(id_t couple_id, double left_border) {

  auto one_of_partners_id = db->getCoupleById(couple_id)->person1_id;

  auto couple_data = preprocessor_data.couple_data;

  DescendantsNodePlacer placer{preprocessor_data};
  placer.init_placement_from_couple(left_border, couple_id);

  // `node` type basically represents a pair of a primary person
  //  and some their couple id, if present.

  auto place_node = [&](node idvar) {
    auto placement = placer.place_node(idvar);

    if (placement.primary_person_pos.has_value()) {
      place_person(idvar.primary_person, *placement.primary_person_pos, RENDERER_IS_DESCENDANT);
    }

    if (placement.partner_pos.has_value()) {
      place_couple(*idvar.couple_id, placement.family_connector_point_x, persons_placement[idvar.primary_person].couple_counter);
      ++persons_placement[idvar.primary_person].couple_counter;
      auto partner = db->getCoupleById(*idvar.couple_id)
                         ->getAnotherPerson(idvar.primary_person);
      place_person(partner, *placement.partner_pos, RENDERER_IS_SECONDARY | RENDERER_IS_DESCENDANT);
    }
  };

  auto get_lower_nodes_lambda = [&](node id) {
		return get_lower_nodes(id);
	};

  TreeTraversal<node>::breadth_first(node{one_of_partners_id, couple_id},
                                     get_lower_nodes_lambda, place_node, false);
  place_couple(couple_id, std::nullopt, 0);
}

FamilyTreeCluster::ClusterPlacement FamilyTreeCluster::getPlacementData() {
  return {persons_placement, couple_placement};
}

// TODO : handle 'partner of partner' relationship
std::vector<DescendantsNodePlacer::node> FamilyTreeCluster::get_lower_nodes(node nd) {

  if (!nd.couple_id.has_value()) {
    return {};
  }

  std::vector<node> lower_nodes;

  auto children = db->getCoupleChildren(nd.couple_id.value());

  for (auto child : children) {

    auto partners = db->getPersonCouplesId(child);

    if (partners.empty()) {
      lower_nodes.push_back(node{
          child,
      });
    }

    for (auto npp_couple : partners) {
      lower_nodes.push_back(node{child, npp_couple});
    }
  }

	// set `has descendants` flag
	if (!lower_nodes.empty()) {
		persons_placement[nd.primary_person].flags |= RENDERER_HAS_DESCENDANTS;
		id_t second_partner = db->getCoupleById(nd.couple_id.value())->getAnotherPerson(nd.primary_person);
		if(second_partner != 0){
			persons_placement[second_partner].flags |= RENDERER_HAS_DESCENDANTS;
		}
	}

  SPDLOG_DEBUG("lower nodes for {} (couple_id {})", nd.primary_person,
               (signed)nd.couple_id.value_or(-1));

  return lower_nodes;
}


// bullshit
void FamilyTreeCluster::place_persons_ancestors(id_t person_id, double lborder, id_t ignored_partner) {

  //there should be AncestorNodePlacer
  AncestorNodePlacer placer(preprocessor_data,db);
  SPDLOG_DEBUG("PLACING {} ANCESTORS. LEFT BORDER : {}",person_id, lborder );
  placer.init_placement(lborder, ignored_partner, std::make_pair(person_id, ignored_partner));

  auto get_parents_couples =
      [&](id_t couple_id) -> std::vector<id_t> {
    std::vector<id_t> parents_couples;
    auto couple = db->getCoupleById(couple_id);

    

    auto p1 = couple->person1_id;
    auto p2 = couple->person2_id;

    SPDLOG_DEBUG("Process couple ({}, {})", p1, p2);

    if (p1 != 0 && db->getParentsCoupleId(p1) != 0) {
      auto parents_couple = db->getParentsCoupleId(p1);
      SPDLOG_DEBUG("Added parents couple {}", *parents_couple);
      parents_couples.push_back(*parents_couple);
			persons_placement[p1].flags |= RENDERER_HAS_ANCESTORS;
    }

    if (p2 != 0 && db->getParentsCoupleId(p2) != 0) {
      auto parents_couple = db->getParentsCoupleId(p2);
      SPDLOG_DEBUG("Added parents couple {}", *parents_couple);
      parents_couples.push_back(*parents_couple);
			persons_placement[p2].flags |= RENDERER_HAS_ANCESTORS;
    }
    return parents_couples;
  };

  auto place_children = [&](id_t id) {
    SPDLOG_DEBUG("PLACE CHILDREN of couple {}", id);
    auto placement_data = placer.place_family(id);
    auto children = placement_data.first;
    auto couple = placement_data.second;
    for(const auto nd : children){

			renderer_flags_t flags = RENDERER_IS_ANCESTOR;
			if(nd.is_secondary){
				flags |= RENDERER_IS_SECONDARY;
			}
      place_person(nd.id, nd.pos, flags);
    }

    place_couple(id, couple.connector_pos_x, 0);
  };

  auto parents_couple = db->getParentsCoupleId(person_id);
  TreeTraversal<id_t>::breadth_first(parents_couple.value(),
                                    get_parents_couples,
                                     place_children, true);
}
