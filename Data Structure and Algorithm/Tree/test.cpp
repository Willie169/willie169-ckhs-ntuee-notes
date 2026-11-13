#include <algorithm>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <vector>

#include "binary_tree.h"   // the header shown in the prompt

// ===================================================================
// Test harness
// ===================================================================

namespace test {
int  g_passed = 0;
int  g_failed = 0;

inline void check(bool cond, const char *expr, int line) {
  if (cond) ++g_passed;
  else { ++g_failed; std::cerr << "FAIL [line " << line << "]: " << expr << std::endl; }
}

template <class A, class B>
inline void check_eq(const A &a, const B &b, const char *ea, const char *eb, int line) {
  if (a == b) ++g_passed;
  else { ++g_failed; std::cerr << "FAIL [line " << line << "]: " << ea << " == " << eb << std::endl; }
}

inline void summary() {
  std::cerr << "\n" << g_passed << " passed, " << g_failed << " failed\n";
}
} // namespace test

#define CHECK(cond)       test::check((cond), #cond, __LINE__)
#define CHECK_EQ(a, b)    test::check_eq((a), (b), #a, #b, __LINE__)

// ===================================================================
// Helpers
// ===================================================================

using IntNode = BinaryTreeNode<int>;

template <class Node>
bool parents_ok(Node *node, Node *expected_parent) {
  if (!node) return true;
  if (node->parent != expected_parent) return false;
  return parents_ok(node->left, node) && parents_ok(node->right, node);
}

// For a normal tree (root->parent == nullptr)
template <class Node>
bool tree_parents_ok(Node *root) { return parents_ok(root, (Node *)nullptr); }

bool avl_bf_ok(AVLNode<int> *n) {
  if (!n) return true;
  int bf = AVLNode<int>::bf(n);
  if (bf < -1 || bf > 1) return false;
  return avl_bf_ok(n->left) && avl_bf_ok(n->right);
}

bool avl_height_ok(AVLNode<int> *n) {
  if (!n) return true;
  int expected = 1 + std::max(AVLNode<int>::get_height(n->left),
                               AVLNode<int>::get_height(n->right));
  if (n->height != expected) return false;
  return avl_height_ok(n->left) && avl_height_ok(n->right);
}

// ===================================================================
// BinaryTree basics
// ===================================================================

