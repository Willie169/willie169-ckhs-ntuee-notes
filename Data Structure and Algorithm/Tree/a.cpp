#include "binary_tree.h"
#include <iostream>

int main() {
  auto sp = SplayTree<int>(
      from_latex_forest<int>("[40[20[10][30[25][35]]][70[55[50][62[58][66[,"
                             "phantom][68]]]][85[,phantom][90]]]]"),
      &std::cout);
  sp.insert(67);
  sp.log("% ----------");
  sp.top_down_remove(62);
  sp.log("% ----------");
  sp.top_down_insert(60);  
}
