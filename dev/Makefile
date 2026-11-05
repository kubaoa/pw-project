.PHONY: all clean

all: hirschberg omp_2_threads omp_mixed omp_small std_thread

%: %.cpp
	g++ -o $@ $^ -std=c++20 -O3 -fopenmp

stdg: std.cpp
	g++ -o $@ $^ -std=c++20 -g -D_GLIBCXX_DEBUG -pthread

clean:
	rm -f hirschberg omp_2_threads omp_mixed omp_small std_thread