void test_binary_tree_traversals() {
  //     4
  //    / \
  //   2   6
  //  / \ / \
  // 1  3 5  7
  auto *n4 = new IntNode(4);
  auto *n2 = new IntNode(2, n4);
  auto *n6 = new IntNode(6, n4);
  auto *n1 = new IntNode(1, n2);
  auto *n3 = new IntNode(3, n2);
  auto *n5 = new IntNode(5, n6);
  auto *n7 = new IntNode(7, n6);
  n4->left = n2; n4->right = n6;
  n2->left = n1; n2->right = n3;
  n6->left = n5; n6->right = n7;

  BinaryTree<int> bt(n4);

  CHECK_EQ(bt.preorder(),  (std::vector<int>{4, 2, 1, 3, 6, 5, 7}));
  CHECK_EQ(bt.inorder(),   (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
  CHECK_EQ(bt.postorder(), (std::vector<int>{1, 3, 2, 5, 7, 6, 4}));

  CHECK_EQ(bt.preorder_node().size(),  7u);
  CHECK_EQ(bt.inorder_node().size(),   7u);
  CHECK_EQ(bt.postorder_node().size(), 7u);

  // Subtree traversals
  CHECK_EQ(bt.preorder(n2), (std::vector<int>{2, 1, 3}));
  CHECK_EQ(bt.inorder(n6),  (std::vector<int>{5, 6, 7}));

  // Empty subtree returns empty
  CHECK(bt.inorder((IntNode *)nullptr).empty());
}

void test_binary_tree_move() {
  BST<int> bst;
  for (int v : {3, 1, 2}) bst.insert(v);
  auto *orig_root = bst.root;

  BST<int> moved(std::move(bst));
  CHECK(bst.root == nullptr);
  CHECK(moved.root == orig_root);
  CHECK_EQ(moved.inorder(), (std::vector<int>{1, 2, 3}));

  BST<int> bst2;
  for (int v : {10, 20}) bst2.insert(v);
  bst2 = std::move(moved);
  CHECK(moved.root == nullptr);
  CHECK_EQ(bst2.inorder(), (std::vector<int>{1, 2, 3}));
}

void test_binary_tree_clear() {
  for (int mode : {-1, 0, 1}) {
    BST<int> bst;
    for (int v : {5, 3, 7, 1, 4}) bst.insert(v);
    bst.clear(bst.root, (signed char)mode);
    CHECK(bst.root == nullptr);
  }
  BST<int> bst;
  for (int v : {5, 3, 7}) bst.insert(v);
  bst.clear();
  CHECK(bst.root == nullptr);
  bst.insert(10);
  CHECK(bst.root != nullptr);
}

void test_binary_tree_leftmost_rightmost() {
  BST<int> bst;
  CHECK(bst.leftmost_child()  == nullptr);
  CHECK(bst.rightmost_child() == nullptr);

  for (int v : {5, 3, 7, 1, 9}) bst.insert(v);
  CHECK(bst.leftmost_child()->value  == 1);
  CHECK(bst.rightmost_child()->value == 9);

  // Starting from a subtree
  CHECK(bst.leftmost_child(bst.root->right)->value  == 7);
  CHECK(bst.rightmost_child(bst.root->left)->value  == 3);
}

// ===================================================================
// BST basics
// ===================================================================

void test_bst_insert_search() {
  BST<int> bst;
  CHECK(bst.search(5).second == false);
  CHECK(bst.root == nullptr);

  auto [n5, ok5] = bst.insert(5);
  CHECK(ok5 && n5->value == 5 && bst.root == n5);

  auto [n5dup, ok5dup] = bst.insert(5);
  CHECK(!ok5dup && n5dup == n5);

  bst.insert(3);
  bst.insert(7);

  auto [f3, ok3] = bst.search(3);
  CHECK(ok3 && f3->value == 3);

  auto [f4, ok4] = bst.search(4);
  CHECK(!ok4);
  CHECK(f4->value == 3);   // insertion point returned when not found

  CHECK_EQ(bst.inorder(), (std::vector<int>{3, 5, 7}));
}

void test_bst_traversals() {
  BST<int> bst;
  CHECK(bst.preorder().empty());
  CHECK(bst.inorder().empty());
  CHECK(bst.postorder().empty());

  for (int v : {4, 2, 6, 1, 3, 5, 7}) bst.insert(v);
  CHECK_EQ(bst.inorder(),   (std::vector<int>{1, 2, 3, 4, 5, 6, 7}));
  CHECK_EQ(bst.preorder(),  (std::vector<int>{4, 2, 1, 3, 6, 5, 7}));
  CHECK_EQ(bst.postorder(), (std::vector<int>{1, 3, 2, 5, 7, 6, 4}));
}

void test_bst_ordered() {
  BST<int> bst;
  for (int v : {1, 3, 5, 7, 9}) bst.insert(v);

  CHECK(bst.ordered_after(5)->value == 7);
  CHECK(bst.ordered_after(6)->value == 7);
  CHECK(bst.ordered_after(9)        == nullptr);
  CHECK(bst.ordered_after(0)->value == 1);

  CHECK(bst.ordered_not_before(5)->value == 5);
  CHECK(bst.ordered_not_before(6)->value == 7);
  CHECK(bst.ordered_not_before(10)       == nullptr);
  CHECK(bst.ordered_not_before(0)->value == 1);

  CHECK(bst.ordered_before(5)->value == 3);
  CHECK(bst.ordered_before(4)->value == 3);
  CHECK(bst.ordered_before(1)        == nullptr);
  CHECK(bst.ordered_before(10)->value == 9);

  CHECK(bst.ordered_not_after(5)->value == 5);
  CHECK(bst.ordered_not_after(4)->value == 3);
  CHECK(bst.ordered_not_after(0)        == nullptr);
  CHECK(bst.ordered_not_after(10)->value == 9);
}

void test_bst_successor_predecessor() {
  BST<int> bst;
  for (int v : {5, 3, 7, 2, 4, 6, 8}) bst.insert(v);

  auto get = [&](int v) {
    auto [n, ok] = bst.search(v);
    CHECK(ok);
    return n;
  };

  CHECK(bst.inorder_successor(get(5))->value == 6);
  CHECK(bst.inorder_successor(get(8))        == nullptr);
  CHECK(bst.inorder_successor(get(2))->value == 3);
  CHECK(bst.inorder_successor(get(3))->value == 4);

  CHECK(bst.inorder_predecessor(get(5))->value == 4);
  CHECK(bst.inorder_predecessor(get(2))        == nullptr);
  CHECK(bst.inorder_predecessor(get(7))->value == 6);
}

void test_bst_remove() {
  // leaf
  {
    BST<int> bst;
    for (int v : {5, 3, 7}) bst.insert(v);
    auto [p, ok] = bst.remove(3);
    CHECK(ok);
    CHECK_EQ(bst.inorder(), (std::vector<int>{5, 7}));
    CHECK(tree_parents_ok(bst.root));
  }
  // only node
  {
    BST<int> bst;
    bst.insert(5);
    CHECK(bst.remove(5).second);
    CHECK(bst.root == nullptr);
  }
  // one child (left)
  {
    BST<int> bst;
    for (int v : {5, 3}) bst.insert(v);
    CHECK(bst.remove(5).second);
    CHECK(bst.root->value == 3);
    CHECK(tree_parents_ok(bst.root));
  }
  // one child (right)
  {
    BST<int> bst;
    for (int v : {5, 7}) bst.insert(v);
    CHECK(bst.remove(5).second);
    CHECK(bst.root->value == 7);
  }
  // two children, successor is right child
  {
    BST<int> bst;
    for (int v : {5, 3, 7}) bst.insert(v);
    CHECK(bst.remove(5).second);
    CHECK(bst.root->value == 7);
    CHECK_EQ(bst.inorder(), (std::vector<int>{3, 7}));
    CHECK(tree_parents_ok(bst.root));
  }
  // two children, successor not right child
  {
    BST<int> bst;
    for (int v : {5, 3, 7, 6}) bst.insert(v);
    CHECK(bst.remove(5).second);
    CHECK(bst.root->value == 6);
    CHECK_EQ(bst.inorder(), (std::vector<int>{3, 6, 7}));
    CHECK(tree_parents_ok(bst.root));
  }
  // not found
  {
    BST<int> bst;
    for (int v : {5, 3, 7}) bst.insert(v);
    CHECK(!bst.remove(4).second);
    CHECK_EQ(bst.inorder(), (std::vector<int>{3, 5, 7}));
  }
}

void test_bst_custom_comparator() {
  BST<int, IntNode, std::greater<int>> bst;
  for (int v : {5, 3, 7, 1, 9}) bst.insert(v);
  CHECK_EQ(bst.inorder(), (std::vector<int>{9, 7, 5, 3, 1}));
}

void test_bst_stress() {
  BST<int> bst;
  std::set<int> ref;
  std::mt19937 rng(42);

  for (int iter = 0; iter < 2000; ++iter) {
    int v = (int)(rng() % 100);
    if (rng() % 2 == 0) { bst.insert(v); ref.insert(v); }
    else                { bst.remove(v); ref.erase(v);  }
    if (iter % 50 == 0) {
      CHECK_EQ(bst.inorder(), (std::vector<int>(ref.begin(), ref.end())));
      CHECK(tree_parents_ok(bst.root));
    }
  }
  CHECK_EQ(bst.inorder(), (std::vector<int>(ref.begin(), ref.end())));
}

// ===================================================================
// AVL
// ===================================================================

void test_avl_insert_rotations() {
  // LL
  {
    AVL<int> avl;
    avl.insert(3); avl.insert(2); avl.insert(1);
    CHECK(avl.root->value == 2);
    CHECK(avl.root->left->value  == 1);
    CHECK(avl.root->right->value == 3);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
  }
  // RR
  {
    AVL<int> avl;
    avl.insert(1); avl.insert(2); avl.insert(3);
    CHECK(avl.root->value == 2);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
  }
  // LR
  {
    AVL<int> avl;
    avl.insert(3); avl.insert(1); avl.insert(2);
    CHECK(avl.root->value == 2);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
  }
  // RL
  {
    AVL<int> avl;
    avl.insert(1); avl.insert(3); avl.insert(2);
    CHECK(avl.root->value == 2);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
  }
}

void test_avl_insert_balance() {
  AVL<int> avl;
  std::vector<int> vals(100);
  for (int i = 0; i < 100; ++i) vals[i] = i;
  std::shuffle(vals.begin(), vals.end(), std::mt19937(42));

  for (int v : vals) {
    avl.insert(v);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
    CHECK(tree_parents_ok(avl.root));
  }
}

void test_avl_remove_balance() {
  AVL<int> avl;
  std::vector<int> vals(50);
  for (int i = 0; i < 50; ++i) vals[i] = i;
  std::shuffle(vals.begin(), vals.end(), std::mt19937(42));
  for (int v : vals) avl.insert(v);

  std::shuffle(vals.begin(), vals.end(), std::mt19937(43));
  for (int v : vals) {
    avl.remove(v);
    CHECK(avl_bf_ok(avl.root));
    CHECK(avl_height_ok(avl.root));
    CHECK(tree_parents_ok(avl.root));
  }
  CHECK(avl.root == nullptr);
}

// ===================================================================
// Splay
// ===================================================================

void test_splay_insert_and_search() {
  Splay<int> sp;
  for (int v : {1, 2, 3, 4, 5}) sp.insert(v);
  CHECK(sp.root->value == 5);                // last inserted is root

  sp.search(1);
  CHECK(sp.root->value == 1);
  CHECK_EQ(sp.inorder(), (std::vector<int>{1, 2, 3, 4, 5}));

  sp.search(3);
  CHECK(sp.root->value == 3);
  CHECK_EQ(sp.inorder(), (std::vector<int>{1, 2, 3, 4, 5}));

  CHECK(tree_parents_ok(sp.root));
}

void test_splay_find_min_max() {
  Splay<int> sp;
  for (int v : {5, 3, 7, 1, 9}) sp.insert(v);

  sp.find_min();
  CHECK(sp.root->value == 1);

  sp.find_max();
  CHECK(sp.root->value == 9);
}

void test_splay_ordered() {
  Splay<int> sp;
  for (int v : {5, 3, 7, 1, 9}) sp.insert(v);

  IntNode *n = sp.ordered_after(3);
  CHECK(n->value == 5);
  CHECK(sp.root == n);

  n = sp.ordered_before(7);
  CHECK(n->value == 5);
  CHECK(sp.root == n);
}

void test_splay_remove() {
  Splay<int> sp;
  for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);

  sp.remove(4);
  CHECK_EQ(sp.inorder(), (std::vector<int>{1, 2, 3, 5, 6, 7}));
  CHECK(tree_parents_ok(sp.root));

  sp.remove(1);
  CHECK_EQ(sp.inorder(), (std::vector<int>{2, 3, 5, 6, 7}));
  CHECK(tree_parents_ok(sp.root));

  sp.remove(7);
  CHECK_EQ(sp.inorder(), (std::vector<int>{2, 3, 5, 6}));
  CHECK(tree_parents_ok(sp.root));
}

// ===================================================================
// Splay split
// ===================================================================

void test_splay_split_partition() {
  // split_after: this = {≤ x}, returned = {> x}
  for (int x : {-1, 0, 3, 5, 7, 10}) {
    Splay<int> sp;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);
    Splay<int> right = sp.split_after(x);

    std::vector<int> el, er;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) (v <= x ? el : er).push_back(v);
    CHECK_EQ(sp.inorder(),    el);
    CHECK_EQ(right.inorder(), er);
    CHECK(tree_parents_ok(sp.root));
    CHECK(tree_parents_ok(right.root));
  }

  // split_not_before: this = {< x}, returned = {≥ x}
  for (int x : {-1, 0, 3, 5, 7, 10}) {
    Splay<int> sp;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);
    Splay<int> right = sp.split_not_before(x);

    std::vector<int> el, er;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) (v < x ? el : er).push_back(v);
    CHECK_EQ(sp.inorder(),    el);
    CHECK_EQ(right.inorder(), er);
    CHECK(tree_parents_ok(sp.root));
    CHECK(tree_parents_ok(right.root));
  }

  // split_before: this = {≥ x}, returned = {< x}
  for (int x : {-1, 0, 3, 5, 7, 10}) {
    Splay<int> sp;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);
    Splay<int> left = sp.split_before(x);

    std::vector<int> el, er;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) (v < x ? el : er).push_back(v);
    CHECK_EQ(left.inorder(), el);
    CHECK_EQ(sp.inorder(),   er);
    CHECK(tree_parents_ok(left.root));
    CHECK(tree_parents_ok(sp.root));
  }

  // split_not_after: this = {> x}, returned = {≤ x}
  for (int x : {-1, 0, 3, 5, 7, 10}) {
    Splay<int> sp;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);
    Splay<int> left = sp.split_not_after(x);

    std::vector<int> el, er;
    for (int v : {1, 2, 3, 4, 5, 6, 7}) (v <= x ? el : er).push_back(v);
    CHECK_EQ(left.inorder(), el);
    CHECK_EQ(sp.inorder(),   er);
    CHECK(tree_parents_ok(left.root));
    CHECK(tree_parents_ok(sp.root));
  }
}

