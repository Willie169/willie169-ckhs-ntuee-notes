#pragma once
#include <algorithm>
#include <cctype>
#include <concepts>
#include <cstddef>
#include <functional>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

template <class T, class Node> class BaseBinaryTreeNode {
public:
  using value_type = T;
  using node_type = Node;

  T value;
  Node *parent;
  Node *left = nullptr;
  Node *right = nullptr;

  BaseBinaryTreeNode(const T &_value = T(), Node *_parent = nullptr)
      : value(_value), parent(_parent) {}

  virtual ~BaseBinaryTreeNode() = default;
};

template <class T>
class BinaryTreeNode : public BaseBinaryTreeNode<T, BinaryTreeNode<T>> {
public:
  using Base = BaseBinaryTreeNode<T, BinaryTreeNode<T>>;
  using Base::Base;
  using value_type = T;
  using node_type = Base::node_type;
};

template <class T, class Node = BinaryTreeNode<T>> class BinaryTreeStatic {
public:
  using node_type = Node;

  static std::vector<T> preorder(Node *node) {
    if (node == nullptr)
      return std::vector<T>();
    std::vector<T> result;
    std::vector<Node *> stack;
    stack.push_back(node);
    while (!stack.empty()) {
      Node *cur = stack.back();
      stack.pop_back();
      result.push_back(cur->value);
      if (cur->right != nullptr)
        stack.push_back(cur->right);
      if (cur->left != nullptr)
        stack.push_back(cur->left);
    }
    return result;
  }

  static std::vector<Node *> preorder_node(Node *node) {
    if (node == nullptr)
      return std::vector<Node *>();
    std::vector<Node *> result, stack;
    stack.push_back(node);
    while (!stack.empty()) {
      Node *cur = stack.back();
      stack.pop_back();
      result.push_back(cur);
      if (cur->right != nullptr)
        stack.push_back(cur->right);
      if (cur->left != nullptr)
        stack.push_back(cur->left);
    }
    return result;
  }

  static std::vector<T> inorder(Node *node) {
    if (node == nullptr)
      return std::vector<T>();
    std::vector<T> result;
    std::vector<Node *> stack;
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

  static std::vector<Node *> inorder_node(Node *node) {
    if (node == nullptr)
      return std::vector<Node *>();
    std::vector<Node *> result, stack;
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

  static std::vector<T> postorder(Node *node) {
    if (node == nullptr)
      return std::vector<T>();
    std::vector<T> result;
    std::vector<Node *> stack;
    Node *last_visited = nullptr;
    while (!stack.empty() || node != nullptr) {
      if (node != nullptr) {
        stack.push_back(node);
        node = node->left;
      } else {
        Node *right = stack.back()->right;
        if (right != nullptr && last_visited != right)
          node = right;
        else {
          result.push_back(stack.back()->value);
          last_visited = stack.back();
          stack.pop_back();
        }
      }
    }
    return result;
  }

  static std::vector<Node *> postorder_node(Node *node) {
    if (node == nullptr)
      return std::vector<Node *>();
    std::vector<Node *> result, stack;
    Node *last_visited = nullptr;
    while (!stack.empty() || node != nullptr) {
      if (node != nullptr) {
        stack.push_back(node);
        node = node->left;
      } else {
        Node *right = stack.back()->right;
        if (right != nullptr && last_visited != right)
          node = right;
        else {
          result.push_back(stack.back());
          last_visited = stack.back();
          stack.pop_back();
        }
      }
    }
    return result;
  }

  // 1: left child
  // 0: right child
  // -1: node's parent is nullptr
  // -2: node is nullptr
  static signed char is_left_child(Node *node) noexcept {
    if (node == nullptr)
      return -2;
    if (node->parent == nullptr)
      return -1;
    return node->parent->left == node;
  }

  static Node *leftmost_child(Node *node) {
    if (node == nullptr)
      return nullptr;
    while (node->left != nullptr)
      node = node->left;
    return node;
  }

  static Node *rightmost_child(Node *node) {
    if (node == nullptr)
      return nullptr;
    while (node->right != nullptr)
      node = node->right;
    return node;
  }

  // LaTeX forest representation: [value[left][right]]
  // A missing child whose sibling exists is emitted as [,phantom]
  static void latex_forest(Node *node, std::ostream *oss, bool outer = true) {
    if (node == nullptr || oss == nullptr)
      return;
    if (outer)
      *oss << "\\begin{center}\n\\begin{forest}\n";
    *oss << "[" << node->value;
    bool has_left = (node->left != nullptr);
    bool has_right = (node->right != nullptr);
    if (has_left)
      latex_forest(node->left, oss, false);
    else if (has_right)
      *oss << "[,phantom]";
    if (has_right)
      latex_forest(node->right, oss, false);
    else if (has_left)
      *oss << "[,phantom]";
    *oss << "]";
    if (outer)
      *oss << "\n\\end{forest}\n\\end{center}\n";
  }
};

// Take LaTeX forest representation: [value[left][right]]
// Output root node
template <class T, class Node = BinaryTreeNode<T>>
  requires requires { T{}; } && requires(std::istream &is, T &value) {
    { is >> value } -> std::same_as<std::istream &>;
  }
Node *from_latex_forest(std::string s) {
  std::erase_if(s, [](unsigned char c) { return std::isspace(c); });
  if (s.empty())
    return nullptr;
  std::size_t pos = 0;
  std::vector<Node *> owned;
  auto consume = [&](char expected) {
    if (pos >= s.size() || s[pos] != expected)
      throw std::invalid_argument(std::string("from_latex_forest: expected '") +
                                  expected + "'");
    ++pos;
  };
  auto parse_node = [&](auto &&self) -> Node * {
    consume('[');
    if (pos + 9 <= s.size() && s[pos] == ',' &&
        s.compare(pos + 1, 7, "phantom") == 0 && s[pos + 8] == ']') {
      pos += 9;
      return nullptr;
    }
    std::size_t start = pos;
    while (pos < s.size() && s[pos] != '[' && s[pos] != ']')
      ++pos;
    std::string value_str = s.substr(start, pos - start);
    std::istringstream vs(value_str);
    T value{};
    if (!(vs >> value))
      throw std::invalid_argument("from_latex_forest: bad value '" + value_str +
                                  "'");
    Node *node = new Node(value);
    owned.push_back(node);
    if (pos < s.size() && s[pos] == '[') {
      node->left = self(self);
      if (node->left)
        node->left->parent = node;
      node->right = self(self);
      if (node->right)
        node->right->parent = node;
    }
    consume(']');
    return node;
  };
  Node *root = nullptr;
  try {
    root = parse_node(parse_node);
    if (pos != s.size())
      throw std::invalid_argument("from_latex_forest: trailing characters");
  } catch (...) {
    for (Node *n : owned)
      delete n;
    throw;
  }
  return root;
}

template <class T, class Node = BinaryTreeNode<T>>
class BinaryTree : public BinaryTreeStatic<T, Node> {
public:
  using node_type = Node;
  using Base = BinaryTreeStatic<T, Node>;
  using Base::inorder;
  using Base::inorder_node;
  using Base::is_left_child;
  using Base::latex_forest;
  using Base::leftmost_child;
  using Base::postorder;
  using Base::postorder_node;
  using Base::preorder;
  using Base::preorder_node;
  using Base::rightmost_child;

  mutable Node *root; // Allow root->parent != nullptr
  std::ostream *oss;  // latex_forest logger

  BinaryTree(Node *_root = nullptr, std::ostream *_oss = nullptr)
      : root(_root), oss(_oss) {}

  BinaryTree(const BinaryTree &) = delete;

  BinaryTree(BinaryTree &&other) noexcept
      : root(std::exchange(other.root, nullptr)) {}

  void set_logger(std::ostream *_oss) { oss = _oss; }

  // O(n)
  std::vector<T> preorder() const { return preorder(root); }
  std::vector<Node *> preorder_node() const { return preorder_node(root); }
  std::vector<T> inorder() const { return inorder(root); }
  std::vector<Node *> inorder_node() const { return inorder_node(root); }
  std::vector<T> postorder() const { return postorder(root); }
  std::vector<Node *> postorder_node() const { return postorder_node(root); }

  // O(h)
  Node *leftmost_child() const { return leftmost_child(root); }
  Node *rightmost_child() const { return rightmost_child(root); }

  virtual void log(std::string_view msg) const {
    if (oss != nullptr)
      *oss << msg << '\n';
  }

  virtual void log_forest(Node *node) const {
    if (oss != nullptr)
      latex_forest(node, oss);
  }

  // O(n)
  virtual void log_forest() const { return log_forest(root); }

protected:
  // O(1)
  virtual Node *replace(Node *node, Node *replacement,
                        bool deleting = false) const {
    switch (is_left_child(node)) {
    case -2:
      throw std::invalid_argument("Node is nullptr");
      break;
    case 1:
      node->parent->left = replacement;
      break;
    case 0:
      node->parent->right = replacement;
      break;
    }
    if (root == node)
      root = replacement;
    if (replacement != nullptr)
      replacement->parent = node->parent;
    Node *parent = node->parent;
    if (deleting)
      delete node;
    return parent;
  }

  void recursive_clear(Node *node) noexcept {
    if (node == nullptr)
      return;
    recursive_clear(node->left);
    recursive_clear(node->right);
    delete node;
  }

public:
  // recursive:
  // >0: stack
  // 0: try stack catch recursive
  // <0: recursive
  void clear(Node *node, signed char recursive = 0) noexcept {
    if (node == nullptr)
      return;
    if (root == node)
      root = nullptr;
    switch (is_left_child(node)) {
    case 1:
      node->parent->left = nullptr;
      break;
    case 0:
      node->parent->right = nullptr;
      break;
    }
    auto stack_clear = [](Node *node) {
      std::vector<Node *> stack = preorder_node(node);
      for (auto it = stack.rbegin(); it != stack.rend(); ++it)
        delete *it;
    };
    if (recursive > 0)
      stack_clear(node);
    else if (recursive == 0) {
      try {
        stack_clear(node);
      } catch (...) {
        recursive_clear(node);
      }
    } else
      recursive_clear(node);
  }

  void clear() noexcept { clear(root); }

  virtual ~BinaryTree() { clear(); }

  BinaryTree &operator=(const BinaryTree &) = delete;

  BinaryTree &operator=(BinaryTree &&other) noexcept {
    if (this != &other) {
      clear();
      root = std::exchange(other.root, nullptr);
    }
    return *this;
  }

  // O(1)
  virtual Node *rotate_right(Node *node) const {
    if (node == nullptr || node->left == nullptr)
      throw std::invalid_argument("Node is nullptr");
    Node *child = node->left;
    replace(node, child);
    node->left = child->right;
    if (child->right != nullptr)
      child->right->parent = node;
    child->right = node;
    node->parent = child;
    log("Rotate right:\n");
    log_forest();
    return child;
  }

  // O(1)
  virtual Node *rotate_left(Node *node) const {
    if (node == nullptr || node->right == nullptr)
      throw std::invalid_argument("Node is nullptr");
    Node *child = node->right;
    replace(node, child);
    node->right = child->left;
    if (child->left != nullptr)
      child->left->parent = node;
    child->left = node;
    node->parent = child;
    log("Rotate left:\n");
    log_forest();
    return child;
  }

  // O(1)
  virtual Node *rotate_parent(Node *child) const {
    if (child == nullptr || child->parent == nullptr)
      throw std::invalid_argument("Node is nullptr");
    if (is_left_child(child) == 1)
      return rotate_right(child->parent);
    return rotate_left(child->parent);
  }
};

template <class T, class Node = BinaryTreeNode<T>, class Compare = std::less<T>>
  requires std::strict_weak_order<const Compare &, const T &, const T &>
class BST : public BinaryTree<T, Node> {
public:
  using node_type = Node;
  using Base = BinaryTree<T, Node>;
  using Base::clear;
  using Base::inorder;
  using Base::inorder_node;
  using Base::leftmost_child;
  using Base::log;
  using Base::log_forest;
  using Base::postorder;
  using Base::postorder_node;
  using Base::preorder;
  using Base::preorder_node;
  using Base::rightmost_child;
  using Base::root;
  using Base::rotate_left;
  using Base::rotate_right;

  [[no_unique_address]] Compare comp;

protected:
  using Base::replace;

public:
  BST() : BST(nullptr, Compare(), nullptr) {}
  explicit BST(const Compare &_comp, std::ostream *_oss = nullptr)
      : BST(nullptr, _comp, _oss) {}
  BST(Node *_root, const Compare &_comp, std::ostream *_oss = nullptr)
      : Base(_root, _oss), comp(_comp) {}

  BST(const BST &) = delete;
  BST &operator=(const BST &) = delete;

  BST(BST &&) = default;
  BST &operator=(BST &&) = default;

  // O(h)
  virtual Node *find_min(Node *node) const { return leftmost_child(node); }
  virtual Node *find_max(Node *node) const { return rightmost_child(node); }
  virtual Node *find_min() const { return find_min(root); }
  virtual Node *find_max() const { return find_max(root); }

  // Return node, found
  // O(h)
  virtual std::pair<Node *, bool> search(const T &x) const {
    if (root == nullptr)
      return {nullptr, false};
    Node *node = root;
    while (true) {
      if (comp(x, node->value)) {
        if (node->left == nullptr)
          return {node, false};
        node = node->left;
      } else if (comp(node->value, x)) {
        if (node->right == nullptr)
          return {node, false};
        node = node->right;
      } else
        return {node, true};
    }
  }

  // O(h)
  Node *inorder_successor(Node *node) const {
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
    if (node->right != nullptr)
      return leftmost_child(node->right);
    if (node == root)
      return nullptr;
    Node *p = node->parent;
    while (p != nullptr && node == p->right) {
      if (p == root)
        return nullptr;
      node = p;
      p = p->parent;
    }
    return p;
  }

  // O(h)
  Node *inorder_predecessor(Node *node) const {
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
    if (node->left != nullptr)
      return rightmost_child(node->left);
    if (node == root)
      return nullptr;
    Node *p = node->parent;
    while (p != nullptr && node == p->left) {
      if (p == root)
        return nullptr;
      node = p;
      p = p->parent;
    }
    return p;
  }

  // O(h)
  virtual Node *ordered_after(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (comp(x, node->value)) {
        result = node;
        node = node->left;
      } else
        node = node->right;
    }
    return result;
  }

  // O(h)
  virtual Node *ordered_not_before(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (!comp(node->value, x)) {
        result = node;
        node = node->left;
      } else
        node = node->right;
    }
    return result;
  }

  // O(h)
  virtual Node *ordered_before(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (comp(node->value, x)) {
        result = node;
        node = node->right;
      } else
        node = node->left;
    }
    return result;
  }

  // O(h)
  virtual Node *ordered_not_after(const T &x) const {
    Node *result = nullptr;
    Node *node = root;
    while (node != nullptr) {
      if (!comp(x, node->value)) {
        result = node;
        node = node->right;
      } else
        node = node->left;
    }
    return result;
  }

  // Return node, inserted
  // O(h)
  virtual std::pair<Node *, bool> insert(const T &x) {
    if (root == nullptr) {
      root = new Node(x);
      log("Insert:\n");
      log_forest();
      return {root, true};
    }
    Node *node = root;
    while (true) {
      if (comp(x, node->value)) {
        if (node->left == nullptr) {
          node->left = new Node(x, node);
          log("Insert:\n");
          log_forest();
          return {node->left, true};
        }
        node = node->left;
      } else if (comp(node->value, x)) {
        if (node->right == nullptr) {
          node->right = new Node(x, node);
          log("Insert:\n");
          log_forest();
          return {node->right, true};
        }
        node = node->right;
      } else
        return {node, false};
    }
  }

  // Return rebalance_start
  // O(h)
  virtual Node *remove_node(Node *node) {
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
    if (node->left == nullptr) {
      return replace(node, node->right, true);
    }
    if (node->right == nullptr) {
      return replace(node, node->left, true);
    }
    Node *succ = inorder_successor(node);
    Node *rebalance_start;
    if (succ->parent != node) {
      rebalance_start = succ->parent;
      replace(succ, succ->right);
      succ->right = node->right;
      node->right->parent = succ;
    } else
      rebalance_start = succ;
    replace(node, succ);
    succ->left = node->left;
    succ->left->parent = succ;
    delete node;
    log("Remove:\n");
    log_forest();
    return rebalance_start;
  }

  // Return rebalance_start, removed
  // O(h)
  virtual std::pair<Node *, bool> remove(const T &x) {
    auto [node, found] = search(x);
    if (!found)
      return {node, false};
    return {remove_node(node), true};
  }
};

