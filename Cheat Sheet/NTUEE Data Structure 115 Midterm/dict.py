# A dict is a mapping object maps hashable values to arbitrary objects
a = dict(one=1, two=2, three=3)
b = {"one": 1, "two": 2, "three": 3}
c = dict(zip(["one", "two", "three"], [1, 2, 3]))
d = dict([("two", 2), ("one", 1), ("three", 3)])
e = dict({"three": 3, "one": 1, "two": 2})
f = dict({"one": 1, "three": 3}, two=2)
a == b == c == d == e == f  # True
type(d)  # <class 'dict'>
type({})  # <class 'dict'>
list(d)  # ['two', 'one', 'three'], list of keys
len(d)  # 3, len(list(d))
d["two"]  # 2
# d["a"]  # KeyError: 'a'
d["four"] = 5  # d == {'two': 2, 'one': 1, 'three': 3, 'four': 5}
d["four"] = 4  # d == {'two': 2, 'one': 1, 'three': 3, 'four': 4}
del d["four"]  # d == {'two': 2, 'one': 1, 'three': 3}
# del d["five"]  # KeyError: 'five'
"one" in d  # True
"five" in d  # False
iter(d)  # iter(d.keys())
e.clear()  # e == {}