void test_splay_split_edge_cases() {
  // Empty tree
  for (int which = 0; which < 4; ++which) {
    Splay<int> sp;
    Splay<int> other =
        which == 0 ? sp.split_after(5)
      : which == 1 ? sp.split_not_before(5)
      : which == 2 ? sp.split_before(5)
                   : sp.split_not_after(5);
    CHECK(sp.root == nullptr);
    CHECK(other.root == nullptr);
  }

  // x smaller than all
  for (int which = 0; which < 4; ++which) {
    Splay<int> sp;
    for (int v : {3, 5, 7}) sp.insert(v);
    Splay<int> other =
        which == 0 ? sp.split_after(0)
      : which == 1 ? sp.split_not_before(0)
      : which == 2 ? sp.split_before(0)
                   : sp.split_not_after(0);
    if (which == 0 || which == 1) {         // returned = whole tree
      CHECK(sp.root == nullptr);
      CHECK_EQ(other.inorder(), (std::vector<int>{3, 5, 7}));
    } else {                                // this = whole tree
      CHECK_EQ(sp.inorder(), (std::vector<int>{3, 5, 7}));
      CHECK(other.root == nullptr);
    }
  }

  // x larger than all
  for (int which = 0; which < 4; ++which) {
    Splay<int> sp;
    for (int v : {3, 5, 7}) sp.insert(v);
    Splay<int> other =
        which == 0 ? sp.split_after(10)
      : which == 1 ? sp.split_not_before(10)
      : which == 2 ? sp.split_before(10)
                   : sp.split_not_after(10);
    if (which == 0 || which == 1) {         // this = whole tree
      CHECK_EQ(sp.inorder(), (std::vector<int>{3, 5, 7}));
      CHECK(other.root == nullptr);
    } else {                                // returned = whole tree
      CHECK(sp.root == nullptr);
      CHECK_EQ(other.inorder(), (std::vector<int>{3, 5, 7}));
    }
  }
}