template <class T>
class AVLTreeNode : public BaseBinaryTreeNode<T, AVLTreeNode<T>> {
public:
  using Base = BaseBinaryTreeNode<T, AVLTreeNode<T>>;
  using Base::Base;
  using Base::left;
  using Base::right;
  using value_type = T;
  using node_type = Base::node_type;

  mutable int height = 1; // leaf 1

  static int get_height(AVLTreeNode *node) noexcept {
    return node == nullptr ? 0 : node->height;
  }

  static int bf(AVLTreeNode *node) {
    return get_height(node->left) - get_height(node->right);
  }

  void update_height() const {
    height = 1 + std::max(get_height(left), get_height(right));
  }
};

template <class T, class Node = AVLTreeNode<T>, class Compare = std::less<T>>
class AVLTree : public BST<T, Node, Compare> {
public:
  using node_type = Node;
  using Base = BST<T, Node, Compare>;
  using Base::Base;
  using Base::log;
  using Base::log_forest;
  using Base::root;

  AVLTree() : Base() {};
  AVLTree(const AVLTree &) = delete;
  AVLTree &operator=(const AVLTree &) = delete;

  AVLTree(AVLTree &&) = default;
  AVLTree &operator=(AVLTree &&) = default;

  Node *rotate_left(Node *node) const override {
    Node *child = Base::rotate_left(node);
    node->update_height();
    child->update_height();
    return child;
  }

