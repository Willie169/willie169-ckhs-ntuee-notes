# Python operator precedence order
# ()
# ** (a ** b is a to the power of b, also pow(a, b))
# +x -x ~x (bitwise NOT)
# * / (float quotient) // (floor quotient) % (modulo)
# + - > << >> (preserve sign)
# & (bitwise AND)
# \^ (bitwise XOR)
# | (bitwise OR)
# == != > >= < <= (value comparison) is is not (identity) in not in
# not (logical NOT)
# and (logical AND)
# or (logical OR)
bool(0)  # False, only 0 False, other numbers True
2**72  # int is of infinite precision, ** is power
float("inf")  # infinite float
(1).__add__(2)  # 3, must use (1). since 1. is treated as decimal
3 // 2  # 1
-3 // 2  # -2
3 / 2  # 1.5
type(3/3) # float
divmod(3, 2)  # (1, 1), the pair (x // y, x % y)
abs(-2)  # 2
c = complex(1, 2)  # (1+2j), complex(re, im)
c.conjugate()  # (1-2j)
a = 0.5  # a == 0.5
type(True)  # <class 'bool'>
type(1)  # <class 'int'>
type(1.0)  # <class 'float'>
type(complex(1, 1))  # <class 'complex'>
a = None
a is None  # True
a = [1]
b = a
a == b  # True
a != b  # False
a is b  # True, i.e., id(a) == id(b)
a is not b  # False
b = a.copy()  # Shallow copy
a == b  # True
a is b  # False
a[0] is b[0]  # True
c, d = a, b
c, d = d, c  # swap
3 if 2 > 1 else 2  # 3
sradius = input("Please enter the radius of the circle ")
# ...
# Please enter the radius of the circle (input here, say 1)
# sradius == 1
# print(obj[, obj ,...], sep=' ', end='\n', file=sys.stdout, flush=False)
# obj: Arbitrary number of objects that can be converted to str. Will be converted to str and printed.
# sep: Separator between objects
# end: What to print at end
# file: An object with a write method to print to
# flush: True for flushed, False for buffered
print("a")  # a
print(1, [1])  # 1 [1]
print(1, 2, sep=",", end="1")  # 1,21<no_newline>
print(1, 2, sep=", ", end="")  # 1, 2<no_newline>
