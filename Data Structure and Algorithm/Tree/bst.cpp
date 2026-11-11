#include <algorithm>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>
using namespace std;

template <class T, class Node> class Base_BST_Node {
public:
  using value_type = T;
  T value;
  Node *parent;
  Node *left;
  Node *right;

  Base_BST_Node(const T &value, Node *parent = nullptr)
      : value(value), parent(parent), left(nullptr), right(nullptr) {}

  friend void swap(Node &lhs, Node &rhs) {
    if (lhs.left != nullptr)
      lhs.left->parent = rhs;
    if (lhs.right != nullptr)
      lhs.right->parent = rhs;
    switch (is_left_child(lhs)) {
    case 1:
      lhs.parent->left = rhs;
      break;
    case 0:
      lhs.parent->right = rhs;
      break;
    }
    if (rhs.left != nullptr)
      rhs.left->parent = lhs;
    if (rhs.right != nullptr)
      rhs.right->parent = lhs;
    switch (is_left_child(rhs)) {
    case 1:
      rhs.parent->left = lhs;
      break;
    case 0:
      rhs.parent->right = lhs;
      break;
    }
    swap(lhs.parent, rhs.parent);
    swap(lhs.left, rhs.left);
    swap(lhs.right, rhs.right);
  }
};

template <class T>
class BST_Node : public Base_BST_Node<T, class BST_Node<T>> {};

