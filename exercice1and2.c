
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <string.h>

#define HORIZONTAL_SKEWING 1
#define VERTICAL_SKEWING 0

#define PRE_SKEWING 1 
#define POST_SKEWING 0

#define TOP_NEIGHBOR 0
#define BOTTOM_NEIGHBOR 1
#define LEFT_NEIGHBOR 2
#define RIGHT_NEIGHBOR 3

#define NO_ALGO -1
#define ALGO_FOX 0
#define ALGO_CANNON 1

#define DEFAULT_SIZE 4

int create_rows(MPI_Comm* cart_comm, MPI_Comm* row_comm){    
    int remain_dims[2] = {0, 1}, row_rank;
    MPI_Cart_sub(*cart_comm, remain_dims, row_comm); 
    MPI_Comm_rank(*row_comm, &row_rank);
    return row_rank;
}

int create_cols(MPI_Comm* cart_comm, MPI_Comm* col_comm){    
    int remain_dims[2] = {1, 0}, col_rank;
    MPI_Cart_sub(*cart_comm, remain_dims, col_comm); 
    MPI_Comm_rank(*col_comm, &col_rank);
    return col_rank;
}

int create_cartesian(int* dims, MPI_Comm* cart_comm){
    int period[2] = {1, 1}, reorder = 1, cart_rank, coord[2];
    
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, cart_comm);
    MPI_Comm_rank(*cart_comm, &cart_rank);
    MPI_Cart_coords(*cart_comm, cart_rank, 2, coord);
    
    return cart_rank;
}

void skewing(char type, char direction, int* mat, int size, MPI_Comm cart_comm, MPI_Comm loc_comm, int cart_rank, int loc_rank){
    int shift_count = (type == HORIZONTAL_SKEWING) ? (cart_rank / size) : (cart_rank % size);
    if (shift_count){
        int rank_nightboor[2];
        MPI_Cart_shift(cart_comm, type, (direction * 2 -1) * shift_count, rank_nightboor, rank_nightboor+1);   
        MPI_Status status;
        
        MPI_Sendrecv_replace(mat, 1, MPI_INT,
                             rank_nightboor[0], shift_count, rank_nightboor[1], shift_count,
                             cart_comm, &status);
     }
}

int mult_mat_cannon(int matA, int matB, int size, MPI_Comm cart_comm, int cart_rank){

    MPI_Comm row_comm, col_comm;
    int matC = 0, row_rank = create_rows(&cart_comm, &row_comm), 
                  col_rank = create_cols(&cart_comm, &col_comm);
    
    skewing(HORIZONTAL_SKEWING, PRE_SKEWING, &matA, size, cart_comm, row_comm, cart_rank, row_rank);
    skewing(VERTICAL_SKEWING, PRE_SKEWING, &matB, size, cart_comm, col_comm, cart_rank, col_rank);
    
    int rank_neighbor[4];
    MPI_Cart_shift(cart_comm, 0, 1, rank_neighbor + TOP_NEIGHBOR, rank_neighbor + BOTTOM_NEIGHBOR);   
    MPI_Cart_shift(cart_comm, 1, 1, rank_neighbor + LEFT_NEIGHBOR, rank_neighbor + RIGHT_NEIGHBOR);   
    MPI_Status status;
    
    for (int k = 0; k < size; k++){
        // Computation
        matC += matA * matB;
        
        // Horizontal shift
        MPI_Sendrecv_replace(&matA, 1, MPI_INT,
                             rank_neighbor[LEFT_NEIGHBOR], k, 
                             rank_neighbor[RIGHT_NEIGHBOR], k,
                             cart_comm, &status);
        
        // Vertical shift
        MPI_Sendrecv_replace(&matB, 1, MPI_INT,
                             rank_neighbor[TOP_NEIGHBOR], k, 
                             rank_neighbor[BOTTOM_NEIGHBOR], k,
                             cart_comm, &status);
    }
    
    skewing(HORIZONTAL_SKEWING, POST_SKEWING, &matA, size, cart_comm, row_comm, cart_rank, row_rank);
    skewing(VERTICAL_SKEWING, POST_SKEWING, &matB, size, cart_comm, col_comm, cart_rank, col_rank);
    
    MPI_Comm_free(&row_comm);
    MPI_Comm_free(&col_comm);
       
    return matC;    
}

