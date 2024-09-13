#include "cluster.h"
#include "datamodel.h"
#include "tree-traversal.h"
#include "node-placer.h"
#include <QtLogging>
#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>

FamilyTreeCluster::FamilyTreeCluster(
    mftb::DB *db_, const FamilyTreeBalancerPreprocessor::data &data)
    : db(db_), preprocessor_data(data) {}

FamilyTreeCluster FamilyTreeCluster::fromCouple(
    DB *db, const FamilyTreeBalancerPreprocessor::data &data, id_t id) {

  FamilyTreeCluster cluster(db, data);
  cluster.place_descendants(id);

  // ... place ancestors routine

  return cluster;
}

double FamilyTreeCluster::place_person(id_t id, double pos) {
  persons_placement[id].x = pos;
  persons_placement[id].processed = true;
  qDebug() << "placed person " << id << " at rel pos " << pos;
  return pos;
}

// TODO: rewrite this fucking shit
void FamilyTreeCluster::place_descendants(id_t couple_id) {

  const auto current_placement_borders = getPlacementBorders(couple_id);
  last_placement_borders = current_placement_borders;
  right_x = std::max(right_x, current_placement_borders.second);
  left_x = std::min(left_x, current_placement_borders.first);

  auto one_of_partners_id = db->getCoupleById(couple_id).value().person1_id;
  id_t last_primary_person = 0;

  NodePlacer placer{preprocessor_data};
  placer.init_placement_from_couple(current_placement_borders.first, 0);
  // tsg.init_placement_from_couple(current_placement_borders.first, couple_id);

  auto place_node = [&](node idvar) {
    auto primary_person_data =
        preprocessor_data.person_data.find(idvar.primary_person)->second;

    id_t person_being_placed;

    if (last_primary_person != idvar.primary_person) {
      place_person(idvar.primary_person,
                   placer.new_primary_person(idvar.primary_person));
      last_primary_person = idvar.primary_person;

      if (!idvar.couple_id.has_value()) {
        placer.next();
        return;
      }
      person_being_placed = db->getCoupleById(*idvar.couple_id)
                                ->getAnotherPerson(idvar.primary_person);
      if (person_being_placed == 0) {
        placer.pass_zero_partner(*idvar.couple_id);
        placer.next();
        return;
      }

      placer.next();
    }


    person_being_placed = db->getCoupleById(*idvar.couple_id)
                                ->getAnotherPerson(idvar.primary_person);

    auto couple_id = idvar.couple_id.value();

    auto partner_placement =
        placer.get_partner_placement(idvar.primary_person, couple_id);
    place_person(person_being_placed, partner_placement.partner_pos);
    couple_placement[couple_id].family_line_connection_point_x =
        partner_placement.family_connector_point_x;
    couple_placement[couple_id].family_line_y_bias =
        persons_placement[idvar.primary_person].couple_counter;
    ++persons_placement[idvar.primary_person].couple_counter;
    placer.next();
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

std::pair<const std::unordered_map<id_t, FamilyTreeCluster::person_data> &,
          const std::unordered_map<id_t, FamilyTreeCluster::couple_data> &>
FamilyTreeCluster::getPlacementData() {
  return {persons_placement, couple_placement};
}

// TODO : handle 'partner of partner' relationship
std::vector<FamilyTreeCluster::node> FamilyTreeCluster::getLowerNodes(node nd) {

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

  qDebug() << "lower nodes for " << nd.primary_person << "( couple "
           << (signed)nd.couple_id.value_or(-1) << ")";

  for (auto ln : lower_nodes) {
    qDebug() << ln.primary_person << " ( couple "
             << (signed)ln.couple_id.value_or(-1) << ")";
  }

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
      new_cluster_candidates.insert(couple_id);
    }
  }

  qDebug() << "person " << person_id << " has " << no_parents_partners.size()
           << "partners with no parents";

  return no_parents_partners;
}
