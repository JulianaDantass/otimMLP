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
int dimension; // quantidade total de vertices


void printData();

struct InsertionInfo{
  int noInserido;
  int arestaRemovida;
  double custo;
};

struct Solution{
  vector<int> sequence;
  double custoSolucao;
};

bool compares(InsertionInfo a, InsertionInfo b);

std:: vector<InsertionInfo> calcularCustoInsercao (Solution& s, vector<int>& CL);

Solution Construcao(vector<int>& CL);

bool BestImprovementSwap (Solution& s);

bool BestImprovement2Opt (Solution& s);

bool BestImprovementOrOpt (Solution& s, int quantity);

void BuscaLocal (Solution& s);

Solution Pertubacao (Solution s);

int main(int argc, char** argv) {

    clock_t start= clock();    //inicia a contagem do tempo de execucao  

    Solution s, bestS, bestOfAll;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    
    vector<int> CL2; 

    readData(argc, argv, &dimension, &matrizAdj);


    if(dimension <= 150){
      maxIterIls= dimension/ 2.0;
    }else{
      maxIterIls= dimension;
    }
    srand(time(NULL));

    for(i= 2; i <= dimension; i++){ //comeca do 2 pq a c1 é adicionada na s.sequence na construcao
      CL.push_back(i);
    }

   bestOfAll.custoSolucao= (double) INFINITY;
   maxIter= 50;
   for(i= 0; i < maxIter; i++){
      
      
      CL2= CL;
      s= Construcao(CL2);

      bestS= s;

      count= 0;
      

      while(count < maxIterIls){
        BuscaLocal(s);
        
        if(s.custoSolucao < bestS.custoSolucao){
          bestS= s;
          count= 0;
        }
        // for(j= 0; j < s.sequence.size(); j++){
        //   cout << s.sequence[j] << " ";
        // }
        // cout << endl;
          s= Pertubacao(bestS);  
         
        // for(j= 0; j < s.sequence.size(); j++){
        //   cout << s.sequence[j] << " ";
        // }
        // cout << s.custoSolucao << endl;
        count++;
      }

    if(bestS.custoSolucao < bestOfAll.custoSolucao){
      bestOfAll= bestS;
    }
     
    
   }
    // printData();
    cout << "melhor " << bestOfAll.custoSolucao << endl;
    
    clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "tempo: " << time << endl;   //imprime o tempo total de execucao

    return 0;  

}  

//------------------------------------------declarao das funcoes

void printData() {
  cout << "dimension: " << dimension << endl;
  for (size_t i = 1; i <= dimension; i++) {
    for (size_t j = 1; j <= dimension; j++) {
      cout << matrizAdj[i][j] << " ";
    }
    cout << endl;
  }
}

bool compares(InsertionInfo a, InsertionInfo b){  //funcao auxiliar na ordenacao dos custos
  return a.custo < b.custo;
}


