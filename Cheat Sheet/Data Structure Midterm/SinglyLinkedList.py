class _SNode:
    def __init__(self, data, next=None):
        self.data = data
        self.next = next


class SinglyLinkedList:
    def __init__(self):
        self.head = None
        self.tail = None
        self.size = 0

    def __len__(self):
        return self.size

    def is_empty(self):
        return self.head is None

    def prepend(self, x):
        self.head = _SNode(x, self.head)
        if self.tail is None:
            self.tail = self.head
        self.size += 1

    def append(self, x):
        self.tail = _DNode(x, None)
        if self.head is None:
            self.head = self.tail
        self.size += 1

    def pop_front(self):
        if self.head is None:
            raise IndexError("remove from empty list")
        x = self.head.data
        self.head = self.head.next
        self.size -= 1
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
            self.pop_front()

    def __iter__(self):
        cur = self.head
        while cur is not None:
            yield cur.data
            cur = cur.next


class LinkedStack(SinglyLinkedList):  # LIFO O(1)
    def push(self, x):
        return self.prepend(x)

    def pop(self):
        return self.pop_front()

    def top(self):
        return self.peek_front()


class LinkedQueue(SinglyLinkedList):  # FIFO O(1)
    def enqueue(self, x):
        return self.append(x)

    def dequeue(self):
        return self.pop_front()

    def peek(self):
        return self.peek_front()
