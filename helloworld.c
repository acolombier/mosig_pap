#include <stdio.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    char msg[20];
    int my_rank, size;
    
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    printf(my_rank % 2 ? "I'm odd. " : "I'm even. ");
        //~ strcpy(msg, "Hello !");
        //~ MPI_Send(msg, strlen(msg), MPI_CHAR, 1, 99, MPI_COMM_WORLD);
    //~ }
    //~ else {
        //~ MPI_Recv(msg, 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
    printf("Rank: %d,  Total proc: %d\n", my_rank, size);
    
    MPI_Finalize();
}
