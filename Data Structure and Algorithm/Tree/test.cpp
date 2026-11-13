// Tests for BinaryTree / BST / AVL / Splay (binary_tree.hpp).
// Build: g++ -std=c++20 -O1 -g -Wall -Wextra -fsanitize=address,undefined
//            test_binary_tree.cpp -o test_binary_tree && ./test_binary_tree
//
// "Subtree mode" = the tree's root has a non-null parent that lives in an
// outer tree (Host below). The outer tree must never be modified except that
// its child pointer to our root is kept up to date.
#include "binary_tree.hpp"

#include <cmath>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <type_traits>
#include <vector>

static int g_checks = 0, g_fail = 0;
#define CHECK(cond)                                                            \
  do {                                                                         \
    ++g_checks;                                                                \
    if (!(cond)) {                                                             \
      ++g_fail;                                                                \
      std::cerr << __FILE__ << ":" << __LINE__ << ": CHECK failed: " #cond     \
                << "\n";                                                       \
    }                                                                          \
  } while (0)
#define CHECK_THROWS(expr)                                                     \
  do {                                                                         \
    ++g_checks;                                                                \
    bool thrown_ = false;                                                      \
    try {                                                                      \
      (void)(expr);                                                            \
    } catch (const std::invalid_argument &) {                                  \
      thrown_ = true;                                                          \
    }                                                                          \
    if (!thrown_) {                                                            \
      ++g_fail;                                                                \
      std::cerr << __FILE__ << ":" << __LINE__                                 \
                << ": expected invalid_argument: " #expr "\n";                 \
    }                                                                          \
  } while (0)

// ---------------------------------------------------------------- node types
template <class T> struct CNode : BaseBinaryTreeNode<T, CNode<T>> { // leak counter
  static inline int live = 0;
  CNode(const T &v = T(), CNode *p = nullptr)
      : BaseBinaryTreeNode<T, CNode<T>>(v, p) {
    ++live;
  }
  CNode(const CNode &) = delete;
  ~CNode() override { --live; }
};

template <class T> struct SAVLNode : BaseBinaryTreeNode<T, SAVLNode<T>> { // augmented
  using Base = BaseBinaryTreeNode<T, SAVLNode<T>>;
  using Base::Base;
  mutable int height = 1;
  mutable int size = 1;
  static int get_height(SAVLNode *n) noexcept { return n ? n->height : 0; }
  static int get_size(SAVLNode *n) noexcept { return n ? n->size : 0; }
  static int bf(SAVLNode *n) { return get_height(n->left) - get_height(n->right); }
  void update() const override {
    height = 1 + std::max(get_height(this->left), get_height(this->right));
    size = 1 + get_size(this->left) + get_size(this->right);
  }
};

template <class Tree>
using NodeOf = std::remove_pointer_t<decltype(std::declval<Tree &>().root)>;
template <class Tree>
using CmpOf = std::remove_cvref_t<decltype(std::declval<Tree &>().comp)>;
template <class N> concept HasHeight = requires(N n) { n.height; };
template <class N> concept HasSize = requires(N n) { n.size; };
template <class Tree> concept IsAVL =
    requires(const Tree &t, NodeOf<Tree> *n) { t.rebalance(n); };
template <class Tree> concept IsSplay =
    requires(const Tree &t, NodeOf<Tree> *n) { t.splay(n); };

// --------------------------------------------------------------- outer tree
//        top (parent nullptr)
//       /    \.
//   [tree]   sib
template <class Node> struct Host {
  Node *top, *sib;
  Host() : top(new Node(100000)), sib(new Node(200000)) {
    top->right = sib;
    sib->parent = top;
    if constexpr (HasHeight<Node>) top->height = 777; // must never change
  }
  template <class Tree> void attach(Tree &t) {
    top->left = t.root;
    if (t.root) t.root->parent = top;
  }
  template <class Tree> bool intact(const Tree &t) const {
    bool ok = top->parent == nullptr && top->right == sib &&
              sib->parent == top && !sib->left && !sib->right &&
              top->left == t.root && top->value == 100000 &&
              sib->value == 200000;
    if (t.root) ok = ok && t.root->parent == top;
    if constexpr (HasHeight<Node>) ok = ok && top->height == 777 && sib->height == 1;
    return ok;
  }
  ~Host() { // tree must be gone / detached by now
    delete sib;
    delete top;
  }
};

