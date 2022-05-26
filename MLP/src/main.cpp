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
  double costAcumulate;
};

struct InsertionInfo{
  int noInserido;
  int arestaRemovida;
  double custo;
};

// struct Subsequence{
//   double tempoTotal;
//   double custoAcumulado;
// };

bool compares(InsertionInfo a, InsertionInfo b){  //funcao auxiliar na ordenacao dos custos
  return a.custo < b.custo;
}


struct Subsequence{
  
  double t, c;
  int w;
  int first, last;

  inline static Subsequence Concatenate(Subsequence &sigma1, Subsequence &sigma2){

    Subsequence sigma;

    double temp= matrizAdj[sigma1.last][sigma2.first];
    sigma.w = sigma1.w + sigma2.w;
    sigma.t= sigma1.t + temp + sigma2.t;
    sigma.c = sigma1.c + sigma2.w * (sigma1.t + temp) + sigma2.c;
    sigma.first = sigma1.first; 
    sigma.last = sigma2.last;

    return sigma;
  }


};


void UpdateAllSubseq(Solution s, vector<vector<Subsequence>> &subseq_matrix){
  
  int n = s.sequence.size();


  for (int i = 0; i < n; i++){

    int v = s.sequence[i];
    
    subseq_matrix[i][i].w = (i > 0);
    subseq_matrix[i][i].c = 0;
    subseq_matrix[i][i].t = 0;
    subseq_matrix[i][i].first = s.sequence[i];
    subseq_matrix[i][i].last = s.sequence[i];

    
  }

  for (int i = 0; i < n; i++){
    for (int j = i + 1; j < n; j++){
      subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j]);

    
    }
  }
  
  
  for (int i = n - 1; i >= 0; i--){
    for (int j = i - 1; j >= 0; j--){
      subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j]);

     
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



bool BestImprovementSwap (Solution& s, vector<vector<Subsequence>> &subseq_matrix){         //estrutura de vizinhança: SWAP ja alterada


  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  Subsequence sigma1, sigma2, sigma3, sigma4, sigma5;

  
  double bestCost= subseq_matrix[0][vertices].c;

  for(i= 1; i < vertices - 1; i++) {

    for (j= i + 1; j < vertices-1; j++){

      if(i == j-1){
        sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j]);
        sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i]);
        sigma4= Subsequence::Concatenate(sigma1, subseq_matrix[i][j+1]);
      
      }else{

        sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j]);
        sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i+2][j-1]);
        sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[i][i]);
        sigma4= Subsequence::Concatenate(sigma3, subseq_matrix[j+1][vertices]);
      }

      if(sigma4.c < bestCost){
        bestCost= cost;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestCost < subseq_matrix[0][vertices].c){

    swap(s.sequence[best_i], s.sequence[best_j]);
    UpdateAllSubseq(s, subseq_matrix);

    return true;
  }

  return false;
}


bool BestImprovement2Opt (Solution& s, vector<vector<Subsequence>> &subseq_matrix){         //estrutura de vizinhança: 2opt    //alterada

  double bestCost;
  int best_i, best_j;
  int i, j;

  Subsequence sigma1, sigma2;


  bestCost= subseq_matrix[0][vertices].c;

  for(i= 1; i < vertices - 2; i++){

    for (j= i + 2; j < vertices-1; j++){
      
      sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i]);
      sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][s.sequence.size()-1]);


      if(sigma2.c < bestCost){
        bestCost= sigma2.c;
        best_i= i;
        best_j= j;
      }
    }
  }

  if (bestCost < subseq_matrix[0][vertices].c){

    j= best_j;
    for(i= best_i; i < j; i++){               //for para inverter a subsequencia obtida anteriormente
      swap(s.sequence[i], s.sequence[j]);
      j--;
    }

    UpdateAllSubseq(s, subseq_matrix);

    return true;
  }

  return false; 
}


