#include "binary_tree.h"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <vector>

// Verify parent pointers, BST ordering, and count.
template <typename Node, typename Compare>
int verify_tree(
    Node *node, Node *parent, const Compare &comp,
    const typename std::remove_reference_t<decltype(node->value)> *lower =
        nullptr,
    const typename std::remove_reference_t<decltype(node->value)> *upper =
        nullptr) {
  if (!node)
    return 0;

  assert(node->parent == parent);

  if (lower)
    assert(comp(*lower, node->value));

  if (upper)
    assert(comp(node->value, *upper));

  return 1 + verify_tree(node->left, node, comp, lower, &node->value) +
         verify_tree(node->right, node, comp, &node->value, upper);
}

template <typename Tree>
void test_search(Tree &tree, const std::set<int> &reference, int x) {
  // Original, non-splaying search.
  auto [expected, expected_found] = tree.search(x);

  // Splaying search.
  auto [actual, actual_found] = tree.top_down_splay_search(x);

  // Compare search results.
  assert(expected_found == actual_found);
  assert(actual_found == (reference.count(x) != 0));

  assert(actual != nullptr);

  // If found, both searches must identify the same value.
  if (actual_found) {
    assert(expected != nullptr);
    assert(actual->value == expected->value);
    assert(actual->value == x);
  }

  // The last accessed node must become the root.
  assert(tree.root == actual);
  assert(actual->parent == nullptr);

  // Verify structural integrity.
  int count = verify_tree(tree.root, static_cast<decltype(tree.root)>(nullptr),
                          tree.comp);

  assert(count == static_cast<int>(reference.size()));
}
template <typename Tree> void test_basic_cases() {
  Tree tree;
  std::set<int> reference;

  for (int x : {50, 30, 70, 20, 40, 60, 80, 35, 45, 55, 65, 75, 85}) {
    tree.insert(x);
    reference.insert(x);
  }

  const std::vector<int> queries = {50, 20, 60, 40, 37, 10,  90, 42,
                                    85, 15, 35, 75, 55, 100, 30};

  for (int x : queries) {
    test_search(tree, reference, x);
  }
}
template <typename Tree, typename Node>
void test_external_parent(Tree &tree, Node *sentinel, bool attach_left,
                          const std::vector<int> &queries,
                          const std::set<int> &reference) {
  assert(tree.root != nullptr);

  // Attach the tree below an external node.
  Node *original_root = tree.root;

  original_root->parent = sentinel;

  if (attach_left)
    sentinel->left = original_root;
  else
    sentinel->right = original_root;

  for (int x : queries) {
    auto [expected, expected_found] = tree.search(x);

    auto [actual, actual_found] = tree.top_down_splay_search(x);

    assert(expected_found == actual_found);
    assert(actual_found == (reference.count(x) != 0));

    // The external parent must remain connected.
    assert(tree.root == actual);
    assert(actual->parent == sentinel);

    if (attach_left)
      assert(sentinel->left == actual);
    else
      assert(sentinel->right == actual);

    if (actual_found) {
      assert(expected != nullptr);
      assert(actual->value == expected->value);
    }

    // Verify the subtree, allowing its external parent.
    int count = verify_tree(tree.root, sentinel, tree.comp);

    assert(count == static_cast<int>(reference.size()));
  }
}
template <typename Tree> void test_randomized() {
  Tree tree;
  std::set<int> reference;

  std::mt19937 rng(12345);
  std::uniform_int_distribution<int> dist(-100, 100);

  // Random insertion order.
  std::vector<int> values;
  for (int i = 0; i < 100; ++i)
    values.push_back(dist(rng));

  for (int x : values) {
    if (reference.insert(x).second)
      tree.insert(x);
  }

  // Random searches, including unsuccessful ones.
  for (int i = 0; i < 10000; ++i) {
    int x = dist(rng);
    test_search(tree, reference, x);
  }
}
int main() {
  std::cout << "Running basic cases...\n";
  test_basic_cases<SplayTree<int>>();

  std::cout << "Running randomized tests...\n";
  test_randomized<SplayTree<int>>();

  std::cout << "All tests passed!\n";
  return 0;
}
