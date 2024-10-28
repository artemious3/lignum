#include "family-tree-builder.h"
#include "family-connector.h"
#include "family-tree-item.h"
#include "abstract-person-item.h"
#include <stack>
#include <stdexcept>

FamilyTreeBuilder::FamilyTreeBuilder(FamilyTreeItem *family_tree_, mftb::DB *db_)
    : family_tree(family_tree_), db(db_) {}

void FamilyTreeBuilder::add_all_people() {
  for (auto id : ids_to_process) {
    auto person = db->getPersonById(id).value();
    family_tree->addPersonWithId(id, person);
  }
}

void FamilyTreeBuilder::build_tree_from(id_t start_id) {
  auto people_ids = db->getPeopleIds();
  ids_to_process = std::set<id_t>(people_ids.begin(), people_ids.end());

  add_all_people();

  id_t current_id = start_id;

  while (!ids_to_process.empty()) {
    current_id = *ids_to_process.begin();

    if (!flags[current_id].ancestors_processed) {
      process_ancestors(current_id);
      set_processed_ancestors_flag(current_id);
    }

    if (!flags[current_id].descendants_processed) {
      process_descendants(current_id);
      set_processed_descendants_flag(current_id);
    }
  }
  
  flags.clear();
}

// source:
// https://stackoverflow.com/questions/5987867/traversing-a-n-ary-tree-without-using-recurrsion

void FamilyTreeBuilder::process_descendants(id_t start_id) {
  std::stack<id_t> stack;
  stack.push(start_id);

  while (!stack.empty() && stack.size() < STACK_LIMIT) {

    id_t current = stack.top();
    stack.pop();

    std::vector<id_t> partners = db->getPersonPartners(current);
    for (auto partner : partners) {

      if (flags[partner].descendants_processed) {
        continue;
      }

      auto couple_id = db->getCoupleIdByPersons(current, partner).value();
      auto couple = db->getCoupleById(couple_id).value();
      auto children = db->getParentsChildren(current, partner);
      auto *family = family_tree->getFamilyWithCoupleId(couple_id);

      if (family == nullptr) {
        family = family_tree->addFamilyWithCoupleId(couple_id, couple, {});
        qDebug() << "added family with couple_id " << couple_id;
      }

      for (auto child : children) {
        AbstractPersonItem *child_item = family_tree->getPersonItemById(child);
        family->addChild(child_item);
        stack.push(child);
      }
    }

    set_processed_descendants_flag(current);
  }

  if (stack.size() >= STACK_LIMIT) {
    throw std::runtime_error("Stack overflow");
  }
}


void FamilyTreeBuilder::process_ancestors(id_t start_id) {

  std::stack<id_t> stack;
  put_parents_onto_stack(stack, start_id);

  while (!stack.empty() && stack.size() < STACK_LIMIT) {

    id_t top = stack.top();
    stack.pop();

    auto [couple_id, couple] = put_parents_onto_stack(stack, top);
    if(couple_id != 0){
      family_tree->addFamilyWithCoupleId(couple_id, couple, {});
    }

    set_processed_ancestors_flag(top);
  }

  if (stack.size() >= STACK_LIMIT) {
    throw std::runtime_error("Stack overflow");
  }
}

void FamilyTreeBuilder::set_processed_ancestors_flag(id_t id) {
  flags[id].ancestors_processed = true;

  if (flags[id].descendants_processed) {
    ids_to_process.erase(id);
  }
}

void FamilyTreeBuilder::set_processed_descendants_flag(id_t id) {
  flags[id].descendants_processed = true;
  if (flags[id].ancestors_processed) {
    ids_to_process.erase(id);
  }
}

bool FamilyTreeBuilder::need_to_process(id_t id) const {
  return ids_to_process.contains(id);
}

std::pair<id_t, Couple> FamilyTreeBuilder::put_parents_onto_stack(std::stack<id_t> &stack,
                                              id_t person_id) {
  auto parents_couple_id = db->getParentsCoupleId(person_id).value();
  if (parents_couple_id == 0) {
    return {0, {}};
  }

  auto couple = db->getCoupleById(parents_couple_id).value();
  if (couple.person1_id != 0 && !flags[couple.person1_id].ancestors_processed) {
    stack.push(couple.person1_id);
  }
  if (couple.person2_id != 0 && !flags[couple.person2_id].ancestors_processed) {
    stack.push(couple.person2_id);
  }

  return {parents_couple_id, couple};
  
}
