class SNode:
    def __init__(self, data, next=None):
        self.data = data
        self.next = next


class SinglyLinkedList:
    def __init__(self):
        self.head = None
        self.size = 0

    def __len__(self):
        return self.size

    def is_empty(self):
        return self.head is None

    def prepend(self, x):
        self.head = SNode(x, self.head)
        self.size += 1

    def append(self, x):
        node = SNode(x)
        if self.head is None:
            self.head = node
        else:
            cur = self.head
            while cur.next is not None:
                cur = cur.next
            cur.next = node
        self.size += 1

    def pop_front(self):
        if self.head is None:
            raise IndexError("remove from empty list")
        x = self.head.data
        self.head = self.head.next
        self.size -= 1
        return x

    def __iter__(self):
        cur = self.head
        while cur is not None:
            yield cur.data
            cur = cur.next


class DNode:
    def __init__(self, data):
        self.data = data
        self.prev = None
        self.next = None


class DoublyLinkedList:
    def __init__(self):
        self.head = None
        self.tail = None
        self.size = 0

    def __len__(self):
        return self.size

    def is_empty(self):
        return self.size == 0

    def prepend(self, x):
        node = DNode(x)
        node.next = self.head
        if self.head is None:
            self.tail = node
        else:
            self.head.prev = node
        self.head = node
        self.size += 1
        return node

    def append(self, x):
        node = DNode(x)
        node.prev = self.tail
        if self.tail is None:
            self.head = node
        else:
            self.tail.next = node
        self.tail = node
        self.size += 1
        return node

    def pop_front(self):
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

    def pop_back(self):
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

    def __iter__(self):
        cur = self.head
        while cur is not None:
            yield cur.data
            cur = cur.next


class ListStack:
    def __init__(self):
        self.data = []

    def __len__(self):
        return self.data.__len__()

    def push(self, x):
        self.data.append(x)

    def pop(self):
        if self.is_empty():
            raise IndexError("pop from empty stack")
        return self.data.pop()

    def peek(self):
        if self.is_empty():
            raise IndexError("peek at empty stack")
        return self.data[-1]

    def is_empty(self):
        return self.data.__len__() == 0


class LinkedStack:
    def __init__(self):
        self.data = SinglyLinkedList()

    def __len__(self):
        return self.data.__len__()

    def push(self, x):
        self.data.prepend(x)

    def pop(self):
        if self.is_empty():
            raise IndexError("pop from empty stack")
        return self.data.pop_front()

    def peek(self):
        if self.is_empty():
            raise IndexError("peek at empty stack")
        return self.data[0]

    def is_empty(self):
        return self.data.__len__() == 0
