#include "binary_tree.h"
#include <iostream>

int main() {
  auto sp = SplayTree<int>(
      from_latex_forest<int>("[40[20[10][30[25][35]]][70[55[50][62[58][66[,"
                             "phantom][68]]]][85[,phantom][90]]]]"),
      &std::cout);
  sp.log_forest();
  sp.log("(a) Insert 67 using bottom-up splaying.\n");
  sp.insert(67);
  sp.log("(b) Delete 62 using top-down splaying.\n");
  sp.top_down_remove(62);
  // sp.log("(c) Insert 60 using top-down splaying.\n");
  // sp.top_down_insert(60);
  // sp.log("(d) Delete 30 using bottom-up splaying.\n");
  // sp.remove(30);
}
