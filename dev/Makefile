%: %.cpp
	g++ -o $@ $^ -std=c++20 -O3 -fopenmp

1debug: 1.cpp
	g++ -o $@ $^ -std=c++20 -g -D_GLIBCXX_DEBUG -pthread

stdg: std.cpp
	g++ -o $@ $^ -std=c++20 -g -D_GLIBCXX_DEBUG -pthread
