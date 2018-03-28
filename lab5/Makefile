THREADS=8

all: bubble mergesort qsort triangular_matrix

bubble: bubble.c
	gcc -g -fopenmp -O2 -DTHREADS=$(THREADS) -o bubble bubble.c
	
mergesort: mergesort.c
	gcc -fopenmp -O2 -DTHREADS=$(THREADS) -o mergesort mergesort.c
	
qsort: qsort.c
	gcc -g -fopenmp -O2 -DTHREADS=$(THREADS) -o  qsort qsort.c
	
triangular_matrix: triangular_matrix.c
	gcc -fopenmp -O2 -DTHREADS=$(THREADS) -o triangular_matrix triangular_matrix.c

clean:
	rm -f bubble mergesort qsort triangular_matrix