bool BestImprovementOrOpt (Solution& s, vector<vector<Subsequence>> &subseq_matrix, int quantity){   //as 3 outras estruturas de vizinhança 

  double bestCost;
  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  Subsequence sigma1, sigma2, sigma3;

  bestCost= subseq_matrix[0][vertices].c;

  switch(quantity){
    
    case 1:             //método: REINSERTION

      for(i= 1; i < vertices-1; i++) {

        for (j= i + 1; j < vertices-1; j++){

          if(j== i+1){
            
            sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j]);
            sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i]);
            sigma3= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][s.sequence.size()-1]);
          }

            sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+1][j-1]);
            sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i]);
            sigma3= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][s.sequence.size()-1]);

          if(sigma3.c < bestCost){

            bestCost= cost;
            best_i= i;
            best_j= j;
          }
        }
      }

      if (bestCost < subseq_matrix[0][vertices].c){

        s.sequence.insert(s.sequence.begin() + best_j + 1, s.sequence[best_i]);
        s.sequence.erase(s.sequence.begin() + best_i);
        
        UpdateAllSubseq(s, subseq_matrix);
        
        return true;
      }
      return false;

    case 2:          //método: OR-OPT-2

      for(i= 1; i < vertices- 2; i++) {

        for (j= i + 2; j < vertices- 1; j++){
        
            sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+2][j]);
            sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i+1]);
            sigma3= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][s.sequence.size()-1]);

            if(sigma3.c < bestCost){
              bestCost= cost;
              best_i= i;
              best_j= j;
            }
        }
      }

      if (bestCost < subseq_matrix[0][vertices].c){

        s.sequence.insert(s.sequence.begin() + best_j + 2, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i+1]);
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        
        UpdateAllSubseq(s, subseq_matrix);    //fazer o update dos custos da nova solucao melhorada
        
        return true;
      }                                                                            
      return false;                                                               

    case 3:      //método: OR-OPT-3

      for(i= 1; i < vertices - 3; i++) {

        for (j= i + 3; j < vertices- 1; j++){

          sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+3][j]);
          sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i+2]);
          sigma3= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][s.sequence.size()-1]);
      

          if(sigma3.c < bestCost){
            bestCost= cost;
            best_i= i;
            best_j= j;
          }

        }
      }
        

      if (bestCost < 0){
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 4, s.sequence[best_i+1]);
        s.sequence.insert(s.sequence.begin() + best_j + 5, s.sequence[best_i+2]);
        s.sequence.erase(s.sequence.begin() + (best_i + 2));
        s.sequence.erase(s.sequence.begin() + (best_i + 1));
        s.sequence.erase(s.sequence.begin() + best_i);
        
        UpdateAllSubseq(s, subseq_matrix);
        
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
        improved= BestImprovementSwap(s, subseqMatrix);    //erro 
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

 Solution Pertubacao (Solution s, vector<vector<Subsequence>> &subseq_matrix){  

  int subseqMax;
  int subseq1, subseq2;
  int index1, index2;
  int i, times;
  bool changed= false;
  int large, small;

  if (vertices <= 29){
    subseq1= subseq2= 2;

    large= small= subseq1;
  }else{
     subseqMax= vertices / 10;
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
    index1= rand() % vertices + 1;

    if(index1 <= vertices-subseq2-subseq1){
        break;
    }
  }                                            
  while(1){
    index2= rand() % vertices + index1 + subseq1;      //pq o index2 tem q ser sorteado a partir do indice index1+subseq1;

    if(index2 <= vertices-subseq2){
      break;
    }
  }

  
  // if( (index1 + subseq1) == index2 ){
  //   s.custoSolucao= s.custoSolucao - matrizAdj[s.sequence[index1-1]][s.sequence[index1]] - matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2]]
  //                                  - matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index2+subseq2]]
  //                                  + matrizAdj[s.sequence[index1-1]][s.sequence[index2]] + matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index1]]
  //                                  + matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2+subseq2]];
  // }else{
  //   s.custoSolucao= s.custoSolucao - matrizAdj[s.sequence[index1-1]][s.sequence[index1]] - matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index1+subseq1]]
  //                                  - matrizAdj[s.sequence[index2-1]][s.sequence[index2]] - matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index2+subseq2]]
  //                                  + matrizAdj[s.sequence[index1-1]][s.sequence[index2]] + matrizAdj[s.sequence[index2+subseq2-1]][s.sequence[index1+subseq1]]
  //                                  + matrizAdj[s.sequence[index2-1]][s.sequence[index1]] + matrizAdj[s.sequence[index1+subseq1-1]][s.sequence[index2+subseq2]];
  // }

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

    UpdateAllSubseq(s, subseq_matrix);

  return s;
}


int main(int argc, char** argv) {

    clock_t start= clock();    //inicia a contagem do tempo de execucao  

    Solution s, bestS, bestOfAll;
    double sCustoAcum, bestCustoAcum;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    

    readData(argc, argv, &vertices, &matrizAdj);                                                      

    
    vector<vector<Subsequence>> subseq_matrix(vertices+1, vector<Subsequence>(vertices+1)); 


    if(vertices < 100){
      maxIterIls= vertices;
    }else{
      maxIterIls= 100;
    }
    srand(time(NULL));

    for(i= 2; i <= vertices; i++){ 
      CL.push_back(i);
    }

    

   maxIter= 10; // 10 iteracoes

   for(i= 0; i < maxIter; i++){
      
      s= Construction(s, CL);

      UpdateAllSubseq(s, subseq_matrix);

      bestCustoAcum= subseq_matrix[0][vertices].c;    //fica como melhor solução, a primeira gerada na construcao

      bestS= s;


      count= 0;
      while(count < maxIterIls){

        BuscaLocal(s, subseq_matrix);
      
        cout << "passou da busca" << endl; 

        if(subseq_matrix[0][vertices].c < bestCustoAcum){
          bestS= s;
          count= 0;
        }
        
        s= Pertubacao(bestS, subseq_matrix); 
        
        // cout << s.custoSolucao << endl;
        
        count++;
      }
    
   }
}


// for(j= 0; j < s.sequence.size(); j++){
//   cout << s.sequence[j] << " ";
// }
// cout << endl;