a = [10, 20, 30]  # list is a dynamic array of elements of arbitrary types
a[0]  # 10, O(1)
a[1]  # 20, O(1)
a[-1]  # 30, O(1)
a[-2]  # 20, O(1)
len(a) # 3, O(1)
a.__len__() # 3, O(1)
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
a.index(1)  # 0, O(n), first occurrence
a.extend([0, 4, 3, 4]),  # a == [1, 1, 1, 0, 4, 3, 4], O(k)
a.remove(4)  # a.pop(a.index(x))
a.reverse()  # a == [4, 3, 0, 1, 1, 1], O(n)
a.sort()  # [0, 1, 1, 1, 3, 4], O(n log n) worst-case O(n) best-case, Powersort
sorted(a) # same as a.sort()
del a  # a is now not defined
a = [1, "1", [1]]  # arbitrary types
