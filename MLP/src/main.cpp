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
  double custo;
};

bool compares(InsertionInfo a, InsertionInfo b){  //funcao auxiliar na ordenacao dos custos
  return a.custo < b.custo;
}


struct Subsequence{
  
  double t, c;
  int w;
  int first, last;

  inline static Subsequence Concatenate(Subsequence &sigma1, Subsequence &sigma2){

    Subsequence sigma;

    double temp = matrizAdj[sigma1.last][sigma2.first];
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


vector<InsertionInfo> calcularCustoInsercao (int r, std::vector<int>& CL){  //calcular o custo da insercao de cada vertice para Construcao

  std::vector<InsertionInfo> custoInsercao (CL.size());

  int a, b, i, k;
  int count= 0;
  int l= 0;
  

  for(int a= 0; a < CL.size(); a++){
    
    custoInsercao[a].custo= matrizAdj[r][CL[a]]; 
    custoInsercao[a].noInserido= CL[a];
  }
  
  return custoInsercao;
}


Solution Construction (vector<int> CL){  //gerando uma solucao inicial

  Solution s;
  int indexRandom, i, j;  
  
  s.sequence.push_back(1);

  int r= 1;            //porque a primeira cidade eh a 1
  int selecionado;

  std::vector<InsertionInfo> custoInsercao;

  while(!CL.empty()){

    custoInsercao= calcularCustoInsercao(r, CL);  //calculando os custos de cada insercao

    sort(custoInsercao.begin(), custoInsercao.end(), compares);    //ordenando os custos
    
    double alpha= (double) rand() / RAND_MAX;
    int selecionado= rand() % ( (int) ceil(alpha * custoInsercao.size()) );       //selecionando um dos candidatos de melhor custo
    

    r= custoInsercao[selecionado].noInserido;

    j= 0;
    while(1){          
      if(r == CL[j]){
        CL.erase(CL.begin()+j);   //retirando a cidade inserida da lista de candidatos
        break;
      }

      j++;
    }

    s.sequence.push_back(r);

  }
   s.sequence.push_back(1);      //adicionando a c1 no final

  return s;
}



bool BestImprovementSwap (Solution& s, vector<vector<Subsequence>> &subseq_matrix){         //estrutura de vizinhança: SWAP ja alterada


  int best_i, best_j;
  int i, j;
  double partialCost, secondCost, cost;

  Subsequence sigma1, sigma2, sigma3, sigma4, sigma5;

  
  double bestCost= subseq_matrix[0][vertices].c;

  for(i= 1; i < vertices - 1; i++) {           //
 
    for (j= i + 1; j < vertices-1; j++){           

      if(i == j-1){
        sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j]);
        sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i]);
        sigma4= Subsequence::Concatenate(sigma2, subseq_matrix[j+1][vertices]);

      
      }else{

        sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j]);
        sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i+1][j-1]);
        sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[i][i]);
        sigma4= Subsequence::Concatenate(sigma3, subseq_matrix[j+1][vertices]);
      }

      if(sigma4.c < bestCost){
        bestCost= sigma4.c;
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
      sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[j+1][vertices]);


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

inline void reinsert(std::vector<int> &vec, int i, int j, int pos){
    std::vector<int> seq (vec.begin() + i, vec.begin() +j+1);
    if(pos < i){
        vec.erase(vec.begin() + i, vec.begin() + j+1);
        vec.insert(vec.begin() + pos, seq.begin(), seq.end());
    }else{
        vec.insert(vec.begin() + pos, seq.begin(), seq.end());
        vec.erase(vec.begin() + i, vec.begin() + j+1);
    }

}