// ------------------------------------------------------------------ helpers
// Random-shaped BST over keys 0..n-1 (in-order index is the key).
template <class Node>
Node *shape(int n, std::mt19937 &g, int &counter, Node *parent = nullptr) {
  if (n == 0) return nullptr;
  int l = g() % n;
  Node *x = new Node(0, parent);
  x->left = shape<Node>(l, g, counter, x);
  x->value = counter++;
  x->right = shape<Node>(n - 1 - l, g, counter, x);
  return x;
}
template <class Node> void ref_pre(Node *n, std::vector<int> &o) {
  if (!n) return;
  o.push_back(n->value); ref_pre(n->left, o); ref_pre(n->right, o);
}
template <class Node> void ref_in(Node *n, std::vector<int> &o) {
  if (!n) return;
  ref_in(n->left, o); o.push_back(n->value); ref_in(n->right, o);
}
template <class Node> void ref_post(Node *n, std::vector<int> &o) {
  if (!n) return;
  ref_post(n->left, o); ref_post(n->right, o); o.push_back(n->value);
}

template <class Tree>
void validate(const Tree &t, const std::vector<int> &expected) {
  using Node = NodeOf<Tree>;
  CHECK(t.inorder() == expected);
  auto nodes = t.preorder_node();
  CHECK(nodes.size() == expected.size());
  for (Node *n : nodes) {
    if (n->left) CHECK(n->left->parent == n);
    if (n->right) CHECK(n->right->parent == n);
  }
  for (Node *n : t.postorder_node()) {
    if constexpr (HasHeight<Node>) {
      CHECK(n->height == 1 + std::max(Node::get_height(n->left),
                                      Node::get_height(n->right)));
      if constexpr (IsAVL<Tree>) CHECK(std::abs(Node::bf(n)) <= 1);
    }
    if constexpr (HasSize<Node>)
      CHECK(n->size == 1 + Node::get_size(n->left) + Node::get_size(n->right));
  }
}

template <class Tree> bool contains(const Tree &t, NodeOf<Tree> *n) {
  for (auto *m : t.preorder_node()) if (m == n) return true;
  return false;
}

// ------------------------------------------------------- BinaryTree: basics
void test_traversals() {
  using Node = BinaryTreeNode<int>;
  using Static = BinaryTreeStatic<int>;
  CHECK(Static::preorder(nullptr).empty());
  CHECK(Static::inorder(nullptr).empty());
  CHECK(Static::postorder(nullptr).empty());
  CHECK(Static::preorder_node(nullptr).empty());
  CHECK(Static::inorder_node(nullptr).empty());
  CHECK(Static::postorder_node(nullptr).empty());
  std::mt19937 g(1);
  for (int n : {1, 2, 3, 7, 50, 200}) {
    for (int rep = 0; rep < 20; ++rep) {
      int c = 0;
      Node *r = shape<Node>(n, g, c);
      std::vector<int> a, b, d;
      ref_pre(r, a); ref_in(r, b); ref_post(r, d);
      CHECK(Static::preorder(r) == a);
      CHECK(Static::inorder(r) == b);
      CHECK(Static::postorder(r) == d);
      auto vals = [](const std::vector<Node *> &v) {
        std::vector<int> o;
        for (auto *x : v) o.push_back(x->value);
        return o;
      };
      CHECK(vals(Static::preorder_node(r)) == a);
      CHECK(vals(Static::inorder_node(r)) == b);
      CHECK(vals(Static::postorder_node(r)) == d);
      // traversing an inner subtree only visits that subtree
      Node *inner = r->left ? r->left : r;
      std::vector<int> e; ref_in(inner, e);
      CHECK(Static::inorder(inner) == e);
      // member versions + non-null parent root
      BinaryTree<int> t(r);
      CHECK(t.preorder() == a); CHECK(t.inorder() == b); CHECK(t.postorder() == d);
      CHECK(t.preorder_node().size() == (size_t)n);
      CHECK(t.leftmost_child()->value == 0);
      CHECK(t.rightmost_child()->value == n - 1);
      Host<Node> host;           // declared after t -> destroyed before t,
      host.attach(t);            // so detach before it dies:
      CHECK(t.inorder() == b); CHECK(t.postorder() == d);
      CHECK(t.leftmost_child()->value == 0);
      CHECK(t.rightmost_child()->value == n - 1);
      host.top->left = nullptr;  // give ownership back to t
      r->parent = nullptr;
    }
  }
}

