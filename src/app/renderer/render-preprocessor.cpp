#include "render-preprocessor.h"
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
#include <vector>

RenderPreprocessor::RenderPreprocessor(mftb::DB *db_) : db(db_) {}

RenderPreprocessor::data RenderPreprocessor::preprocess_from_id(id_t id) {
  person_data.clear();
  couple_data.clear();

  person_data[id].relative_generation = 0;
  preprocess_queue.push(id);

  while (!preprocess_queue.empty()) {
    auto target = preprocess_queue.front();
    preprocess_queue.pop();

    // do not change the order of processing
    if (!person_data[target].descendants_processed) {
      process_descendants(target);
      SPDLOG_DEBUG("PREPROCESSED PERSON_ID {} DESCENDANTS", target);
    }
    if (!person_data[target].ancestors_processed) {
      process_ancestors(target);
      SPDLOG_DEBUG("PREPROCESSED PERSON_ID {} ANCESTORS", target);
    }
  }


  

  return {person_data, couple_data};
}

void RenderPreprocessor::process_ancestors(id_t id) {
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

  auto distance_between_families = Config::AncestorNodePlacerConfig().distance_between_families;

  auto get_nonempty_parents = [&](id_t id) {
    std::vector<id_t> non_empty_parents;
    non_empty_parents.reserve(2);
    auto parents = db->getPersonParentsById(id);
    if (parents.first != 0 && 
       !person_data[parents.first].ancestors_processed) {

      person_data[id].has_parents_specified = true;
      non_empty_parents.push_back(parents.first);
      person_data[parents.first].relative_generation =
          person_data[id].relative_generation + 1;

    }
    if (parents.second != 0 &&
        !person_data[parents.second].ancestors_processed) {

      person_data[id].has_parents_specified = true;
      non_empty_parents.push_back(parents.second);
      person_data[parents.second].relative_generation =
          person_data[id].relative_generation + 1;

    }

    return non_empty_parents;
  };

  auto inorder_process = [&](id_t current) { preprocess_queue.push(current); };

  auto bfs_process = [&](id_t current) {
      auto parents_couple = db->getParentsCoupleId(current);
      if(parents_couple == 0){
        person_data[current].ancestors_and_siblings_width = 1;
      } else {
        auto parents = db->getCoupleById(*parents_couple);
        auto p1 = parents->person1_id;
        auto p2 = parents->person2_id;
        auto parents_ancestor_width =
            (p1 != 0 ? person_data[p1].ancestors_and_siblings_width : 0) +
            (p2 != 0 ? person_data[p2].ancestors_and_siblings_width : 0);

	if((p1 != 0 && db->getParentsCoupleId(p1) == 0)
		   != 
	    (p2 != 0 && db->getParentsCoupleId(p2) == 0)){
		// if only one of parents has no parents specified
		// this parent will be placed together with children
		parents_ancestor_width--;
	}

        auto siblings_count = accumulate_children_count(*parents_couple);

        person_data[current].ancestors_and_siblings_width
        = (int)std::max(parents_ancestor_width, siblings_count) + distance_between_families;
        couple_data[*parents_couple].ancestors_and_children_width
         = (int)std::max(parents_ancestor_width, siblings_count) + distance_between_families;
      }


    SPDLOG_DEBUG("PERSON {} HAS {} ANCESTOR_WIDTH", current, person_data[current].ancestors_and_siblings_width);
    person_data[current].ancestors_processed = true;
  };

  TreeTraversal<id_t>::breadth_first_from_leaves(id, get_nonempty_parents,
                                                 bfs_process, inorder_process);

  // TreeTraversal<id_t>::preorder(id, get_nonempty_parents, inorder_process,
  //                               false);
}