vector<InsertionInfo> calcularCustoInsercao (Solution& s, std::vector<int>& CL){

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

Solution Construcao(vector<int>& CL){
  
  int indexRandom, i, j;  
  Solution s;

   s.custoSolucao= 0;             //zerando o custo da solucao
  
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

    s.custoSolucao= matrizAdj[s.sequence[0]][s.sequence[1]] + matrizAdj[s.sequence[1]][s.sequence[2]] + matrizAdj[s.sequence[2]][s.sequence[3]] + 
                    matrizAdj[s.sequence[3]][s.sequence[4]];

    //

  while(!CL.empty()){

    std::vector<InsertionInfo> custoInsercao= calcularCustoInsercao(s, CL);

    sort(custoInsercao.begin(), custoInsercao.end(), compares);   //ordena os custos
    
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

    s.custoSolucao += custoInsercao[selecionado].custo;

  }

  return s;
}

bool BestImprovementSwap (Solution& s){         //estrutura de vizinhança: SWAP 

  double delta;
  double bestDelta= 0;
  int best_i, best_j;
  int i, j;
  double partialCost;


  for(i= 1; i < dimension - 1; i++) {

    partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[i]][s.sequence[i+1]];

    for (j= i + 1; j < dimension; j++){
      
      if(i == j-1){
        delta= -matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[j]][s.sequence[j+1]] + matrizAdj[s.sequence[i-1]][s.sequence[j]] + matrizAdj[s.sequence[i]][s.sequence[j+1]];
      }else{
        delta= partialCost - matrizAdj[s.sequence[j]][s.sequence[j+1]] - matrizAdj[s.sequence[j-1]][s.sequence[j]] 
                                + matrizAdj[s.sequence[i-1]][s.sequence[j]] + matrizAdj[s.sequence[i+1]][s.sequence[j]] + matrizAdj[s.sequence[i]][s.sequence[j+1]] + matrizAdj[s.sequence[i]][s.sequence[j-1]];
      }

      if(delta < bestDelta){
        bestDelta= delta;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestDelta < 0){
    swap(s.sequence[best_i], s.sequence[best_j]);
    s.custoSolucao= s.custoSolucao + bestDelta;
    return true;
  }

  return false;
}

bool BestImprovement2Opt (Solution& s){         //estrutura de vizinhança: 2opt 

  double delta, bestDelta= 0;
  int best_i, best_j;
  int i, j;
  double partialCost;


  for(i= 1; i < dimension - 2; i++) {

    partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]];

    for (j= i + 2; j < dimension; j++){
      
      delta= partialCost - matrizAdj[s.sequence[j]][s.sequence[j+1]] 
                              + matrizAdj[s.sequence[i-1]][s.sequence[j]] + matrizAdj[s.sequence[i]][s.sequence[j+1]];


      if(delta < bestDelta){
        bestDelta= delta;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestDelta < 0){

    j= best_j;
    for(i= best_i; i < j; i++){               //for para inverter a subsequencia obtida anteriormente
      swap(s.sequence[i], s.sequence[j]);
      j--;
    }
    s.custoSolucao= s.custoSolucao + bestDelta;
    
    return true;
  }

  return false; 
}

bool BestImprovementOrOpt (Solution& s, int quantity){   //as 3 outras estruturas de vizinhança 

  double delta, bestDelta= 0;
  int best_i, best_j;
  int i, j;
  double partialCost;


  switch(quantity){
    
    case 1:             //método: REINSERTION

      for(i= 1; i < dimension; i++) {

        partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[i]][s.sequence[i+1]]  + matrizAdj[s.sequence[i-1]][s.sequence[i+1]];

        for (j= i + 1; j < dimension; j++){
          
          if(i == j-1){
            delta= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[j]][s.sequence[j+1]]
                                       + matrizAdj[s.sequence[i-1]][s.sequence[j]] + matrizAdj[s.sequence[i]][s.sequence[j+1]];
          }else{
            delta= partialCost - matrizAdj[s.sequence[j]][s.sequence[j+1]] 
                                    + matrizAdj[s.sequence[i]][s.sequence[j]] + matrizAdj[s.sequence[i]][s.sequence[j+1]];
          }

          if(delta < bestDelta){
            bestDelta= delta;
            best_i= i;
            best_j= j;
          }
        }
      }

      if (bestDelta < 0){
        s.sequence.insert(s.sequence.begin() + best_j + 1, s.sequence[best_i]);
        s.sequence.erase(s.sequence.begin() + best_i);
        s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      return false;

    case 2:          //método: OR-OPT-2

      for(i= 1; i < dimension- 2; i++) {

        partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[i+1]][s.sequence[i+2]] + matrizAdj[s.sequence[i-1]][s.sequence[i+2]];

        for (j= i + 2; j < dimension- 1; j++){
          
          delta=  partialCost - matrizAdj[s.sequence[j+1]][s.sequence[j+2]]
                                   + matrizAdj[s.sequence[j+1]][s.sequence[i]] + matrizAdj[s.sequence[i+1]][s.sequence[j+2]];


          if(delta < bestDelta){
            bestDelta= delta;
            best_i= i;
            best_j= j;
          }
        }
      }

      if (bestDelta < 0){
        s.sequence.insert(s.sequence.begin() + best_j + 2, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i+1]);
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      return false;

    case 3:      //método: OR-OPT-3

      for(i= 1; i < dimension - 3; i++) {

        partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[i+2]][s.sequence[i+3]] + matrizAdj[s.sequence[i-1]][s.sequence[i+3]];

        for (j= i + 3; j < dimension - 2; j++){

          delta=  partialCost - matrizAdj[s.sequence[j+2]][s.sequence[j+3]]
                              + matrizAdj[s.sequence[j+2]][s.sequence[i]] + matrizAdj[s.sequence[i+2]][s.sequence[j+3]];
      

          if(delta < bestDelta){
            bestDelta= delta;
            best_i= i;
            best_j= j;
          }
        }
        
      }

      if (bestDelta < 0){
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 4, s.sequence[best_i+1]);
        s.sequence.insert(s.sequence.begin() + best_j + 5, s.sequence[best_i+2]);
        s.sequence.erase(s.sequence.begin() + (best_i + 2));
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      
      return false;  
  }
}