void test_is_left_child_and_extremes() {
  using Node = BinaryTreeNode<int>;
  using S = BinaryTreeStatic<int>;
  CHECK(S::is_left_child(nullptr) == -2);
  Node *r = new Node(2);
  r->left = new Node(1, r);
  r->right = new Node(3, r);
  CHECK(S::is_left_child(r) == -1);
  CHECK(S::is_left_child(r->left) == 1);
  CHECK(S::is_left_child(r->right) == 0);
  CHECK(S::leftmost_child(nullptr) == nullptr);
  CHECK(S::rightmost_child(nullptr) == nullptr);
  CHECK(S::leftmost_child(r) == r->left);
  CHECK(S::rightmost_child(r) == r->right);
  CHECK(S::leftmost_child(r->right) == r->right);
  BinaryTree<int> t(r);
  Host<Node> h; h.attach(t);
  CHECK(S::is_left_child(r) == 1); // root with parent still reports its side
  h.top->left = nullptr; r->parent = nullptr;
  BinaryTree<int> empty;
  CHECK(empty.leftmost_child() == nullptr && empty.rightmost_child() == nullptr);
  CHECK(empty.inorder().empty() && empty.preorder().empty());
}

// ------------------------------------------------- rotations / replace
template <class Node> struct Exposed : BinaryTree<int, Node> {
  using BinaryTree<int, Node>::BinaryTree;
  using BinaryTree<int, Node>::replace;
};

void test_rotations(bool sub) {
  using Node = BinaryTreeNode<int>;
  std::mt19937 g(sub ? 7 : 8);
  Host<Node> host;
  int c = 0;
  const int n = 60;
  BinaryTree<int> t(shape<Node>(n, g, c));
  if (sub) host.attach(t);
  std::vector<int> expected(n);
  for (int i = 0; i < n; ++i) expected[i] = i;
  for (int step = 0; step < 1500; ++step) {
    auto nodes = t.preorder_node();
    Node *x = nodes[g() % nodes.size()];
    Node *old_parent = x->parent;
    switch (g() % 3) {
    case 0:
      if (x->right) {
        Node *c1 = x->right; bool was_root = t.root == x;
        CHECK(t.rotate_left(x) == c1);
        CHECK(x->parent == c1 && c1->left == x && c1->parent == old_parent);
        if (was_root) CHECK(t.root == c1);
      } else CHECK_THROWS(t.rotate_left(x));
      break;
    case 1:
      if (x->left) {
        Node *c1 = x->left; bool was_root = t.root == x;
        CHECK(t.rotate_right(x) == c1);
        CHECK(x->parent == c1 && c1->right == x && c1->parent == old_parent);
        if (was_root) CHECK(t.root == c1);
      } else CHECK_THROWS(t.rotate_right(x));
      break;
    default:
      if (x == t.root) CHECK_THROWS(t.rotate_parent(x));
      else {
        Node *p = x->parent;
        CHECK(t.rotate_parent(x) == x);
        CHECK(p->parent == x);
      }
    }
    validate(t, expected);
    if (sub) CHECK(host.intact(t));
  }
  CHECK_THROWS(t.rotate_left(nullptr));
  CHECK_THROWS(t.rotate_right(nullptr));
  CHECK_THROWS(t.rotate_parent(nullptr));
  if (!sub) CHECK_THROWS(t.rotate_parent(t.root));
}

void test_replace() {
  using Node = BinaryTreeNode<int>;
  Node *r = new Node(2);
  r->left = new Node(1, r);
  r->right = new Node(3, r);
  Exposed<Node> t(r);
  CHECK_THROWS(t.replace(nullptr, nullptr));
  Node *leaf = r->left;
  CHECK(t.replace(leaf, nullptr, true) == r);
  CHECK(r->left == nullptr && t.root == r);
  // replacing a root returns nullptr (old parent would be outside the tree)
  Node *nr = new Node(9);
  CHECK(t.replace(r, nr) == nullptr);
  CHECK(t.root == nr && nr->parent == nullptr);
  nr->right = r; r->parent = nr; r->left = nullptr; // keep ownership sane
}

