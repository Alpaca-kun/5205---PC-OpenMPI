#define TAG 1
#include

int main(int argc, char *argv[]){
    int id, size,i,val,max,min, local_val;
    MPI_Init(&argc,&argv);
    MPI_Status st;
    MPI_Comm_rank(MPI_COMM_WORLD,&id);
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    srand(time(NULL)+id);
//gerar um valor aleatório baseado no tempo e no id;
    local_val = rand()%1000;
    if(id != 0){
//se nao for o processo responsável por calcular o mínimo e o máximo
// ele apenas envia seu valor e recebe o mínimo e o máximo
 MPI_Send(&local_val,1,MPI_INT,0,TAG,MPI_COMM_WORLD);
 MPI_Recv(&max,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);
 MPI_Recv(&min,1,MPI_INT,i,TAG,MPI_COMM_WORLD,&st);
    }
