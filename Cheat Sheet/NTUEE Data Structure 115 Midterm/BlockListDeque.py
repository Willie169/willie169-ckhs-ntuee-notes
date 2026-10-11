class BlockListDeque:  # Both ends IO amortized O(1)
    def __init__(self, block_size=64, front_step=1, back_step=1):
        if block_size <= 0:
            raise ValueError("deque block_size must >0")
        if front_step <= 0:
            raise ValueError("deque front_step must >0")
        if back_step <= 0:
            raise ValueError("deque back_step must >0")
        self.block_size = block_size
        self.front_step = front_step
        self.back_step = back_step
        self.data = [[None] * self.block_size for _ in range(self.front_step + self.back_step - 1)]
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
        return self.size == self.block_size * (self.block_end - self.block_begin - 1) + self.in_end - self.in_begin

    def __getitem__(self, index):  # O(1)
        if not -self.size <= index < self.size:
            raise IndexError("deque index out of range")
        if index < 0:
            index += self.size
        block_offset, in_block = divmod(
            self.in_begin + index,
            self.block_size,
        )
        return self.data[self.block_begin + block_offset][in_block]

    def __setitem__(self, index, x):  # O(1)
        if not -self.size <= index < self.size:
            raise IndexError("deque assignment index out of range")
        if index < 0:
            index += self.size

        block_offset, in_block = divmod(
            self.in_begin + index,
            self.block_size,
        )
        self.data[self.block_begin + block_offset][in_block] = x

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

    def __reversed__(self):  # O(n)
        block = self.block_end - 1
        index = self.in_end - 1
        for _ in range(self.size):
            yield self.data[block][index]
            index -= 1
            if index < 0:
                index = self.block_size - 1
                block -= 1
