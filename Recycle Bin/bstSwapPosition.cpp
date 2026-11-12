  // Detach the other if one == nullptr
  // O(1)
  void swap_position(Node *lhs, Node *rhs) {
    if (lhs == rhs)
      return;
    if (lhs == nullptr) {
      return swap_position(rhs, lhs, root);
    }
    lhs->_swap_position_helper(rhs);
    if (rhs == nullptr) {
      if (root == lhs)
        root = rhs;
      switch (is_left_child(lhs)) {
      case 1:
        lhs->parent->left = nullptr;
        break;
      case 0:
        lhs->parent->right = nullptr;
        break;
      }
      return;
    }
    if (rhs->parent == lhs)
      return swap_position(rhs, lhs, root);
    if (root == lhs)
      root = rhs;
    else if (root == rhs)
      root = lhs;
    if (lhs->parent == rhs) {
      switch (is_left_child(lhs)) {
      case 1:
        swap(lhs->right, rhs->right);
        rhs->left = lhs->left;
        if (lhs->left != nullptr)
          lhs->left->parent = rhs;
        lhs->left = rhs;
        break;
      case 0:
        swap(lhs->left, rhs->left);
        rhs->right = lhs->right;
        if (lhs->right != nullptr)
          lhs->right->parent = rhs;
        lhs->right = rhs;
        break;
      }
      switch (is_left_child(rhs)) {
      case 1:
        rhs->parent->left = lhs;
        break;
      case 0:
        rhs->parent->right = lhs;
        break;
      }
      lhs->parent = rhs->parent;
      rhs->parent = lhs;
      return;
    }
    if (lhs->left != nullptr)
      lhs->left->parent = rhs;
    if (lhs->right != nullptr)
      lhs->right->parent = rhs;
    if (rhs->left != nullptr)
      rhs->left->parent = lhs;
    if (rhs->right != nullptr)
      rhs->right->parent = lhs;
    if (lhs->parent != rhs->parent) {
      switch (is_left_child(lhs)) {
      case 1:
        lhs->parent->left = rhs;
        break;
      case 0:
        lhs->parent->right = rhs;
        break;
      }
      switch (is_left_child(rhs)) {
      case 1:
        rhs->parent->left = lhs;
        break;
      case 0:
        rhs->parent->right = lhs;
        break;
      }
      swap(lhs->parent, rhs->parent);
    } else
      swap(lhs->parent->left, lhs->parent->right);
    swap(lhs->left, rhs->left);
    swap(lhs->right, rhs->right);
  }

