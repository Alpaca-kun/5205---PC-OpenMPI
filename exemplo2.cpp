#include <iostream>
#include <mpi.h>


int main(int argc, char *argv[]){
    int id, size,i, val_send, val_rec;
    MPI_Status st;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    if(id == 0){
      val_send=100;
      MPI_Send(&val_send,1,MPI_INT,1,3, MPI_COMM_WORLD);
      MPI_Recv(&val_rec, 1,MPI_INT,1,3, MPI_COMM_WORLD,&st);
      printf("id %d enviou %d e recebeu %d\n",id,val_send,val_rec);
    }
    else if (id==1){
      val_send=8008;
      MPI_Send(&val_send,1,MPI_INT,0,3, MPI_COMM_WORLD);
      MPI_Recv(&val_rec, 1,MPI_INT,0,3, MPI_COMM_WORLD,&st);
      printf("id %d enviou %d e recebeu %d\n",id,val_send,val_rec);
    }else{
      printf("id %d nao enviou nem recebeu mensagem\n",id);
    }
    MPI_Finalize();
    return 0;
}