  Node *rotate_right(Node *node) const override {
    Node *child = Base::rotate_right(node);
    node->update_height();
    child->update_height();
    return child;
  }

  // Rebalance until rebalanced once or parent == nullptr
  // up to outer tree if root->parent != nullptr
  virtual Node *rebalance(Node *node) const {
    node->update_height();
    int bf = Node::bf(node);
    if (bf > 1) {
      Node *child = node->left;
      if (Node::bf(child) < 0) { // LR
        log("LR case left-then-right double rotation:\n\n");
        rotate_left(child);
      } else // LL
        log("LL case right single rotation:\n\n");
      return rotate_right(node); // LL, LR
    }
    if (bf < -1) {
      Node *child = node->right;
      if (Node::bf(child) > 0) { // RL
        log("RL case right-then-left double rotation:\n\n");
        rotate_right(child);
      } else // RR
        log("RR case left single rotation:\n\n");
      return rotate_left(node); // RL, RR
    }
    if (node->parent == nullptr)
      return node;
    return rebalance(node->parent);
  }

  std::pair<Node *, bool> insert(const T &x) override {
    auto [node, inserted] = Base::insert(x);
    if (inserted && node->parent != nullptr)
      rebalance(node->parent);
    return {node, inserted};
  }

  Node *remove_node(Node *node) override {
    Node *rebalance_start = Base::remove_node(node);
    Node *cur = rebalance_start;
    while (cur != nullptr) {
      cur = rebalance(cur)->parent;
    }
    return rebalance_start;
  }
};

