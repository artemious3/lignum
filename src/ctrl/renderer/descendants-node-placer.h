
#pragma once
#include "render-preprocessor.h"
#include "entities.h"
#include <vector>

/* DescendantsNodePlacer is an entity that places descendants of couple.
 *  
 * It's crucial state variable is `sliding_left_border` -- a left border 
 * of a block, where person will be placed. 
 *
 *  left-border of a person without partners of descendants ("leaf"):
 *      ________
 *      |      |
 *  >|  o  |   o  | 
 *      
 *
 *
 *  left border of a couple with descendants:
 *  >|      o  o      |
 *          |__|
 *      _____|______
 *      |    |      |
 *
 *  
 *  Besides, the entity keeps two lists of nodes. Their sequence is the same 
 *  as they are placed from left to right. 
 *
 *    |          |   |      |   |
 *    1   2      3   4      5   6   7    >>>last_generation_data
 *    |___|      |              |___|
 *      |        |               | |
 *      1        2               3 4     >>>new_generation_data
 *
 *
 *  Basically, when the DescendantsNodePlacer is requested to add a node 
 *  it does 2 things:
 *         1. Return the placement info of the given node based on 
 *            placement of LAST generation (so that descendants 
 *            are placed exactly under the parents)
 *
 *         2. Add the given node into NEW generation and advance `sliding_left_border`.
 *
 *  After DescendantsNodePlacer detects that all nodes of `new_generation_data` were placed,
 *  the `last_generation_data` is assigned to `new_generation_data`, and `new_generation_data` is 
 *  cleared. 
 *  
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
	/*
	 * Struct, representing a node from point of view of DescendantsNodePlacer.
	 */
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
