# A tuple is an immutable ordered collection of arbitrary number of elements of arbitrary types
a = (
    1,
    "a",
    "a",
    [0],
)
type(a)  # <class 'tuple'>
a = 1, "a", "a", [0]  # can also be created without the parentheses
a[0]  # 1
a[-1]  # [0]
# a[0] = 0  # TypeError: 'tuple' object does not support item assignment
a[3][0] = 1  # allowed
a = ("a",)  # a == ('a',), a tuple. One-item tuple requires comma
a = ("a",)  # a == ('a',), a tuple
a = "a"  # a == 'a', a string
a = (1, 1, 1)
a.count(1)  # 3
a.index(1)  # 0, first occurrence
1 in a  # True
0 not in a  # True
