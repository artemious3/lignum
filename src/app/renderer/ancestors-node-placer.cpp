#include "ancestors-node-placer.h"
#include "SqlDB.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <strings.h>
#include "Config.h"

AncestorNodePlacer::AncestorNodePlacer(const RenderPreprocessor::data &prep,
                                       mftb::DB *db_)
    : prep_data(prep), db(db_) {}

void AncestorNodePlacer::set_left_border(double lpos) {
  cluster_left_border = lpos;
  sliding_left_border = cluster_left_border;
  current_placement_entries.push_back({
    .excluded_partner_of_child = 0,
    .left_border = lpos
  });
}

void AncestorNodePlacer::set_globally_ignored_partner(id_t id){
	globally_ignored_partner = id;
}

std::pair< std::vector<AncestorNodePlacer::person_placement>, AncestorNodePlacer::couple_placement>
AncestorNodePlacer::place_family(id_t couple_id) {

  auto couple = db->getCoupleById(couple_id);
  auto p1 = couple->person1_id;
  auto p2 = couple->person2_id;
  assert(p1 != 0 || p2 != 0);

  //auto cfg = Config::AncestorNodePlacerConfig();
  auto person_data = prep_data.person_data;
  auto couple_data = prep_data.couple_data;
  
  sliding_left_border = current_placement_entries[index].left_border;

  double placement_center = sliding_left_border +
                            std::max((double)person_data[p1].ancestors_and_siblings_width, 
                                   (couple_data[couple_id].ancestors_and_children_width)/2.0 + 0.5);

  double precise_center = sliding_left_border + couple_data[couple_id].ancestors_and_children_width/2.0 + 0.5;

  double placement_anchor = sliding_left_border + 
                              person_data[p1].ancestors_and_siblings_width;

  //  double placement_center = sliding_left_border + 
  //                                  couple_data[couple_id].ancestors_and_children_width/2.0+0.5;

 std::vector<person_placement> placement;
  short number_of_parents_to_place = 0;
  short number_of_parents = 0;
  id_t  no_parents_person = 0;
  id_t  has_parents_person = 0;
  double single_parent_placement_pos = 0;
  double other_parent_left_border = 0;


  if (p1 != 0) {
    ++number_of_parents;
    if (db->getParentsCoupleId(p1) == 0) {
      ++number_of_parents_to_place;
      no_parents_person = p1;
      single_parent_placement_pos = placement_anchor+0.5;
      other_parent_left_border = placement_anchor;
    } else {
      has_parents_person = p1;
      ++next_generation_counter;
    }
  }
  if (p2 != 0) {
    ++number_of_parents;
    if (db->getParentsCoupleId(p2) == 0) {
      ++number_of_parents_to_place;
      no_parents_person = p2;
      single_parent_placement_pos = placement_anchor+0.5;
      other_parent_left_border = sliding_left_border; 
    } else {
      has_parents_person = p2;
      ++next_generation_counter;
      
    }
  }

  if(number_of_parents == 1){
    single_parent_placement_pos = placement_center;
    other_parent_left_border = sliding_left_border;
  }


  // double placement_center = sliding_left_border +
  //                           couple_data[couple_id].ancestors_and_children_width/2.0 + left_grandparent_width +0.5;

  SPDLOG_DEBUG("COUPLE ({}, {}) SLIDING LEFT BORDER : {}", p1, p2,
               sliding_left_border);
  SPDLOG_DEBUG("COUPLE ({}, {}) PLACEMENT CENTER : {}", p1, p2,
               placement_center);
  SPDLOG_DEBUG("COUPLE ({}, {}) PLACEMENT ANCHOR : {}", p1, p2,
               placement_anchor);


  if (number_of_parents_to_place == 2) {
      placement.push_back(
          {.id = p1,
           .pos = precise_center - 0.5});
      placement.push_back(
          {.id = p2,
           .pos = precise_center + 0.5});

    } else if (number_of_parents_to_place == 1) {

      placement.push_back({.id = no_parents_person, .pos = single_parent_placement_pos});
      if(number_of_parents == 2){
        next_placement_entries.push_back(
          {.excluded_partner_of_child = no_parents_person,
           .left_border = other_parent_left_border});
      }

      // if(number_of_parents == 1){
      //   placement.push_back({.id = no_parents_person, .pos = single_parent_placement_pos});
      // } else if(number_of_parents == 2){
      //   next_placement_entries.push_back(
      //     {.excluded_partner_of_child = 0,
      //      .left_border = sliding_left_border});
      // }
      
    }  else if (number_of_parents == 1){

        next_placement_entries.push_back(
          {.excluded_partner_of_child = 0,
           .left_border = sliding_left_border + (couple_data[couple_id].ancestors_and_children_width)/2.0
                                                -person_data[has_parents_person].ancestors_and_siblings_width/2.0});

      } else{
        next_placement_entries.push_back(
          {.excluded_partner_of_child = p2,
           .left_border = sliding_left_border});
        next_placement_entries.push_back(
          {.excluded_partner_of_child = p1,
           .left_border = placement_anchor});
      }

    // 2. Place childrens
    SPDLOG_DEBUG("EXCLUDING PARTNER {}, ",
                 current_placement_entries[index].excluded_partner_of_child);
    auto lower_nodes = get_children_to_place(
        couple_id, current_placement_entries[index].excluded_partner_of_child);

    auto pos = precise_center - lower_nodes.size() / 2.0 + 0.5;
    for (auto lower_node_id : lower_nodes) {
      placement.push_back({.id = lower_node_id, .pos = pos});
      pos++;
    }

    // sliding_left_border +=
    //     (double)couple_data[couple_id].ancestors_and_children_width;

    next_family();
    // auto center_between_parents = sliding_left_border + 3*person_data[p1].ancestors_and_siblings_width/4.0 
    //                                       + person_data[p2].ancestors_and_siblings_width/4.0;

    couple_placement couple_placement = {
      .connector_pos_x = precise_center
    };

    return {placement, couple_placement};
  }

  void AncestorNodePlacer::next_family() {
    index++;
    if (index == this_generation_counter) {
      this_generation_counter = next_generation_counter;
      next_generation_counter = 0;
      index = 0;
      current_placement_entries = std::move(next_placement_entries);
      next_placement_entries.clear();
      SPDLOG_DEBUG("NEW GENERATION");
      SPDLOG_DEBUG("THIS GENERATION COUNTER {}", this_generation_counter);
    }
  }

  void AncestorNodePlacer::add_ancestor_family() {
    // next_generation_counter++;
    // SPDLOG_DEBUG("ANCESTOR FAMILY INCEREMENTED : {}",
    // next_generation_counter);
  }

  std::vector<id_t> AncestorNodePlacer::get_children_to_place(
      id_t couple_id, id_t except_partner_of_child) {
    std::vector<id_t> children_to_place;
    auto children = db->getCoupleChildren(couple_id);

    for (auto child : children) {

      children_to_place.push_back(child);
      auto partners = db->getPersonPartners(child);
      for (auto partner : partners) {

        // exclude partner that is to be placed within
        // separate ancestor tree
        if (partner != 0 && partner != except_partner_of_child && partner != globally_ignored_partner) {
          children_to_place.push_back(partner);
        }
      }
    }

    return children_to_place;
  }
