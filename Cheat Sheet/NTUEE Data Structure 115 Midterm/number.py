bool(0)  # False, only 0 False, other numbers True
2**72  # int is of infinite precision, ** is power
pow(2, 72)  # same as 2**72
float("inf")  # infinite float
(1).__add__(2)  # 3, must use (1). since 1. is treated as decimal
3 // 2  # 1, floor quotient
3 / 2  # 1.5, quotient
divmod(3, 2)  # (1, 1), the pair (x // y, x % y)
abs(-2)  # 2
c = complex(1, 2)  # (1+2j), complex(re, im)
c.conjugate()  # (1-2j)
