#pragma once
#include "DB.h"
#include "datamodel.h"
#include <queue>
#include <unordered_map>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

class FamilyTreeItem;

class RenderPreprocessor {

public:

  struct person_preprocessing_data {
    int width;
    double x;
    int relative_generation;
    bool ancestors_processed;
    bool descendants_processed;
  };
  struct couple_preprocessing_data {
    int hourglass_descendants_width;
    int children_count;
  };

  struct data{
     std::unordered_map<id_t, person_preprocessing_data> person_data;
     std::unordered_map<id_t, couple_preprocessing_data> couple_data;
  };


public:
  RenderPreprocessor(mftb::DB* db);
  data preprocess_from_id(id_t id);
  static void display_preprocessor_data(FamilyTreeItem* ftree, mftb::DB*, id_t);

private:
  std::unordered_map<id_t, person_preprocessing_data> person_data;
  std::unordered_map<id_t, couple_preprocessing_data> couple_data;
  std::queue<id_t> preprocess_queue;

  std::pair<int, int> accumulate_children_width_and_count(id_t couple_id);

  mftb::DB *db;

  void process_ancestors(id_t id);
  void process_descendants(id_t id);


  static const int STACK_LIMIT = 32268;
};