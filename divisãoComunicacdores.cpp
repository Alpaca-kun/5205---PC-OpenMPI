#include <iostream>
#include <mpi.h>
#include <string.h>

#define ID_GROUP_ROOT 0

int main(int argc, char *argv[]){
MPI_Init(&argc, &argv);
int id, size, color,id_new_comm,new_comm_size;
char frase[50];
MPI_Comm new_comm;

MPI_Comm_rank(MPI_COMM_WORLD, &id);
MPI_Comm_size(MPI_COMM_WORLD, &size);
color = id%2;

MPI_Comm_split(MPI_COMM_WORLD, color, id, &new_comm);
MPI_Comm_rank(new_comm, &id_new_comm);
MPI_Comm_size(new_comm, &new_comm_size);

if((id_new_comm == ID_GROUP_ROOT) && (color == 0)){
  strcpy(frase,"par");
}
else if ((id_new_comm == ID_GROUP_ROOT) && (color == 1)){
  strcpy(frase,"impar");
}

MPI_Bcast(frase,50,MPI_CHAR,ID_GROUP_ROOT,new_comm);
printf("sou o id global:%d, sou o processo %s com id:%d, entre %d processos\n",
id,frase,id_new_comm,new_comm_size);
MPI_Comm_free(&new_comm);
MPI_Finalize();
}
