CXX = clang++
CXX_FLAGS = -Wall --std=c++14 -O3
CC = clang
CC_FLAGS = -Wall -O3

.PHONY:all

all:example cexample

example:example.cpp sugarless.hpp
	$(CXX) $(CXX_FLAGS) example.cpp -o example

cexample:sample.c sugarless.c
	$(CC) $(CC_FLAGS) sample.c sugarless.c -o sample


