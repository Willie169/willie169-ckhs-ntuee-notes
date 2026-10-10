class Circular:
    def __init__(self, data):
        self.data = data

    def __iter__(self):
        while True:
            yield from self.data
