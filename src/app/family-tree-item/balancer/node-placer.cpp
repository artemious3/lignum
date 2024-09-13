
#include "node-placer.h"
#include <qdebug.h>
#include <qlogging.h>
#include <stdexcept>

NodePlacer::NodePlacer(const FamilyTreeBalancerPreprocessor::data &prep_data)
    : preprocessor_data(prep_data) {}

void NodePlacer::new_generation() {
  last_generation_data = std::move(new_generation_data);
  person_counter = 0;
  index = 0;
}

void NodePlacer::add_couple(double left_border, id_t couple_id) {
  auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
  if (couple_data.children_count != 0) {
    couple_children_placement ccp;
    ccp.children_count = couple_data.children_count;
    ccp.left_border = left_border;
    new_generation_data.push_back(ccp);
  }
}

double NodePlacer::new_primary_person(id_t id) {
  qDebug() << "new primary person " << id;
  auto position = sliding_left_border;
  sliding_left_border += parameters.primary_person_border_increment;
  return position;
}

void NodePlacer::NodePlacer::init_placement_from_couple(double left_border,
                                                        id_t couple_id) {
  last_generation_data.push_back(
      {.children_count = 1, .left_border = left_border});
}

void NodePlacer::NodePlacer::pass_zero_partner(id_t couple_id) {

  auto couple_data = preprocessor_data.couple_data.find(couple_id)->second;
  add_couple(sliding_left_border -
                 parameters.zero_partner_children_left_border_decrement,
             couple_id);
  sliding_left_border += couple_data.hourglass_descendants_width;
}

NodePlacer::partner_placement_data
NodePlacer::NodePlacer::get_partner_placement(id_t primary_person,
                                              id_t couple_with_primary_person) {

  auto couple_data =
      preprocessor_data.couple_data.find(couple_with_primary_person)->second;

  auto couple_left_pos =
      sliding_left_border -
      parameters.nonzero_partner_children_left_border_decrement;

  double couple_right_pos =
      couple_left_pos + std::max((double)couple_data.hourglass_descendants_width, 0.5);

  add_couple(couple_left_pos, couple_with_primary_person);

  double connection_point = (couple_left_pos + couple_right_pos - 0.5) / 2;
  sliding_left_border =
      couple_right_pos + parameters.couple_right_pos_and_next_border_diff;

  return {.partner_pos = couple_right_pos,
          .family_connector_point_x = connection_point};
}

void NodePlacer::next() {

  if (last_generation_data.empty()) {
    return;
  }

  ++person_counter;
  qDebug() << "INCREMENTING PERSON COUNTER: " << person_counter;
  if (person_counter == last_generation_data[index].children_count) {
    ++index;
    person_counter = 0;
    if (index >= last_generation_data.size()) {
      qDebug() << "NEW GENERATION";
      new_generation();
    }
    if (index < last_generation_data.size()) {
      sliding_left_border = last_generation_data.at(index).left_border;
      qDebug() << "RESET LEFT BORDER TO " << sliding_left_border;
    }
  }
}
