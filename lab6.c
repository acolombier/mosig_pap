
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  int my_rank,my_newrank;
  int size, dims[2], period[2] = {1, 0}, reorder = 1, coord[2];
  
  int matA[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};
                    
  int matB[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};
  int matC[4][4]= {{0}};
  
  //~ MPI_Comm newcomm, oldcomm;
  MPI_Comm comm, subcomm;
  MPI_Group group;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  
  int q = floor(sqrt(size));
  //~ printf(" the sqrt is %d\n", q );
  dims[0] = dims[1] = q;
  
  int nodes = q * q;

  MPI_Dims_create( nodes, 2, dims );
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &comm);
  MPI_Comm_rank(comm, &my_newrank);
  
  MPI_Cart_coords(comm, my_newrank, 2, coord);
  
  int remain_dims[2] = {1, 0};
  MPI_Cart_sub(comm, remain_dims, &subcomm);
  
  for (int k = 0; k < q; k++){
    if (my_rank % 4 == (my_rank / 4 + k) % 4) 
        MPI_Bcast(matA[my_rank / 4] + ((my_rank / 4 + k) % 4), 1 , MPI_INT, ((my_rank / 4 + k) % 4), subcomm); //everybody needs to send my_rank, matA[my_rank / 4][my_rank % 4], matB[my_rank / 4][my_rank % 4], my_newrank, coord[0],coord[1]);
    matC[my_rank / 4][my_rank % 4] = matA[my_rank / 4][my_rank % 4] * matC[my_rank / 4][my_rank % 4];
    
    int rank_nightboor[2];
    MPI_Cart_shift(comm, 0, 1, rank_nightboor, rank_nightboor+1);
    //~ printf("My rank is %d, up is %d and down is %d, transmitting %d\n", my_rank, rank_nightboor[0], rank_nightboor[1], matB[my_rank / 4][my_rank % 4]);
    MPI_Status status;
    MPI_Sendrecv(matB[my_rank / 4] + (my_rank % 4), sizeof(int), MPI_INT,
            rank_nightboor[0], 0,
            matB[my_rank / 4] + (my_rank % 4), sizeof(int), MPI_INT,
            rank_nightboor[1], 0,
            MPI_COMM_WORLD, &status);
    
    printf("K=%d result to=%d for %d\n", k, status.MPI_ERROR, my_rank);
  }

  //~ printf("Hello, world. My rank is %d, and this I have A = %d and B = %d with coordinates (%d %d)\n",

  MPI_Finalize();
}
