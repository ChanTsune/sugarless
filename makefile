CXX = clang++
CXX_FLAGS = -Wall --std=c++14

.PHONY:all

all:example

example:example.cpp sugarless.hpp
	$(CXX) $(CXX_FLAGS) example.cpp -o example

