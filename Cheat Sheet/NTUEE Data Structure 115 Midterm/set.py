a = {
    1,
    "a",
    "a",
}  # t == {1, 'a'}, set is an immutable unordered collection of arbitrary number of elements of hashable types with duplicates ignored
"a".__hash__()  # -8760825294024040742, a hashable type has __hash__ method that returns an int
# a = {[0]} # TypeError: cannot use 'list' as a set element (unhashable type: 'list')
type(a)  # <class 'set'>
type({})  # <class 'dict'>
a = {True, 1}  # t == {True}
a = {1, True}  # t == {1}
a = {False, 0}  # t == {False}
a = {0, False}  # t == {0}
len(a)  # 1
a.clear()  # a == set()
a = {1, 0, 2}  # a == {0, 1, 2}

a.add(3)  # a == {0, 1, 2, 3}
a.update({1})  # a == {0, 1, 2, 3}, take any iterable and add all its elements
a.update([1])  # a == {0, 1, 2, 3}
a.update((1,))  # a == {0, 1, 2, 3}
b = a.copy()  # id(a) != id(b)
a.difference({1})  # {0, 2, 3}, a.difference(b) returns a \setminus b, take any iterable
a.difference([1])  # {0, 2, 3}
a.difference_update(
    {1}
)  # a == {0, 2, 3}, same as difference but not return but assign to a


# 'difference', 'difference_update', 'discard', 'intersection', 'intersection_update', 'isdisjoint', 'issubset', 'issuperset', 'pop', 'remove', 'symmetric_difference', 'symmetric_difference_update', 'union', 'update'
{c for c in "abracadabra" if c not in "abc"}  # {'d', 'r'}, set comprehension
