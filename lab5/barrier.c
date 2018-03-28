#include <mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
int my_rank;
int size;
double t1,t2;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

t1 = MPI_Wtime();
sleep(my_rank);
MPI_Barrier(MPI_COMM_WORLD);
t2 = MPI_Wtime();
printf("Hello, world.  I am %d of %d\n", my_rank, size);
printf( "Elapsed time is %f\n", t2 - t1 );
MPI_Finalize();
}
