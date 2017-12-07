#include <iostream>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    int j,i,n,id, size, job,iter;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    srand(time(NULL)+id);
    iter=atoi(argv[1]);
    for(n=0;n<iter;n++){
      job=rand()%10000;
      for(i=0;i<job;i++)
      for(j=0;j<job;j++);
        printf("processo %d chegou na barreira pela %d vez\n",id,n);
        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
