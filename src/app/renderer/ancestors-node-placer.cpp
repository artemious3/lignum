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
}

std::vector<AncestorNodePlacer::person_placement>
AncestorNodePlacer::place_couple(id_t couple_id) {
  auto person_data = prep_data.person_data;

  std::vector<person_placement> placement;
  double placement_center;

  auto couple = db->getCoupleById(couple_id);
  auto p1 = couple->person1_id;
  auto p2 = couple->person2_id;

  assert(p1 != 0 || p2 != 0);

  // 1. Place parents that have no parents specified

  if(p1 != 0 && p2 != 0){
    if(db->getParentsCoupleId(p1) == 0){
      placement.push_back({
        .id = p1,
        .pos = sliding_left_border
         + person_data[p1].ancestors_and_siblings_width/2.0
      });
      ++next_generation_counter;
    }

    if(db->getParentsCoupleId(p2) == 0){
      placement.push_back({
        .id = p2,
        .pos = sliding_left_border
               + person_data[p1].ancestors_and_siblings_width
               + person_data[p2].ancestors_and_siblings_width/2.0
      });
      ++next_generation_counter;
    }

  } 
  else {

    auto nonzero_parent = p1 != 0 ? p1 : p2;
    if(db->getParentsCoupleId(nonzero_parent) == 0){
      placement.push_back({
        .id = nonzero_parent,
        .pos = sliding_left_border + person_data[nonzero_parent].ancestors_and_siblings_width/2.0
      });
      ++next_generation_counter;
    }
    

    
  }

  //2. Determine center and increment left border
  if (p1 != 0 || p2 != 0) {
    placement_center =
        sliding_left_border + person_data[p1].ancestors_and_siblings_width;

    sliding_left_border += person_data[p1].ancestors_and_siblings_width +
                   person_data[p2].ancestors_and_siblings_width;    
    
  } else  {
    auto nonzero_parent = p1 != 0 ? p1 : p2;
    placement_center =
        sliding_left_border + person_data[nonzero_parent].ancestors_and_siblings_width / 2.0;

    sliding_left_border += person_data[nonzero_parent].ancestors_and_siblings_width;
  }

  SPDLOG_DEBUG("COUPLE ({}, {}) SLIDING LEFT BORDER : {}", p1, p2,  sliding_left_border);
  SPDLOG_DEBUG("COUPLE ({}, {}) PLACEMENT CENTER : {}", p1, p2, placement_center);

  // 3. PLace children
  auto lower_nodes = get_children_to_place(couple_id);

  auto pos = placement_center - lower_nodes.size() / 2.0;
  for (auto lower_node_id : lower_nodes) {
    placement.push_back({.id = lower_node_id, .pos = pos});
    pos++;
  }

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
