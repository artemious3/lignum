#include "balancer-preprocessor.h"
#include "DB.h"
#include "datamodel.h"
#include "family-tree-item.h"
#include "spdlog/spdlog.h"
#include "tree-traversal.h"
#include <functional>
#include <qapplication.h>
#include <qgraphicsitem.h>
#include <stack>
#include <stdexcept>



FamilyTreeBalancerPreprocessor::FamilyTreeBalancerPreprocessor(mftb::DB *db_)
    : db(db_) {}

FamilyTreeBalancerPreprocessor::data
FamilyTreeBalancerPreprocessor::preprocess_from_id(id_t id) {
  person_data.clear();
  couple_data.clear();

  person_data[id].relative_generation = 0;
  preprocess_queue.push(id);

  while (!preprocess_queue.empty()) {
    auto target = preprocess_queue.front();
    preprocess_queue.pop();

    if (!person_data[target].descendants_processed) {
      process_descendants(target);
    }
    if (!person_data[target].ancestors_processed) {
      process_ancestors(target);
    }
  }

  return {person_data, couple_data};
}

void FamilyTreeBalancerPreprocessor::process_ancestors(id_t id) {
  // std::stack<id_t> post_order;

  // {
  //   std::stack<id_t> traverse_stack;
  //   auto parents = db->getPersonParentsById(id);
  //   traverse_stack.push(parents.first);
  //   traverse_stack.push(parents.second);

  //   while (!traverse_stack.empty() && traverse_stack.size() < STACK_LIMIT) {
  //     auto current = traverse_stack.top();
  //     traverse_stack.pop();

  //     // idk if it works, will see on tests
  //     if (person_flags[current].ancestors_processed) {
  //       continue;
  //     }

  //     post_order.push(current);

  //     auto parents = db->getPersonParentsById(current);
  //     traverse_stack.push(parents.first);
  //     traverse_stack.push(parents.second);
  //   }

  //   if (traverse_stack.size() >= STACK_LIMIT) {
  //     throw std::runtime_error("Stack overflow");
  //   }
  // }

  // while (post_order.empty()) {

  //   auto current = post_order.top();
  //   post_order.pop();

  //   auto parents_couple_id = db->getParentsCoupleId(current).value();
  //   if (parents_couple_id == 0) {
  //     person_flags[current].width = 1;
  //   } else {
  //     auto couple = db->getCoupleById(parents_couple_id).value();
  //     auto parent1 = couple.person1_id;
  //     auto parent2 = couple.person2_id;
  //     int siblings_count = db->getCoupleChildren(parents_couple_id).size();

  //     person_flags[current].width =
  //         std::max(person_flags[parent1].width + person_flags[parent2].width,
  //                  siblings_count);

  //     preprocess_queue.push(parent1);
  //     preprocess_queue.push(parent2);
  //   }

  //   person_flags[current].ancestors_processed = true;
  // }

  auto get_nonempty_parents = [&](id_t id) {
    std::vector<id_t> non_empty_parents;
    auto parents = db->getPersonParentsById(id);
    if (parents.first != 0) {
      non_empty_parents.push_back(parents.first);
      person_data[parents.first].relative_generation =
          person_data[id].relative_generation + 1;
    }
    if (parents.second != 0) {
      non_empty_parents.push_back(parents.second);
      person_data[parents.second].relative_generation =
          person_data[id].relative_generation + 1;
    }

    return non_empty_parents;
  };

  auto inorder_process = [&](id_t current) { preprocess_queue.push(current); };

  TreeTraversal<id_t>::preorder(id, get_nonempty_parents, inorder_process,
                                false);
}

void FamilyTreeBalancerPreprocessor::process_descendants(id_t id) {

  std::stack<id_t> post_order;

  auto inorder_process = [&](id_t current) {
    if (person_data[current].descendants_processed) {
      return;
    }

    auto partners = db->getPersonPartners(current);

    for (auto partner : partners) {

      if (partner != 0 && !person_data[partner].descendants_processed) {
        person_data[partner].relative_generation =
            person_data[current].relative_generation;
        preprocess_queue.push(partner);
      }

      auto children = db->getParentsChildren(current, partner);
      for (auto child : children) {
        person_data[child].relative_generation =
            person_data[current].relative_generation - 1;
      }
    }
  };

  auto get_descendants_lambda = [&](id_t current) {
    return db->getPersonChildren(current);
  };

  auto bfs_process = [&](id_t current) {
    auto couples = db->getPersonCouplesId(current);
    int width_accumulator = 1;
    for (auto couple_id : couples) {
     
      auto [children_width, children_count] =
          accumulate_children_width_and_count(couple_id);
      
      auto partner =
          db->getCoupleById(couple_id).value().getAnotherPerson(current);

      SPDLOG_DEBUG("CHILDREN WIDTH FOR COUPLE ({}, {}) IS {}", current, partner, children_width);


      couple_data[couple_id].hourglass_descendants_width = children_width;
      couple_data[couple_id].children_count = children_count;

      if (partner == 0){
        width_accumulator += children_width-1;
      } else if(db->getParentsCoupleId(partner) == 0) {
        width_accumulator += std::max(1, children_width);
      }
    }

    person_data[current].width = width_accumulator;
    SPDLOG_DEBUG("CHILDREN WIDTH FOR PERSON_ID {} IS {}", current, person_data[current].width);
    person_data[current].descendants_processed = true;
  };

  TreeTraversal<id_t>::breadth_first_from_leaves(id, get_descendants_lambda, bfs_process,
                                   inorder_process);
}

std::pair<int, int>
FamilyTreeBalancerPreprocessor::accumulate_children_width_and_count(
    id_t couple_id) {
  auto children = db->getCoupleChildren(couple_id);
  int hourglass_descendants_width_accumulator = 0;
  int no_parents_partners_counter = 0;
  for (auto child : children) {
    hourglass_descendants_width_accumulator += person_data[child].width;
    auto partners = db->getPersonPartners(child);
    for(auto partner : partners){
      if(partner != 0 && db->getParentsCoupleId(partner).value() == 0){
        ++no_parents_partners_counter;
      }
    }
  }

  return {hourglass_descendants_width_accumulator, children.size() + no_parents_partners_counter};
}

void FamilyTreeBalancerPreprocessor::display_preprocessor_data(
    FamilyTreeItem *ftree, mftb::DB *db, id_t start_id) {

  FamilyTreeBalancerPreprocessor preprocessor(db);
  auto pdata = preprocessor.preprocess_from_id(start_id);

  for (auto person_iter : pdata.person_data) {
    auto *person_item = ftree->getPersonItemById(person_iter.first);
    auto person_data = person_iter.second;

    auto generation_str = QString::number(person_data.relative_generation);
    auto width_str = QString::number(person_iter.second.width);

    auto info_str = QString("gen:%1\nw:%2").arg(generation_str, width_str);

    auto *width_text = new QGraphicsSimpleTextItem(info_str, person_item);
    width_text->setBrush(QApplication::palette().text());
  }
}
