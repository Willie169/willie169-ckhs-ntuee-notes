class _DNode:
    def __init__(self, data, prev=None, next=None):
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
        self.head = _DNode(x, None, self.head)
        if self.tail is None:
            self.tail = self.head
        self.head.next.prev = self.head
        self.size += 1

    def append(self, x):
        self.tail = _DNode(x, self.tail, None)
        if self.head is None:
            self.head = self.tail
        self.size += 1

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

    def peek_front(self):
        if self.is_empty():
            raise IndexError("peek at empty list")
        return self.head.data

    def peek_back(self):
        if self.is_empty():
            raise IndexError("peek at empty list")
        return self.tail.data

    def clear(self):
        for _ in range(self.size):
            self.pop_back()

    def __iter__(self):
        cur = self.head
        while cur is not None:
            yield cur.data
            cur = cur.next


class LinkedDequeue(DoublyLinkedList):  # Both sides IO O(1)
    def push(self, x):
        return self.prepend(x)

    def enqueue(self, x):
        return self.append(x)

    def dequeue(self):
        return self.pop_front()

    def pop(self):
        return self.pop_front()

    def top(self):
        return self.peek_front()

    def peek(self):
        return self.peek_front()