// ------------------------------------------------------------------- clear
void test_clear() {
  using Node = CNode<int>;
  using Tree = BinaryTree<int, Node>;
  const int base = Node::live;
  std::mt19937 g(3);
  for (signed char mode : {(signed char)-1, (signed char)0, (signed char)1}) {
    for (int sub = 0; sub < 2; ++sub) {
      int c = 0;
      {
        Host<Node> host;
        Tree t(shape<Node>(40, g, c));
        if (sub) host.attach(t);
        auto nodes = t.preorder_node();
        Node *victim = nodes[1 + g() % (nodes.size() - 1)];
        size_t removed = Tree::preorder(victim).size();
        Node *p = victim->parent;
        bool was_left = p->left == victim;
        t.clear(victim, mode);
        CHECK((was_left ? p->left : p->right) == nullptr);
        CHECK(t.preorder().size() == 40 - removed);
        CHECK(t.root != nullptr);
        if (sub) CHECK(host.top->left == t.root);
        t.clear(nullptr, mode);              // no-op
        t.clear(t.root, mode);               // whole tree
        CHECK(t.root == nullptr);
        if (sub) CHECK(host.top->left == nullptr && host.top->right == host.sib);
        t.clear();                           // on empty: no-op
        CHECK(Node::live == base + (sub ? 2 : 2));
      }
      CHECK(Node::live == base);
    }
  }
  { // destructor frees everything, also a subtree held in an outer tree
    Host<Node> host;
    int c = 0;
    {
      Tree t(shape<Node>(30, g, c));
      host.attach(t);
    }
    CHECK(host.top->left == nullptr);
    CHECK(Node::live == base + 2);
  }
  CHECK(Node::live == base);
  { // very deep tree: clear must not overflow the stack (stack/auto modes)
    for (signed char mode : {(signed char)0, (signed char)1}) {
      Node *r = new Node(0), *cur = r;
      for (int i = 1; i < 100000; ++i) {
        cur->left = new Node(i, cur);
        cur = cur->left;
      }
      Tree t(r);
      t.clear(t.root, mode);
      CHECK(Node::live == base);
    }
  }
}

void test_move_semantics() {
  using Node = CNode<int>;
  using Tree = BST<int, Node>;
  const int base = Node::live;
  static_assert(!std::is_copy_constructible_v<BST<int>>);
  static_assert(!std::is_copy_assignable_v<AVL<int>>);
  static_assert(!std::is_copy_constructible_v<Splay<int>>);
  static_assert(std::is_nothrow_move_constructible_v<BinaryTree<int>>);
  static_assert(std::is_default_constructible_v<AVL<int>>);
  static_assert(std::is_default_constructible_v<Splay<int>>);
  static_assert(std::is_move_constructible_v<AVL<int>>);
  static_assert(std::is_move_assignable_v<Splay<int>>);
  Tree a;
  for (int x : {5, 2, 8, 1}) a.insert(x);
  Tree b(std::move(a));
  CHECK(a.root == nullptr);
  CHECK((b.inorder() == std::vector<int>{1, 2, 5, 8}));
  Tree c2;
  c2.insert(100);
  c2 = std::move(b);               // old content of c2 must be freed
  CHECK(b.root == nullptr);
  CHECK((c2.inorder() == std::vector<int>{1, 2, 5, 8}));
  Tree *self = &c2;
  c2 = std::move(*self);           // self-move keeps the data
  CHECK((c2.inorder() == std::vector<int>{1, 2, 5, 8}));
  c2.clear();
  CHECK(Node::live == base);
}

