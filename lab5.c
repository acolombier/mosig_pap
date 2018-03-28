#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <unistd.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char msg[20];
    int my_rank, size;
    
    float buffer[10];
            
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    
        
    srand(time(NULL));
    
    if (!my_rank){
        
        for (int i = 0; i < 10; i++)
            buffer[i] = rand();
        
        double time = MPI_Wtime();
        MPI_Send(buffer, 10, MPI_FLOAT, 1, 0, MPI_COMM_WORLD);
        
        MPI_Recv(buffer, 10, MPI_FLOAT, 1, 0, MPI_COMM_WORLD, &status);
        printf("RTT: %f\n", MPI_Wtime() - time);
    } else if (my_rank ==  10) {
        MPI_Recv(buffer, 10, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, &status);
        printf("Status: %d\n", status.MPI_ERROR);
        
        for (int i = 0; i < 10; i++)
            buffer[i] = rand();
        
        MPI_Send(buffer, 10, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }
    
    //~ printf(my_rank % 2 ? "I'm odd. " : "I'm even. ");
        //~ strcpy(msg, "Hello !");
        //~ MPI_Send(msg, strlen(msg), MPI_CHAR, 1, 99, MPI_COMM_WORLD);
    //~ }
    //~ else {
        //~ MPI_Recv(msg, 20, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);
    //~ printf("Rank: %d,  Total proc: %d\n", my_rank, size);
    
    char name[256];
    
    gethostname(name, 256);
    printf("Hello from %d on %s (out of %d procs.!)\n", my_rank, name, size);
    
    MPI_Finalize();
}