void test_splay_split_independent() {
  Splay<int> sp;
  for (int v : {1, 2, 3, 4, 5, 6, 7}) sp.insert(v);

  Splay<int> right = sp.split_after(4);
  CHECK(right.root->parent == nullptr);
  CHECK(sp.root->parent    == nullptr);

  right.insert(8);
  CHECK_EQ(right.inorder(), (std::vector<int>{5, 6, 7, 8}));

  sp.insert(0);
  CHECK_EQ(sp.inorder(), (std::vector<int>{0, 1, 2, 3, 4}));
}

// ===================================================================
// Splay join
// ===================================================================

void test_splay_join_right() {
  Splay<int> a, b;
  for (int v : {1, 2, 3}) a.insert(v);
  for (int v : {4, 5, 6}) b.insert(v);

  IntNode *b_root = b.root;
  b.root = nullptr;                  // prevent double-free
  a.join_right(b_root);
  CHECK_EQ(a.inorder(), (std::vector<int>{1, 2, 3, 4, 5, 6}));
  CHECK(tree_parents_ok(a.root));
}

void test_splay_join_left() {
  Splay<int> a, b;
  for (int v : {4, 5, 6}) a.insert(v);
  for (int v : {1, 2, 3}) b.insert(v);

  IntNode *b_root = b.root;
  b.root = nullptr;
  a.join_left(b_root);
  CHECK_EQ(a.inorder(), (std::vector<int>{1, 2, 3, 4, 5, 6}));
  CHECK(tree_parents_ok(a.root));
}

