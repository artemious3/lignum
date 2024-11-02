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

FamilyTreeCluster::FamilyTreeCluster(mftb::DB *db_,
                                     const RenderPreprocessor::data &data)
    : db(db_), preprocessor_data(data) {}

FamilyTreeCluster
FamilyTreeCluster::fromCouple(DB *db, const RenderPreprocessor::data &data,
                              id_t id) {

  FamilyTreeCluster cluster(db, data);
  cluster.place_descendants(id);
  // cluster.place_ancestors(id);
  return cluster;
}

double FamilyTreeCluster::place_person(id_t id, double pos) {
  persons_placement[id].x = pos;
  persons_placement[id].processed = true;
  SPDLOG_DEBUG("placed person {} at relative pos {}", id, pos);
  return pos;
}

void FamilyTreeCluster::place_descendants(id_t couple_id) {

  const auto current_placement_borders = getPlacementBorders(couple_id);
  // last_placement_borders = current_placement_borders;
  // last_processed_couple = couple_id;
  // right_x = std::max(right_x, current_placement_borders.second);
  // left_x = std::min(left_x, current_placement_borders.first);

  auto one_of_partners_id = db->getCoupleById(couple_id)->person1_id;

  // PRIMARY PERSON is a direct ancestor or descendant
  // of a couple, that generated this cluster
  id_t last_primary_person = 0;

  NodePlacer placer{preprocessor_data};
  placer.init_placement_from_couple(current_placement_borders.first, 0);

  // `node` type basically represents a pair of a primary person
  //  and some their couple id, if present.

  auto place_node = [&](node idvar) {
    auto placement = placer.place_node(idvar);

    if (placement.primary_person_pos.has_value()) {
      place_person(idvar.primary_person, *placement.primary_person_pos);
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
    }
  };

  auto get_lower_nodes_lambda = [&](node id) { return getLowerNodes(id); };

  TreeTraversal<node>::breadth_first(node{one_of_partners_id, couple_id},
                                     get_lower_nodes_lambda, place_node);
}

std::pair<int, int> FamilyTreeCluster::getPlacementBorders(id_t couple_id) {

  auto children = db->getCoupleChildren(couple_id);
  auto procesed_child =
      std::find_if(children.begin(), children.end(),
                   [&](id_t id) { return persons_placement[id].processed; });

  if (procesed_child == children.end()) {
    auto preprocessed_couple_data =
        preprocessor_data.couple_data.find(couple_id)->second;

    return {0, preprocessed_couple_data.hourglass_descendants_width};
  }

  std::size_t processed_child_idx =
      std::distance(procesed_child, children.begin());

  auto add_width = [&](std::size_t sum, id_t id) {
    auto person_data = preprocessor_data.person_data.find(id)->second;
    return sum + person_data.width;
  };

  auto left_width =
      std::accumulate(children.begin(), procesed_child, 0, add_width);

  auto right_width =
      std::accumulate(procesed_child, children.end(), 0, add_width);

  std::pair<int, int> new_placement_borders = {
      last_placement_borders.first - left_width,
      last_placement_borders.second + right_width};

  return new_placement_borders;
}

FamilyTreeCluster::placement_data
FamilyTreeCluster::getPlacementData() {
  return {persons_placement, couple_placement};
}

// TODO : handle 'partner of partner' relationship
std::vector<NodePlacer::node> FamilyTreeCluster::getLowerNodes(node nd) {

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

void FamilyTreeCluster::place_ancestors(id_t couple_id) {

  auto get_parents = [&](id_t id) -> std::vector<id_t> {
    std::vector<id_t> parents_vector;
    auto parents = db->getPersonParentsById(id);
    if (parents.first != 0) {
      parents_vector.push_back(parents.first);
    }
    if (parents.second != 0) {
      parents_vector.push_back(parents.second);
    }
    return parents_vector;
  };

  // ASSUMING couple descendants are already placed
  auto couple = db->getCoupleById(couple_id);
  auto person1 = couple->person1_id;

  TreeTraversal<id_t>::breadth_first(
      person1, get_parents, [&](id_t id) { place_descendants(id); }, false);
}
