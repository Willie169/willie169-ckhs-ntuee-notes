#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

template <typename T> struct binary_tree_node {
  using value_type = T;
  T value;
  binary_tree_node *parent;
  binary_tree_node *left;
  binary_tree_node *right;
  int height;

  binary_tree_node(T value, binary_tree_node *parent = nullptr)
      : value(value), parent(parent), left(nullptr), right(nullptr), height(1) {
  } // only used in AVL, leaf 1
};

template <class T, class Node = binary_tree_node<T>> class BST {
public:
  using value_type = typename Node::value_type;

  Node *root = nullptr;

  virtual void clear(Node *node) {
    if (node == nullptr)
      return;
    clear(node->left);
    clear(node->right);
    delete node;
  }

  // O(n)
  virtual void clear() { clear(root); }

  ~BST() { clear(); }

  // node, found
  // O(h)
  virtual pair<Node *, bool> search(const T &x) const {
    Node *node = root;
    if (node == nullptr)
      return {nullptr, false};
    while (true) {
      if (node->value > x) {
        if (node->left == nullptr)
          return {node, false};
        node = node->left;
      } else if (node->value < x) {
        if (node->right == nullptr)
          return {node, false};
        node = node->right;
      } else {
        return {node, true};
      }
    }
  }

  virtual Node *find_min(Node *node) const {
    if (node == nullptr)
      return nullptr;
    while (node->left != nullptr)
      node = node->left;
    return node;
  }

  // O(h)
  virtual Node *find_min() const { return find_min(root); }

  virtual Node *find_max(Node *node) const {
    if (node == nullptr)
      return nullptr;
    while (node->right != nullptr)
      node = node->right;
    return node;
  }

  // O(h)
  virtual Node *find_max() const { return find_max(root); }

  // largest node <= x
  // O(h)
  Node *upper_bound(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (node->value <= x) {
        result = node;
        node = node->right;
      } else {
        node = node->left;
      }
    }
    return result;
  }

  // smallest node >= x
  // O(h)
  Node *lower_bound(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (node->value >= x) {
        result = node;
        node = node->left;
      } else {
        node = node->right;
      }
    }
    return result;
  }

  // O(h)
  Node *inorder_successor(Node *node) const {
    if (node == nullptr || node->right == nullptr)
      return nullptr;
    Node *result = node->right;
    while (result->left != nullptr)
      result = result->left;
    return result;
  }

  // O(h)
  Node *inorder_predecessor(Node *node) const {
    if (node == nullptr || node->left == nullptr)
      return nullptr;
    Node *result = node->left;
    while (result->right != nullptr)
      result = result->right;
    return result;
  }

  // node, inserted
  // O(h)
  virtual pair<Node *, bool> insert(const T &x) {
    if (root == nullptr) {
      root = new Node(x);
      return {root, true};
    }
    Node *node = root;
    while (true) {
      if (node->value > x) {
        if (node->left == nullptr) {
          node->left = new Node(x, node);
          return {node->left, true};
        }
        node = node->left;
      } else if (node->value < x) {
        if (node->right == nullptr) {
          node->right = new Node(x, node);
          return {node->right, true};
        }
        node = node->right;
      } else {
        return {node, false};
      }
    }
  }

  // O(1)
  Node *transplant(Node *node, Node *replacement) {
    Node *parent = node->parent;
    if (parent == nullptr) {
      root = replacement;
    } else if (parent->left == node) {
      parent->left = replacement;
    } else {
      parent->right = replacement;
    }
    if (replacement != nullptr)
      replacement->parent = parent;
    return parent;
  }

  // O(h)
  virtual Node *remove_node(Node *node) {
    if (node->left == nullptr && node->right == nullptr) {
      return transplant(node, nullptr);
    }
    if (node->left == nullptr) {
      return transplant(node, node->right);
    }
    if (node->right == nullptr) {
      return transplant(node, node->left);
    }
    Node *succ = inorder_successor(node);
    node->value = succ->value;
    Node *parent = transplant(succ, succ->right);
    delete succ;
    return parent;
  }

  // O(h)
  virtual Node *remove(const T &x) {
    auto [node, found] = search(x);
    if (!found)
      return nullptr;
    return remove_node(node);
  }

  // O(n)
  vector<T> preorder() const {
    vector<T> result;
    vector<Node *> stack;
    if (root != nullptr)
      stack.push_back(root);
    while (!stack.empty()) {
      Node *node = stack.back();
      stack.pop_back();
      result.push_back(node->value);
      if (node->right != nullptr)
        stack.push_back(node->right);
      if (node->left != nullptr)
        stack.push_back(node->left);
    }
    return result;
  }

  // O(n)
  vector<T> inorder() const {
    vector<T> result;
    vector<Node *> stack;
    Node *node = root;
    while (!stack.empty() || node != nullptr) {
      while (node != nullptr) {
        stack.push_back(node);
        node = node->left;
      }
      node = stack.back();
      stack.pop_back();
      result.push_back(node->value);
      node = node->right;
    }
    return result;
  }

  // O(n)
  vector<T> postorder() const {
    vector<T> result;
    vector<Node *> stack;
    Node *node = root;
    Node *last_visited = nullptr;
    while (!stack.empty() || node != nullptr) {
      if (node != nullptr) {
        stack.push_back(node);
        node = node->left;
      } else {
        Node *right = stack.back()->right;
        if (right != nullptr && last_visited != right) {
          node = right;
        } else {
          result.push_back(stack.back()->value);
          last_visited = stack.back();
          stack.pop_back();
        }
      }
    }
    return result;
  }

  // 1: left child
  // 0: right child
  // -1: nullptr or no parent
  char is_left_child(Node *node) const {
    if (node == nullptr || node->parent == nullptr)
      return -1;
    return char(node->parent->left == node);
  }

  // Rotate right around parent of child
  Node *rotate_right(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      return nullptr;

    Node *parent = child->parent;

    transplant(parent, child);

    parent->left = child->right;

    if (child->right != nullptr)
      child->right->parent = parent;

    child->right = parent;
    parent->parent = child;

    return child;
  }

  // Rotate left around parent of child
  Node *rotate_left(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      return nullptr;

    Node *parent = child->parent;

    transplant(parent, child);

    parent->right = child->left;

    if (child->left != nullptr)
      child->left->parent = parent;

    child->left = parent;
    parent->parent = child;

    return child;
  }

  Node *rotate(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      return nullptr;

    if (is_left_child(child).value())
      return rotate_right(child);

    return rotate_left(child);
  }
};