// ---------------------------------------------------- BST/AVL/Splay fuzzing
template <class Tree>
void fuzz(unsigned seed, int ops, int range, int pre, bool sub) {
  using Node = NodeOf<Tree>;
  using Cmp = CmpOf<Tree>;
  std::mt19937 g(seed);
  std::set<int, Cmp> ref;
  Host<Node> host; // before t: destroyed after t
  Tree t = [&] { // also exercises the (root, comp) constructor
    Tree src;
    for (int i = 0; i < pre; ++i) {
      int k = g() % range;
      src.insert(k); ref.insert(k);
    }
    Node *r = src.root;
    src.root = nullptr;
    return Tree(r, Cmp());
  }();
  if (sub && t.root) host.attach(t);
  auto vec = [&] { return std::vector<int>(ref.begin(), ref.end()); };
  auto check = [&] {
    validate(t, vec());
    if (sub) CHECK(host.intact(t));
  };
  auto splayed = [&](Node *n) {
    if constexpr (IsSplay<Tree>) if (n) CHECK(t.root == n);
  };
  auto expect = [&](Node *n, bool has, int v) {
    if (!has) CHECK(n == nullptr);
    else { CHECK(n != nullptr); if (n) CHECK(n->value == v); }
  };
  check();
  for (int step = 0; step < ops; ++step) {
    int x = g() % range;
    switch (g() % 9) {
    case 0: case 1: {
      bool was_empty = t.root == nullptr;
      auto [n, ins] = t.insert(x);
      CHECK(ins == ref.insert(x).second);
      CHECK(n && n->value == x);
      splayed(n);
      if (sub && was_empty) host.attach(t);
      break;
    }
    case 2: {
      bool existed = ref.count(x);
      auto [rs, ok] = t.remove(x);
      CHECK(ok == existed);
      ref.erase(x);
      if (ok && rs) CHECK(contains(t, rs)); // rebalance_start is inside tree
      break;
    }
    case 3: {
      auto [n, found] = t.search(x);
      CHECK(found == (ref.count(x) > 0));
      if (found) CHECK(n->value == x);
      else if (t.root) {
        CHECK(n != nullptr);
        if (n) {
          bool less = t.comp(x, n->value);
          CHECK(less || t.comp(n->value, x));
          if constexpr (!IsSplay<Tree>) // splay rotates n afterwards
            CHECK((less ? n->left : n->right) == nullptr);
        }
      } else CHECK(n == nullptr);
      splayed(n);
      break;
    }
    case 4: {
      auto ub = ref.upper_bound(x), lb = ref.lower_bound(x);
      Node *n;
      n = t.ordered_after(x);       expect(n, ub != ref.end(), ub != ref.end() ? *ub : 0); splayed(n);
      n = t.ordered_not_before(x);  expect(n, lb != ref.end(), lb != ref.end() ? *lb : 0); splayed(n);
      n = t.ordered_before(x);
      expect(n, lb != ref.begin(), lb != ref.begin() ? *std::prev(lb) : 0); splayed(n);
      n = t.ordered_not_after(x);
      expect(n, ub != ref.begin(), ub != ref.begin() ? *std::prev(ub) : 0); splayed(n);
      break;
    }
    case 5: {
      Node *n = t.find_min(); expect(n, !ref.empty(), ref.empty() ? 0 : *ref.begin()); splayed(n);
      n = t.find_max(); expect(n, !ref.empty(), ref.empty() ? 0 : *ref.rbegin()); splayed(n);
      break;
    }
    case 6: {
      if (ref.empty()) { CHECK_THROWS(t.inorder_successor(nullptr)); CHECK_THROWS(t.inorder_predecessor(nullptr)); break; }
      auto it = ref.begin();
      std::advance(it, g() % ref.size());
      int k = *it;
      Node *n = t.search(k).first;
      Node *s = t.inorder_successor(n);
      Node *p = t.inorder_predecessor(n);
      auto nx = std::next(it);
      expect(s, nx != ref.end(), nx != ref.end() ? *nx : 0);
      expect(p, it != ref.begin(), it != ref.begin() ? *std::prev(it) : 0);
      break;
    }
    case 7: {
      if (ref.empty()) { CHECK_THROWS(t.remove_node(nullptr)); break; }
      auto nodes = t.inorder_node();
      Node *n = nodes[g() % nodes.size()];
      int v = n->value;
      Node *rs = t.remove_node(n);
      ref.erase(v);
      if (rs) CHECK(contains(t, rs));
      break;
    }
    default: {
      CHECK(t.preorder().size() == ref.size());
      CHECK(t.postorder().size() == ref.size());
      CHECK((t.leftmost_child() == nullptr) == ref.empty());
    }
    }
    check();
  }
  // drain: remove everything in random order
  std::vector<int> keys = vec();
  std::shuffle(keys.begin(), keys.end(), g);
  for (int k : keys) {
    CHECK(t.remove(k).second);
    ref.erase(k);
    check();
  }
  CHECK(t.root == nullptr);
  if (sub) CHECK(host.top->left == nullptr);
}

