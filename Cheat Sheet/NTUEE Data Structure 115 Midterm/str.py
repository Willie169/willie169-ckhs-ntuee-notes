a = "a"
type(a)  # <class 'str'>
a += "b"  # a == 'ab'
"He".__add__("llo")  # 'Hello'
str(1)  # '1'
"g" in "egg"  # True
"f" in "egg"  # False
len("Hello")  # 5
"Hello World".capitalize()  # 'Hello world', first capitalize, other lowercase
# str.center(width, fillchar=' ') # Return centered in a string of length width. Padding is done using the specified fillchar (default is an ASCII space). The original string is returned if width is less than or equal to len(s).
"Python".center(10)  # '  Python  '
"Python".center(10, "-")  # '--Python--'
"Python".center(4)  # 'Python'
# str.count(sub[, start[, end]])
"spam, spam, spam".count("spam")  # 3
"spam, spam, spam".count("spam", 5)  # 2
"spam, spam, spam".count("spam", 5, 10)  # 1
"spam, spam, spam".count("eggs")  # 0
"spam, spam, spam".count("")  # 17
# str.endswith(suffix[, start[, end]]), Return True if the string ends with the specified suffix, otherwise return False. suffix can also be a tuple of suffixes to look for. With optional start, test beginning at that position. With optional end, stop comparing at that position. Using start and end is equivalent to str[start:end].endswith(suffix).
# suffix can also be a tuple of suffixes to look for.
"Python".endswith("on")  # True
"a tuple of suffixes".endswith(("at", "in"))  # False
"a tuple of suffixes".endswith(("at", "es"))  # True
"Python is amazing".endswith("is", 0, 9)  # True
# str.startswith(prefix[, start[, end]]), endswith but startswith
# str.expandtabs(tabsize=8)
"01\t012\t0123\t01234".expandtabs()  # '01      012     0123    01234'
"01\t012\t0123\t01234".expandtabs(4)  # '01  012 0123    01234'
# str.find(sub[, start[, end]]), Return the lowest index in the string where substring sub is found within the slice s[start:end]. Optional arguments start and end are interpreted as in slice notation. Return -1 if sub is not found.
"spam, spam, spam".find("sp")  # 0
"spam, spam, spam".find("sp", 5)  # 6
# str.rfind(sub[, start[, end]]), find but highest index
# str.index(sub[, start[, end]]), Like find(), but raise ValueError when the substring is not found.
"spam, spam, spam".index("spam")  # 0
# 'spam, spam, spam'.index('eggs') # ValueError: substring not found
# str.rindex(sub[, start[, end]]), index but highest index
# str.join(iterable), Return a string which is the concatenation of the strings in iterable. A TypeError will be raised if there are any non-string values in iterable, including bytes objects. The separator between elements is the string providing this method.
", ".join(["spam", "spam", "spam"])  # 'spam, spam, spam'
"-".join("Python")  # 'P-y-t-h-o-n'
"".join(["a", "b", "c"])  # 'abc'
"Hello World".lower()  # 'hello world', all lowercase
# str.lstrip(chars=None), Return a copy of the string with leading characters removed. The chars argument is a string specifying the set of characters to be removed. If omitted or None, the chars argument defaults to removing whitespace. The chars argument is not a prefix; rather, all combinations of its values are stripped.
"   spacious   ".lstrip()  # 'spacious   '
"www.example.com".lstrip("cmowz.")  # 'example.com'
# str.rstrip(chars=None), lstrip but trailing
# str.strip(chars=None), lstrip and rstrip
# str.partition(sep), Split the string at the first occurrence of sep, and return a 3-tuple containing the part before the separator, the separator itself, and the part after the separator. If the separator is not found, return a 3-tuple containing the string itself, followed by two empty strings.
"Monty Python".partition(" ")  # ('Monty', ' ', 'Python')
"Monty Python's Flying Circus".partition(
    " "
)  # ('Monty', ' ', "Python's Flying Circus")
"Monty Python".partition("-")  # ('Monty Python', '', '')
# str.rpartition(sep), partition but last occurrence
# str.removeprefix(prefix), If the string starts with the prefix string, return string[len(prefix):]. Otherwise, return a copy of the original string:
"Arthur: three!".removeprefix("Arthur: ")  # 'three!'
# str.removesuffix(suffix, /), If the string ends with the suffix string and that suffix is not empty, return string[:-len(suffix)]. Otherwise, return a copy of the original string
# str.replace(old, new, count=-1), Return a copy of the string with all occurrences of substring old replaced by new. If count is given, only the first count occurrences are replaced. If count is not specified or -1, then all occurrences are replaced.
"spam, spam, spam".replace("spam", "eggs")  # 'eggs, eggs, eggs'
"spam, spam, spam".replace("spam", "eggs", 1)  # 'eggs, spam, spam'
# str.split(sep=None, maxsplit=-1), Return a list of the words in the string, using sep as the delimiter string. If maxsplit is given, at most maxsplit splits are done, the leftmost ones (thus, the list will have at most maxsplit+1 elements). If maxsplit is not specified or -1, then there is no limit on the number of splits (all possible splits are made). If sep is given, consecutive delimiters are not grouped together and are deemed to delimit empty strings (for example, '1,,2'.split(',') returns ['1', '', '2']). The sep argument may consist of multiple characters as a single delimiter. Splitting an empty string with a specified separator returns ['']. If sep is not specified or is None, a different splitting algorithm is applied: runs of consecutive whitespace are regarded as a single separator, and the result will contain no empty strings at the start or end if the string has leading or trailing whitespace. Consequently, splitting an empty string or a string consisting of just whitespace with a None separator returns [].
"1,2,3".split(",")  # ['1', '2', '3']
"1,2,3".split(",", maxsplit=1)  # ['1', '2,3']
"1,2,,3,".split(",")  # ['1', '2', '', '3', '']
"1<>2<>3<4".split("<>")  # ['1', '2', '3<4']
# str.rsplit(sep=None, maxsplit=-1), split but rightmost
# str.swapcase()
"Hello World".swapcase()  # 'hELLO wORLD'
# str.title(), Return a titlecased version of the string where words start with an uppercase character and the remaining characters are lowercase. The algorithm uses a simple language-independent definition of a word as groups of consecutive letters.
"Hello world".title()  # 'Hello World'
"they're bill's friends from the UK".title()  # "They'Re Bill'S Friends From The Uk"
# str.upper(), all uppercase
