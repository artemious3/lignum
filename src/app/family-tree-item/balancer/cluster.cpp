#include "cluster.h"
#include "datamodel.h"
#include "tree-traversal.h"
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

double FamilyTreeCluster::place_person(id_t id, std::pair<int, int> borders,
                                       double koef) {
  assert(koef <= 1.0 && koef >= 0.0);

  auto pos = borders.first + koef * (borders.second - borders.first);
  persons_placement[id].x = pos;

  qDebug() << "placed person " << id << " at rel pos " << pos;
  qDebug() << "borders were " << borders;

  persons_placement[id].processed = true;
  return pos;
}


// TODO: rewrite this fucking shit
void FamilyTreeCluster::place_descendants(id_t couple_id) {

  const auto current_placement_borders = getPlacementBorders(couple_id);
  last_placement_borders = current_placement_borders;
  right_x = std::max(right_x, current_placement_borders.second);
  left_x = std::min(left_x, current_placement_borders.first);

  auto one_of_partners_id = db->getCoupleById(couple_id).value().person1_id;
  auto last_generation = INT_MAX;
  id_t last_primary_person = 0;
  double primary_person_pos = 0;

  ccp_add_couple(current_placement_borders.first, couple_id);

  double current_left_border = current_placement_borders.first;

  auto place_node = [&](node idvar) {
    auto primary_person_data =
        preprocessor_data.person_data.find(idvar.primary_person)->second;

    if (primary_person_data.relative_generation != last_generation) {
      current_left_border = ccp_new_generation();
      last_generation = primary_person_data.relative_generation;

      qDebug() << "RESET LEFT BORDER to " << current_left_border;
      qDebug() << "last_generation was " << last_generation << "; new is "
               << primary_person_data.relative_generation;
    }

    if (last_primary_person != idvar.primary_person) {
      qDebug() << "NEW PRIMARY PERSON : " << idvar.primary_person;
      primary_person_pos = place_person(idvar.primary_person,
                   {current_left_border, current_left_border + 1});
      current_left_border += 1;
      last_primary_person = idvar.primary_person;
      ccp_next_person(current_left_border);
    }

    qDebug() << ">>>>>> placing id " << idvar.primary_person << " ( couple "
             << (signed)idvar.couple_id.value_or(-1) << ") ";

    id_t person_being_placed_id;
    double current_right_border;

    if (!idvar.couple_id.has_value()) {
      return;
    }

    auto couple_id = idvar.couple_id.value();

    auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
    person_being_placed_id =
        db->getCoupleById(couple_id).value().getAnotherPerson(
            idvar.primary_person);

    if (person_being_placed_id == 0) {
      qDebug() << "the second partner is 0";
      ccp_add_couple(current_left_border-1, couple_id);
      current_left_border += couple_data.hourglass_descendants_width;
      return;
    }

    qDebug() << "the second partner is " << person_being_placed_id;

    current_right_border = current_left_border +
                           std::max(couple_data.hourglass_descendants_width, 1);

    persons_placement[idvar.primary_person].couple_counter++;

    couple_placement[couple_id].family_line_y_bias =
        persons_placement[idvar.primary_person].couple_counter;
    couple_placement[couple_id].family_line_connection_point_x =
        (double)(current_left_border + current_right_border) / 2.0;

    auto placed_pos =
        place_person(person_being_placed_id,
                     {current_left_border, current_right_border}, 0.5);
    ccp_add_couple(current_left_border-1, couple_id);

    current_left_border = current_right_border;
    ccp_next_person(current_left_border);
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

void FamilyTreeCluster::ccp_next_person(double &left_border) {
  ++ccp_person_counter;
  if (ccp_person_counter == last_generation_ccp[ccp_idx].children_count) {
    ++ccp_idx;
    if (ccp_idx < last_generation_ccp.size())
      left_border = last_generation_ccp.at(ccp_idx).left_border;
    ccp_person_counter = 0;
  }
}

void FamilyTreeCluster::ccp_add_couple(double left_border, id_t couple_id) {
  auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;

  if (couple_data.children_count != 0) {
    couple_children_placement ccp;
    ccp.children_count = couple_data.children_count;
    ccp.left_border = left_border;
    new_generation_ccp.push_back(ccp);
  }
}

double FamilyTreeCluster::ccp_new_generation() {
  last_generation_ccp = std::move(new_generation_ccp);
  ccp_person_counter = 0;
  ccp_idx = 0;
  return last_generation_ccp[0].left_border;
}