template <class Tree> void fuzz_all(const char *name) {
  (void)name;
  for (unsigned seed = 1; seed <= 6; ++seed) {
    for (bool sub : {false, true}) {
      fuzz<Tree>(seed, 700, 40, 0, sub);
      fuzz<Tree>(seed + 100, 700, 120, 60, sub);
      fuzz<Tree>(seed + 200, 400, 6, 3, sub);  // tiny, many collisions
    }
  }
}

void test_custom_comparators() {
  fuzz<BST<int, BinaryTreeNode<int>, std::greater<int>>>(11, 600, 50, 20, false);
  fuzz<BST<int, BinaryTreeNode<int>, std::greater<int>>>(12, 600, 50, 20, true);
  fuzz<AVL<int, SAVLNode<int>, std::greater<int>>>(13, 600, 50, 20, true);
  fuzz<Splay<int, SAVLNode<int>, std::greater<int>>>(14, 600, 50, 20, true);
  // stateful comparator with equivalent keys: second insert is rejected
  struct ModCmp {
    int m;
    bool operator()(int a, int b) const { return a % m < b % m; }
  };
  BST<int, BinaryTreeNode<int>, ModCmp> a{ModCmp{10}};
  CHECK(a.insert(5).second);
  auto [n, ins] = a.insert(15);
  CHECK(!ins && n->value == 5);
  AVL<int, AVLNode<int>, ModCmp> v{ModCmp{10}};
  for (int i = 0; i < 100; ++i) v.insert(i);
  CHECK(v.inorder().size() == 10);
  Splay<int, BinaryTreeNode<int>, ModCmp> s{ModCmp{3}};
  for (int i = 0; i < 100; ++i) s.insert(i);
  CHECK((s.inorder() == std::vector<int>{0, 1, 2}));
  // pair keys compared on .first only
  using P = std::pair<int, int>;
  struct ByFirst { bool operator()(const P &a, const P &b) const { return a.first < b.first; } };
  BST<P, BinaryTreeNode<P>, ByFirst> pt;
  pt.insert({1, 10});
  auto [pn, pins] = pt.insert({1, 20});
  CHECK(!pins && pn->value.second == 10);
}

void test_bst_remove_return_values() {
  using T = BST<int>;
  T t;
  for (int x : {50, 30, 70, 20, 40, 60, 80, 35}) t.insert(x);
  // leaf: rebalance start is the parent
  auto [rs, ok] = t.remove(20);
  CHECK(ok && rs && rs->value == 30);
  // two children, successor is not the right child: start = successor's old parent
  auto [rs2, ok2] = t.remove(30); // 30 has only right child (40->35) now
  CHECK(ok2 && rs2 && rs2->value == 50);
  auto [rs3, ok3] = t.remove(50); // root, two children, succ=60 (leaf of right)
  CHECK(ok3 && rs3 && rs3->value == 70);
  CHECK(t.root->value == 60 && t.root->parent == nullptr);
  CHECK(!t.remove(12345).second);
  // removing the root with <=1 child returns nullptr (nothing above to fix)
  T single; single.insert(1);
  auto [r4, ok4] = single.remove(1);
  CHECK(ok4 && r4 == nullptr && single.root == nullptr);
}

