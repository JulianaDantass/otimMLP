#include "readData.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

double ** matrizAdj; // matriz de adjacencia
int vertices; // quantidade total de vertices


void printData();

struct Solution{
  vector<int> sequence;
};

struct InsertionInfo{
  int noInserido;
  int arestaRemovida;
  double custo;
};

struct Subsequence{
  double tempoTotal;
  double custoAcumulado;
};

bool compares(InsertionInfo a, InsertionInfo b){  //funcao auxiliar na ordenacao dos custos
  return a.custo < b.custo;
}

vector<InsertionInfo> calcularCustoInsercao (Solution& s, std::vector<int>& CL){  //calcular o custo da insercao de cada vertice para Construcao

  std::vector<InsertionInfo> custoInsercao ((s.sequence.size()-1) * CL.size());

  int a, b, i, k;
  int count= 0;
  int l= 0;
  

  for(int a= 0, b= 1; count < s.sequence.size() - 1; a++, b++){
    
    int i= s.sequence[a];
    int j= s.sequence[b];

    for (auto k : CL){
      custoInsercao[l].custo= matrizAdj[i][k] + matrizAdj[j][k] - matrizAdj[i][j];
      custoInsercao[l].noInserido= k;
      custoInsercao[l].arestaRemovida= a;
      l++;
      
    }
    count++;
  }
  
  return custoInsercao;
}

Solution Construction (Solution &s, vector<int> CL){  //gerando uma solucao inicial

  int indexRandom, i, j;  
  
  s.sequence.push_back(1);
  s.sequence.push_back(1);      //adicionando a c1 no inicio e no final


  //escolher 3 cidades aleatorias
      
    indexRandom= rand() % CL.size();
    s.sequence.insert(s.sequence.end()-1, CL[indexRandom]);
    CL.erase(CL.begin() + indexRandom);

    indexRandom= rand() % CL.size();
    s.sequence.insert(s.sequence.end()-1, CL[indexRandom]);
    CL.erase(CL.begin() + indexRandom);

    indexRandom= rand() % CL.size();
    s.sequence.insert(s.sequence.end()-1, CL[indexRandom]);
    CL.erase(CL.begin() + indexRandom);


  while(!CL.empty()){

    std::vector<InsertionInfo> custoInsercao= calcularCustoInsercao(s, CL);  //calculando os custos de cada insercao

    sort(custoInsercao.begin(), custoInsercao.end(), compares);    //ordenando os custos
    
    double alpha= (double) rand() / RAND_MAX;
    int selecionado= rand() % ( (int) ceil(alpha * custoInsercao.size()) );       //selecionando um dos candidatos de melhor custo

    s.sequence.insert(s.sequence.begin()+ custoInsercao[selecionado].arestaRemovida + 1, custoInsercao[selecionado].noInserido);  //inserindo o candidato

    j= 0;
    while(1){                     //retirando a cidade inserida da lista de candidatos  
      if(custoInsercao[selecionado].noInserido == CL[j]){
        CL.erase(CL.begin()+j);
        break;
      }
      j++;
    }

  }

  return s;
}


void updateSubseq(Solution &s, vector<vector<Subsequence>> &subseqMatrix){  //fazer o calculo do custo das subsequencias

  int i, j;

  int n= s.sequence.size();
                                            
      
  for(i= 0; i < n; i++){                                                //matriz[0][1] 

    subseqMatrix[i][i].tempoTotal= 0;

      for(j= i+1; j < n; j++){    

        subseqMatrix[i][j].tempoTotal= subseqMatrix[i][j-1].tempoTotal + matrizAdj[s.sequence[j-1]][s.sequence[j]];
        subseqMatrix[i][j].custoAcumulado= subseqMatrix[i][j-1].custoAcumulado + subseqMatrix[i][j-1].tempoTotal;
      }
     
   }

}



int main(int argc, char** argv) {

    clock_t start= clock();    //inicia a contagem do tempo de execucao  

    Solution s, bestS, bestOfAll;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    

    readData(argc, argv, &vertices, &matrizAdj);

    vector<vector<Subsequence>> subseqMatrix;

    if(vertices <= 150){
      maxIterIls= vertices/ 2.0;
    }else{
      maxIterIls= vertices;
    }
    srand(time(NULL));

    for(i= 2; i <= vertices; i++){ 
      CL.push_back(i);
    }


   maxIter= 1;
   for(i= 0; i < maxIter; i++){
      
      s= Construction(s, CL);


      updateSubseq(s, subseqMatrix);

      // bestS= s;

      // count= 0;
      

      // // while(count < maxIterIls){
      // //   BuscaLocal(s);
        
      // //   if(s.costSolution < bestS.costSolution){
      // //     bestS= s;
      // //     count= 0;
      // //   }
      // //   // for(j= 0; j < s.sequence.size(); j++){
      // //   //   cout << s.sequence[j] << " ";
      // //   // }
      // //   // cout << endl;
      // //     s= Pertubacao(bestS);  
         
      // //   // for(j= 0; j < s.sequence.size(); j++){
      // //   //   cout << s.sequence[j] << " ";
      // //   // }
      // //   // cout << s.custoSolucao << endl;
      // //   count++;
      // // }
    
   }
}