// ============================================================
// AVL TREE
// ============================================================

template <typename T> class AVL : public BST<T> {
  using Base = BST<T>;
  using Node = typename Base::Node;

public:
  static int height(Node *node) { return node == nullptr ? 0 : node->height; }

  static void update(Node *node) {
    node->height = 1 + max(height(node->left), height(node->right));
  }

  static int bf(Node *node) { return height(node->left) - height(node->right); }

  Node *rotate_right(Node *child) {
    Node *result = Base::rotate_right(child);

    if (result != nullptr) {
      update(result->right);
      update(result);
    }

    return result;
  }

  Node *rotate_left(Node *child) {
    Node *result = Base::rotate_left(child);

    if (result != nullptr) {
      update(result->left);
      update(result);
    }

    return result;
  }

  Node *rebalance(Node *node) {
    if (node == nullptr)
      return nullptr;

    update(node);

    int balance = bf(node);

    if (balance > 1) {
      Node *child = node->left;

      if (bf(child) < 0)
        rotate_left(child->right);

      return rotate_right(child);
    }

    if (balance < -1) {
      Node *child = node->right;

      if (bf(child) > 0)
        rotate_right(child->left);

      return rotate_left(child);
    }

    if (node->parent == nullptr)
      return node;

    return rebalance(node->parent);
  }

  pair<Node *, bool> insert(const T &x) override {
    auto [node, inserted] = Base::insert(x);

    if (inserted && node->parent != nullptr)
      rebalance(node->parent);

    return {node, inserted};
  }

  Node *remove(const T &x) override {
    Node *node = Base::remove(x);

    while (node != nullptr) {
      Node *new_root = rebalance(node);
      node = new_root->parent;
    }

    return nullptr;
  }
};

// ============================================================
// SPLAY TREE
// ============================================================

template <typename T> class Splay : public BST<T> {
  using Base = BST<T>;
  using Node = typename Base::Node;

public:
  Node *splay(Node *node) {
    if (node == nullptr)
      return nullptr;

    while (node->parent != nullptr) {
      Node *parent = node->parent;
      Node *grandparent = parent->parent;

      // Zig
      if (grandparent == nullptr) {
        this->rotate(node);
      }

      // Zig-zig
      else if ((parent->left == node && grandparent->left == parent) ||
               (parent->right == node && grandparent->right == parent)) {
        this->rotate(parent);
        this->rotate(node);
      }

      // Zig-zag
      else {
        this->rotate(node);
        this->rotate(node);
      }
    }

    return node;
  }

  pair<Node *, bool> search(const T &x) override {
    auto [node, found] = Base::search(x);
    return {splay(node), found};
  }

  Node *find_min() const override {
    Node *node = Base::find_min();

    // The original Python implementation splays this node.
    // const_cast is needed because splaying modifies the tree.
    return const_cast<Splay *>(this)->splay(node);
  }

  Node *find_max() const override {
    Node *node = Base::find_max();

    return const_cast<Splay *>(this)->splay(node);
  }
};
