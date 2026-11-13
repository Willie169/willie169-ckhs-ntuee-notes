#!/usr/bin/env bash
g++ -std=c++20 -g -Wall -Wextra -Wshadow -Woverloaded-virtual -fsanitize=address,undefined test.cpp -o test && ./test 2> test.log
