#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

int main(int argc, char *argv[])
{
  int my_rank,my_newrank;
  int size, dims[2], period[2] = {1, 1}, reorder = 1, coord[2];

  int matA[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};

  int matB[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};
  int matC[4][4]= {{0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0},
                    {0, 0, 0, 0}};

  int buffA[4][4];


  MPI_Comm comm, subcomm;
  MPI_Group group;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Status status;
  int source,dest;
  int q = floor(sqrt(size));
  dims[0] = dims[1] = q;


  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &comm);
  MPI_Comm_rank(comm, &my_newrank);

  MPI_Cart_coords(comm, my_newrank, 2, coord);

  int remain_dims[2] = {0, 1};    //row communicator -> so keep all the columns and create as many subcomms as there are rows
  MPI_Cart_sub(comm, remain_dims, &subcomm);

  for (int k = 0; k < q; k++){
//creating a copy of A
   for(int i = 0;i<4;i++){
     for(int j = 0;j<4;j++)
     buffA[i][j] = matA[i][j];
   }
    //if (my_rank % 4 == (my_rank / 4 + k) % 4)
    //k-th diagonal is broadcast for each subcomm
    MPI_Bcast(buffA[0] + my_newrank, 1 , MPI_INT, (my_newrank / 4 + k) % 4, subcomm);
    matC[my_newrank / 4][my_newrank % 4]+= buffA[my_newrank / 4][my_newrank % 4] * matB[my_newrank / 4][my_newrank % 4];
    MPI_Cart_shift(comm,0,1,&source,&dest);
    //printf("My rank is %d, up is %d and down is %d\n" ,my_newrank, source,dest);
    MPI_Sendrecv_replace(matB[my_rank / 4] + (my_rank % 4),1,MPI_INT,dest,0,source,0,comm,&status);

    }

/*

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
 */
  MPI_Finalize();
  for(int i =0; i<4; i++){
    printf("\n");
    for(int j = 0; j<4;j++)
     printf("%d\t",matC[i][j] );
  }

}
