#include "DB.h"
#include <functional>
#include <queue>
#include <stack>

template <typename T> class TreeTraversal {

public:
  static void preorder(T start_id,
                       std::function<std::vector<T>(T)> get_descendants,
                       std::function<void(T)> process,
                       bool process_start_id = true) {
    std::stack<T> stack;

    if (process_start_id) {
      stack.push(start_id);
    } else {
      auto descendants = get_descendants(start_id);
      for (auto desc : descendants) {
        stack.push(desc);
      }
    }

    while (!stack.empty() && stack.size() < STACK_LIMIT) {
      auto current = stack.top();
      stack.pop();
      process(current);

      auto descendants = get_descendants(current);
      for (auto desc : descendants) {
        stack.push(desc);
      }
    }

    if (stack.size() > STACK_LIMIT) {
      throw std::runtime_error("Stack overflow");
    }
  }

  static void breadth_first_from_leaves(
      T start_id, std::function<std::vector<T>(T)> get_descendants,
      std::function<void(T)> process, std::function<void(T)> inorder_process,
      bool process_start_id = true) {

    std::stack<T> post_order;

    {
      std::queue<T> traverse_queue;

      if (process_start_id) {
        traverse_queue.push(start_id);
      } else {
        auto descendants = get_descendants(start_id);
        for (auto desc : descendants) {
          traverse_queue.push(desc);
        }
      }

      while (!traverse_queue.empty() && traverse_queue.size() < STACK_LIMIT) {
        auto current = traverse_queue.front();
        traverse_queue.pop();
        post_order.push(current);

        inorder_process(current);

        auto descendants = get_descendants(current);
        for (auto desc : descendants) {
          traverse_queue.push(desc);
        }
      }

      if (traverse_queue.size() >= STACK_LIMIT) {
        throw std::runtime_error("Stack overflow");
      }

      qDebug() << "built stack of size" << post_order.size();
    }

    while (!post_order.empty() && post_order.size() < STACK_LIMIT) {
      auto current = post_order.top();
      post_order.pop();
      process(current);
    }

    if (post_order.size() >= STACK_LIMIT) {
      throw std::runtime_error("Stack overflow");
    }
  }

  static void breadth_first(T start_id,
                          std::function<std::vector<T>(T)> get_descendants,
                          std::function<void(T)> process,
                          bool process_start_id = true) {

    std::queue<T> queue;

    if (process_start_id) {
      queue.push(start_id);
    } else {
      auto descendants = get_descendants(start_id);
      for (auto desc : descendants) {
        queue.push(desc);
      }
    }

    while (!queue.empty() && queue.size() < STACK_LIMIT) {
      auto current = queue.front();
      queue.pop();


      process(current);
      auto descendants = get_descendants(current);
      for (auto desc : descendants) {
        queue.push(desc);
      }
    }

    if (queue.size() >= STACK_LIMIT) {
      throw std::runtime_error("Stack overflow");
    }
  }

  static inline int STACK_LIMIT = 32268;
};