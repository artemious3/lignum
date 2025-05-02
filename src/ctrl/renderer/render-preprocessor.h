#pragma once
#include "FamilyTreeModel.h"
#include "entities.h"
#include <queue>
#include <unordered_map>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

class FamilyTreeView;

class RenderPreprocessor {

public:

  struct person_preprocessing_data {
    int descendants_width;
    int ancestors_and_siblings_width;
    double x;
    int relative_generation;

    bool ancestors_processed;
    bool descendants_processed;

    bool has_parents_specified = false;
  };
  struct couple_preprocessing_data {
    int hourglass_descendants_width;
    int ancestors_and_children_width;
    int children_count;
    bool has_grandchildren;
  };

  struct data{
     std::unordered_map<id_t, person_preprocessing_data> person_data;
     std::unordered_map<id_t, couple_preprocessing_data> couple_data;
  };


public:
  RenderPreprocessor(mftb::FamilyTreeModel* db);
  data preprocess_from_id(id_t id);
  static void DBG_display_preprocessor_data(FamilyTreeView* ftree, mftb::FamilyTreeModel*, id_t);

private:
  std::unordered_map<id_t, person_preprocessing_data> person_data;
  std::unordered_map<id_t, couple_preprocessing_data> couple_data;
  std::queue<id_t> preprocess_queue;

  std::pair<int, int> accumulate_children_width_and_count(id_t couple_id);
  int accumulate_children_count(id_t couple_id);

  mftb::FamilyTreeModel *db;

  void process_ancestors(id_t id);
  void process_descendants(id_t id);


  static const int STACK_LIMIT = 32268;
};