template <class T, class Node = BinaryTreeNode<T>, class Compare = std::less<T>>
class SplayTree : public BST<T, Node, Compare> {
public:
  using node_type = Node;
  using Base = BST<T, Node, Compare>;
  using Base::Base;
  using Base::comp;
  using Base::is_left_child;
  using Base::log;
  using Base::log_forest;
  using Base::oss;
  using Base::root;
  using Base::rotate_parent;

protected:
  using Base::replace;

public:
  SplayTree() : Base() {};
  SplayTree(const SplayTree &) = delete;
  SplayTree &operator=(const SplayTree &) = delete;

  SplayTree(SplayTree &&) = default;
  SplayTree &operator=(SplayTree &&) = default;

  // O(h) worst case, O(log n) amortized
  virtual Node *splay(Node *node) const {
    if (node == nullptr)
      return nullptr;
    while (node != root) {
      Node *parent = node->parent;
      Node *grandparent = parent->parent;
      // Zig
      if (parent == root) {
        log("Zig step:\n\n");
        rotate_parent(node);
        // Zig-zig
      } else if ((parent->left == node && grandparent->left == parent) ||
                 (parent->right == node && grandparent->right == parent)) {
        log("Zig-zig step:\n\n");
        rotate_parent(parent);
        rotate_parent(node);
      }
      // Zig-zag
      else {
        log("Zig-zag step:\n\n");
        rotate_parent(node);
        rotate_parent(node);
      }
    }
    return node;
  }

