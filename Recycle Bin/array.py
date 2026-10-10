from array import array
a = array('b', [10, 20, 30]) # array.array is a dynamic array of elements of the same primitive data type in a compact, contiguous memory buffer
# 'b' char
# 'B' unsigned char
# 'h' short
# 'H' unsigned short
# 'i' int
# 'I' unsigned int
# 'l' long
# 'f' float
# 'd' double
a.itemsize # 1
a.buffer_info() # (address, len(a))
b = a.tolist() # b == [10, 20, 30]
# As len(a) gets large, array tends to be smaller than list
# Methods for list are available for array with same complexity
