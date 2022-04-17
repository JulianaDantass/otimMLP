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
  double T, C;
  int W;
  int first, last;

  inline static Subsequence Concatenate(Subsequence &sigma1, Subsequence &sigma2){
    
    Subsequence sigma;

    double temp= matrizAdj[sigma1.last][sigma2.first];

    sigma.W= sigma1.W + sigma2.W;
    sigma.T= sigma1.T + sigma2.T;
    sigma.C= sigma1.C + sigma2.W * (sigma1.T + temp) + sigma2.C;
    sigma.first= sigma1.first;
    sigma.last= sigma2.last;

    return sigma;
  }

};

bool compares(InsertionInfo a, InsertionInfo b){  //funcao auxiliar na ordenacao dos custos
  return a.custo < b.custo;
}

void updateSubseq(Solution &s, vector<vector<Subsequence>> &subseqMatrix){  //fazer o calculo do custo das subsequencias

    int n= s.sequence.size();

    cout << "update";

    for(int i= 0; i < n; i++){

      int v = s.sequence[i];

      subseqMatrix[i][i].W = (i > 0);
      subseqMatrix[i][i].C = 0;
      subseqMatrix[i][i].T = 0;
      subseqMatrix[i][i].first = s.sequence[i];
      subseqMatrix[i][i].last = s.sequence[i];
    }

    for (int i = 0; i < n; i++){          

      for (int j = i + 1; j < n; j++){
        subseqMatrix[i][j] = Subsequence::Concatenate(subseqMatrix[i][j-1], subseqMatrix[j][j]);
      }
    
    }
    
  for(int i= n-1; i >= 0; i--){

    for(int j= i-1; j >= 0; j--){
      subseqMatrix[i][j] = Subsequence::Concatenate(subseqMatrix[i][j+1], subseqMatrix[j][j]);
   
    }
  
  }

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

Solution Construction (Solution &s, vector<int> CL){  //metodo da construcao

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

    //

  while(!CL.empty()){

    std::vector<InsertionInfo> custoInsercao= calcularCustoInsercao(s, CL);

    sort(custoInsercao.begin(), custoInsercao.end(), compares);   
    
    double alpha= (double) rand() / RAND_MAX;
    int selecionado= rand() % ( (int) ceil(alpha * custoInsercao.size()) );

    s.sequence.insert(s.sequence.begin()+ custoInsercao[selecionado].arestaRemovida + 1, custoInsercao[selecionado].noInserido);

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

/////

int main(int argc, char** argv) {

    clock_t start= clock();    //inicia a contagem do tempo de execucao  

    Solution s, bestS, bestOfAll;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    

    cout << "ola";

    readData(argc, argv, &vertices, &matrizAdj);

    vector<vector<Subsequence>> subseqMatrix(vertices, vector<Subsequence>(vertices));

    cout << "ola";

    if(vertices <= 150){
      maxIterIls= vertices/ 2.0;
    }else{
      maxIterIls= vertices;
    }
    srand(time(NULL));

    for(i= 2; i <= vertices; i++){ 
      CL.push_back(i);
    }


   maxIter= 1; ///hgoihgohgg
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