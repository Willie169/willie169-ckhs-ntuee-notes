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
    def search(self, x):
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
    def find_min(self):
        if self.root == None:
            return None
        node = self.root
        while node.left_child != None:
            node = node.left_child
        return node.value

    # Time complexity: O(height)
    def find_max(self):
        if self.root == None:
            return None
        node = self.root
        while node.right_child != None:
            node = node.right_child
        return node.value

    # Time complexity: O(height)
    def upper_bound(self, x):
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
    def lower_bound(self, x):
        r = None
        node = self.root
        while node != None:
            if node.value >= x:
                r = node
                node = node.left_child
            else:
                node = node.right_child
        return r

    # Time complexity: O(height)
    def insert(self, x):
        if self.root == None:
            self.root = Node(x)
            return self.root, True
        node = self.root
        while True:
            if node.value > x:
                if node.left_child == None:
                    node.left_child = Node(x, node)
                    return node.left_child, True
                node = node.left_child
            elif node.value < x:
                if node.right_child == None:
                    node.right_child = Node(x, node)
                    return node.right_child, True
                node = node.right_child
            else:
                return node, False

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
        node = self.search(x)
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
    def preorder(self):
        result, stack = [], []
        if self.root != None:
            stack.append(self.root)
        while stack != []:
            node = stack.pop()
            result.append(node.value)
            if node.right_child != None:
                stack.append(node.right_child)
            if node.left_child != None:
                stack.append(node.left_child)
        return result

    # Time complexity: O(n)
    def postorder(self):
        result, stack, node = [], [], self.root
        last_visited = None
        while stack != [] or node != None:
            if node != None:
                stack.append(node)
                node = node.left_child
            else:
                right = stack[-1].right_child
                if right != None and last_visited is not right:
                    node = right
                else:
                    result.append(stack[-1].value)
                    last_visited = stack.pop()
        return result

    # Time complexity: O(n)
    def inorder(self):
        result, stack, node = [], [], self.root
        while stack != [] or node != None:
            while node != None:
                stack.append(node)
                node = node.left_child
            node = stack.pop()
            result.append(node.value)
            node = node.right_child
        return result

    @staticmethod
    def height(node):
        return node.height if node != None else -1

    @staticmethod
    def _update(node):
        node.height = 1 + max(BST.height(node.left_child), BST.height(node.right_child))

    @staticmethod
    def bf(node):
        return BST.height(node.left_child) - BST.height(node.right_child)

    def rotate_right(self, node):
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

    def rotate_left(self, node):
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


# height = Theta(log n)
class AVL(BST):
    def _rebalance(self, node):
        self._update(node)
        bf = self.bf(node)
        if bf > 1:
            child = node.left_child
            if self.bf(child) < 0:
                self.rotate_left(child)
            return self.rotate_right(node)
        if bf < -1:
            child = node.right_child
            if self.bf(child) > 0:
                self.rotate_right(child)
            return self.rotate_left(node)
        if node.parent == None:
            return node
        return self._rebalance(node.parent)

    def insert(self, x):
        node, inserted = super().insert(x)
        if inserted == True and node.parent != None:
            self._rebalance(node.parent)

    def remove(self, x):
        node = super().remove(x)
        while node != None:
            node = self._rebalance(node).parent
