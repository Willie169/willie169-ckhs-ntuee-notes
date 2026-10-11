# A list is a dynamic array of elements of arbitrary types
a = [10, 20, 30]
type(a)  # <class 'list'>
a[0]  # 10, O(1)
a[1]  # 20, O(1)
a[-1]  # 30, O(1)
a[-2]  # 20, O(1)
len(a)  # 3, O(1)
a.__len__()  # 3, O(1)
a.append(40)  # a == [10, 20, 30, 40], amortized O(1)
a.pop()  # a == [10, 20, 30], O(1)
a.insert(1, 50)  # a == [10, 50, 20, 30], O(n)
a.pop(0)  # a == [50, 20, 30], O(n)
a[0] = 0  # a == [0, 20, 30], O(1)
b, c, d = a  # b == 0, c == 20, d == 30
a.clear()  # a == []
a = [0, 1, 2]
b = a  # id(a) == id(b)
b.pop()  # a == b == [0, 1], id(a) == id(b)
b = [1, 2]  # id(a) != id(b), a == [0, 1]
a = [1, 1, 1]
a.count(1)  # 3, O(n)
b = a.copy()  # a == b but id(a) != id(b)
a.index(1)  # 0, first occurrence, O(n)
a.extend([0, 4, 3, 4]),  # a == [1, 1, 1, 0, 4, 3, 4], O(k)
a.remove(4)  # a.pop(a.index(x))
a.reverse()  # a == [4, 3, 0, 1, 1, 1], O(n)
a.sort()  # [0, 1, 1, 1, 3, 4], O(n log n) worst-case O(n) best-case, Powersort
sorted(a)  # same as a.sort()
del a[0:5:2]  # a == [1, 1, 4]
1 in a  # True
0 not in a  # True
a = [None] * 5  # a == [None, None, None, None, None]
del a  # a is now not defined
a = [1, "1", [1], True]  # arbitrary types
a.__getitem__(0)  # 1, same as a[0]
a.__setitem__(0, (1,))  # a == [(1,), '1', [1], True], same as a[0] = (1,)
it = iter([1, 2, 3])  # also it = [1, 2, 3].__iter__()
iter(a) is iter(a)  # False
next(it)  # 1, also it.__next__()
# Return of __next__ is defined by the yield clause in the method.
# __next__ is used by for loop.
# range(start, stop, step), all are integers, start + k step, k = [0, (stop - start) / step - 1] integers
reversed(a)  # a reversed iterator of a, also a.__reversed__()
next(a)  # True
list(range(0, 10, 1))  # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
list(range(9, -1, -2))  # [9, 7, 5, 3, 1]
# List comprehensions provide a concise way to create lists. Common applications are to make new lists where each element is the result of some operations applied to each member of another sequence or iterable, or to create a subsequence of those elements that satisfy a certain condition.
list(map(lambda x: x**2, range(10)))  # [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
[x**2 for x in range(10)]  # [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]
[
    (x, y) for x in [1, 2, 3] for y in [3, 1, 4] if x != y
]  # [(1, 3), (1, 4), (2, 3), (2, 1), (2, 4), (3, 1), (3, 4)]
[
    w.strip() for w in ["  banana", "  loganberry ", "passion fruit  "]
]  # ['banana', 'loganberry', 'passion fruit']
[
    num for elem in [[1, 2, 3], [4, 5, 6], [7, 8, 9]] for num in elem
]  # [1, 2, 3, 4, 5, 6, 7, 8, 9]
[
    [
        row[i]
        for row in [
            [1, 2, 3, 4],
            [5, 6, 7, 8],
            [9, 10, 11, 12],
        ]
    ]
    for i in range(4)
]  # [[1, 5, 9], [2, 6, 10], [3, 7, 11], [4, 8, 12]]
# zip(*iterables, strict=False), Iterate over several iterables in parallel, producing tuples with an item from each one, similar to transposing a matrix. By default, zip() stops when the shortest iterable is exhausted. With strict=True, it raises a ValueError if one iterable is exhausted before the others.
list(
    zip(
        *[
            [1, 2, 3, 4],
            [5, 6, 7, 8],
            [9, 10, 11, 12],
        ]
    )
)  # [(1, 5, 9), (2, 6, 10), (3, 7, 11), (4, 8, 12)]
list(zip(range(3), ["fee", "fi", "fo", "fum"]))  # [(0, 'fee'), (1, 'fi'), (2, 'fo')]
# list(zip(range(3), ['fee', 'fi', 'fo', 'fum'], strict=True))  # ValueError: zip() argument 2 is longer than argument 1
[0, *[1, 2, 3], 4, 5, 6]  # [0, 1, 2, 3, 4, 5, 6]
