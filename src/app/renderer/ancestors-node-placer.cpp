#include "ancestors-node-placer.h"
#include "SqlDB.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <strings.h>

AncestorNodePlacer::AncestorNodePlacer(const RenderPreprocessor::data &prep,
                                       mftb::DB *db_)
    : prep_data(prep), db(db_) {}

void AncestorNodePlacer::set_left_border(double lpos){
  cluster_left_border = lpos;
  sliding_left_border = cluster_left_border;
}

std::vector<AncestorNodePlacer::person_placement>
AncestorNodePlacer::place_couple(id_t couple_id) {

  auto person_data = prep_data.person_data;
  auto couple_data = prep_data.couple_data;
  std::vector<person_placement> placement;
  auto couple = db->getCoupleById(couple_id);
  auto p1 = couple->person1_id;
  auto p2 = couple->person2_id;
  double placement_center = sliding_left_border + couple_data[couple_id].ancestors_and_children_width/2.0;

  SPDLOG_DEBUG("COUPLE ({}, {}) SLIDING LEFT BORDER : {}", p1, p2,  sliding_left_border);
  SPDLOG_DEBUG("COUPLE ({}, {}) PLACEMENT CENTER : {}", p1, p2, placement_center);

  assert(p1 != 0 || p2 != 0);

  // 1. Place parents that have no parents specified

  int number_of_parents_specified = 0;
  id_t no_parents_person = 0;

  if(p1 != 0){
    if(db->getParentsCoupleId(p1) == 0){
      ++number_of_parents_specified;
      no_parents_person = p1;
    } else {
      ++next_generation_counter;
    }
  } 
  if(p2 != 0){
    if(db->getParentsCoupleId(p2) == 0){
      ++number_of_parents_specified;
      no_parents_person = p2;
    } else {
      ++next_generation_counter;
    }
  }

  if(number_of_parents_specified == 2){
    placement.push_back({
      .id = p1,
      .pos = placement_center-0.5
    });
    placement.push_back({
      .id = p2,
      .pos = placement_center+0.5
    });
  } else if (number_of_parents_specified == 1){
    placement.push_back({
      .id = no_parents_person,
      .pos = placement_center
    });
  }

  // 2. Place children

  auto lower_nodes = get_children_to_place(couple_id);

  auto pos = placement_center - lower_nodes.size() / 2.0 + 0.5;
  for (auto lower_node_id : lower_nodes) {
    placement.push_back({.id = lower_node_id, .pos = pos});
    pos++;
  }

  sliding_left_border += (double)couple_data[couple_id].ancestors_and_children_width;

  next_family();

  return placement;
}

void AncestorNodePlacer::next_family() {
  index++;
  // calculate current center...
  if (index == this_generation_counter) {
    this_generation_counter = next_generation_counter;
    next_generation_counter = 0;
    index = 0;

    sliding_left_border = cluster_left_border;

    SPDLOG_DEBUG("NEW GENERATION");
    SPDLOG_DEBUG("THIS GENERATION COUNTER {}", this_generation_counter);
  }
}

void AncestorNodePlacer::add_ancestor_family() { 
  // next_generation_counter++; 
  // SPDLOG_DEBUG("ANCESTOR FAMILY INCEREMENTED : {}", next_generation_counter);
}

std::vector<id_t> AncestorNodePlacer::get_children_to_place(id_t couple_id) {
  std::vector<id_t> children_to_place;
  auto children = db->getCoupleChildren(couple_id);

  for (auto child : children) {

    children_to_place.push_back(child);
    auto partners = db->getPersonPartners(child);
    for (auto partner : partners) {
      if (partner != 0 && db->getParentsCoupleId(partner) == 0) {
        children_to_place.push_back(partner);
      }
    }
  }

  return children_to_place;
}