template <class T, class Node = BST_Node<T>> class BST {
public:
  using value_type = T;

  Node *root; // Allow root->parent != nullptr

  BST(Node *root = nullptr) : root(root) {}

  BST(const BST &) = delete;

  BST &operator=(const BST &) = delete;

  BST(BST &&other) noexcept : root(other.root) { other.root = nullptr; }

  BST &operator=(BST &&other) noexcept {
    if (this != &other) {
      clear();
      root = other.root;
      other.root = nullptr;
    }
    return *this;
  }

  // 1: left child
  // 0: right child
  // -1: node or its parent is nullptr
  static char is_left_child(Node *node) {
    if (node == nullptr || node->parent == nullptr)
      return -1;
    return char(node->parent->left == node);
  }

  vector<T> preorder(Node *node) const {
    if (node == nullptr)
      return vector<T>();
    vector<T> result;
    vector<Node *> stack;
    stack.push_back(node);
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

  vector<Node *> preorder_node(Node *node) const {
    if (node == nullptr)
      return vector<Node *>();
    vector<Node *> result, stack;
    stack.push_back(node);
    while (!stack.empty()) {
      Node *node = stack.back();
      stack.pop_back();
      result.push_back(node);
      if (node->right != nullptr)
        stack.push_back(node->right);
      if (node->left != nullptr)
        stack.push_back(node->left);
    }
    return result;
  }

  // O(n)
  vector<T> preorder() const { return preorder(root); }
  vector<Node *> preorder_node() const { return preorder_node(root); }

  vector<T> inorder(Node *node) const {
    if (node == nullptr)
      return vector<T>();
    vector<T> result;
    vector<Node *> stack;
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

  vector<Node *> inorder_node(Node *node) const {
    if (node == nullptr)
      return vector<Node *>();
    vector<Node *> result, stack;
    while (!stack.empty() || node != nullptr) {
      while (node != nullptr) {
        stack.push_back(node);
        node = node->left;
      }
      node = stack.back();
      stack.pop_back();
      result.push_back(node);
      node = node->right;
    }
    return result;
  }

  // O(n)
  vector<T> inorder() const { return inorder(root); }
  vector<Node *> inorder_node() const { return inorder_node(root); }

  vector<T> postorder(Node *node) const {
    if (node == nullptr)
      return vector<T>();
    vector<T> result;
    vector<Node *> stack;
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

  vector<Node *> postorder_node(Node *node) const {
    if (node == nullptr)
      return vector<Node *>();
    vector<Node *> result, stack;
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
          result.push_back(stack.back());
          last_visited = stack.back();
          stack.pop_back();
        }
      }
    }
    return result;
  }

  // O(n)
  vector<T> postorder() const { return postorder(root); }
  vector<Node *> postorder_node() const { return postorder_node(root); }

  virtual void clear(Node *node) {
    if (node == nullptr)
      throw invalid_argument("Node is nullptr");
    switch (is_left_child(node)) {
    case 1:
      node->parent->left = nullptr;
      break;
    case 0:
      node->parent->right = nullptr;
      break;
    }
    vector<Node *> stack = preorder_node(node);
    for (auto it = stack.rbegin(); it != stack.rend(); ++it)
      delete *it;
  }

  // O(n)
  virtual void clear() {
    if (root != nullptr) {
      clear(root);
      root = nullptr;
    }
  }

  virtual ~BST() { clear(); }

  // Return node, found
  // O(h)
  virtual pair<Node *, bool> search(const T &x) const {
    if (root == nullptr)
      return {nullptr, false};
    Node *node = root;
    while (true) {
      if (std::less<>{}(x, node->value)) {
        if (node->left == nullptr)
          return {node, false};
        node = node->left;
      } else if (std::less<>{}(node->value, x)) {
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

  // O(h)
  Node *inorder_successor(Node *node) const {
    if (node == nullptr)
      throw invalid_argument("Node is nullptr");
    if (node->right != nullptr)
      return find_min(node->right);
    Node *p = node->parent;
    while (p != nullptr && node == p->right) {
      node = p;
      p = p->parent;
    }
    return p;
  }

  // O(h)
  Node *inorder_predecessor(Node *node) const {
    if (node == nullptr)
      throw invalid_argument("Node is nullptr");
    if (node->left != nullptr)
      return find_max(node->left);
    Node *p = node->parent;
    while (p != nullptr && node == p->left) {
      node = p;
      p = p->parent;
    }
    return p;
  }

  // Return smallest node > x
  // O(h)
  Node *upper_bound(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (std::less<>{}(x, node->value)) {
        result = node;
        node = node->left;
      } else {
        node = node->right;
      }
    }
    return result;
  }

  // Return smallest node >= x
  // O(h)
  Node *lower_bound(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (!std::less<>{}(node->value, x)) {
        result = node;
        node = node->left;
      } else {
        node = node->right;
      }
    }
    return result;
  }

  // Return node, inserted
  // O(h)
  virtual pair<Node *, bool> insert(const T &x) {
    if (root == nullptr) {
      root = new Node(x);
      return {root, true};
    }
    Node *node = root;
    while (true) {
      if (std::less<>{}(x, node->value)) {
        if (node->left == nullptr) {
          node->left = new Node(x, node);
          return {node->left, true};
        }
        node = node->left;
      } else if (std::less<>{}(node->value, x)) {
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
  Node *replace(Node *node, Node *replacement) {
    if (node == nullptr)
      throw invalid_argument("Node is nullptr");
    Node *parent = node->parent;
    if (root == node)
      root = replacement;
    if (parent != nullptr) {
      if (parent->left == node) {
        parent->left = replacement;
      } else {
        parent->right = replacement;
      }
    }
    if (replacement != nullptr)
      replacement->parent = parent;
    return parent;
  }

  // O(1)
  Node *replace_delete(Node *node, Node *replacement) {
    Node *result = replace(node, replacement);
    delete node;
    return result;
  }

  // O(h)
  virtual Node *remove_node(Node *node) {
    if (node == nullptr)
      throw invalid_argument("Node is nullptr");
    if (node->left == nullptr)
      return replace_delete(node, node->right);
    if (node->right == nullptr)
      return replace_delete(node, node->left);
    node->swap(*inorder_successor(node));
    return replace_delete(node, node->right);
  }

  // Return parent, removed
  // O(h)
  virtual pair<Node *, bool> remove(const T &x) {
    auto [node, found] = search(x);
    if (!found)
      return {node, false};
    return {remove_node(node), true};
  }

  Node *rotate_right(Node *node) {
    if (node == nullptr || node->left == nullptr)
      throw invalid_argument("Node is nullptr");
    Node *child = node->left;
    replace(node, child);
    node->left = child->right;
    if (child->right != nullptr)
      child->right->parent = node;
    child->right = node;
    node->parent = child;
    return child;
  }

  Node *rotate_left(Node *node) {
    if (node == nullptr || node->right == nullptr)
      throw invalid_argument("Node is nullptr");
    Node *child = node->right;
    replace(node, child);
    node->right = child->left;
    if (child->left != nullptr)
      child->left->parent = node;
    child->left = node;
    node->parent = child;
    return child;
  }

  Node *rotate_parent(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      throw invalid_argument("Node is nullptr");
    if (is_left_child(child) == 1)
      return rotate_right(child->parent);
    return rotate_left(child->parent);
  }
};

template <class T> class AVL_Node : public Base_BST_Node<T, AVL_Node<T>> {
public:
  using Base = Base_BST_Node<T, AVL_Node<T>>;
  unsigned int height = 1; // leaf 1

  friend void swap(AVL_Node &lhs, AVL_Node &rhs) {
    Base::swap(lhs, rhs);
    swap(lhs.height, rhs.height);
  }
};

template <class T, class Node = AVL_Node<T>> class AVL : public BST<T, Node> {
public:
  using Base = BST<T, Node>;

  static int height(Node *node) { return node == nullptr ? 0 : node->height; }

  static void update(Node *node) {
    node->height = 1 + max(height(node->left), height(node->right));
  }

  static int bf(Node *node) { return height(node->left) - height(node->right); }

  Node *rotate_right(Node *parent) override {
    Node *result = Base::rotate_right(parent);
    if (result != nullptr) {
      update(result->right);
      update(result);
    }
    return result;
  }

  Node *rotate_left(Node *parent) override {
    Node *result = Base::rotate_left(parent);
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
    int bf = bf(node);
    if (bf > 1) {
      Node *child = node->left;
      if (bf(child) < 0) // LR
        rotate_left(child);
      return rotate_right(node); // LL, LR
    }
    if (bf < -1) {
      Node *child = node->right;
      if (bf(child) > 0) // RL
        rotate_right(child);
      return rotate_left(node); // RL, RR
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

  pair<Node *, bool> remove(const T &x) override {
    auto [parent, removed] = Base::remove(x);
    if (!removed)
      return {parent, removed};
    Node *node = parent;
    while (node != nullptr) {
      node = rebalance(node)->parent;
    }
    return {parent, removed};
  }
};

template <class T, class Node = BST_Node<T>> class Splay : public BST<T, Node> {
public:
  using Base = BST<T>;

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
    auto [result, found] = Base::search(x);
    return {splay(result), found};
  }

  virtual Node *find_min(Node *node) const {
    auto result = Base::find_min(node);
    return splay(result);
  }

  virtual Node *find_max(Node *node) const {
    auto result = Base::find_max(node);
    return splay(result);
  }
};