int mult_mat_fox(int matA, int matB, int size, MPI_Comm cart_comm, int cart_rank){    
    MPI_Comm row_comm;    
    
    int mat_diag, matC = 0,
        row_rank = create_rows(&cart_comm, &row_comm);
        
    int rank_neighbor[2];
    MPI_Cart_shift(cart_comm, 0, 1, rank_neighbor + TOP_NEIGHBOR, rank_neighbor + BOTTOM_NEIGHBOR);   
    MPI_Status status;
    
    for (int k = 0; k < size; k++){
        
        // Broadcast        
        if (row_rank == (cart_rank / size + k) % size)
            mat_diag = matA;
        MPI_Bcast(&mat_diag, 1, MPI_INT, (cart_rank / size + k) % size, row_comm);
        
        // Computation
        matC += mat_diag * matB;
        
        // Vertical shift
        MPI_Sendrecv_replace(&matB, 1, MPI_INT,
                             rank_neighbor[TOP_NEIGHBOR], cart_rank,
                             rank_neighbor[BOTTOM_NEIGHBOR], rank_neighbor[BOTTOM_NEIGHBOR],
                             cart_comm, &status);
    }
    
    MPI_Comm_free(&row_comm);
    
    return matC;
}

void usage(){
    fprintf(stderr, "lab5 --algorithm <fox|cannon>\n");
}

int main(int argc, char *argv[])
{
    int main_rank, cart_rank;
    int nprocess, matsize = -1, dims[2];
    
    char algorithm = NO_ALGO;

    int matA[4 * 4] = {5, 9, 8, 8,
                       6, 2, 4, 9,
                       5, 1, 2, 2,
                       1, 1, 2, 5};

    int matB[4 * 4] = {5, 4, 9, 8,
                       4, 8, 2, 4,
                       1, 7, 4, 7,
                       2, 3, 3, 6};
    int matC[4 * 4] = {0};
    
    // Initialise MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocess);
    MPI_Datatype elementtype;

    // Retrieve the MPI constants
    MPI_Comm cart_comm;
    MPI_Comm_rank(MPI_COMM_WORLD, &main_rank);
    
    if (main_rank == 0){
        int argCount;
        for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount){
            argCount = 1;
            if (!strncmp (*argv, "--algorithm", 6)){
                argCount++;
                if (!strcmp (*(argv + 1), "fox"))
                    algorithm = ALGO_FOX;
                else if (!strcmp (*(argv + 1), "cannon"))
                    algorithm = ALGO_CANNON;
                else
                    usage();
            } else {
                fprintf(stderr, "Unknow parameter '%s'\n\n", *argv);
            }
        }
        
        if (algorithm == NO_ALGO){
            fprintf(stderr, "Algorithm missing.\n\n");
            usage();
        }       
    }

    // Gathering main constant from root node
    MPI_Bcast(&algorithm, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (algorithm == NO_ALGO){
        MPI_Finalize();
        return 0;
    }    
    
    // Create the cartesian
    dims[0] = dims[1] = sqrt(nprocess);
    cart_rank = create_cartesian(dims, &cart_comm);
    
    if (algorithm == ALGO_FOX)
        matC[cart_rank] = mult_mat_fox(matA[cart_rank], matB[cart_rank], dims[0], cart_comm, cart_rank);
    else
        matC[cart_rank] = mult_mat_cannon(matA[cart_rank], matB[cart_rank], dims[0], cart_comm, cart_rank);
    
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return EXIT_SUCCESS;
}

