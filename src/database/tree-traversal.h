#include "DB.h"
#include <functional>
#include <stack>
#include <queue>

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

  static void depth_first(T start_id,
                        std::function<std::vector<T>(T)> get_descendants,
                        std::function<void(T)> process,
                        std::function<void(T)> inorder_process,
                        bool process_start_id = true) {

    std::stack<T> post_order;

    {
      std::queue<T> traverse_stack;

      if (process_start_id) {
        traverse_stack.push(start_id);
      } else {
        auto descendants = get_descendants(start_id);
        for (auto desc : descendants) {
          traverse_stack.push(desc);
        }
      }

      while (!traverse_stack.empty() && traverse_stack.size() < STACK_LIMIT) {
        auto current = traverse_stack.front();
        traverse_stack.pop();
        post_order.push(current);

        inorder_process(current);

        auto descendants = get_descendants(current);
        for (auto desc : descendants) {
          traverse_stack.push(desc);
        }
      }

      if (traverse_stack.size() >= STACK_LIMIT) {
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

  static void postorder_with_put_on_stack(T start_id,
                        std::function<void(T, std::stack<T>&)> put_descendants_on_stack,
                        std::function<void(T)> process,
                        std::function<void(T)> inorder_process,
                        bool process_start_id = true) {

    std::stack<T> post_order;

    {
      std::stack<T> traverse_stack;

      if (process_start_id) {
        traverse_stack.push(start_id);
      } else {
        put_descendants_on_stack(traverse_stack);
      }

      while (!traverse_stack.empty() && traverse_stack.size() < STACK_LIMIT) {
        auto current = traverse_stack.top();
        traverse_stack.pop();
        post_order.push(current);

        inorder_process(current);
        put_descendants_on_stack(traverse_stack);
      }

      if (traverse_stack.size() >= STACK_LIMIT) {
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

  static inline int STACK_LIMIT = 32268;
};