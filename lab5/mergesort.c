#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include<omp.h>

#include <x86intrin.h>

#define NBEXPERIMENTS   7

static long long unsigned int experiments [NBEXPERIMENTS] ;

static   unsigned int N ;

typedef  int  *array_int ;

static array_int X ;

void init_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
    {
      T [i] = N - i ;
    }
}

void print_array (array_int T)
{
  register int i ;

  for (i = 0 ; i < N ; i++)
    {
      printf ("%d ", T[i]) ;
    }
  printf ("\n") ;
}

int is_sorted (array_int T)
{
  register int i ;
  
  for (i = 1 ; i < N ; i++)
    {
        /* test designed specifically for our usecase */
        if (T[i-1] +1  != T [i] )
            return 0 ;
    }
  return 1 ;
}

long long unsigned int average (long long unsigned int *exps)
{
  unsigned int i ;
  long long unsigned int s = 0 ;

  for (i = 2; i < (NBEXPERIMENTS-2); i++)
    {
      s = s + exps [i] ;
    }

  return s / (NBEXPERIMENTS-2) ;
}



void merge_sort (int *T, const int size)
{
    /* TODO: sequential version of the merge sort algorithm */
    if (size > 2){
        merge_sort(T, size / 2);
        merge_sort(T + (size / 2), size / 2);
    }
    
    if (T[0] > T[size / 2]){
        for (register int i = 0; i < size / 2; i++){
            register int tmp = T[i];
            T[i] = T[i + size / 2];
            T[i + size / 2] = tmp;
        }
    }
    
}

void parallel_merge_sort_worker (int *T, const int size, const int tot_size)
{
    /* TODO: sequential version of the merge sort algorithm */
    
    if (size > 2){
        #pragma omp task final(tot_size / size >= THREADS)
        parallel_merge_sort_worker(T, size / 2, tot_size);
        
        #pragma omp task final(tot_size / size >= THREADS)
        parallel_merge_sort_worker(T + (size / 2), size / 2, tot_size);
        
        #pragma omp taskwait
    }
    
  
    if (T[0] > T[size / 2]){
        for (register int i = 0; i < size / 2; i++){
            register int tmp = T[i];
            T[i] = T[i + size / 2];
            T[i + size / 2] = tmp;
        }
    }
}

void parallel_merge_sort (int *T, const int size)
{
    /* TODO: sequential version of the merge sort algorithm */
    
    #pragma omp parallel
    {
        #pragma omp single
        parallel_merge_sort_worker(T, size, size);
    }
}

int main (int argc, char **argv)
{
  unsigned long long int start, end, residu ;
  unsigned long long int av ;
  unsigned int exp ;

    if (argc != 2)
    {
      fprintf (stderr, "mergesort N \n") ;
      exit (-1) ;
    }

  N = 1 << (atoi(argv[1])) ;
  X = (int *) malloc (N * sizeof(int)) ;

  printf("--> Sorting an array of size %u\n",N);
  
  start = _rdtsc () ;
  end   = _rdtsc () ;
  residu = end - start ; 

  // print_array (X) ;

  printf("sequential sorting ...\n");


    for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
      init_array (X) ;
      
      start = _rdtsc () ;

               merge_sort (X, N) ;
     
      end = _rdtsc () ;
      experiments [exp] = end - start ;
      
      if (! is_sorted (X))
	{
            fprintf(stderr, "ERROR: the array is not properly sorted\n") ;
            exit (-1) ;
	}      
    }
  
  av = average (experiments) ;  
  printf ("\n merge sort serial\t\t %Ld cycles\n\n", av-residu) ;

  printf("parallel sorting ...\n");
  
  for (exp = 0 ; exp < NBEXPERIMENTS; exp++)
    {
      init_array (X) ;
      
      start = _rdtsc () ;

           parallel_merge_sort (X, N) ;
     
      end = _rdtsc () ;
      experiments [exp] = end - start ;

      if (! is_sorted (X))
	{
            fprintf(stderr, "ERROR: the array is not properly sorted\n") ;
            exit (-1) ;
	}      
    }
  
  av = average (experiments) ;
  printf ("\n merge sort parallel with tasks\t %Ld cycles\n\n", av-residu) ;

  
}
