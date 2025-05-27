#include "ancestors-node-placer.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <qtextoption.h>
#include <strings.h>
#include "Config.h"

AncestorNodePlacer::AncestorNodePlacer(const RenderPreprocessor::data &prep,
                                       mftb::FamilyTreeModel *db_)
    : prep_data(prep), db(db_) {}

void AncestorNodePlacer::init_placement(double lpos, id_t ignored_partner, std::pair<id_t,id_t> primary_persons) {
  cluster_left_border = lpos;
  sliding_left_border = cluster_left_border;
  current_placement_entries.push_back({
    .primary_persons = primary_persons,
    .excluded_partner_of_child = ignored_partner,
    .left_border = lpos,
  });
}


std::pair< std::vector<AncestorNodePlacer::person_placement>, AncestorNodePlacer::couple_placement>
AncestorNodePlacer::place_family(id_t couple_id) {
  auto cfg = Config::AncestorNodePlacerConfig();

  auto couple = db->getCoupleById(couple_id);
  auto p1 = couple->person1_id;
  auto p2 = couple->person2_id;
  assert(p1 != 0);

  //auto cfg = Config::AncestorNodePlacerConfig();
  auto person_data = prep_data.person_data;
  auto couple_data = prep_data.couple_data;

  auto ancestors_and_children_width = couple_data[couple_id].ancestors_and_children_width + cfg.distance_between_families; 
  
  sliding_left_border = current_placement_entries[index].left_border;

  double precise_center = sliding_left_border + ancestors_and_children_width/2.0 + cfg.distance_between_families/2.0 + 0.5;

  double placement_anchor = sliding_left_border + 
                              person_data[p1].ancestors_and_siblings_width;

  auto lower_nodes = get_children_to_place(
        couple_id, current_placement_entries[index].excluded_partner_of_child);
  SPDLOG_DEBUG("PLACING COUPLE ID {}", couple_id);
  SPDLOG_DEBUG("COUPLE ({}, {}) SLIDING LEFT BORDER : {}", p1, p2,
               sliding_left_border);
  SPDLOG_DEBUG("COUPLE ({}, {}) PRECISE CENTER : {}", p1, p2,
               precise_center);
  SPDLOG_DEBUG("COUPLE ({}, {}) PLACEMENT ANCHOR : {}", p1, p2,
               placement_anchor);



  std::vector<person_placement> placement;


  std::optional<double> couple_connector_point_x;

  // difference between couple width, including children,
  // and separate parents' width
  if(p2 != 0){
	  // both parents specified 
	  auto p1_has_parents = (db->getParentsCoupleId(p1) != 0);
	  auto p2_has_parents = (db->getParentsCoupleId(p2) != 0);

	  if(p1_has_parents && p2_has_parents ){
		  double centering_correction = 
			  (ancestors_and_children_width
			  	- person_data[p1].ancestors_and_siblings_width
				- person_data[p2].ancestors_and_siblings_width)/2.0;

		  next_placement_entries.push_back({.primary_persons = {p1,p2},
				  .excluded_partner_of_child = p2,
				  .left_border = sliding_left_border + centering_correction,
				  });
		  next_placement_entries.push_back({.primary_persons = {p1,p2},
				  .excluded_partner_of_child = p1,
				  .left_border = placement_anchor + centering_correction,
				  });
                  couple_connector_point_x =
                      std::min(placement_anchor + centering_correction + 0.5,
                               precise_center + centering_correction);
          }
	  else if(p1_has_parents  xor  p2_has_parents ){
		  SPDLOG_DEBUG("1 of 2 COUPLE MEMBERS HAS PARENTS");
		  double ancestors_width = p1_has_parents ? 
			  person_data[p1].ancestors_and_siblings_width :
			  person_data[p2].ancestors_and_siblings_width;

                  double centering_correction =
                      (ancestors_and_children_width -
		       ancestors_width - 
		         cfg.distance_between_families)/2.0;
                  next_placement_entries.push_back(
                      {.primary_persons = {p1,p2},
		      .excluded_partner_of_child = 0,
                       .left_border = sliding_left_border + centering_correction});

                  couple_connector_point_x = {};
      } else {
		  placement.push_back({.id = p1,
				  .pos = precise_center-0.5});
		  placement.push_back({.id=p2,
				  .pos = precise_center+0.5});
		  couple_connector_point_x = precise_center;
	  }

  } else {
	  // only p1 is specified
	  if(db->getParentsCoupleId(p1) == 0){
		  placement.push_back({.id = p1,
				  .pos = precise_center});
	  } else {
		  SPDLOG_DEBUG("1 of 1 COUPLE MEMBERS HAS PARENTS");
		  double centering_correction = 
			  (ancestors_and_children_width - person_data[p1].ancestors_and_siblings_width)/2.0;
		  next_placement_entries.push_back({.primary_persons = {p1,p2},
				  .excluded_partner_of_child = 0,
				  .left_border = sliding_left_border + centering_correction, 
				  });
	  }
	  couple_connector_point_x = precise_center;

  }


    // 2. Place children
    SPDLOG_DEBUG("EXCLUDING PARTNER {}, ",
                 current_placement_entries[index].excluded_partner_of_child);

    auto pos = precise_center - (double)lower_nodes.size() / 2.0 + 0.5;
    for (auto lower_node_id : lower_nodes) {
      placement.push_back({.id = lower_node_id,
                           .pos = pos,
                           .is_secondary = lower_node_id != current_placement_entries[index].primary_persons.first &&
			   		   lower_node_id != current_placement_entries[index].primary_persons.second});
      pos++;
    }

    next_family();

    couple_placement couple_placement = {
      .connector_pos_x = couple_connector_point_x 
    };

    return {placement, couple_placement};
  }

  void AncestorNodePlacer::next_family() {
    index++;
    if (index == current_placement_entries.size()) {
      index = 0;
      current_placement_entries = std::move(next_placement_entries);
      next_placement_entries.clear();
      SPDLOG_DEBUG("NEW GENERATION");
      SPDLOG_DEBUG("THIS GENERATION COUNTER {}", this_generation_counter);
    }
  }


  std::vector<id_t> AncestorNodePlacer::get_children_to_place(
      id_t couple_id, id_t except_partner_of_child) {
    std::vector<id_t> children_to_place;
    auto children = db->getCoupleChildren(couple_id);

    for (auto child : children) {

      auto partners = db->getPersonPartners(child);
      children_to_place.push_back(child);
      for (auto partner : partners) {

        // exclude partner that is to be placed within
        // separate ancestor tree
        if (partner != 0 && partner != except_partner_of_child) {
          children_to_place.push_back(partner);
        }
      }
    }

    return children_to_place;
  }
