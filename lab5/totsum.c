#include <mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>
int main(int argc, char *argv[])
{
int arr[10];
int my_rank;
int size;
int sum = 0;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);
int pid = getpid();
printf("my pid is %d \n", pid);
MPI_Reduce(
    &pid,
    &sum,
    1,
    MPI_INT,
    MPI_SUM,
    0,
    MPI_COMM_WORLD);


  MPI_Bcast(&sum, 1 , MPI_INT, 0, MPI_COMM_WORLD); //everybody needs to send


printf("rank %d: I received the sum %d \n", my_rank, sum);
MPI_Finalize();
}
