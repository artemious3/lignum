

#include "family-connector.h"
#include "family-connectors-db.h"
#include "individual-item.h"
#include <cstdint>
#include <memory>
#include <vector>


struct balancer_data{
  int leaf_descentands;
  int generation;
};

struct tree_node{
  PersonItem* person = nullptr;
  balancer_data balancer_data;
  std::shared_ptr<tree_node> parent1, parent2;
  std::vector< std::shared_ptr<tree_node> > children;
};


class TreeBalancer {

public:
  TreeBalancer(const FamilyDB &family_connectors,
               FamilyTreeItem &ftree);

  void balanceTree();

private:

  void buildInternalTree(bool* success);
  void computeBalancerData();

  void setItemsPosition(std::shared_ptr<tree_node> node);

  std::shared_ptr<tree_node> makeTreeNodeForPerson(uint32_t id, uint32_t came_from);

  void processBalancerDataDownwards(std::shared_ptr<tree_node> node, int current_generation);
  void passBalancerDataUpwards(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> child);




  const FamilyDB &family_connectors;
  FamilyTreeItem &family_tree;

  std::shared_ptr<tree_node> internal_tree;

  qreal xDistanceBetweenLeafs = 10;
  qreal yDistanceBetweenGenerations = 10;
};