
#include "FamilyTreeModel.h"
#include <cstddef>
#include <map>
#include <set>
#include <stack>

class FamilyTreeView;


class FamilyTreeBuilder {

public:
  struct processing_flags{
    bool ancestors_processed : 1 = false;
   bool descendants_processed : 1 = false;
  };

  FamilyTreeBuilder(FamilyTreeView *family_tree, mftb::FamilyTreeModel* db);
  void build_tree_from(id_t id);

private:
  std::pair<id_t, Couple> put_parents_onto_stack(std::stack<id_t>& stack, id_t person);
  void add_all_people();
  void process_ancestors(id_t);
  void process_descendants(id_t);
  bool need_to_process(id_t) const;

  void set_processed_ancestors_flag(id_t);
  void set_processed_descendants_flag(id_t);

  FamilyTreeView *const family_tree;
  mftb::FamilyTreeModel* const db;

  std::map<id_t, processing_flags> flags;
  std::set<id_t> ids_to_process;

  static constexpr size_t STACK_LIMIT = 32768;

};