// ------------------------------------------------------------------- AVL
void test_avl_rotation_cases() {
  using A = AVL<int>;
  struct Case { std::vector<int> ins; };
  for (auto &c : std::vector<Case>{{{3, 2, 1}}, {{1, 2, 3}}, {{3, 1, 2}}, {{1, 3, 2}}}) {
    A t;
    for (int x : c.ins) t.insert(x);
    CHECK((t.preorder() == std::vector<int>{2, 1, 3}));
    validate(t, {1, 2, 3});
    CHECK(t.root->height == 2);
  }
  // monotone inserts give a (near) perfect tree
  A t;
  for (int i = 0; i < 1000; ++i) t.insert(i);
  validate(t, [] { std::vector<int> v(1000); for (int i = 0; i < 1000; ++i) v[i] = i; return v; }());
  CHECK(t.root->height <= 10 + 1);
  A d;
  for (int i = 1000; i-- > 0;) d.insert(i);
  CHECK(d.root->height <= 11);
  // height bound for random inserts
  std::mt19937 g(5);
  A r;
  std::set<int> ref;
  for (int i = 0; i < 5000; ++i) { int k = g(); r.insert(k); ref.insert(k); }
  CHECK(r.root->height <= 1.4405 * std::log2(ref.size() + 2.0));
  // remove in ascending and descending order => cascading rotations
  for (int dir = 0; dir < 2; ++dir) {
    A q;
    std::set<int> s;
    for (int i = 0; i < 300; ++i) { q.insert(i); s.insert(i); }
    for (int i = 0; i < 300; ++i) {
      int k = dir ? 299 - i : i;
      CHECK(q.remove(k).second);
      s.erase(k);
      validate(q, std::vector<int>(s.begin(), s.end()));
    }
    CHECK(q.root == nullptr);
  }
}

void test_avl_subtree_untouched_by_rebalance() {
  // A subtree whose parent (the outer node) would itself be unbalanced if it
  // were rebalanced: the AVL must stop at its own root.
  using A = AVL<int, SAVLNode<int>>;
  using Node = SAVLNode<int>;
  for (int dir = 0; dir < 2; ++dir) {
    Host<Node> host;
    A t;
    for (int i = 0; i < 31; ++i) t.insert(dir ? 100 + i : 1000 - i);
    host.attach(t);
    for (int i = 0; i < 200; ++i) {
      t.insert(dir ? 200 + i : 500 - i); // grows height a lot
      CHECK(host.intact(t));
    }
    CHECK(host.top->height == 777); // outer node was never "updated"
    for (int i = 0; i < 200; ++i) {
      t.remove(dir ? 200 + i : 500 - i);
      CHECK(host.intact(t));
    }
    // sub-root replaced by a rotation: outer child pointer must follow
    CHECK(host.top->left == t.root);
    t.clear(); // detaches itself from host
    CHECK(host.top->left == nullptr);
  }
}

// ------------------------------------------------------------------- Splay
void test_splay_specifics() {
  using S = Splay<int, SAVLNode<int>>;
  S t;
  for (int i = 0; i < 20000; ++i) {
    auto [n, ins] = t.insert(i);
    CHECK(ins && t.root == n); // insert splays
  }
  CHECK(t.root->value == 19999);
  CHECK(t.find_min()->value == 0 && t.root->value == 0);
  CHECK(t.find_max()->value == 19999 && t.root->value == 19999);
  CHECK(t.search(777).first == t.root && t.root->value == 777);
  // not found: last visited node ends at the root
  S u;
  for (int x : {10, 20, 30}) u.insert(x);
  auto [n, found] = u.search(25);
  CHECK(!found && u.root == n && (n->value == 20 || n->value == 30));
  // splay on a node of a tree held below an outer node (zig/zig-zig/zig-zag)
  using Node = SAVLNode<int>;
  std::mt19937 g(9);
  Host<Node> host;
  int c = 0;
  S sh(shape<Node>(200, g, c), std::less<int>());
  for (auto *x : sh.preorder_node()) x->update(); // heights/sizes were not set
  // fix augmented data bottom-up
  for (auto *x : sh.postorder_node()) x->update();
  host.attach(sh);
  for (int i = 0; i < 500; ++i) {
    auto nodes = sh.preorder_node();
    Node *x = nodes[g() % nodes.size()];
    CHECK(sh.splay(x) == x && sh.root == x);
    std::vector<int> e(200);
    for (int k = 0; k < 200; ++k) e[k] = k;
    validate(sh, e);
    CHECK(host.intact(sh));
  }
  CHECK(sh.splay(nullptr) == nullptr);
}