void BuscaLocal (Solution& s){

  vector<int>NL= {1, 2, 3, 4, 5};
  bool improved= false;
  
  while(!NL.empty()){
    
    int n= rand() % NL.size();

    switch (NL[n]) {
      case 1: 
        improved= BestImprovementSwap(s);    
        break;
      case 2: 
        improved= BestImprovement2Opt(s);  
        break;
      case 3:
        improved= BestImprovementOrOpt(s, 1);   //reinsertion    
        break;
      case 4:
        improved= BestImprovementOrOpt(s, 2);   //Or-opt2      
        break;
      case 5:
        improved= BestImprovementOrOpt(s, 3);   //Or-opt3      
        break;
    }
    

    if(improved){
      NL={1, 2, 3, 4, 5};

    }else{
      NL.erase(NL.begin() + n);
    }
    
  } 
}

Solution Pertubacao (Solution s){  

  int subseqMax;
  int subseq1, subseq2;
  int index1, index2;
  int i, times;
  bool changed= false;
  int large, small;

  if (dimension <= 29){
    subseq1= subseq2= 2;

    large= small= subseq1;
  }else{
     subseqMax= dimension / 10;
     subseqMax--;       //recurso utilizado para facilitar o uso do rand

     subseq1= rand() % subseqMax + 2;
     subseq2= rand() % subseqMax + 2;

     if(subseq1 > subseq2){
      large= subseq1;
      small= subseq2;
      changed = true;
    }else{
      large= subseq2;
      small= subseq1;
    }
  }
 
  while(1){
    index1= rand() % dimension + 1;

    if(index1 <= dimension-subseq2-subseq1){
        break;
    }
  }                                            
  while(1){
    index2= rand() % dimension + index1 + subseq1;      //pq o index2 tem q ser sorteado a partir do indice index1+subseq1;

    if(index2 <= dimension-subseq2){
      break;
    }
  }

  
  if( (index1 + subseq1) == index2 ){
    s.custoSolucao= s.custoSolucao - matrizAdj[s.sequence[index1-1]][s.sequence[index1]] - matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2]]
                                   - matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index2+subseq2]]
                                   + matrizAdj[s.sequence[index1-1]][s.sequence[index2]] + matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index1]]
                                   + matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2+subseq2]];
  }else{
    s.custoSolucao= s.custoSolucao - matrizAdj[s.sequence[index1-1]][s.sequence[index1]] - matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index1+subseq1]]
                                   - matrizAdj[s.sequence[index2-1]][s.sequence[index2]] - matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index2+subseq2]]
                                   + matrizAdj[s.sequence[index1-1]][s.sequence[index2]] + matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index1+subseq1]]
                                   + matrizAdj[s.sequence[index2-1]][s.sequence[index1]] + matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2+subseq2]];
  }

  for(i= 0; i < small; i++){
    swap(s.sequence[index1+i], s.sequence[index2+i]);
  }

  if(subseq1 != subseq2){
    
    times= large- small;

    i= 0;
    if(changed){   
        while(times){
          s.sequence.insert(s.sequence.begin() + index2 + subseq2, s.sequence[index1 + subseq2] );
          s.sequence.erase(s.sequence.begin() + index1 + subseq2);
          times--;
          i++;
        }

    }else{
      while(times){
        s.sequence.insert(s.sequence.begin() + index1 + subseq1 + i, s.sequence[index2 + subseq1+i] );
        s.sequence.erase(s.sequence.begin() + index2 + subseq1 + i+1);
        times--;
        i++;
      }  
     }
     
    }

  return s;
}
