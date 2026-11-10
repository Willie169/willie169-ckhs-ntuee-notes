class BST:
    class Node:
        def __init__(self, value=None, parent=None):
            self.value = value
            self.parent: BST.Node | None = parent
            self.left: BST.Node | None = None
            self.right: BST.Node | None = None
            self.height: int = 1  # only used in AVL, leaf 1

    def __init__(self):
        self.root = None

    # Time complexity: O(height)
    def search(self, x) -> tuple[BST.Node | None, bool]:
        if self.root == None:
            return None, False
        node = self.root
        while True:
            if node.value > x:
                if node.left == None:
                    return node, False
                node = node.left
            elif node.value < x:
                if node.right == None:
                    return node, False
                node = node.right
            else:
                return node, True

    # Time complexity: O(height)
    def find_min(self) -> BST.Node | None:
        if self.root == None:
            return None
        node = self.root
        while node.left != None:
            node = node.left
        return node.value

    # Time complexity: O(height)
    def find_max(self) -> BST.Node | None:
        if self.root == None:
            return None
        node = self.root
        while node.right != None:
            node = node.right
        return node.value

    # Time complexity: O(height)
    def upper_bound(self, x) -> BST.Node | None:
        r = None
        node = self.root
        while node != None:
            if node.value <= x:
                r = node
                node = node.right
            else:
                node = node.left
        return r

    # Time complexity: O(height)
    def lower_bound(self, x) -> BST.Node | None:
        r = None
        node = self.root
        while node != None:
            if node.value >= x:
                r = node
                node = node.left
            else:
                node = node.right
        return r

    # Time complexity: O(height)
    def insert(self, x) -> tuple[BST.Node, bool]:
        if self.root == None:
            self.root = self.Node(x)
            return self.root, True
        node = self.root
        while True:
            if node.value > x:
                if node.left == None:
                    node.left = self.Node(x, node)
                    return node.left, True
                node = node.left
            elif node.value < x:
                if node.right == None:
                    node.right = self.Node(x, node)
                    return node.right, True
                node = node.right
            else:
                return node, False

    def _transplant(self, node, new) -> BST.Node | None:
        if node.parent == None:
            self.root = new
        elif node.parent.left == node:
            node.parent.left = new
        elif node.parent.right == node:
            node.parent.right = new
        if new != None:
            new.parent = node.parent
        return node.parent

    # Time complexity: O(height)
    def remove(self, x) -> BST.Node | None:
        node, found = self.search(x)
        if found == False or node == None:
            return None
        if node.right == None and node.left == None:
            return self._transplant(node, None)
        if node.right == None:
            return self._transplant(node, node.left)
        if node.left == None:
            return self._transplant(node, node.right)
        succ = node.right
        while succ.left != None:
            succ = succ.left
        node.value = succ.value
        return self._transplant(succ, succ.right)

    # Time complexity: O(n)
    def preorder(self) -> list:
        result, stack = [], []
        if self.root != None:
            stack.append(self.root)
        while stack != []:
            node = stack.pop()
            result.append(node.value)
            if node.right != None:
                stack.append(node.right)
            if node.left != None:
                stack.append(node.left)
        return result

    # Time complexity: O(n)
    def postorder(self) -> list:
        result, stack, node = [], [], self.root
        last_visited = None
        while stack != [] or node != None:
            if node != None:
                stack.append(node)
                node = node.left
            else:
                right = stack[-1].right
                if right != None and last_visited is not right:
                    node = right
                else:
                    result.append(stack[-1].value)
                    last_visited = stack.pop()
        return result

    # Time complexity: O(n)
    def inorder(self) -> list:
        result, stack, node = [], [], self.root
        while stack != [] or node != None:
            while node != None:
                stack.append(node)
                node = node.left
            node = stack.pop()
            result.append(node.value)
            node = node.right
        return result

    def __repr__(self) -> str:
        return repr(self.inorder())

    def is_left_child(self, node) -> bool | None:
        if node.parent == None:
            return None
        if node.parent.left is node:
            return True
        return False

    def _rotate_right(self, child) -> BST.Node:
        parent = child.parent
        self._transplant(parent, child)
        if child.right != None:
            child.right.parent = parent
        parent.left = child.right
        child.right = parent
        parent.parent = child
        return child

    def _rotate_left(self, child) -> BST.Node:
        parent = child.parent
        self._transplant(parent, child)
        if child.left != None:
            child.left.parent = parent
        parent.right = child.left
        child.left = parent
        parent.parent = child
        return child

    def rotate(self, child) -> BST.Node:
        if self.is_left_child(child):
            return self._rotate_right(child.parent)
        return self._rotate_left(child.parent)


# height = Theta(log n)
class AVL(BST):
    @staticmethod
    def height(node) -> int:
        return node.height if node != None else 0

    @staticmethod
    def _update(node):
        node.height = 1 + max(AVL.height(node.left), AVL.height(node.right))

    @staticmethod
    def bf(node) -> int:
        return AVL.height(node.left) - AVL.height(node.right)

    def _rotate_right(self, child) -> BST.Node:
        super()._rotate_right(child)
        self._update(child.parent)
        self._update(child)
        return child

    def _rotate_left(self, child) -> BST.Node:
        super()._rotate_left(child)
        self._update(child.parent)
        self._update(child)
        return child

    def _rebalance(self, node) -> BST.Node:
        self._update(node)
        bf = self.bf(node)
        if bf > 1:
            child = node.left
            if self.bf(child) < 0:
                self._rotate_left(child.right)
            return self._rotate_right(child)
        if bf < -1:
            child = node.right
            if self.bf(child) > 0:
                self._rotate_right(child.left)
            return self._rotate_left(child)
        if node.parent == None:
            return node
        return self._rebalance(node.parent)

    def insert(self, x) -> tuple[BST.Node, bool]:
        node, inserted = super().insert(x)
        if inserted == True and node.parent != None:
            self._rebalance(node.parent)
        return node, inserted

    def remove(self, x) -> BST.Node | None:
        node = super().remove(x)
        while node != None:
            node = self._rebalance(node).parent


class Splay(BST):
    def splay(self, node) -> BST.Node | None:
        if node == None:
            return None
        if node.parent == None:
            return node
        if node.parent == self.root:  # zig
            return self.rotate(node)
        if (self.is_left_child(node) and self.is_left_child(node.parent)) or not (
            self.is_left_child(node) or self.is_left_child(node.parent)
        ):  # zig-zig
            self.rotate(node.parent)
            return self.rotate(node)
        # zig-zag
        return self.rotate(self.rotate(node))

    def search(self, x) -> tuple[BST.Node | None, bool]:
        node, found = super().search(x)
        return self.splay(node), found

    def find_min(self) -> BST.Node | None:
        node = super().find_min()
        return self.splay(node)

    def find_max(self) -> BST.Node | None:
        node = super().find_max()
        return self.splay(node)
