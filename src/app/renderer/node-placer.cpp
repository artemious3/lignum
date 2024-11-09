
#include "node-placer.h"
#include "SqlDB.h"
#include "spdlog/common.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <qdebug.h>
#include <qlogging.h>
#include <stdexcept>

DescendantsNodePlacer::DescendantsNodePlacer(const RenderPreprocessor::data &prep_data)
    : preprocessor_data(prep_data) {}

void DescendantsNodePlacer::new_generation() {
  last_generation_data = std::move(new_generation_data);
  person_counter = 0;
  index = 0;
  if (!last_generation_data.empty()) {
    sliding_left_border = last_generation_data[0].left_border;
    SPDLOG_DEBUG("RESET LEFT BORDER TO {}", sliding_left_border);
  }
}

void DescendantsNodePlacer::add_couple_to_new_generation(double left_border, id_t couple_id) {
  auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
  if (couple_data.children_count != 0) {
    couple_children_placement ccp;
    ccp.children_count = couple_data.children_count;
    ccp.left_border = left_border;
    new_generation_data.push_back(ccp);

    leftmost_person_x = std::min(leftmost_person_x, ccp.left_border);
    rightmost_person_x = std::max(rightmost_person_x, ccp.left_border + couple_data.hourglass_descendants_width);
  }
}

// double NodePlacer::new_primary_person(id_t id) {
//   sliding_left_border += PARAMETERS.primary_person_border_increment;
//   SPDLOG_DEBUG("NEW PRIMARY PERSON {}", id);
//   auto position = sliding_left_border;
//   this->primary_person_changed = true;
//   return position;
// }

void DescendantsNodePlacer::init_placement_from_couple(double left_border,
                                              id_t couple_id) {
  auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
  SPDLOG_DEBUG("INIT PLACEMENT FROM COUPLE {}; LEFT BORDER {}", couple_id, left_border);
  sliding_left_border = left_border;
  last_generation_data.push_back(
      {.children_count = couple_data.children_count, .left_border = left_border});
}


void DescendantsNodePlacer::next_person() {
  SPDLOG_DEBUG("NEXT");
  if (last_generation_data.empty()) {
    SPDLOG_DEBUG("LAST GENERATION DATA IS EMPTY");
    return;
  }

  ++person_counter;
  SPDLOG_DEBUG("INCREMENTING PERSON COUNTER: {}", person_counter);
  if (person_counter == last_generation_data[index].children_count) {
    ++index;
    person_counter = 0;
    if (index >= last_generation_data.size()) {
      SPDLOG_DEBUG("NEW GENERATION");
      new_generation();
    } else {
      sliding_left_border = last_generation_data.at(index).left_border;
      SPDLOG_DEBUG("RESET LEFT BORDER TO {}", sliding_left_border);
    }
  }
}

// void NodePlacer::skip_previously_placed_couple(id_t couple_id) {
//   SPDLOG_DEBUG("SKIP {}",couple_id);
//   const auto couple_data =
//       preprocessor_data.couple_data.find(couple_id)->second;
//   sliding_left_border += std::max(couple_data.hourglass_descendants_width,
//   1);
// }

DescendantsNodePlacer::node_placement_data DescendantsNodePlacer::place_node(node nd) {

  const auto *db = mftb::SqlDB::getInstance();

  bool primary_person_changed = (nd.primary_person != current_primary_person);

  node_placement_data npd;

  if (primary_person_changed) {

    current_primary_person = nd.primary_person;

    if (!nd.couple_id.has_value()) {
      // case 1 : single primary person
      npd = place_single_primary_person(nd);
      
    } else {

      auto partner =
          db->getCoupleById(*nd.couple_id)->getAnotherPerson(nd.primary_person);

      if (partner == 0) {
        // case 2 : empty partner
        npd = place_primary_person_with_empty_partner(nd);
      } else {
        // case 3 : the first nonempty partner
        npd = place_primary_person_with_first_nonempty_partner(nd);
      }
    }

  } else {
    // not the first partner
    npd = place_other_partner(nd);
  }



  return npd;
}

DescendantsNodePlacer::node_placement_data DescendantsNodePlacer::place_single_primary_person(node nd) {
      node_placement_data npd;
      SPDLOG_DEBUG("BORDER {}", sliding_left_border);
      npd.primary_person_pos = sliding_left_border + 1;
      sliding_left_border += 1;
      SPDLOG_DEBUG("PLACED SINGLE PRIMARY PERSON id:{}", nd.primary_person);

      next_person();

      return npd;
}

DescendantsNodePlacer::node_placement_data DescendantsNodePlacer::place_primary_person_with_empty_partner(node nd) {
        node_placement_data npd;
        auto couple_id = *nd.couple_id;
        auto couple_data =
            preprocessor_data.couple_data.find(couple_id)->second;
        npd.primary_person_pos =
            (sliding_left_border + 1 +
             couple_data.hourglass_descendants_width / 2);
        add_couple_to_new_generation(sliding_left_border, *nd.couple_id);
        sliding_left_border += couple_data.hourglass_descendants_width;

        SPDLOG_DEBUG("PLACED PRIMARY PERSON {} WITH EMPTY PARTNER",
                     nd.primary_person);

        next_person();

        return npd;

}

DescendantsNodePlacer::node_placement_data DescendantsNodePlacer::place_primary_person_with_first_nonempty_partner(node nd) {
        node_placement_data npd;

        SPDLOG_DEBUG("BORDER {}", sliding_left_border);
        auto couple_id = *nd.couple_id;
        auto couple_data =
            preprocessor_data.couple_data.find(couple_id)->second;

        auto width = std::max(couple_data.hourglass_descendants_width, 2);
        auto center = sliding_left_border + 0.5 + width/2.0;

        add_couple_to_new_generation(sliding_left_border, *nd.couple_id);

        npd.primary_person_pos = center - 0.5;
        npd.partner_pos = center + 0.5;
        
        npd.family_connector_point_x = center;

        sliding_left_border += width;

        SPDLOG_DEBUG("POSITIONS {} {}", *npd.primary_person_pos, *npd.partner_pos);

        SPDLOG_DEBUG("PLACED PRIMARY PERSON {} WITH FIRST PARTNER", nd.primary_person);

        next_person();
        next_person();


        return npd;

}

DescendantsNodePlacer::node_placement_data DescendantsNodePlacer::place_other_partner(node nd) {
     node_placement_data npd;
  // case 4 : second or other nonempty partner
    assert(nd.couple_id.has_value() && nd.couple_id != 0);

    auto couple_id = *nd.couple_id;
    auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
    auto width = std::max(couple_data.hourglass_descendants_width, 1);

    auto center = sliding_left_border + 0.5 + (double)width/2;

    npd.partner_pos =
        sliding_left_border + width;
    npd.family_connector_point_x = center;

    add_couple_to_new_generation(sliding_left_border+0.5, *nd.couple_id);

    SPDLOG_DEBUG("BORDER {}", sliding_left_border);

    sliding_left_border += width;

    SPDLOG_DEBUG("BORDER {}", sliding_left_border);

    SPDLOG_DEBUG("PLACED PRIMARY PERSON {} WITH NON-FIRST PARTNER", nd.primary_person);

    next_person();

    return npd;
}

std::pair<double, double> DescendantsNodePlacer::getPlacementBorders() const {
  return {leftmost_person_x, rightmost_person_x};
}
