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

  for(i= 0; i < n-1; i++){                   
    
    subseqMatrix[i][i].tempoTotal= 0;
    subseqMatrix[i][i].custoAcumulado= 0;

      for(j= i+1; j < n-1; j++){    

        subseqMatrix[i][j].tempoTotal= subseqMatrix[i][j-1].tempoTotal + matrizAdj[s.sequence[j-1]][s.sequence[j]];
        subseqMatrix[i][j].custoAcumulado= subseqMatrix[i][j-1].custoAcumulado + subseqMatrix[i][j].tempoTotal;
      }
     
   }


}


bool BestImprovementSwap (Solution& s, vector<vector<Subsequence>> &subseqMatrix){         //estrutura de vizinhança: SWAP ja alterada

  double delta;
  double bestDelta= 0;
  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  double bestCost= subseqMatrix[0][vertices].custoAcumulado;

  for(i= 1; i < vertices - 1; i++) {

    for (j= i + 1; j < vertices-1; j++){
      
      partialCost = subseqMatrix[0][i-1].custoAcumulado + ( (j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]]) ) + subseqMatrix[j][i].custoAcumulado;


      secondCost= subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]] + subseqMatrix[j][i].tempoTotal;


      cost = partialCost + ((vertices-j) * (partialCost + matrizAdj[s.sequence[i]][s.sequence[j+1]]) + subseqMatrix[j+1][vertices].custoAcumulado);

      if(cost < bestCost){
        bestCost= cost;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestDelta < subseqMatrix[0][vertices].custoAcumulado){
    swap(s.sequence[best_i], s.sequence[best_j]);
    // s.custoSolucao= s.custoSolucao + bestDelta;
    return true;
  }

  return false;
}


bool BestImprovement2Opt (Solution& s, vector<vector<Subsequence>> &subseqMatrix){         //estrutura de vizinhança: 2opt    //alterada

  double bestCost;
  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  bestCost= subseqMatrix[0][vertices].custoAcumulado;


  for(i= 1; i < vertices - 2; i++) {

    for (j= i + 2; j < vertices-1; j++){
      
      partialCost= subseqMatrix[0][i-1].custoAcumulado + ( (j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]]) ) + subseqMatrix[j][i].custoAcumulado;
                            
      secondCost= subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]] + subseqMatrix[j][i].tempoTotal;

      cost = partialCost + ( (vertices-j) * (secondCost + matrizAdj[s.sequence[i]][s.sequence[j+1]]) ) + subseqMatrix[j+1][vertices].custoAcumulado;


      if(cost < bestCost){
        bestCost= cost;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestCost < subseqMatrix[0][vertices].custoAcumulado){

    j= best_j;
    for(i= best_i; i < j; i++){               //for para inverter a subsequencia obtida anteriormente
      swap(s.sequence[i], s.sequence[j]);
      j--;
    }
    
    return true;
  }

  return false; 
}


