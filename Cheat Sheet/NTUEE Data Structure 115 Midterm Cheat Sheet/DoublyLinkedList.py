class _DNode:
    def __init__(self, data, prev=None, next=None):
        self.data = data
        self.prev = prev
        self.next = next


class DoublyLinkedList:
    def __init__(self):
        self.head = None
        self.tail = None
        self.size = 0

    def __len__(self):  # O(1)
        return self.size

    def is_empty(self):  # O(1)
        return self.size == 0

    def __getitem__(self, num):  # O(min(n, size - n)), n = num if num > 0 else num + size
        if not -self.size <= num < self.size:
            raise IndexError("list index out of range")
        if num * 2 >= self.size:
            num -= self.size
        elif num * 2 < -self.size:
            num += self.size
        if num >= 0:
            cur = self.head
            for _ in range(0, num):
                cur = cur.next
        else:
            cur = self.tail
            for _ in range(-1, num, -1):
                cur = cur.prev
        return cur.data

    def __setitem__(self, num, x):  # O(min(n, size - n)), n = num if num > 0 else num + size
        if not -self.size <= num < self.size:
            raise IndexError("list index out of range")
        if num * 2 >= self.size:
            num -= self.size
        elif num * 2 < -self.size:
            num += self.size
        if num >= 0:
            cur = self.head
            for _ in range(0, num):
                cur = cur.next
        else:
            cur = self.tail
            for _ in range(-1, num, -1):
                cur = cur.prev
        cur.data = x

    def push_front(self, x):  # O(1)
        node = _DNode(x, None, self.head)
        if self.head is None:
            self.head = self.tail = node
        else:
            self.head.prev = node
            self.head = node
        self.size += 1

    def push_back(self, x):  # O(1)
        node = _DNode(x, self.tail, None)
        if self.tail is None:
            self.head = self.tail = node
        else:
            self.tail.next = node
            self.tail = node
        self.size += 1

    def pop_front(self):  # O(1)
        if self.head is None:
            raise IndexError("pop from empty list")
        x = self.head.data
        self.head = self.head.next
        self.size -= 1
        if self.head is None:
            self.tail = None
        else:
            self.head.prev = None
        return x

    def pop_back(self):  # O(1)
        if self.tail is None:
            raise IndexError("pop from empty list")
        x = self.tail.data
        self.tail = self.tail.prev
        self.size -= 1
        if self.tail is None:
            self.head = None
        else:
            self.tail.next = None
        return x

    def peek_front(self):  # O(1)
        if self.is_empty():
            raise IndexError("peek at empty list")
        return self.head.data

    def peek_back(self):  # O(1)
        if self.is_empty():
            raise IndexError("peek at empty list")
        return self.tail.data

    def insert_after_node(self, pos, x):  # O(1)
        if self.is_empty():
            raise IndexError("insert at empty list")
        if pos is self.tail:
            return self.push_back(x)
        node = _DNode(x, pos, pos.next)
        pos.next.prev = node
        pos.next = node
        self.size += 1

    def insert_before_node(self, pos, x):  # O(1)
        if self.is_empty():
            raise IndexError("insert at empty list")
        if pos is self.head:
            return self.push_front(x)
        node = _DNode(x, pos.prev, pos)
        pos.prev.next = node
        pos.prev = node
        self.size += 1

    def remove_node(self, node):  # O(1)
        if self.head is None:
            raise IndexError("remove from empty list")
        if self.head is node:
            return self.pop_front()
        elif self.tail is node:
            return self.pop_back()
        node.prev.next = node.next
        node.next.prev = node.prev
        self.size -= 1
        return node.data

    def clear(self):  # O(1)
        self.head = None
        self.tail = None
        self.size = 0

    def __iter__(self):
        cur = self.head
        while cur is not None:
            yield cur.data
            cur = cur.next

    def __reversed__(self):
        cur = self.tail
        while cur is not None:
            yield cur.data
            cur = cur.prev