void RenderPreprocessor::process_descendants(id_t id) {

  std::stack<id_t> post_order;

  auto inorder_process = [&](id_t current) {
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
        preprocess_queue.push(child);
      }
    }
  };

  auto get_descendants_lambda = [&](id_t current) {
    auto children = db->getPersonChildren(current);
    std::erase_if(children, [&](id_t id) {
      return person_data[id].descendants_processed;
    });
    return children;
  };

  auto bfs_process = [&](id_t current) {
    auto couples = db->getPersonCouplesId(current);
    int width_accumulator = 1;
    bool first_couple_processed = false;
    for (auto couple_id : couples) {

      auto [children_width, children_count] =
          accumulate_children_width_and_count(couple_id);

      auto partner =
          db->getCoupleById(couple_id).value().getAnotherPerson(current);

      SPDLOG_DEBUG("CHILDREN WIDTH FOR COUPLE ({}, {}) IS {}", current, partner,
                   children_width);

      couple_data[couple_id].hourglass_descendants_width = children_width;
      couple_data[couple_id].children_count = children_count;

      if (partner == 0) {
	//if partner is zero, we guarantee that there are no other partners
        // then total width is covered by children
        width_accumulator -= 1;
        width_accumulator += children_width;
      } else if (db->getParentsCoupleId(partner) == 0) {
        if (!first_couple_processed) {
          width_accumulator -= 1;
        }
        width_accumulator += std::max(1, children_width);
      }

      first_couple_processed = true;
    }

    person_data[current].descendants_width = width_accumulator;
    SPDLOG_DEBUG("CHILDREN WIDTH FOR PERSON_ID {} IS {}", current,
                 person_data[current].descendants_width);
    person_data[current].descendants_processed = true;
  };

  TreeTraversal<id_t>::breadth_first_from_leaves(id, get_descendants_lambda,
                                                 bfs_process, inorder_process);
}

std::pair<int, int>
RenderPreprocessor::accumulate_children_width_and_count(id_t couple_id) {
  auto children = db->getCoupleChildren(couple_id);
  int hourglass_descendants_width_accumulator = 0;
  int rendered_partners_counter = 0;
  for (auto child : children) {

    auto child_width = 1;
    bool first_couple_processed = false;

    auto couples = db->getPersonCouplesId(child);
    for (auto couple : couples) {
      auto partner = db->getCoupleById(couple)->getAnotherPerson(child);
      if (partner != 0) {
	rendered_partners_counter++;
	child_width += std::max(couple_data[couple].hourglass_descendants_width
					- (first_couple_processed ? 0 : 1),1); 
      } else{
	child_width += std::max(couple_data[couple].hourglass_descendants_width-1,0);
      }
      first_couple_processed = true;
    }

    // if(couples.empty()){
    //   child_width = 1;
    // }

    hourglass_descendants_width_accumulator += child_width;
  }

  return {hourglass_descendants_width_accumulator,
          children.size() + rendered_partners_counter};
}


int RenderPreprocessor::accumulate_children_count(id_t couple_id) {
  int accumulator = 0;
  auto children = db->getCoupleChildren(couple_id);
  for(auto child : children){
    ++accumulator;
    auto partners = db->getPersonPartners(child);
    for(auto partner : partners){
      if(partner != 0 && db->getParentsCoupleId(partner) == 0){
        ++accumulator;
      }
    }
  }
  return accumulator;
}

void RenderPreprocessor::DBG_display_preprocessor_data(FamilyTreeItem *ftree,
                                                   mftb::DB *db,
                                                   id_t start_id) {

  RenderPreprocessor preprocessor(db);
  auto pdata = preprocessor.preprocess_from_id(start_id);

  for (auto person_iter : pdata.person_data) {
    auto *person_item = ftree->getPerson(person_iter.first);
    auto person_data = person_iter.second;

    auto generation_str = QString::number(person_data.relative_generation);
    auto width_str = QString::number(person_iter.second.descendants_width);

    auto info_str = QString("gen:%1\nw:%2").arg(generation_str, width_str);

    auto *width_text = new QGraphicsSimpleTextItem(info_str, person_item);
    width_text->setBrush(QApplication::palette().text());
  }
}

