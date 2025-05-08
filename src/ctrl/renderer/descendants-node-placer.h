#pragma once
#include "render-preprocessor.h"
#include "entities.h"
#include <vector>


/* NodePlacer is kind of machine that stores a sliding window,
 * consisting of 2 generations - currently being rendered and 
 * next to be rendered. 
 *
 * While placing the nodes, the data from previous generation is used to
 * place children right under the couple of their parents, and the data for new
 * generation is generated as well. When the whole generation is placed, the data for new
 * generation is used as an old - and the new generation is built again. 
 */
 
 class DescendantsNodePlacer {
private:
  struct couple_children_placement {
    int children_count;
    double left_border;
  };

  struct node_placement_data{
    std::optional<double> primary_person_pos;
    std::optional<double> partner_pos;
    std::optional<double> family_connector_point_x;
  };

  const RenderPreprocessor::data &preprocessor_data;

  std::vector<couple_children_placement> last_generation_data;
  std::vector<couple_children_placement> new_generation_data;
  int index = 0;
  int person_counter = 0;
  double sliding_left_border;
  id_t current_primary_person = 0;

  double leftmost_person_x;
  double rightmost_person_x;
  
public:
  struct node{
    id_t primary_person;
    std::optional<id_t> couple_id;
  };

  DescendantsNodePlacer(const RenderPreprocessor::data &prep_data);

  void init_placement_from_couple(double left_border, id_t couple_id);

  node_placement_data place_node(node nd);

  std::pair<double, double> getPlacementBorders() const;


  

  struct parameters_t {
    double primary_person_border_increment = 1;
    double zero_partner_children_left_border_decrement = 1;
    double nonzero_partner_children_left_border_decrement = 1;
    double couple_right_pos_and_next_border_diff = 1;
  };

  parameters_t PARAMETERS;

private:
  void add_couple_to_new_generation(double left_border, id_t couple_id);
  void new_generation();
  void next_person();


  node_placement_data place_single_primary_person(node nd);
  node_placement_data place_primary_person_with_empty_partner(node nd);
  node_placement_data place_primary_person_with_first_nonempty_partner(node nd);
  node_placement_data place_other_partner(node nd);
  
};
