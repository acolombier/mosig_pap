#include <mpi.h>
#include<stdlib.h>
#include<stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
int my_rank,my_newrank;
int size;
int dims[2];
dims[0] = dims[1] = 0;
int period[2],reorder;
reorder = 1;
period[1] = period[0] = 0;
int coord[2];
MPI_Comm newcomm;

MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);


MPI_Dims_create( size, 2, dims );
MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &newcomm);
MPI_Comm_rank(newcomm, &my_newrank);
MPI_Cart_coords(newcomm,my_newrank, 2, coord);

printf("Hello, world.  I was %d and now I am %d with coordinates (%d %d)\n", my_rank, my_newrank, coord[0],coord[1]);

MPI_Finalize();
}
