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

struct Subsequence{
  double T, C;
  int W;
  int first, last;

  inline static Subsequence Concatenate(Subsequence &sigma1, Subsequence &sigma2){
    
    Subsequence sigma;

    double temp= t[sigma1.last][sigma2.first];

    sigma.W= sigma1.W + sigma2.W;
    sigma.T= sigma1.T + sigma2.T;
    sigma.C= sigma1.C + sigma2.W * (sigma1.T + temp) + sigma2.C;
    sigma.first= sigma1.first;
    sigma.last= sigma2.last;

    return sigma;
  }

};

void UpdateAllSubseq(Solution *s, vector<vector<Subsequence>> &subseqMatrix){

    int n= s->sequence.size();

    for(int i= 0; i < n; i++){

      int v = s->sequence[i];
      subseq_matrix[i][i].W = (i > 0);
      subseq_matrix[i][i].C = 0;
      subseq_matrix[i][i].T = 0;
      subseq_matrix[i][i].first = s->sequence[i];
      subseq_matrix[i][i].last = s->sequence[i];
    }

    for (int i = 0; i < n; i++){

      for (int j = i + 1; j < n; j++){
        subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j]);
      }
    
    }
    
  for(int i= n-1; i >= 0; i--){

    for(int j= i-1; j >= 0; j--){
      subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j]);
    }
  }
}



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
