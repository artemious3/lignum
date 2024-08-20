#if 0
#include "tree-balancer.h"
#include "family-tree-item.h"
#include "individual-item.h"
#include <cstdint>
#include <memory>
#include <qassert.h>
#include <qforeach.h>
#include <qlogging.h>

void TreeBalancer::balanceTree() {
  bool build_tree_success;
  buildInternalTree(&build_tree_success);

  if (!build_tree_success) {
    return;
  }
}

void TreeBalancer::buildInternalTree(bool *success) {
  *success = true;

  auto *arbirtrary_person = family_tree.findChild<PersonItem *>();
  if (arbirtrary_person == nullptr) {
    qWarning("Tree is empty, hence can not be balanced");
    *success = false;
    return;
  }

  auto arbitrary_person_id = arbirtrary_person->getId();
  this->internal_tree = makeTreeNodeForPerson(arbitrary_person_id, 0);

  computeBalancerData();
}

std::shared_ptr<tree_node>
TreeBalancer::makeTreeNodeForPerson(uint32_t id, uint32_t came_from_id) {
  Q_ASSERT(id != came_from_id);

  auto *person = family_tree.getPersonById(id);
  if (person == nullptr) {
    return nullptr;
  }

  std::shared_ptr<tree_node> node = std::make_shared<tree_node>();
  node->person = person;

  auto parents_id = family_connectors.getParents(id);
  if (parents_id.first != came_from_id) {
    node->parent1 = makeTreeNodeForPerson(parents_id.first, id);
  }
  if (parents_id.second != came_from_id) {
    node->parent2 = makeTreeNodeForPerson(parents_id.second, id);
  }

  auto children_ids = family_connectors.getChildren(id);
  foreach (auto child_id, children_ids) {
    if (child_id != came_from_id) {
      node->children.push_back(makeTreeNodeForPerson(child_id, id));
    }
  }
}

void TreeBalancer::computeBalancerData() {
  processBalancerDataDownwards(internal_tree, 0);
  passBalancerDataUpwards(internal_tree->parent1, internal_tree);
  passBalancerDataUpwards(internal_tree->parent2, internal_tree);
}

void TreeBalancer::processBalancerDataDownwards(std::shared_ptr<tree_node> node,
                                                int parent_generation) {

  const int current_generation = parent_generation - 1;

  if (node->children.empty()) {
    node->balancer_data = {0, current_generation};
    return;
  }

  balancer_data data;
  data.generation = current_generation;

  int sum_leaf_descendants = 0;
  foreach (auto child, node->children) {
    processBalancerDataDownwards(child, current_generation);
    sum_leaf_descendants += child->balancer_data.leaf_descentands;
  }

  data.leaf_descentands = sum_leaf_descendants;

  node->balancer_data = data;
}

void TreeBalancer::passBalancerDataUpwards(
    std::shared_ptr<tree_node> node,
    std::shared_ptr<tree_node> child_come_from) {

  const int current_generation = child_come_from->balancer_data.generation + 1;

  if (node == nullptr) {
    return;
  }

  balancer_data data;

  data.generation = current_generation;
  int leaf_descendants_sum = 0;
  foreach (auto child, node->children) {
    if (child != child_come_from) {
      processBalancerDataDownwards(child, current_generation);
    }

    leaf_descendants_sum += child->balancer_data.leaf_descentands;
  }

  data.leaf_descentands = leaf_descendants_sum;

  passBalancerDataUpwards(node->parent1, node);
  passBalancerDataUpwards(node->parent2, node);
}

void TreeBalancer::setItemsPosition(std::shared_ptr<tree_node> node) {
  static generatio
  auto *const person_item = node->person;
  auto balancer_data = node->balancer_data;

  person_item->setX()

}
#endif
