#include <iostream>
#include <string.h>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
#include <mpi.h>

using namespace std;

int contaLinhas(string nomeArquivo){
  int cont=0;
  string unused;
  fstream arquivo;
  arquivo.open(nomeArquivo);
  while(getline(arquivo, unused)){
    cont+=1;
  }
  arquivo.close();
  return cont;
}

struct dadosArquivo {
  string genero;
  vector<float> linhasArquivo;
};

struct distanciaClasse {
  string genero;
  float distancia;
};

void printaVetor(dadosArquivo vetor[], int tamVetor){
  for(int i=0;i<tamVetor;i++){
    for(int j=0;j<vetor[i].linhasArquivo.size();j++){
      cout << vetor[i].linhasArquivo[j] << endl;
    }
    cout << vetor[i].genero << endl << endl;
  }
}

void insereVetor(int tamLinhas, dadosArquivo *vetorDados, string nomeArquivo){
  int j = 0, k = 0;
  float numchars;
  string linhaArquivo, aux;
  fstream arquivo;

  arquivo.open(nomeArquivo);

  while(getline(arquivo, linhaArquivo)){
    k = linhaArquivo.find_first_not_of("0123456789.,-");
    for(int l=k;l<=linhaArquivo.size();l++){
      aux += linhaArquivo[l];
    }
    vetorDados[j].genero = aux;
    aux.clear();
    for(int i=0;i<linhaArquivo.size();i++){
      if(linhaArquivo[i] != ','){
        aux += linhaArquivo[i];
      }
      else{
        numchars = stof(aux);
        aux.clear();
        vetorDados[j].linhasArquivo.push_back(numchars);
      }
    }
    j++;
    aux.clear();
  }
}


vector<distanciaClasse> vetorDistancias;
distanciaClasse *vetorAux;


vector<distanciaClasse> calculaDistancia(int linhasTest, int linhasBase, dadosArquivo dadosTest[], dadosArquivo dadosBase[]){
  float menorDistancia = 100000;
  float distancia = 0;
  distanciaClasse distClasse;
  vector<distanciaClasse> vetorDistancias;

  for(int i=0;i<linhasTest;i++){
    for(int j=0;j<linhasBase;j++){
      for(int k=0;k<dadosBase[j].linhasArquivo.size();k++){  // dadosTest.linhasArquivo e dadosBase.linhasArquivo tem o mesmo tamanho.
        distancia = distancia + abs(pow((dadosTest[i].linhasArquivo[k] - dadosBase[j].linhasArquivo[k]), 2));
      }
      distClasse.distancia = sqrt(distancia);
      distClasse.genero = dadosBase[j].genero;
      vetorAux[j] = distClasse;
      distancia = 0;
    }
    distanciaClasse distAux;
    distAux.distancia = 100000;
    for(int l=0;l<linhasBase;l++){
      if(vetorAux[l].distancia < distAux.distancia){
        distAux.genero = vetorAux[l].genero;
        distAux.distancia = vetorAux[l].distancia;
      }
    }
      vetorDistancias.push_back(distAux);
  }
  return vetorDistancias;
}