  Node *find_min(Node *node) const override {
    return splay(Base::find_min(node));
  }

  Node *find_max(Node *node) const override {
    return splay(Base::find_max(node));
  }

  Node *find_min() const override { return find_min(root); }
  Node *find_max() const override { return find_max(root); }

  Node *ordered_after(const T &x) const override {
    return splay(Base::ordered_after(x));
  }

  Node *ordered_not_before(const T &x) const override {
    return splay(Base::ordered_not_before(x));
  }

  Node *ordered_not_after(const T &x) const override {
    return splay(Base::ordered_not_after(x));
  }

  Node *ordered_before(const T &x) const override {
    return splay(Base::ordered_before(x));
  }

  std::pair<Node *, bool> search(const T &x) const override {
    auto [result, second] = Base::search(x);
    return {splay(result), second};
  }

  std::pair<Node *, bool> insert(const T &x) override {
    auto [result, second] = Base::insert(x);
    return {splay(result), second};
  }

  virtual void join_right(Node *other) {
    if (other == nullptr)
      return;
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      find_max();
      root->right = other;
      other->parent = root;
    }
    log("Join right:\n");
    log_forest();
  }

  virtual void join_left(Node *other) {
    if (other == nullptr)
      return;
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      find_min();
      root->left = other;
      other->parent = root;
    }
    log("Join left:\n");
    log_forest();
  }

  // Not return rebalance_start but always return nullptr
  Node *remove_node(Node *node) override {
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
    Node *victim = splay(node);
    Node *right = victim->right;
    if (victim->left != nullptr) {
      replace(victim, victim->left, true);
      if (right != nullptr)
        right->parent = nullptr;
      join_right(right);
    } else
      replace(victim, right, true);
    return nullptr;
  }

