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
b = a.copy()  # id(a) != id(b)
a.add(3)  # a == {0, 1, 2, 3}
a.remove(3)  # a == {0, 1, 2}, remove if present, KeyError if not
# a.remove(3)  # KeyError: 3
a.discard(2)  # a == {0, 1}, remove if present
a.discard(2)  # a == {0, 1}
a.pop()  # 0, a == {1}, Remove and return an arbitrary element from the set. Raises KeyError if the set is empty.
a.pop()  # 1
# a.pop  # KeyError: 'pop from an empty set'
a = {0, 1, 2, 3}
# arg is any iterable, denoted as b below
a.isdisjoint({0})  # False, a\cap b == \varnothing
a.isdisjoint([0])
a.isdisjoint((0,))
a.issubset(a)  # True, a\subseteq b
a <= a  # True, a\subseteq b
a < a  # False, a\subsetneq b
a.issuperset(a)  # True, a\supseteq b
a >= a  # True, a\supseteq b
a > a  # False, a\supsetneq b
a.union({4})  # {0, 1, 2, 3, 4}, a\cup b
a | {4}  # {0, 1, 2, 3, 4}, a\cup b
a.update({1})  # a == {0, 1, 2, 3}, assign a\cup b to a
a |= {4}  # a == {0, 1, 2, 3, 4}, assign a\cup b to a
a.intersection({3})  # {3}, a\cap b
a & {3}  # {3}, a\cap b
a & {6}  # set(), a\cap b
a.intersection_update(a)  # a == {0, 1, 2, 3, 4}, assign a\cap b to a
a &= {-1, 0, 1, 2, 3}  # a == {0, 1, 2, 3}, assign a\cap b to a
a.difference({1})  # {0, 2, 3}, a\setminus b
a - {1}  # {0, 2, 3}, a\setminus b
a.difference_update({1})  # a == {0, 2, 3}, assign a\setminus b to a
a -= {2}  # a == {0, 3}, assign a\setminus b to a
a.symmetric_difference({1, 3})  # {0, 1}, a\cup b\setminus(a\cap b)
a ^ {1, 3}  # {0, 1}, a\cup b\setminus(a\cap b)
a.symmetric_difference_update({1, 3})  # a == {0, 1}, assign a\cup b\setminus(a\cap b) to a
a ^= {1, 3}  # a == {0, 3}, assign a\cup b\setminus(a\cap b) to a
{c for c in "abracadabra" if c not in "abc"}  # {'d', 'r'}, set comprehension