int main(int argc, char* argv[]){

  int nomeTamanho, id, size, linhasTest = contaLinhas(argv[1]), linhasBase = contaLinhas(argv[2]), divisaoBase, restoBase;
  float speedUp, distancia = 0;
  char nomeProcessador[MPI_MAX_PROCESSOR_NAME];
  distanciaClasse auxiliarLocal[linhasBase], distClasse;
  dadosArquivo dadosTest[linhasTest], dadosBase[linhasBase];
  vector<distanciaClasse> vetMenordist;
  time_t inicio, fim;


  insereVetor(linhasTest, dadosTest, argv[1]);
  insereVetor(linhasBase, dadosBase, argv[2]);
  //printaVetor(dadosTest, linhasTest);
  //printaVetor(dadosBase, linhasBase);


  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  //MPI_Get_processor_name(nomeProcessador, &nomeTamanho);

  cout << size << endl;

  if(id==0){
    cout << "Nome do arquivo de teste: " << argv[1] << endl << "Nome do arquivo de treino " << argv[2] << endl;
    cout << "Calculando com MPI a distância entre as colunas das matrizes..." << endl << endl;
  }

  inicio = time(NULL);

  divisaoBase = linhasBase/(size-1);
  restoBase = linhasBase%(size-1);

  int tag = 1, tag2 = 2, tag3 = 3, posInicial = 0, posFinal = divisaoBase-1;
  float valRecebido[linhasTest][dadosTest[0].linhasArquivo.size()];
  MPI_Status st;


  MPI_Barrier(MPI_COMM_WORLD);

  if(id==0){
    for(int i=0;i<linhasTest;i++){
      for(int j=0;j<dadosTest[i].linhasArquivo.size();j++){
        for(int k=1;k<size;k++){
          MPI_Send(&dadosTest[i].linhasArquivo[j],1, MPI_FLOAT,k,tag,MPI_COMM_WORLD);
        }
      }
    }
    for(int k=1;k<size;k++){
      MPI_Send(&posInicial,1,MPI_INT,k,tag2,MPI_COMM_WORLD);
      MPI_Send(&posFinal,1,MPI_INT,k,tag3,MPI_COMM_WORLD);
      posInicial += divisaoBase;
      if(id<size-1){
        posFinal += divisaoBase;
      }
      else{
        posFinal += divisaoBase + restoBase;
      }
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  cout << "Processo " << id << " saiu da barreira!" << endl;

  //float valRecebido[linhasTest][dadosTest[0].linhasArquivo.size()];

  if(id>0){

    for(int i=0;i<linhasTest;i++){
      for(int j=0;j<dadosTest[0].linhasArquivo.size();j++){
        MPI_Recv(&valRecebido[i][j],1,MPI_FLOAT,0,tag,MPI_COMM_WORLD, &st);
      }
    }
    MPI_Recv(&posInicial,1,MPI_INT,0,tag2,MPI_COMM_WORLD, &st);
    MPI_Recv(&posFinal,1,MPI_INT,0,tag3,MPI_COMM_WORLD, &st);
  }


    for(int i=0;i<linhasTest;i++){
      if(id>0){
        int valAux = 2;
        for(int j=posInicial;j<=posFinal;j++){
          for(int k=0;k<dadosBase[j].linhasArquivo.size();k++){
            distancia = distancia + abs(pow((valRecebido[i][k] - dadosBase[j].linhasArquivo[k]), 2));
          }
          char genero[20];
          distancia = sqrt(distancia);

          for(int n=0;n<dadosBase[j].genero.size();n++){
            genero[n] = dadosBase[j].genero[n];
          }

          MPI_Send(&distancia,1,MPI_FLOAT, 0, valAux, MPI_COMM_WORLD);
          MPI_Send(&genero,20,MPI_CHAR, 0, valAux, MPI_COMM_WORLD);
          valAux++;
          distancia = 0;
        }
        if(id<size-1){
          MPI_Send(&valAux, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        else{
          MPI_Send(&valAux, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
        }
        distancia = 0;
      }

      else{
        int intAux, intAux2;
        float distRecv, menorDistancia = 100000;
        char generoRecv[20];
        vector<distanciaClasse> vetDistclasse;
        distanciaClasse distClasse;

        MPI_Recv(&intAux, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, &st);
        MPI_Recv(&intAux2, 1, MPI_INT, size-1, 1, MPI_COMM_WORLD, &st);

        //cout << "intAux: " << intAux << endl << "intAux2: " << intAux2 << endl;

        for(int i=1;i<size-1;i++){
          for(int j=2;j<intAux;j++){
            MPI_Recv(&distRecv, 1, MPI_FLOAT, i, j, MPI_COMM_WORLD, &st);
            MPI_Recv(&generoRecv,20, MPI_CHAR, i, j, MPI_COMM_WORLD, &st);
            //cout << "J: " << j << " generoRecv " << generoRecv << "  " << "distRecv: " << distRecv << endl;
            distClasse.genero = generoRecv;
            distClasse.distancia = distRecv;
            //cout << "distClasse.genero: " << distClasse.genero << " " << " distClasse.distancia: " << distClasse.distancia << endl;
            vetDistclasse.push_back(distClasse);
          }
        }

        for(int j=2;j<intAux2;j++){
          MPI_Recv(&distRecv, 1, MPI_FLOAT, size-1, j, MPI_COMM_WORLD, &st);
          MPI_Recv(&generoRecv, 20, MPI_CHAR, size-1, j, MPI_COMM_WORLD, &st);
          distClasse.genero = generoRecv;
          distClasse.distancia = distRecv;
          vetDistclasse.push_back(distClasse);
        }


        distClasse.distancia = 100000;

        for(int k=0;k<vetDistclasse.size();k++){
          if(vetDistclasse[k].distancia < distClasse.distancia){
            distClasse.genero = vetDistclasse[k].genero;
            distClasse.distancia = vetDistclasse[k].distancia;
          }
        }
        vetMenordist.push_back(distClasse);
        vetDistclasse.clear();
      }
    //MPI_Barrier(MPI_COMM_WORLD);
  }

  fim = time(NULL);

  if(id==0){
    for(int k=0;k<vetMenordist.size();k++){
      cout << "Processo " << id << "  " << k << ": "<< vetMenordist[k].genero << "  " << vetMenordist[k].distancia << endl;
    }
    cout << difftime(fim, inicio) << " segundos" << endl;
  }

  cout << "Processo " << id << " chegou ao fim!" << endl;
  MPI_Finalize();

}
