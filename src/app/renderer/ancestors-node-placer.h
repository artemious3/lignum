
#include "DB.h"
#include "render-preprocessor.h"
class AncestorNodePlacer {
public:
  struct ancestor_family_placement{
    double center_pos;
    int children_count;
  };
  struct person_placement{
    id_t id;
    double pos;
  };

private:
  const RenderPreprocessor::data &prep_data;

  double sliding_left_border;
  double cluster_left_border = 0;

  int next_generation_counter = 0;
  int this_generation_counter = 1;

  int index = 0;


public:
  AncestorNodePlacer(const RenderPreprocessor::data &prep_data, mftb::DB* );

  void set_left_border(double lpos);

  std::vector<person_placement> place_couple(id_t couple_id);

  std::vector<id_t> get_children_to_place(id_t couple_id);
  void add_ancestor_family();


private:
  void next_family();
  mftb::DB* db;


};