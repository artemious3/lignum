#include "cluster.h"
#include "datamodel.h"
#include "node-placer.h"
#include "spdlog/spdlog.h"
#include "tree-traversal.h"
#include <QtLogging>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>
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

    if (p1_parents != 0) {
      cluster.place_persons_ancestors(
          p1, center - person_data[p1].ancestors_and_siblings_width / 2.0 - 0.5,
          0);
    } else if (p2_parents != 0) {
      cluster.place_persons_ancestors(
          p2, center - person_data[p2].ancestors_and_siblings_width / 2.0 - 0.5,
          0);
    } else {
      double p1pos = (p2 != 0) ? center - 0.5 : center;
      cluster.place_person(p1, p1pos);
      double p2pos = (p1 != 0) ? center + 0.5 : center;
      cluster.place_person(p2, p2pos);
      cluster.couple_placement[id].family_line_connection_point_x = center;
    }
  }


  // AncestorNodePlacer could mark p1 or p2 secondary,
  // so we make sure they are not
  if(p1 != 0){
	  cluster.persons_placement[p1].is_secondary_to_this_cluster = false;
  }
  if(p2 != 0){
	  cluster.persons_placement[p2].is_secondary_to_this_cluster = false;
  }
  return cluster;
}

double FamilyTreeCluster::place_person(id_t id, double pos) {
  persons_placement[id].x = pos;
  persons_placement[id].processed = true;
  SPDLOG_DEBUG("placed person {} at relative pos {}", id, pos);
  return pos;
}

void FamilyTreeCluster::place_couple_descendants(id_t couple_id, double left_border) {

  // last_placement_borders = current_placement_borders;
  // last_processed_couple = couple_id;
  // right_x = std::max(right_x, current_placement_borders.second);
  // left_x = std::min(left_x, current_placement_borders.first);

  auto one_of_partners_id = db->getCoupleById(couple_id)->person1_id;

  auto couple_data = preprocessor_data.couple_data;

  DescendantsNodePlacer placer{preprocessor_data};
  placer.init_placement_from_couple(left_border, couple_id);

  // `node` type basically represents a pair of a primary person
  //  and some their couple id, if present.

  auto place_node = [&](node idvar) {
    auto placement = placer.place_node(idvar);

    if (placement.primary_person_pos.has_value()) {
      place_person(idvar.primary_person, *placement.primary_person_pos);
      persons_placement[idvar.primary_person].is_descendant = true;
    }

    if (placement.partner_pos.has_value()) {
      couple_placement[*idvar.couple_id].family_line_connection_point_x =
          *placement.family_connector_point_x;
      couple_placement[*idvar.couple_id].family_line_y_bias =
          persons_placement[idvar.primary_person].couple_counter;
      ++persons_placement[idvar.primary_person].couple_counter;
      auto partner = db->getCoupleById(*idvar.couple_id)
                         ->getAnotherPerson(idvar.primary_person);
      place_person(partner, *placement.partner_pos);
      persons_placement[partner].is_secondary_to_this_cluster = true;
      persons_placement[partner].is_descendant = true;
    }
  };

  auto get_lower_nodes_lambda = [&](node id) { return getLowerNodes(id); };

  TreeTraversal<node>::breadth_first(node{one_of_partners_id, couple_id},
                                     get_lower_nodes_lambda, place_node, false);
}

FamilyTreeCluster::placement_data FamilyTreeCluster::getPlacementData() {
  return {persons_placement, couple_placement};
}

// TODO : handle 'partner of partner' relationship
std::vector<DescendantsNodePlacer::node> FamilyTreeCluster::getLowerNodes(node nd) {

  if (!nd.couple_id.has_value()) {
    return {};
  }

  std::vector<node> lower_nodes;

  auto children = db->getCoupleChildren(nd.couple_id.value());

  for (auto child : children) {

    auto no_parents_partners_couples = processPartnersWithNoParents(child);

    if (no_parents_partners_couples.empty()) {
      lower_nodes.push_back(node{
          child,
      });
    }

    for (auto npp_couple : no_parents_partners_couples) {
      lower_nodes.push_back(node{child, npp_couple});
    }
  }

  SPDLOG_DEBUG("lower nodes for {} (couple_id {})", nd.primary_person,
               (signed)nd.couple_id.value_or(-1));

  return lower_nodes;
}

std::vector<id_t>
FamilyTreeCluster::processPartnersWithNoParents(id_t person_id) {
  std::vector<id_t> no_parents_partners;
  auto all_partners = db->getPersonPartners(person_id);

  for (auto partner : all_partners) {

    auto couple_id = db->getCoupleIdByPersons(person_id, partner).value();
    // If partner is empty, the couple should be processed as well
    if (partner == 0 || db->getParentsCoupleId(partner).value() == 0) {
      no_parents_partners.push_back(couple_id);
    } else {
      auto generation = preprocessor_data.person_data.find(partner)
                            ->second.relative_generation;
      cluster_candidate candidate;
      candidate.couple_id = couple_id;
      candidate.generation = generation;
      cluster_candidates.push_back(candidate);
    }
  }

  SPDLOG_DEBUG("person {} has {} partners with no parents", person_id,
               no_parents_partners.size());

  return no_parents_partners;
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
    }

    if (p2 != 0 && db->getParentsCoupleId(p2) != 0) {
      auto parents_couple = db->getParentsCoupleId(p2);
      SPDLOG_DEBUG("Added parents couple {}", *parents_couple);
      parents_couples.push_back(*parents_couple);
    }
    return parents_couples;
  };

  auto place_children = [&](id_t id) {
    SPDLOG_DEBUG("PLACE CHILDREN of couple {}", id);
    auto placement_data = placer.place_family(id);
    auto children = placement_data.first;
    auto couple = placement_data.second;
    for(const auto nd : children){
      place_person(nd.id, nd.pos);
      persons_placement[nd.id].is_secondary_to_this_cluster = nd.is_secondary;
      persons_placement[nd.id].is_anccestor = true;
    }
    couple_placement[id].family_line_connection_point_x = couple.connector_pos_x;
  };

  auto parents_couple = db->getParentsCoupleId(person_id);
  TreeTraversal<id_t>::breadth_first(parents_couple.value(),
                                    get_parents_couples,
                                     place_children, true);
}