bool BestImprovementOrOpt (Solution& s, vector<vector<Subsequence>> &subseqMatrix, int quantity){   //as 3 outras estruturas de vizinhança 

  double bestCost;
  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  bestCost= subseqMatrix[0][vertices].custoAcumulado;


  switch(quantity){
    
    case 1:             //método: REINSERTION

      for(i= 1; i < vertices-1; i++) {

        for (j= i + 1; j < vertices-1; j++){

          if(j== i+1){

            partialCost= subseqMatrix[0][i-1].custoAcumulado + ( (j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]]) ) + matrizAdj[j][j+1];
                                         
     	      secondCost= subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]] + matrizAdj[i][j+1] + matrizAdj[j][i];

            cost = partialCost + ( (vertices-j) * secondCost ) + subseqMatrix[j+1][vertices].custoAcumulado;

          }
          
          partialCost= subseqMatrix[0][i-1].custoAcumulado + ( (j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j-1]]) ) + matrizAdj[j][i+1];
                                         
          secondCost= subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j-1]] + matrizAdj[i+1][j-1] + matrizAdj[j][i] ;

          cost = partialCost + ( (vertices-j) * (secondCost + matrizAdj[s.sequence[i]][s.sequence[j+1]] ) + subseqMatrix[j+1][vertices].custoAcumulado);

          if(cost < bestCost){
            bestCost= cost;
            best_i= i;
            best_j= j;
          }
        }
      }

      if (bestCost < 0){
        s.sequence.insert(s.sequence.begin() + best_j + 1, s.sequence[best_i]);
        s.sequence.erase(s.sequence.begin() + best_i);
        // s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      return false;

    case 2:          //método: OR-OPT-2

      for(i= 1; i < vertices- 2; i++) {

        for (j= i + 2; j < vertices- 1; j++){
          
          partialCost = subseqMatrix[0][i-1].custoAcumulado + ( ( j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j-1]]))
                        + subseqMatrix[j][j-1].custoAcumulado + matrizAdj[i][i+1];


          secondCost = subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j-1]] + subseqMatrix[j][j-1].tempoTotal;


          cost = partialCost + ((vertices-j) * (secondCost + (2 * matrizAdj[s.sequence[i]][s.sequence[j]]) ) + matrizAdj[j-1][j] + matrizAdj[i][i+1]) + matrizAdj[i+1][j+1] + subseqMatrix[j+1][vertices].custoAcumulado;


            if(cost < bestCost){
              bestCost= cost;
              best_i= i;
              best_j= j;
            }
        }
      }

      if (bestCost < subseqMatrix[0][vertices].custoAcumulado){
        s.sequence.insert(s.sequence.begin() + best_j + 2, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i+1]);
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        // s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      return false;

    case 3:      //método: OR-OPT-3

      for(i= 1; i < vertices - 3; i++) {

        partialCost= - matrizAdj[s.sequence[i-1]][s.sequence[i]] - matrizAdj[s.sequence[i+2]][s.sequence[i+3]] + matrizAdj[s.sequence[i-1]][s.sequence[i+3]];

        for (j= i + 3; j < vertices - 2; j++){
          
          partialCost=  subseqMatrix[0][i-1].custoAcumulado + ( (j-i+1) * (subseqMatrix[0][i-1].tempoTotal + matrizAdj[s.sequence[i-1]][s.sequence[j]]) ) + 	matrizAdj[j][i] + subseqMatrix[i][i+1].custoAcumulado;


          secondCost= subseqMatrix[0][i-1].tempoTotal + ( 2 * matrizAdj[s.sequence[i-1]][s.sequence[j]] ) + matrizAdj[i][j] + subseqMatrix[i][j-1].tempoTotal;

          cost = partialCost + ( (vertices-j) * (secondCost + matrizAdj[s.sequence[j-1]]	[s.sequence[j+1]] ) + subseqMatrix[j+1][vertices].custoAcumulado);

      

          if(cost < bestCost){
            bestCost= cost;
            best_i= i;
            best_j= j;
          }
        }
        
      }

      if (bestCost < subseqMatrix[0][vertices].custoAcumulado){
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 4, s.sequence[best_i+1]);
        s.sequence.insert(s.sequence.begin() + best_j + 5, s.sequence[best_i+2]);
        s.sequence.erase(s.sequence.begin() + (best_i + 2));
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        // s.custoSolucao= s.custoSolucao + bestDelta;
        
        return true;
      }
      
      return false;  
  }
}

void BuscaLocal (Solution& s, vector<vector<Subsequence>> &subseqMatrix){

  vector<int>NL= {1, 2, 3, 4, 5};
  bool improved= false;
  
  while(!NL.empty()){
    
    int n= rand() % NL.size();

    switch (NL[n]) {
      case 1: 
        improved= BestImprovementSwap(s, subseqMatrix);    
        break;
      case 2: 
        improved= BestImprovement2Opt(s, subseqMatrix);  
        break;
      case 3:
        improved= BestImprovementOrOpt(s, subseqMatrix, 1);   //reinsertion    
        break;
      case 4:
        improved= BestImprovementOrOpt(s, subseqMatrix, 2);   //Or-opt2      
        break;
      case 5:
        improved= BestImprovementOrOpt(s, subseqMatrix, 3);   //Or-opt3      
        break;
    }
    

    if(improved){
      NL={1, 2, 3, 4, 5};

    }else{
      NL.erase(NL.begin() + n);
    }
    
  } 

 }


int main(int argc, char** argv) {

    clock_t start= clock();    //inicia a contagem do tempo de execucao  

    Solution s, bestS, bestOfAll;
    double sCustoAcum, bestCustoAcum;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    

    readData(argc, argv, &vertices, &matrizAdj);                                                      

    
    vector<vector<Subsequence>> subseqMatrix(vertices, vector<Subsequence>(vertices)); 


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

      bestCustoAcum= subseqMatrix[s.sequence[0]][s.sequence[vertices-1]].custoAcumulado;
      bestS= s;

      count= 0;
      
      cout << "construcao" << endl; 
      while(count < maxIterIls){

        cout << "antes busca" << endl; 
        BuscaLocal(s, subseqMatrix);
        
        cout << "depois busca" << endl; 
        sCustoAcum= subseqMatrix[s.sequence[0]][s.sequence[vertices-1]].custoAcumulado;

        if(sCustoAcum < bestCustoAcum){
          bestS= s;
          count= 0;
        }
        // for(j= 0; j < s.sequence.size(); j++){
        //   cout << s.sequence[j] << " ";
        // }
        // cout << endl;
        //   s= Pertubacao(bestS);  
         
        // for(j= 0; j < s.sequence.size(); j++){
        //   cout << s.sequence[j] << " ";
        // }
        // cout << s.custoSolucao << endl;
        
        count++;
      }
    
   }
}