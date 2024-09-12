#pragma once
#include "DB.h"
#include "balancer-preprocessor.h"
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

using namespace mftb;



class FamilyTreeCluster {
public:
  struct person_data {
    bool is_secondary_to_this_cluster = false;
    bool processed = false;
    double x = 0;
    int couple_counter = 0;
  };

  struct couple_data{
    int family_line_y_bias = 1;
    double family_line_connection_point_x;
  };


  struct node{
    id_t primary_person;
    std::optional<id_t> couple_id;
  };


  struct couple_children_placement {
    int children_count;
    double left_border;
  };


private:
  std::unordered_map<id_t, person_data> persons_placement;
  std::unordered_map<id_t, couple_data> couple_placement;

  std::set<id_t> new_cluster_candidates;
  int left_x = 0, right_x = 0;

  // TODO : think about this kostyl`
  std::pair<int, int> last_placement_borders;


  std::vector<couple_children_placement> last_generation_ccp;
  std::vector<couple_children_placement> new_generation_ccp;
  int ccp_idx = 0;
  int ccp_person_counter = 0;


  mftb::DB *const db;
  const FamilyTreeBalancerPreprocessor::data &preprocessor_data;

private:
  FamilyTreeCluster(mftb::DB *db_,
                    const FamilyTreeBalancerPreprocessor::data &data);
  void place_descendants(id_t);
  std::pair<int, int> getPlacementBorders(id_t id);



   std::vector<node> getLowerNodes(node couple_id);
   std::vector<id_t> processPartnersWithNoParents(id_t);

   double place_person(id_t id, std::pair<int, int> borders, double pos = 0.5);

   void ccp_next_person(double& left_border);
   void ccp_add_couple(double left_border, id_t couple_id);
   double ccp_new_generation();


public:
  static FamilyTreeCluster
  fromCouple(DB *db, const FamilyTreeBalancerPreprocessor::data &data, id_t id);

  std::pair<const std::unordered_map<id_t, person_data>&,
           const std::unordered_map<id_t, couple_data>&> getPlacementData();
};