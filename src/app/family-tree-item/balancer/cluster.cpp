#include "cluster.h"
#include "datamodel.h"
#include "tree-traversal.h"
#include <cassert>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <QtLogging>


FamilyTreeCluster FamilyTreeCluster::fromCouple(
    DB *db, const FamilyTreeBalancerPreprocessor::data &data, id_t id) {

  FamilyTreeCluster cluster(db, data);
  cluster.place_descendants(id);

  // ... place ancestors routine

  return cluster;
}

void FamilyTreeCluster::place_person(id_t id, std::pair<int, int> borders,
                                     double pos) {
  assert(pos <= 1.0 && pos >= 0.0);
  persons_placement[id].x = pos * (borders.first + borders.second);

  qDebug() << "placed person " << id << " at rel pos "
           << persons_placement[id].x;

  qDebug() << "borders were " << borders;

  persons_placement[id].processed = true;
}

void FamilyTreeCluster::place_descendants(id_t couple_id) {

  // We assume that only one child of given id
  // could have been processed

  const auto current_placement_borders = getPlacementBorders(couple_id);
  last_placement_borders = current_placement_borders;
  right_x = std::max(right_x, current_placement_borders.second);
  left_x = std::min(left_x, current_placement_borders.first);

  auto one_of_partners_id = db->getCoupleById(couple_id).value().person1_id;
  auto last_generation = preprocessor_data.person_data.find(one_of_partners_id)
                             ->second.relative_generation;
  id_t last_primary_person = 0;

  // start from the rightmost coordinate
  int current_right_border = current_placement_borders.second;

  auto place_node = [&](person_and_couple idvar) {
    // person being placed (either primary_person_being_placed
    // or their partner).

    auto primary_person_data =
        preprocessor_data.person_data.find(idvar.primary_person)->second;

    // reset the right border if the generation is updated
    if (primary_person_data.relative_generation != last_generation) {
      current_right_border = current_placement_borders.second;
      qDebug() << "RESET RIGHT BORDER to " << current_right_border;
      qDebug() << "last_generation was " << last_generation << "; new is " << primary_person_data.relative_generation;
      last_generation = primary_person_data.relative_generation;
    }

    // meet the couple with new primary person
    // it's not placed yet, so place it
    if (last_primary_person != idvar.primary_person) {

      qDebug() << "NEW PRIMARY PERSON : " << idvar.primary_person;
      place_person(idvar.primary_person,
                   {current_right_border - 1, current_right_border});
      current_right_border -= 1;
      last_primary_person = idvar.primary_person;
    }

    qDebug() << ">>>>>> placing id " << idvar.primary_person << " ( couple "
             << (signed)idvar.couple_id.value_or(-1) << ") ";

    id_t person_being_placed_id;
    int current_left_border;

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
      current_right_border -= couple_data.hourglass_descendants_width;
      return;

    } else {

      qDebug() << "the second partner is " << person_being_placed_id;

      current_left_border =
          current_right_border - std::max(couple_data.hourglass_descendants_width, 1);

      persons_placement[idvar.primary_person].couple_counter++;

      couple_placement[couple_id].family_line_y_bias =
          persons_placement[idvar.primary_person].couple_counter;
      couple_placement[couple_id].family_line_connection_point_x =
          (double)(current_left_border + current_placement_borders.second) / 2.0;
    }

    place_person(person_being_placed_id, {current_left_border, current_right_border});
    current_right_border = current_left_border;

  };

  auto get_lower_nodes_lambda = [&](person_and_couple id) {
    return getLowerNodes_c(id);
  };

  TreeTraversal<person_and_couple>::breadth_first_from_leaves(
      person_and_couple{one_of_partners_id, couple_id}, get_lower_nodes_lambda,
      place_node, [](person_and_couple) {});
}

FamilyTreeCluster::FamilyTreeCluster(
    mftb::DB *db_, const FamilyTreeBalancerPreprocessor::data &data)
    : db(db_), preprocessor_data(data) {}

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
std::vector<FamilyTreeCluster::person_and_couple>
FamilyTreeCluster::getLowerNodes_c(
    FamilyTreeCluster::person_and_couple var_id) {

  if (!var_id.couple_id.has_value()) {
    return {};
  }

  std::vector<person_and_couple> lower_nodes;

  auto children = db->getCoupleChildren(var_id.couple_id.value());

  for (auto child : children) {

    auto no_parents_partners_couples = processPartnersWithNoParents_c(child);

    if(no_parents_partners_couples.empty()){
      lower_nodes.push_back(person_and_couple{child,});
    }

    for (auto npp_couple : no_parents_partners_couples) {
      lower_nodes.push_back(person_and_couple{child, npp_couple});
    }

  }

  qDebug() << "lower nodes for " << var_id.primary_person << "( couple "
           << (signed)var_id.couple_id.value_or(-1) << ")";

  for (auto ln : lower_nodes) {
    qDebug() << ln.primary_person << " ( couple " << (signed)ln.couple_id.value_or(-1)
             << ")";
  }

  return lower_nodes;
}

std::vector<id_t>
FamilyTreeCluster::processPartnersWithNoParents_c(id_t person_id) {
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
