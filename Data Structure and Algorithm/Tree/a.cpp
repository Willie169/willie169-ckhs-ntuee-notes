#include "binary_tree.h"
#include <iostream>

int main() {
  const auto sp = SplayTree<int>(
      from_latex_forest<int>("[40[20[10][30[25][35]]][70[55[50][62[58][66[,"
                             "phantom][68]]]][85[,phantom][90]]]]"),
      &std::cout);
  sp.log_forest();
  SplayTree<int> tp;
  // tp = sp;
  // tp.log("(a) Insert 67 using bottom-up splaying.\n");
  // tp.insert(67);
  tp = sp;
  tp.log("(b) Delete 62 using top-down splaying.\n");
  tp.top_down_remove(62);
  // tp = sp;
  // tp.log("(c) Insert 60 using top-down splaying.\n");
  // tp.top_down_insert(60);
  // tp.set_logger(&std::cout);
  // tp = sp;
  // tp.log("(d) Delete 30 using bottom-up splaying.\n");
  // tp.remove(30);
}
