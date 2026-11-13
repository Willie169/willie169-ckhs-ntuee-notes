#!/usr/bin/env bash
g++ -std=c++20 -Wall -Wextra -Wshadow -Woverloaded-virtual test.cpp -o test && ./test 2> test.log