template <class S>
void splay_split_one(unsigned seed, int variant, bool sub) {
  using Node = NodeOf<S>;
  std::mt19937 g(seed);
  std::set<int> ref;
  Host<Node> host;
  S t;
  for (int i = 0; i < 80; ++i) { int k = g() % 100; t.insert(k); ref.insert(k); }
  if (sub) host.attach(t);
  int x = g() % 110 - 5;
  auto goes = [&](int k) {
    switch (variant) {
    case 0: return k > x;   // split_after
    case 1: return k >= x;  // split_not_before
    case 2: return k < x;   // split_before
    default: return k <= x; // split_not_after
    }
  };
  std::vector<int> a, b; // this / other
  for (int k : ref) (goes(k) ? b : a).push_back(k);
  S other = variant == 0 ? t.split_after(x)
          : variant == 1 ? t.split_not_before(x)
          : variant == 2 ? t.split_before(x) : t.split_not_after(x);
  validate(t, a);
  validate(other, b);
  if (other.root) CHECK(other.root->parent == nullptr);
  if (sub) CHECK(host.intact(t)); // outer pointer follows the new root
  // and we can still join them back
  bool was_empty = t.root == nullptr; // an empty tree has forgotten its parent
  if (variant < 2) { t.join_right(other.root); } else { t.join_left(other.root); }
  other.root = nullptr;
  if (sub && was_empty) host.attach(t);
  std::vector<int> all(ref.begin(), ref.end());
  validate(t, all);
  if (sub) CHECK(host.intact(t));
}

void test_splay_split_join() {
  using S = Splay<int, SAVLNode<int>>;
  for (unsigned seed = 1; seed <= 25; ++seed)
    for (int v = 0; v < 4; ++v)
      for (bool sub : {false, true}) splay_split_one<S>(seed, v, sub);
  // split on an empty tree / when nothing qualifies
  S e;
  S o = e.split_after(3);
  CHECK(e.root == nullptr && o.root == nullptr);
  S t;
  for (int i = 0; i < 10; ++i) t.insert(i);
  S none = t.split_after(100);  CHECK(none.root == nullptr && t.inorder().size() == 10);
  S none2 = t.split_before(-1); CHECK(none2.root == nullptr && t.inorder().size() == 10);
  S all = t.split_not_before(-5); // everything moves out
  CHECK(t.root == nullptr && all.inorder().size() == 10);
  // joins
  S a, b, c;
  for (int i = 0; i < 50; ++i) a.insert(i);
  for (int i = 100; i < 150; ++i) b.insert(i);
  for (int i = -50; i < 0; ++i) c.insert(i);
  a.join_right(b.root); b.root = nullptr;
  a.join_left(c.root);  c.root = nullptr;
  a.join_right(nullptr); a.join_left(nullptr);
  validate(a, [] { std::vector<int> v; for (int i = -50; i < 50; ++i) v.push_back(i); for (int i = 100; i < 150; ++i) v.push_back(i); return v; }());
  S empty1, empty2, src;
  src.insert(1);
  empty1.join_right(src.root); src.root = nullptr;
  CHECK(empty1.root && empty1.root->parent == nullptr && empty1.root->value == 1);
  S src2; src2.insert(0);
  empty2.join_left(src2.root); src2.root = nullptr;
  CHECK(empty2.root && empty2.root->value == 0);
}

void test_leaks_bst_cnode() {
  using Tree = BST<int, CNode<int>>;
  const int base = CNode<int>::live;
  fuzz<Tree>(31, 500, 60, 20, false);
  CHECK(CNode<int>::live == base);
  fuzz<Tree>(32, 500, 60, 20, true);
  CHECK(CNode<int>::live == base);
}

int main() {
  test_traversals();
  test_is_left_child_and_extremes();
  test_rotations(false);
  test_rotations(true);
  test_replace();
  test_clear();
  test_move_semantics();
  test_bst_remove_return_values();
  test_leaks_bst_cnode();
  fuzz_all<BST<int, SAVLNode<int>>>("BST");
  fuzz_all<AVL<int, SAVLNode<int>>>("AVL");
  fuzz_all<Splay<int, SAVLNode<int>>>("Splay");
  fuzz_all<AVL<int>>("AVL<AVLNode>");
  test_custom_comparators();
  test_avl_rotation_cases();
  test_avl_subtree_untouched_by_rebalance();
  test_splay_specifics();
  test_splay_split_join();
  std::cout << g_checks << " checks, " << g_fail << " failed\n";
  return g_fail != 0;
}
