#include <algorithm>
#include <vector>
using namespace std;

template <class T> struct BST_Node {
  using value_type = T;
  T value;
  BST_Node<T> *parent;
  BST_Node<T> *left;
  BST_Node<T> *right;

  BST_Node(T value, BST_Node<T> *parent = nullptr)
      : value(value), parent(parent), left(nullptr), right(nullptr) {}
};

template <class T, class Node = BST_Node<T>> class BST {
public:
  using value_type = T;

  Node *root = nullptr; // Allow root->parent != nullptr

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

  // Return node, found
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

  // Return largest node <= x
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

  // Return smallest node >= x
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

  // Return node, inserted
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

  // O(h)
  virtual Node *remove_node(Node *node) {
    if (node->left == nullptr && node->right == nullptr)
      return transplant(node, nullptr);
    if (node->left == nullptr)
      return transplant(node, node->right);
    if (node->right == nullptr)
      return transplant(node, node->left);
    Node *succ = inorder_successor(node);
    node->value = succ->value;
    Node *parent = transplant(succ, succ->right);
    delete succ;
    return parent;
  }

  // Return parent, removed
  // O(h)
  virtual pair<Node *, bool> remove(const T &x) {
    auto [node, found] = search(x);
    if (!found)
      return {nullptr, false};
    return {remove_node(node), true};
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
  // -1: node or its parent is nullptr
  char is_left_child(Node *node) const {
    if (node == nullptr || node->parent == nullptr)
      return -1;
    return char(node->parent->left == node);
  }

  Node *rotate_right(Node *parent) {
    if (parent == nullptr || parent->left == nullptr)
      return nullptr;
    Node *child = parent->left;
    transplant(parent, child);
    parent->left = child->right;
    if (child->right != nullptr)
      child->right->parent = parent;
    child->right = parent;
    parent->parent = child;
    return child;
  }

  Node *rotate_left(Node *parent) {
    if (parent == nullptr || parent->right == nullptr)
      return nullptr;
    Node *child = parent->right;
    transplant(parent, child);
    parent->right = child->left;
    if (child->left != nullptr)
      child->left->parent = parent;
    child->left = parent;
    parent->parent = child;
    return child;
  }

  Node *rotate_parent(Node *child) {
    if (child == nullptr || child->parent == nullptr)
      return nullptr;
    if (is_left_child(child) == 1)
      return rotate_right(child->parent);
    return rotate_left(child->parent);
  }
};

template <class T> struct AVL_Node {
  using value_type = T;
  T value;
  AVL_Node<T> *parent;
  AVL_Node<T> *left;
  AVL_Node<T> *right;
  unsigned int height; // leaf 1

  AVL_Node(T value, AVL_Node<T> *parent = nullptr)
      : value(value), parent(parent), left(nullptr), right(nullptr), height(1) {
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
    if (parent == nullptr)
      return {nullptr, removed};
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
    auto [node, found] = Base::search(x);
    return {splay(node), found};
  }
};
