#include <iostream>
#include <mpi.h>

using namespace std;

int main(){

  int rank, err[3];
  MPI_Init(NULL, NULL);
  MPI_Comm children;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_spawn("child", NULL, 3, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &children, err);

  cout << "childrens: " << children << endl;

}