void test_splay_join_empty() {
  // join_right(nullptr)
  {
    Splay<int> a;
    for (int v : {1, 2, 3}) a.insert(v);
    a.join_right(nullptr);
    CHECK_EQ(a.inorder(), (std::vector<int>{1, 2, 3}));
  }
  // join into empty
  {
    Splay<int> a, b;
    for (int v : {1, 2, 3}) b.insert(v);
    IntNode *b_root = b.root;
    b.root = nullptr;
    a.join_right(b_root);
    CHECK_EQ(a.inorder(), (std::vector<int>{1, 2, 3}));
  }
}

// ===================================================================
// root->parent != nullptr
// ===================================================================

void test_bst_root_with_parent() {
  // watcher is *not* part of the BST; it just holds the root as a child.
  auto *watcher = new IntNode(-1);
  auto *n5 = new IntNode(5);
  auto *n3 = new IntNode(3, n5);
  auto *n7 = new IntNode(7, n5);
  n5->left = n3; n5->right = n7;
  n5->parent = watcher; watcher->left = n5;

  BST<int> bst(n5, std::less<int>());
  CHECK(bst.root == n5);
  CHECK(n5->parent == watcher);            // non-null root parent

  // Basic ops
  CHECK_EQ(bst.inorder(), (std::vector<int>{3, 5, 7}));
  CHECK(bst.search(5).second);
  CHECK(!bst.search(6).second);

  bst.insert(6);
  CHECK_EQ(bst.inorder(), (std::vector<int>{3, 5, 6, 7}));

  // Rotation touches watcher
  IntNode *new_root = bst.rotate_right(n5);
  CHECK(new_root == n3);
  CHECK(watcher->left == n3);
  CHECK(n3->parent == watcher);
  CHECK(bst.root == n3);

  // Clear detaches from watcher
  bst.clear();
  CHECK(bst.root == nullptr);
  CHECK(watcher->left == nullptr);
  delete watcher;
}

