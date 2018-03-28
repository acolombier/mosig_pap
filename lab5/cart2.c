#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

int main(int argc, char *argv[])
{
  int my_rank,my_newrank;
  int size, dims[2], period[2] = {0, 0}, reorder = 1, coord[2];
  
  //~ MPI_Comm newcomm, oldcomm;
  MPI_Comm comm;
  MPI_Group group, newgroup;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  int q = floor(sqrt(size));
  //~ printf(" the sqrt is %d\n", q );
  dims[0] = dims[1] = q;
  
  int nodes = q * q;
  /*
  int allowedranks[nodes];
  for (int ix =0; ix < nodes; ix++) {
    allowedranks[ix] = ix;
  }
  MPI_Comm_group( MPI_COMM_WORLD, &group );
  MPI_Group_incl(group, nodes, allowedranks, &newgroup);
  MPI_Comm_create(MPI_COMM_WORLD, newgroup, &oldcomm );
  */

  MPI_Dims_create( nodes, 2, dims );
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &comm);
  MPI_Comm_rank(comm, &my_newrank);
  MPI_Cart_coords(comm, my_newrank, 2, coord);

  printf("Hello, world.  I was %d and now I am %d with coordinates (%d %d)\n", my_rank, my_newrank, coord[0],coord[1]);

  MPI_Finalize();
}
