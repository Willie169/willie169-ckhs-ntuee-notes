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


class ListStack(list):  # LIFO O(1)
    def push(self, x):
        return self.append(x)

    def top(self):
        return self[-1]

    def is_empty(self):
        return self.__len__() == 0


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


class ListDequeue:  # Both sides IO amortized O(1)
    def __init__(self, block_size=64, front_step=1, back_step=1):
        assert block_size > 0
        assert front_step > 0
        assert back_step > 0
        self.block_size = block_size
        self.front_step = front_step
        self.back_step = back_step
        self.data = [
            [None] * self.block_size
            for _ in range(self.front_step + self.back_step - 1)
        ]
        self.block_begin = 0
        self.block_end = self.block_begin + 1
        self.in_begin = (self.block_size - 1) // 2
        self.in_end = self.in_begin
        self.size = 0

    def __len__(self):
        return self.size

    def is_empty(self):
        return self.size == 0

    def capacity(self):
        return self.block_size * len(self.data)

    def is_valid(self):
        return (
            self.size
            == self.block_size * (self.block_end - self.block_begin - 1)
            + self.in_end
            - self.in_begin
        )

    def reserve_front(self, blocks=None):
        if blocks is None:
            blocks = self.front_step
        new = [[None] * self.block_size for _ in range(blocks)]
        new.extend(self.data)
        self.data = new
        self.block_begin += blocks
        self.block_end += blocks

    def reserve_back(self, blocks=None):
        if blocks is None:
            blocks = self.back_step
        self.data.extend([[None] * self.block_size for _ in range(blocks)])

    def shrink_front_to_fit(self):
        new = self.data[self.block_begin :]
        self.data = new
        self.block_end -= self.block_begin
        self.block_begin = 0

    def shrink_back_to_fit(self):
        for i in range(self.block_end, len(self.data)):
            self.data.pop()

    def prepend(self, x):
        if self.in_begin != 0:
            self.in_begin -= 1
        else:
            if self.block_begin == 0:
                self.reserve_front()
            self.block_begin -= 1
            self.in_begin = self.block_size - 1
        self.data[self.block_begin][self.in_begin] = x
        self.size += 1

    def append(self, x):
        if self.in_end != self.block_size:
            self.in_end += 1
        else:
            if self.block_end == len(self.data):
                self.reserve_back()
            self.block_end += 1
            self.in_end = 1
        self.data[self.block_end - 1][self.in_end - 1] = x
        self.size += 1

    def pop_front(self):
        if self.is_empty():
            raise IndexError("pop from empty deque")
        x = self.data[self.block_begin][self.in_begin]
        self.data[self.block_begin][self.in_begin] = None
        self.size -= 1
        if self.in_begin != self.block_size - 1:
            self.in_begin += 1
        else:
            self.block_begin += 1
            self.in_begin = 0
        return x

    def pop_back(self):
        if self.is_empty():
            raise IndexError("pop from empty deque")
        x = self.data[self.block_end - 1][self.in_end - 1]
        self.data[self.block_end - 1][self.in_end - 1] = None
        self.size -= 1
        if self.in_end != 1:
            self.in_end -= 1
        else:
            self.block_end -= 1
            self.in_end = self.block_size
        return x

    def peek_front(self):
        if self.is_empty():
            raise IndexError("peek from empty deque")
        return self.data[self.block_begin][self.in_begin]

    def peek_back(self):
        if self.is_empty():
            raise IndexError("peek from empty deque")
        return self.data[self.block_end - 1][self.in_end - 1]

    def clear(self):
        for _ in range(self.size):
            self.pop_back()

    def __iter__(self):
        block = self.block_begin
        index = self.in_begin
        for _ in range(self.size):
            yield self.data[block][index]
            index += 1
            if index == self.block_size:
                index = 0
                block += 1

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


class Circular:
    def __init__(self, data):
        self.data = data

    def __iter__(self):
        while True:
            yield from self.data
