
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>

int main(int argc, char *argv[])
{
    int main_rank, cart_rank, row_rank;
    int size, dims[2], period[2] = {1, 0}, reorder = 1, coord[2];

    int matA[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};
                    
    int matB[4][4] = {{0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2},
                    {0, 1, 1, 2}};
    int matC[4][4]= {{0}}, matBuffer[4][4]= {{0}};
    
    // Initialise MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Retrieve the MPI constants
    MPI_Comm cart_comm, row_comm;
    MPI_Comm_rank(MPI_COMM_WORLD, &main_rank);

    int q = floor(sqrt(size));
    
    // Create the cartesian
    //~ MPI_Dims_create(size, 2, dims);
    dims[0] = dims[1] = q;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &cart_comm);
    MPI_Comm_rank(cart_comm, &cart_rank);
    MPI_Cart_coords(cart_comm, cart_rank, 2, coord);

    // Create rows 
    int remain_dims[2] = {0, 1};
    MPI_Cart_sub(cart_comm, remain_dims, &row_comm); 
    MPI_Comm_rank(row_comm, &row_rank);

    //~ printf("Rank %d, Cart_rank %d, Row_rank %d, PID %d\n", main_rank, cart_rank, row_rank, getpid());

    for (int k = 0; k < 3; k++){
        //~ if (k == 3)

        if (row_rank == (cart_rank / 4 + k) % 4)
            matBuffer[cart_rank / 4][row_rank] = matA[cart_rank / 4][row_rank],
        MPI_Bcast(matBuffer[cart_rank / 4] + (cart_rank / 4 + k) % 4, 1, MPI_INT, (cart_rank / 4 + k) % 4, row_comm); //everybody needs to send my_rank, matA[my_rank / 4][my_rank % 4], matB[my_rank / 4][my_rank % 4], row_rank, coord[0],coord[1]);
            
        //~ printf("rank=%d, col=%d, root=%d\n", cart_rank, row_rank, (row_rank + k) % 4);
         
        // Computation
        matC[cart_rank / 4][row_rank] += matBuffer[cart_rank / 4][row_rank] * matB[cart_rank / 4][row_rank];

        int rank_nightboor[2];
        MPI_Cart_shift(cart_comm, 0, 1, rank_nightboor, rank_nightboor+1);
        //~ printf("My rank is %d, up is %d and down is %d, transmitting %d\n", cart_rank, rank_nightboor[0], rank_nightboor[1], matB[cart_rank / 4][cart_rank % 4]);
        MPI_Status status;

        MPI_Sendrecv_replace(matB[cart_rank / 4] + (cart_rank % 4), 1, MPI_INT,
                             rank_nightboor[0], k, rank_nightboor[1], k,
                             cart_comm, &status);
        //~ if (k == 1) 
            //~ printf("DONE: K=%d from=%d: recv %d, send %d\n", k, my_rank, rank_nightboor[0], rank_nightboor[1]);

        //~ if (k == 1) printf("K=%d result to=%d for %d\n", k, status.MPI_ERROR, my_rank);
    }
    
    if (cart_rank == 0){
        printf("[");
        for (int i = 0; i < q; i++){
            for (int j = (!i ? 1 : 0); j < q; j++){
                MPI_Recv(matC[i] + j, 1, MPI_INT, i * q + j, 0, cart_comm, NULL);
                printf("%d, ", matC[i][j]);
            }
            printf("\n ");
        }               
        printf("]"); 
    }
                
    else
        MPI_Send(matC[cart_rank / 4] + row_rank, 1, MPI_INT, 0, 0, cart_comm);


    //~ printf("Hello, world. My rank is %d, and this I have A = %d and B = %d with coordinates (%d %d)\n",

    MPI_Finalize();
}

