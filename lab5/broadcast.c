#include <mpi.h>
#include<stdlib.h>
#include<stdio.h>
int main(int argc, char *argv[])
{
int arr[10];
int my_rank;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
if (my_rank == 0)
{
  for(int i = 0; i<10 ; i++){
    arr[i] = rand();
  }
}
MPI_Bcast(arr, 10, MPI_INT, 0, MPI_COMM_WORLD);
printf("rank %d: I received the array \n", my_rank);
MPI_Finalize();
}
