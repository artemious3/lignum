
#include "DB.h"
#include "render-preprocessor.h"
class AncestorNodePlacer {
public:

  struct ancestor_placer_entry{
    std::pair<id_t, id_t> primary_persons;
    id_t excluded_partner_of_child;
    double left_border;
    
  };

   struct person_placement{
    id_t id;
    double pos;
    bool is_secondary;
  };

  struct couple_placement{
	  std::optional<double> connector_pos_x;
  };


private:
  const RenderPreprocessor::data &prep_data;

  double sliding_left_border;
  double cluster_left_border = 0;

  int next_generation_counter = 0;
  int this_generation_counter = 1;

  int index = 0;

  std::vector<ancestor_placer_entry> next_placement_entries;
  std::vector<ancestor_placer_entry> current_placement_entries;


public:
  AncestorNodePlacer(const RenderPreprocessor::data &prep_data, mftb::FamilyTreeModel* );

  void init_placement(double lpos, id_t ignored_partner, std::pair<id_t,id_t> primary_persons);

  std::pair< std::vector<person_placement>, couple_placement> place_family(id_t couple_id);

  std::vector<id_t> get_children_to_place(id_t couple_id, id_t except_partner);
  void add_ancestor_family();


private:
  void next_family();
  mftb::FamilyTreeModel* db;


};
