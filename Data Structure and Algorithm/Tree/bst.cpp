#include <concepts>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

template <class T, class Node> class BaseBinaryTreeNode {
public:
  T value;
  Node *parent;
  Node *left = nullptr;
  Node *right = nullptr;

  BaseBinaryTreeNode(const T &value = T(), Node *parent = nullptr)
      : value(value), parent(parent) {}

  virtual void update() {}

  virtual ~BaseBinaryTreeNode() = default;
};

template <class T>
class BinaryTreeNode : public BaseBinaryTreeNode<T, BinaryTreeNode<T>> {
  using Base = BaseBinaryTreeNode<T, BinaryTreeNode<T>>;
  using Base::Base;
};

template <class T, class Node = BinaryTreeNode<T>> class BinaryTreeStatic {
public:
  static std::vector<T> preorder(Node *node) {
    if (node == nullptr)
      return std::vector<T>();
    std::vector<T> result;
    std::vector<Node *> stack;
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

  static std::vector<Node *> preorder_node(Node *node) {
    if (node == nullptr)
      return std::vector<Node *>();
    std::vector<Node *> result, stack;
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
};

template <class T, class Node = BinaryTreeNode<T>>
class BinaryTree : public BinaryTreeStatic<T, Node> {
public:
  using Base = BinaryTreeStatic<T, Node>;
  using Base::inorder;
  using Base::inorder_node;
  using Base::is_left_child;
  using Base::leftmost_child;
  using Base::postorder;
  using Base::postorder_node;
  using Base::preorder;
  using Base::preorder_node;
  using Base::rightmost_child;

  Node *root; // Allow root->parent != nullptr

  BinaryTree(Node *root = nullptr) : root(root) {}

  BinaryTree(const BinaryTree &) = delete;

  BinaryTree(BinaryTree &&other) noexcept : root(other.root) {
    other.root = nullptr;
  }

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

protected:
  // O(1)
  virtual Node *replace(Node *node, Node *replacement, bool deleting = false) {
    if (node == nullptr)
      throw std::invalid_argument("Node is nullptr");
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
    if (deleting)
      delete node;
    return parent;
  }

public:
  void clear(Node *node) {
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
    std::vector<Node *> stack = preorder_node(node);
    for (auto it = stack.rbegin(); it != stack.rend(); ++it)
      delete *it;
  }

  void clear() { clear(root); }

  virtual ~BinaryTree() { clear(); }

  virtual BinaryTree &operator=(const BinaryTree &) = delete;

  virtual BinaryTree &operator=(BinaryTree &&other) noexcept {
    if (this != &other) {
      clear();
      root = other.root;
      other.root = nullptr;
    }
    return *this;
  }

  // O(1)
  virtual Node *rotate_right(Node *node) {
    if (node == nullptr || node->left == nullptr)
      throw std::invalid_argument("Node is nullptr");
    Node *child = node->left;
    replace(node, child);
    node->left = child->right;
    if (child->right != nullptr)
      child->right->parent = node;
    child->right = node;
    node->parent = child;
    node->update();
    child->update();
    return child;
  }

  // O(1)
  virtual Node *rotate_left(Node *node) {
    if (node == nullptr || node->right == nullptr)
      throw std::invalid_argument("Node is nullptr");
    Node *child = node->right;
    replace(node, child);
    node->right = child->left;
    if (child->left != nullptr)
      child->left->parent = node;
    child->left = node;
    node->parent = child;
    node->update();
    child->update();
    return child;
  }

  // O(1)
  virtual Node *rotate_parent(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      throw std::invalid_argument("Node is nullptr");
    if (is_left_child(child) == 1)
      return rotate_right(child->parent);
    return rotate_left(child->parent);
  }
};

template <class T, class Compare = std::less<T>>
concept CompareCallable = requires(const T &a, const T &b) {
  { Compare{}(a, b) } -> std::convertible_to<bool>;
};

template <class T, class Node = BinaryTreeNode<T>, class Compare = std::less<T>>
  requires CompareCallable<T, Compare>
class BST : public BinaryTree<T, Node> {
public:
  using Base = BinaryTree<T, Node>;
  using Base::Base;
  using Base::clear;
  using Base::inorder;
  using Base::inorder_node;
  using Base::leftmost_child;
  using Base::postorder;
  using Base::postorder_node;
  using Base::preorder;
  using Base::preorder_node;
  using Base::replace;
  using Base::rightmost_child;
  using Base::root;
  using Base::rotate_left;
  using Base::rotate_right;

  [[no_unique_address]] Compare comp{};

  // O(h)
  virtual Node *find_min(Node *node) const { return leftmost_child(node); }
  virtual Node *find_min() const { return leftmost_child(); }
  virtual Node *find_max(Node *node) const { return rightmost_child(node); }
  virtual Node *find_max() const { return rightmost_child(); }

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

  // Return smallest node > x
  // O(h)
  virtual Node *upper_bound(const T &x) const {
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

  // Return smallest node >= x
  // O(h)
  virtual Node *lower_bound(const T &x) const {
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

  // Return node, inserted
  // O(h)
  virtual std::pair<Node *, bool> insert(const T &x) {
    if (root == nullptr) {
      root = new Node(x);
      return {root, true};
    }
    Node *node = root;
    while (true) {
      if (comp(x, node->value)) {
        if (node->left == nullptr) {
          node->left = new Node(x, node);
          return {node->left, true};
        }
        node = node->left;
      } else if (comp(node->value, x)) {
        if (node->right == nullptr) {
          node->right = new Node(x, node);
          return {node->right, true};
        }
        node = node->right;
      } else
        return {node, false};
    }
  }

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
    return rebalance_start;
  }

  // Return parent, removed
  // O(h)
  virtual std::pair<Node *, bool> remove(const T &x) {
    auto [node, found] = search(x);
    if (!found)
      return {node, false};
    return {remove_node(node), true};
  }
};

template <class T> class AVLNode : public BaseBinaryTreeNode<T, AVLNode<T>> {
public:
  using Base = BaseBinaryTreeNode<T, AVLNode<T>>;
  using Base::Base;
  using Base::left;
  using Base::right;

  int height = 1; // leaf 1

  static int get_height(AVLNode *node) noexcept {
    return node == nullptr ? 0 : node->height;
  }

  static int bf(AVLNode *node) {
    return get_height(node->left) - get_height(node->right);
  }

  void update() override {
    height = 1 + max(get_height(left), get_height(right));
  }
};

template <class T, class Node = AVLNode<T>, class Compare = std::less<T>>
class AVL : public BST<T, Node, Compare> {
public:
  using Base = BST<T, Node, Compare>;
  using Base::Base;
  using Base::root;

  virtual Node *rebalance(Node *node) {
    if (node == root)
      return nullptr;
    node->update();
    int bf = node->bf();
    if (bf > 1) {
      Node *child = node->left;
      if (child->bf() < 0) // LR
        rotate_left(child);
      return rotate_right(node); // LL, LR
    }
    if (bf < -1) {
      Node *child = node->right;
      if (child->bf() > 0) // RL
        rotate_right(child);
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
    if (rebalance_start == root)
      return rebalance_start;
    Node *cur = rebalance_start;
    while (cur != nullptr) {
      cur = rebalance(cur)->parent;
    }
    return rebalance_start;
  }
};

template <class T, class Node = BinaryTreeNode<T>>
class Splay : public BST<T, Node> {
public:
  using Base = BST<T>;

  virtual Node *splay(Node *node) {
    if (node == nullptr)
      return nullptr;
    while (node->parent != nullptr) {
      Node *parent = node->parent;
      Node *grandparent = parent->parent;
      // Zig
      if (grandparent == nullptr)
        this->rotate(node);
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

  Node *find_min(Node *node) override {
    auto result = Base::find_min(node);
    return splay(result);
  }

  Node *find_max(Node *node) override {
    auto result = Base::find_max(node);
    return splay(result);
  }

  std::pair<Node *, bool> search(const T &x) override {
    auto [result, found] = Base::search(x);
    return {splay(result), found};
  }
};
