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
# items(), Return a new view of the dictionary’s items ((key, value) pairs).
i = d.items()  # i == dict_items([('two', 2), ('one', 1), ('three', 3)])
list(i)  # [('two', 2), ('one', 1), ('three', 3)]
# keys(), Return a new view of the dictionary’s keys.
k = d.keys()  # k == dict_keys(['two', 'one', 'three'])
list(k)  # ['two', 'one', 'three']
# values(), Return a new view of the dictionary’s values.
v = d.values()  # v == dict_values([2, 1, 3])
list(v)  # [2, 1, 3]
# iter(dictview), Return an iterator over the keys, values or items (represented as tuples of (key, value)) in the dictionary.
# Keys and values are iterated over in insertion order since 3.7. This allows the creation of (value, key) pairs using zip(): pairs = zip(d.values(), d.keys()). Another way to create the same list is pairs = [(v, k) for (k, v) in d.items()].
# x in dictview
# reversed(dictview), reverse iterator
# classmethod fromkeys(iterable, value=None), Return a new dictionary with keys from iterable and values set to value.
d.fromkeys([1, 2, 3])  # {1: None, 2: None, 3: None}
dict.fromkeys([1, 2, 3])  # {1: None, 2: None, 3: None}
# get(key, default=None), Return the value for key if key is in the dictionary, else default.
d.get("two")  # 2
d.get("four")  # (None)
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
# update(**kwargs), update(mapping, **kwargs), update(iterable, **kwargs), Update the dictionary with the key/value pairs from mapping or iterable and kwargs, overwriting existing keys. Return None. update() accepts either another object with a keys() method (in which case __getitem__() is called with every key returned from the method) or an iterable of key/value pairs (as tuples or other iterables of length two). If keyword arguments are specified, the dictionary is then updated with those key/value pairs: d.update(red=1, blue=2).
d.update({"four": 4}, three=3)  # d == {'two': 2, 'one': 1, 'four': 4, 'three': 3}
d.update(two=0)  # d == {'two': 0, 'one': 1, 'four': 4, 'three': 3}
d.update([["two", 2]])  # d == {'two': 2, 'one': 1, 'four': 4, 'three': 3}
d.update((["two", 2],))
d.update((("two", 2),))
d.update([("two", 2)])
d | {
    "one": 0,
    "five": 5,
}  # {'two': 2, 'one': 0, 'four': 4, 'three': 3, 'five': 5}, Create a new dictionary with the merged keys and values of d and other, which must both be dictionaries. The values of other take priority when d and other share keys.
d |= {
    "one": 0,
    "five": 5,
}  # d == {'two': 2, 'one': 0, 'four': 4, 'three': 3, 'five': 5}
