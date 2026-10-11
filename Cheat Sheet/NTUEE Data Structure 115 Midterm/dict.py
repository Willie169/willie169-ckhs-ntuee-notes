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
c = d.copy()
# classmethod fromkeys(iterable, value=None), Return a new dictionary with keys from iterable and values set to value.
d.fromkeys([1, 2, 3])  # {1: None, 2: None, 3: None}
dict.fromkeys([1, 2, 3])  # {1: None, 2: None, 3: None}
# get(key, default=None), Return the value for key if key is in the dictionary, else default.
d.get("two")  # 2
d.get("four")  # (None)
# items(), Return a new view of the dictionary’s items ((key, value) pairs).
# keys(), Return a new view of the dictionary’s keys.
# pop(key), pop(key, default), If key is in the dictionary, remove it and return its value, else return default. If default is not given and key is not in the dictionary, a KeyError is raised.
d.pop("one")  # d == {'two': 2, 'three': 3}
d.pop("one", 10)  # 10
# d.pop('one')  # KeyError: 'one'
# popitem(), Remove and return a (key, value) pair from the dictionary. Pairs are returned in LIFO order since 3.7.
d.popitem()  # ('three', 3), d == {'two': 2}
reversed(d)  # reversed(list(d))
# setdefault(key, default=None), If key is in the dictionary, return its value. If not, insert key with a value of default and return default.
d.setdefault("two")  # 2, d == {'two': 2}
d.setdefault("one", 1)  # 1, d == {'two': 2, 'one': 1}
