#pragma once
#include <algorithm>
#include <cctype>
#include <complex>
#include <concepts>
#include <cstddef>
#include <functional>
#include <istream>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
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

  virtual void _clone_helper(Node *) const {}
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

  // O(n)
  // Deep-clones the subtree rooted at `node` with given parent of new root used
  // Return the new root
  static Node *clone_subtree(const Node *node, Node *parent = nullptr) {
    if (node == nullptr)
      return nullptr;
    Node *copy = new Node(node->value, parent);
    node->_clone_helper(copy);
    copy->left = clone_subtree(node->left, copy);
    copy->right = clone_subtree(node->right, copy);
    return copy;
  }
};

// Take LaTeX forest representation: [value[left][right]]
// Output root node
template <class T, class Node = BinaryTreeNode<T>>
  requires std::default_initializable<T> &&
           requires(std::istream &is, T &value) {
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
  using Base::clone_subtree;
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

  BinaryTree(const BinaryTree &other)
      : root(clone_subtree(other.root,
                           other.root ? other.root->parent : nullptr)),
        oss(other.oss) {}

  BinaryTree(BinaryTree &&other) noexcept
      : root(std::exchange(other.root, nullptr)),
        oss(std::exchange(other.oss, nullptr)) {}

  void set_logger(std::ostream *_oss) { oss = _oss; }

  bool is_empty() const { return root == nullptr; }

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

  BinaryTree &operator=(const BinaryTree &other) {
    if (this == &other)
      return *this;
    clear();
    root = clone_subtree(other.root, other.root ? other.root->parent : nullptr);
    oss = other.oss;
    return *this;
  }

  BinaryTree &operator=(BinaryTree &&other) noexcept {
    if (this == &other)
      return *this;
    clear();
    root = std::exchange(other.root, nullptr);
    oss = std::exchange(other.oss, nullptr);
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
    log("Rotate right:");
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
    log("Rotate left:");
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
  BST(Node *_root = nullptr, std::ostream *_oss = nullptr,
      const Compare &_comp = Compare())
      : Base(_root, _oss), comp(_comp) {}
  explicit BST(const Compare &_comp) : BST(nullptr, nullptr, _comp) {}

  BST(const BST &other) : Base(other), comp(other.comp) {}
  BST(BST &&other) : Base(other), comp(other.comp) {}

  BST &operator=(const BST &other) {
    if (this != &other) {
      Base::operator=(other);
      comp = other.comp;
    }
    return *this;
  }

  BST &operator=(BST &&other) {
    if (this != &other) {
      Base::operator=(other);
      comp = std::exchange(other.comp, nullptr);
    }
    return *this;
  }

  // O(h)
  virtual Node *find_min() const { return leftmost_child(root); }
  virtual Node *find_max() const { return rightmost_child(root); }

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
      log("Insert:");
      log_forest();
      return {root, true};
    }
    Node *node = root;
    while (true) {
      if (comp(x, node->value)) {
        if (node->left == nullptr) {
          node->left = new Node(x, node);
          log("Insert:");
          log_forest();
          return {node->left, true};
        }
        node = node->left;
      } else if (comp(node->value, x)) {
        if (node->right == nullptr) {
          node->right = new Node(x, node);
          log("Insert:");
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
      log("Delete:");
      Node *result = replace(node, node->right, true);
      log_forest();
      return result;
    }
    if (node->right == nullptr) {
      log("Delete:");
      Node *result = replace(node, node->left, true);
      log_forest();
      return result;
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
    log("Delete:");
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

  void _clone_helper(AVLTreeNode *other) const override {
    other->height = height;
  }

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

  AVLTree(const AVLTree &) = default;
  AVLTree(AVLTree &&) = default;
  AVLTree &operator=(const AVLTree &) = default;
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
      if (Node::bf(child) < 0) {
        log("LR case left-then-right double rotation:\n");
        rotate_left(child);
      } else
        log("LL case right single rotation:\n");
      return rotate_right(node);
    }
    if (bf < -1) {
      Node *child = node->right;
      if (Node::bf(child) > 0) {
        log("RL case right-then-left double rotation:\n");
        rotate_right(child);
      } else
        log("RR case left single rotation:\n");
      return rotate_left(node);
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
  using Base::rotate_left;
  using Base::rotate_parent;
  using Base::rotate_right;

protected:
  using Base::replace;

public:
  SplayTree(const SplayTree &) = default;
  SplayTree(SplayTree &&) = default;
  SplayTree &operator=(const SplayTree &) = default;
  SplayTree &operator=(SplayTree &&) = default;

  // O(h) worst case, O(log n) amortized
  virtual Node *splay(Node *node) const {
    if (node == nullptr)
      return nullptr;
    while (node != root) {
      Node *parent = node->parent;
      Node *grandparent = parent->parent;
      if (parent == root) {
        log("Zig step:\n");
        rotate_parent(node);
      } else if (parent->left == node && grandparent->left == parent) {
        log("Zig-zig step:\n");
        rotate_right(grandparent);
        rotate_right(parent);
      } else if (parent->right == node && grandparent->right == parent) {
        log("Zig-zig step:\n");
        rotate_left(grandparent);
        rotate_left(parent);
      } else {
        log("Zig-zag step:\n");
        rotate_parent(node);
        rotate_parent(node);
      }
    }
    return node;
  }

  Node *find_min() const override { return splay(Base::find_min()); }
  Node *find_max() const override { return splay(Base::find_max()); }

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
    log("Join right:\n");
    log("To join:");
    log_forest(other);
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      find_max();
      root->right = other;
      other->parent = root;
    }
    log("Join:");
    log_forest();
  }

  virtual void join_left(Node *other) {
    if (other == nullptr)
      return;
    log("Join left:\n");
    log("To join:");
    log_forest(other);
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      find_min();
      root->left = other;
      other->parent = root;
    }
    log("Join:");
    log_forest();
  }

  // Not return rebalance_start but always return nullptr if node != nullptr
  Node *remove_node(Node *node) override {
    log("Delete:\n");
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
    splay(node);
    Node *right = root->right;
    if (root->left != nullptr) {
      replace(root, root->left, true);
      if (right != nullptr)
        right->parent = nullptr;
      join_right(right);
    } else {
      replace(root, right, true);
      log_forest();
    }
    return nullptr;
  }

protected:
  virtual SplayTree split_right(Node *other_root) {
    if (other_root == nullptr)
      return SplayTree(nullptr, oss, comp);
    replace(other_root, other_root->left);
    other_root->parent = nullptr;
    other_root->left = nullptr;
    SplayTree result = SplayTree(other_root, oss, comp);
    log_forest();
    result.log_forest();
    return result;
  }

  virtual SplayTree split_left(Node *other_root) {
    if (other_root == nullptr)
      return SplayTree(nullptr, oss, comp);
    replace(other_root, other_root->right);
    other_root->parent = nullptr;
    other_root->right = nullptr;
    SplayTree result = SplayTree(other_root, oss, comp);
    log_forest();
    result.log_forest();
    return result;
  }

public:
  virtual SplayTree split_after(const T &x) {
    log("Split after:\n");
    return split_right(ordered_after(x));
  }

  virtual SplayTree split_not_before(const T &x) {
    log("Split not before:\n");
    return split_right(ordered_not_before(x));
  }

  virtual SplayTree split_before(const T &x) {
    log("Split before:\n");
    return split_left(ordered_before(x));
  }

  virtual SplayTree split_not_after(const T &x) {
    log("Split not after:\n");
    return split_left(ordered_not_after(x));
  }

  // O(h) worst case, O(log n) amortized
  virtual std::pair<Node *, bool> top_down_search(const T &x) const {
    if (root == nullptr)
      return {nullptr, false};
    log("Top-down search:\n");
    bool found = false;
    Node *old_parent = root->parent;
    signed char old_side = is_left_child(root);
    root->parent = nullptr;
    Node *L = nullptr;
    Node *l = nullptr;
    Node *R = nullptr;
    Node *r = nullptr;
    while (true) {
      if (comp(x, root->value)) {
        if (root->left == nullptr)
          break;
        if (comp(x, root->left->value)) {
          log("LL zig-zig step:\n");
          root = rotate_right(root);
          if (root->left == nullptr)
            break;
        } else
          log("LR zig-zag step:\n");
        log("Link right:\n");
        if (R != nullptr) {
          r->left = root;
          root->parent = r;
          r = r->left;
        } else {
          R = root;
          r = R;
        }
        root = root->left;
        root->parent->left = nullptr;
        root->parent = nullptr;
        log("Right tree:");
        log_forest(R);
      } else if (comp(root->value, x)) {
        if (root->right == nullptr)
          break;
        if (comp(root->right->value, x)) {
          log("RR zig-zig step:\n");
          root = rotate_left(root);
          if (root->right == nullptr)
            break;
        } else
          log("RL zig-zag step:\n");
        log("Link left:\n");
        if (L != nullptr) {
          l->right = root;
          root->parent = l;
          l = l->right;
        } else {
          L = root;
          l = L;
        }
        root = root->right;
        root->parent->right = nullptr;
        root->parent = nullptr;
        log("Left tree:");
        log_forest(L);
      } else {
        found = true;
        break;
      }
    }
    if (L != nullptr) {
      if (root->left != nullptr) {
        l->right = root->left;
        root->left->parent = l;
        log("Left tree:");
        log_forest(L);
      }
      root->left = L;
      L->parent = root;
    }
    if (R != nullptr) {
      if (root->right != nullptr) {
        r->left = root->right;
        root->right->parent = r;
        log("Right tree:");
        log_forest(R);
      }
      root->right = R;
      R->parent = root;
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
    log("Link back:");
    log_forest();
    return {root, found};
  }

  // O(h) worst case, O(log n) amortized
  virtual Node *top_down_find_min() const {
    if (root == nullptr)
      return nullptr;
    log("Top-down find min:\n");
    Node *old_parent = root->parent;
    signed char old_side = is_left_child(root);
    root->parent = nullptr;
    Node *R = nullptr;
    Node *r = nullptr;
    while (true) {
      if (root->left == nullptr)
        break;
      log("LL zig-zig step:\n");
      root = rotate_right(root);
      if (root->left == nullptr)
        break;
      log("Link right:\n");
      if (R != nullptr) {
        r->left = root;
        root->parent = r;
        r = r->left;
      } else {
        R = root;
        r = R;
      }
      root = root->left;
      root->parent->left = nullptr;
      root->parent = nullptr;
      log("Right tree:");
      log_forest(R);
    }
    if (R != nullptr) {
      if (root->right != nullptr) {
        r->left = root->right;
        root->right->parent = r;
        log("Right tree:");
        log_forest(R);
      }
      root->right = R;
      R->parent = root;
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
    log("Link back:");
    log_forest();
    return root;
  }

  // O(h) worst case, O(log n) amortized
  virtual Node *top_down_find_max() const {
    if (root == nullptr)
      return nullptr;
    log("Top-down find max:\n");
    Node *old_parent = root->parent;
    signed char old_side = is_left_child(root);
    root->parent = nullptr;
    Node *L = nullptr;
    Node *l = nullptr;
    while (true) {
      if (root->right == nullptr)
        break;
      log("RR zig-zig step:\n");
      root = rotate_left(root);
      if (root->right == nullptr)
        break;
      log("Link left:\n");
      if (L != nullptr) {
        l->right = root;
        root->parent = l;
        l = l->right;
      } else {
        L = root;
        l = L;
      }
      root = root->right;
      root->parent->right = nullptr;
      root->parent = nullptr;
      log("Left tree:");
      log_forest(L);
    }
    if (L != nullptr) {
      if (root->left != nullptr) {
        l->right = root->left;
        root->left->parent = l;
        log("Left tree:");
        log_forest(L);
      }
      root->left = L;
      L->parent = root;
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
    log("Link back:");
    log_forest();
    return root;
  }

  // Return node, inserted
  // O(h)
  virtual std::pair<Node *, bool> top_down_insert(const T &x) {
    if (root == nullptr) {
      root = new Node(x);
      log("Top-down insert:\n");
      log("Insert:");
      log_forest();
      return {root, true};
    }
    log("Top-down insert:\n");
    auto [node, found] = top_down_search(x);
    if (found)
      return {root, false};
    root = new Node(x);
    if (comp(x, node->value)) {
      root->left = node->left;
      node->left->parent = root;
      node->left = nullptr;
      root->right = node;
    } else {
      root->right = node->right;
      node->right->parent = root;
      node->right = nullptr;
      root->left = node;
    }
    root->parent = node->parent;
    switch (is_left_child(node)) {
    case 1:
      node->parent->left = root;
      break;
    case 0:
      node->parent->right = root;
      break;
    }
    node->parent = root;
    log("Insert:");
    log_forest();
    return {root, true};
  }

  virtual void top_down_join_right(Node *other) {
    if (other == nullptr)
      return;
    log("Top-down join right:\n");
    log("To join:");
    log_forest(other);
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      top_down_find_max();
      root->right = other;
      other->parent = root;
    }
    log("Join:");
    log_forest();
  }

  virtual void top_down_join_left(Node *other) {
    if (other == nullptr)
      return;
    log("Top-dowm join left:\n");
    log("To join:");
    log_forest(other);
    if (root == nullptr) {
      root = other;
      other->parent = nullptr;
    } else {
      top_down_find_min();
      root->left = other;
      other->parent = root;
    }
    log("Join:");
    log_forest();
  }

  // O(h) worst case, O(log n) amortized
  virtual std::pair<Node *, bool> top_down_remove(const T &x) {
    log("Top-down delete:\n");
    auto [victim, found] = top_down_search(x);
    if (found == false)
      return {victim, false};
    log("Delete:\n");
    Node *right = root->right;
    if (root->left != nullptr) {
      replace(root, root->left, true);
      if (right != nullptr)
        right->parent = nullptr;
      top_down_join_right(right);
    } else {
      replace(root, right, true);
      log_forest();
    }
    return {nullptr, true};
  }
};