bool BestImprovementOrOpt (Solution& s, vector<vector<Subsequence>> &subseq_matrix, int quantity){   //as 3 outras estruturas de vizinhança 

  double bestCost;
  int best_i, best_j;
  int i, j;
  double partialCost, secondCost;

  Subsequence sigma1, sigma2, sigma3;

  int n= quantity-1; 

  bestCost= subseq_matrix[0][vertices].c;


  // for(i= 1; i < vertices-quantity; i++) {

  //     for (j= 1; j < vertices-1; j++){    

  //       if(j < i){
  //         sigma1= Subsequence::Concatenate(subseq_matrix[0][j-1], subseq_matrix[i][i+n]);
  //         sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[j][i-1]);
  //         sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[i+n+1][s.sequence.size()-1]);
        
  //       }else if(j > i+n+1){

  //         sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+n+1][j-1]);
  //         sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i+n]);
  //         sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[j][s.sequence.size()-1]);
        
  //       }else{
  //         continue;
  //       }

  //       if(sigma3.c < bestCost){

  //           bestCost= sigma3.c;
  //           best_i= i;
  //           best_j= j;
  //       }
  //     }
  // }


  // if (bestCost < subseq_matrix[0][vertices].c){

  //     reinsert(s.sequence, best_i, best_i+n, best_j);

  //       UpdateAllSubseq(s, subseq_matrix);

  //       if(bestCost != subseq_matrix[0][vertices].c){
  //         getchar();
  //       }
        
  //       return true;
  //     }
  //     return false;

  switch(quantity){
    
    case 1:             //método: REINSERTION

      for(i= 1; i < vertices-1; i++) {

       for (j= 1; j < vertices-1; j++){    

           if(j > i){
            
             sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+1][j]);
             sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[i][i]);
             sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[j+1][s.sequence.size()-1]);


           }else if(j < i){
             
             sigma1= Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i][i]);
             sigma2= Subsequence::Concatenate(sigma1, subseq_matrix[j][i-1]);
             sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[i+1][s.sequence.size()-1]);
             

            }else{
              continue;
            }

           if(sigma3.c < bestCost){

             bestCost= sigma3.c;
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
             sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[j+1][s.sequence.size()-1]);

            if(sigma3.c < bestCost){
              bestCost= sigma3.c;
              best_i= i;
              best_j= j;
            }
        }                                             
      }

      if (bestCost < subseq_matrix[0][vertices].c){

        s.sequence.insert(s.sequence.begin() + best_j + 1, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 2, s.sequence[best_i+1]);
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
          sigma3= Subsequence::Concatenate(sigma2, subseq_matrix[j+1][s.sequence.size()-1]);
      

          if(sigma3.c < bestCost){
            bestCost= sigma3.c;
            best_i= i;
            best_j= j;
          }

        }
      }
        

      if (bestCost < subseq_matrix[0][vertices].c){

        s.sequence.insert(s.sequence.begin() + best_j + 1, s.sequence[best_i]);
        s.sequence.insert(s.sequence.begin() + best_j + 2, s.sequence[best_i+1]);
        s.sequence.insert(s.sequence.begin() + best_j + 3, s.sequence[best_i+2]);
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
  
  int i;
  
  while(!NL.empty()){
    
    int n= rand() % NL.size();

    bool improved= false;

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
        } /// 

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
    double sCustoAcum, bestCustoAcum, bestOfAllCost;
    int maxIter, maxIterIls;
    int i, count, j;
    vector<int> CL;    
    double custo= 0;

    readData(argc, argv, &vertices, &matrizAdj);                                                      


    if(vertices < 100){
      maxIterIls= vertices;
    }else{
      maxIterIls= 100;
    }

    srand(time(NULL));

    for(i= 2; i <= vertices; i++){ 
      CL.push_back(i);
    }

    
   bestOfAllCost= (double) INFINITY;
   maxIter= 10; // 10 iteracoes

   for(i= 0; i < maxIter; i++){
      
      s.sequence.clear();

      vector<vector<Subsequence>> subseq_matrix(vertices+1, vector<Subsequence>(vertices+1)); 

      s= Construction(CL);

      UpdateAllSubseq(s, subseq_matrix);
      

      bestCustoAcum= subseq_matrix[0][vertices].c;    //fica como melhor solução, a primeira gerada na construcao
      
      //cout << " na marra " << custo << endl;
      //cout << "update " << bestCustoAcum << endl;
      bestS= s;


      count= 0;
      while(count < maxIterIls){

        //cout << "entrou da busca" << endl; 

        BuscaLocal(s, subseq_matrix);
      
        //cout << "passou da busca" << endl; 

        if(subseq_matrix[0][vertices].c < bestCustoAcum){
          bestS= s;
          bestCustoAcum= subseq_matrix[0][vertices].c;   
          count= 0;
        }
        
        s= Pertubacao(bestS, subseq_matrix); 
        
        // cout << s.custoSolucao << endl;
        
        count++;
      }

      if(bestCustoAcum < bestOfAllCost){
        bestOfAll= bestS;
        bestOfAllCost= bestCustoAcum;
      }
    
   }
    
    // printData();

    // for(j= 0; j < s.sequence.size(); j++){
    //   cout << s.sequence[j] << " ";
    // }
    // cout << endl;


    cout << "COST " << bestOfAllCost << endl;
    
    clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "TIME " << time << endl;   //imprime o tempo total de execucao


    return 0;
}


// for(j= 0; j < s.sequence.size(); j++){
//   cout << s.sequence[j] << " ";
// }
// cout << endl;



//funcao pra calcular o custo das subseq:

// custo= 0;
// for(int j= 0; j < s.sequence.size()-1; j++){

//   custo += (s.sequence.size()-1 - j) * matrizAdj[s.sequence[j]][s.sequence[j+1]];

// }