protected:
  virtual SplayTree split_right(Node *other_root) {
    if (other_root == nullptr)
      return SplayTree(nullptr, comp, oss);
    replace(other_root, other_root->left);
    other_root->parent = nullptr;
    other_root->left = nullptr;
    SplayTree result = SplayTree(other_root, comp, oss);
    log_forest();
    result.log_forest();
    return result;
  }

  virtual SplayTree split_left(Node *other_root) {
    if (other_root == nullptr)
      return SplayTree(nullptr, comp, oss);
    replace(other_root, other_root->right);
    other_root->parent = nullptr;
    other_root->right = nullptr;
    SplayTree result = SplayTree(other_root, comp, oss);
    log_forest();
    result.log_forest();
    return result;
  }

public:
  virtual SplayTree split_after(const T &x) {
    log("Split after:\n\n");
    return split_right(ordered_after(x));
  }

  virtual SplayTree split_not_before(const T &x) {
    log("Split not before:\n\n");
    return split_right(ordered_not_before(x));
  }

  virtual SplayTree split_before(const T &x) {
    log("Split before:\n\n");
    return split_left(ordered_before(x));
  }

  virtual SplayTree split_not_after(const T &x) {
    log("Split not after:\n\n");
    return split_left(ordered_not_after(x));
  }

  // O(h) worst case, O(log n) amortized
  virtual std::pair<Node *, bool> top_down_splay_search(const T &x) const {
    if (root == nullptr)
      return {nullptr, false};
    bool found = false;
    Node *old_parent = root->parent;
    signed char old_side = is_left_child(root);
    Node *L = nullptr;
    Node *l = nullptr;
    Node *R = nullptr;
    Node *r = nullptr;
    Node *m = root;
    while (true) {
      if (comp(x, m->value)) {
        if (m->left == nullptr)
          break;
        if (comp(m->left->value, x)) {
          if (R == nullptr) {
            R = m;
            r = m;
            m = m->left;
            R->left = nullptr;
            m->parent = nullptr;
          } else {
            r->left = m;
            m->parent = r;
            r = m;
            m = m->left;
            m->parent = nullptr;
            log("Right tree:\n");
            log_forest(R);
          }
        } else
          m = rotate_parent(m->left);
      } else if (comp(m->value, x)) {
        if (m->right == nullptr)
          break;
        if (comp(x, m->right->value)) {
          if (L == nullptr) {
            L = m;
            l = m;
            m = m->right;
            L->right = nullptr;
            m->parent = nullptr;
          } else {
            l->right = m;
            m->parent = l;
            l = m;
            m = m->right;
            m->parent = nullptr;
            log("left tree:\n");
            log_forest(L);
          }
        } else
          m = rotate_parent(m->right);
      } else {
        found = true;
        break;
      }
    }
    root = m;
    if (L != nullptr) {
      if (m->left != nullptr) {
        l->right = m->left;
        m->left->parent = l;
        log("left tree:\n");
        log_forest(L);
      }
      m->left = L;
      L->parent = m;
    }
    if (R != nullptr) {
      if (m->right != nullptr) {
        r->left = m->right;
        m->right->parent = r;
        log("Right tree:\n");
        log_forest(R);
      }
      m->right = R;
      R->parent = m;
    }
    root->parent = old_parent;
    switch (old_side) {
    case 1:
      old_parent->left = root;
      break;
    case 0:
      old_parent->right = root;
      break;
    }
    return {root, found};
  }
};
