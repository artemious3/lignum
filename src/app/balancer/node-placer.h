
/*
 * This is a class, used by Cluster in order to keep position of where
 * to place children in the next generation, so that children are right under
 * the parents.
 *
 * While placing the nodes, the data from previous generation is used to
 * place children right under the couple, and the data for new generation is
 * generated as well. When the whole generation is placed, the data for new
 * generation is used as an old - and the new generation is built again. That's
 * why it's TwoSlidingGenerations
 */

#include "balancer-preprocessor.h"
#include "datamodel.h"
#include <vector>


#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE


class NodePlacer {
private:
  struct couple_children_placement {
    int children_count;
    double left_border;
  };

  const FamilyTreeBalancerPreprocessor::data &preprocessor_data;

  std::vector<couple_children_placement> last_generation_data;
  std::vector<couple_children_placement> new_generation_data;
  int index = 0;
  int person_counter = 0;

  double sliding_left_border;

  id_t primary_person;

public:
  struct partner_placement_data {
    double partner_pos;
    double family_connector_point_x;
  };

  NodePlacer(const FamilyTreeBalancerPreprocessor::data &prep_data);

  void init_placement_from_couple(double left_border, id_t couple_id);

  double new_primary_person(id_t id);
  partner_placement_data new_partner(id_t primary_person,
                                               id_t couple_with_primary_person);
  void new_zero_partner(id_t couple_id);

  void skip_previously_placed_couple(id_t couple_id);

  void next();

  struct parameters_t {
    double primary_person_border_increment = 1;
    double zero_partner_children_left_border_decrement = 1;
    double nonzero_partner_children_left_border_decrement = 1;
    double couple_right_pos_and_next_border_diff = 1;
  };

  parameters_t PARAMETERS;

private:
  void add_couple(double left_border, id_t couple_id);
  void new_generation();
};