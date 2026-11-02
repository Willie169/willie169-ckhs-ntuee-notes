class Node:
    def __init__(self, value=None, parent=None):
        self.value = value
        self.parent = parent
        self.left_child = None
        self.right_child = None
        self.height = 1  # only used in AVL, leaf 1


class BST:
    def __init__(self):
        self.root = None

    # Time complexity: O(height)
    def insert(self, x):
        if self.root == None:
            self.root = Node(x)
            return self.root
        node = self.root
        while True:
            if node.value > x:
                if node.left_child == None:
                    node.left_child = Node(x, node)
                    return node.left_child
                node = node.left_child
            elif node.value < x:
                if node.right_child == None:
                    node.right_child = Node(x, node)
                    return node.right_child
                node = node.right_child
            else:
                return None

    # Time complexity: O(height)
    def find(self, x):
        if self.root == None:
            return None
        node = self.root
        while node != None:
            if node.value > x:
                node = node.left_child
            elif node.value < x:
                node = node.right_child
            else:
                return node
        return None

    # Time complexity: O(height)
    def lower_bound(self, x):
        r = None
        node = self.root
        while node != None:
            if node.value <= x:
                r = node
                node = node.right_child
            else:
                node = node.left_child
        return r

    # Time complexity: O(height)
    def upper_bound(self, x):
        r = None
        node = self.root
        while node != None:
            if node.value >= x:
                r = node
                node = node.left_child
            else:
                node = node.right_child
        return r

    def _transplant(self, node, new):
        if node.parent == None:
            self.root = new
        elif node.parent.left_child == node:
            node.parent.left_child = new
        elif node.parent.right_child == node:
            node.parent.right_child = new
        if new != None:
            new.parent = node.parent
        return node.parent

    # Time complexity: O(height)
    def remove(self, x):
        node = self.find(x)
        if node == None:
            return None
        if node.right_child == None and node.left_child == None:
            return self._transplant(node, None)
        if node.right_child == None:
            return self._transplant(node, node.left_child)
        if node.left_child == None:
            return self._transplant(node, node.right_child)
        succ = node.right_child
        while succ.left_child != None:
            succ = succ.left_child
        node.value = succ.value
        return self._transplant(succ, succ.right_child)

    # Time complexity: O(n)
    def print_tree(self):
        result, stack, node = [], [], self.root
        while stack != [] or node != None:
            while node != None:
                stack.append(node)
                node = node.left_child
            node = stack.pop()
            result.append(node.value)
            node = node.right_child
        return result


# height = Theta(log n)
class AVL(BST):
    @staticmethod
    def _height(node):
        return node.height if node != None else 0

    @staticmethod
    def _update(node):
        node.height = 1 + max(
            AVL._height(node.left_child), AVL._height(node.right_child)
        )

    @staticmethod
    def _bf(node):
        return AVL._height(node.left_child) - AVL._height(node.right_child)

    def _rotate_right(self, node):
        child = node.left_child
        self._transplant(node, child)
        if child.right_child != None:
            child.right_child.parent = node
        node.left_child = child.right_child
        child.right_child = node
        node.parent = child
        self._update(node)
        self._update(child)
        return child

    def _rotate_left(self, node):
        child = node.right_child
        self._transplant(node, child)
        if child.left_child != None:
            child.left_child.parent = node
        node.right_child = child.left_child
        child.left_child = node
        node.parent = child
        self._update(node)
        self._update(child)
        return child

    def _rebalance(self, node):
        self._update(node)
        bf = self._bf(node)
        if bf > 1:
            child = node.left_child
            if self._bf(child) < 0:
                self._rotate_left(child)
            return self._rotate_right(node)
        if bf < -1:
            child = node.right_child
            if self._bf(child) > 0:
                self._rotate_right(child)
            return self._rotate_left(node)
        if node.parent == None:
            return node
        return self._rebalance(node.parent)

    def insert(self, x):
        node = super().insert(x)
        if node != None and node.parent != None:
            self._rebalance(node.parent)

    def remove(self, x):
        node = super().remove(x)
        while node != None:
            node = self._rebalance(node).parent