void test_splay_root_with_parent() {
  auto *watcher = new IntNode(-1);
  auto *n5 = new IntNode(5);
  auto *n3 = new IntNode(3, n5);
  auto *n7 = new IntNode(7, n5);
  n5->left = n3; n5->right = n7;
  n5->parent = watcher; watcher->left = n5;

  Splay<int> sp(n5, std::less<int>());

  // Search splays 3 to root; watcher pointer preserved
  auto [found, ok] = sp.search(3);
  CHECK(ok);
  CHECK(sp.root == n3);
  CHECK(n3->parent == watcher);
  CHECK(watcher->left == n3);
  CHECK_EQ(sp.inorder(), (std::vector<int>{3, 5, 7}));

  sp.search(7);
  CHECK(sp.root->value == 7);
  CHECK(sp.root->parent == watcher);
  CHECK(watcher->left->value == 7);

  sp.clear();
  CHECK(sp.root == nullptr);
  CHECK(watcher->left == nullptr);
  delete watcher;
}

// ===================================================================
// Constructing a tree from a subtree of another tree
// ===================================================================

void test_subtree_construction() {
  BST<int> original;
  for (int v : {10, 5, 15, 3, 7, 12, 17}) original.insert(v);

  IntNode *sub_root = original.root->left;
  CHECK(sub_root->parent == original.root);   // invariant for a subtree

  BST<int> sub(sub_root, std::less<int>());
  CHECK(sub.root == sub_root);
  CHECK(sub_root->parent == original.root);   // non-null root parent

  CHECK_EQ(sub.inorder(), (std::vector<int>{3, 5, 7}));
  CHECK(sub.search(5).second);
  CHECK(!sub.search(10).second);

  sub.insert(4);
  CHECK_EQ(sub.inorder(), (std::vector<int>{3, 4, 5, 7}));

  sub.clear();
  CHECK(sub.root == nullptr);
  CHECK(original.root->left == nullptr);      // detached

  // Original tree otherwise intact (only the left subtree was cleared)
  CHECK_EQ(original.inorder(), (std::vector<int>{10, 12, 15, 17}));
}

// ===================================================================
// Exceptions
// ===================================================================

void test_exceptions() {
  BST<int> bst;

  auto expect_throw = [&](auto f) {
    bool threw = false;
    try { f(); } catch (const std::invalid_argument &) { threw = true; }
    CHECK(threw);
  };

  expect_throw([&] { bst.inorder_successor(nullptr); });
  expect_throw([&] { bst.inorder_predecessor(nullptr); });
  expect_throw([&] { bst.remove_node(nullptr); });
}

// ===================================================================
// main
// ===================================================================

int main() {
  test_binary_tree_traversals();
  test_binary_tree_move();
  test_binary_tree_clear();
  test_binary_tree_leftmost_rightmost();

  test_bst_insert_search();
  test_bst_traversals();
  test_bst_ordered();
  test_bst_successor_predecessor();
  test_bst_remove();
  test_bst_custom_comparator();
  test_bst_stress();

  test_splay_insert_and_search();
  test_splay_find_min_max();
  test_splay_ordered();
  test_splay_remove();
  test_splay_split_partition();
  test_splay_split_edge_cases();
  test_splay_split_independent();
  test_splay_join_right();
  test_splay_join_left();
  test_splay_join_empty();

  test_bst_root_with_parent();
  test_splay_root_with_parent();
  test_subtree_construction();
  test_exceptions();

  // AVL last:  the current source has UB in rotate_left/rotate_right
  // (missing `return child;`), so remove_node can crash on rotation.
  test_avl_insert_rotations();
  test_avl_insert_balance();
  test_avl_remove_balance();

  test::summary();
  return test::g_failed == 0 ? 0 : 1;
}
