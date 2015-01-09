
#include "seqHMM.h"
using namespace Rcpp;

// Below is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar)

// For more on using Rcpp click the Help button on the editor toolbar
// install_github( "Rcpp11/attributes" ) ; require('attributes') 

// [[Rcpp::depends(RcppArmadillo)]]
// [[Rcpp::export]]

double logLikHMMx(NumericMatrix transitionMatrix, NumericMatrix emissionArray, 
IntegerMatrix obsArray, NumericMatrix coef_, NumericMatrix X_) {  
  
  IntegerVector eDims = emissionArray.attr("dim"); //m,p
  IntegerVector oDims = obsArray.attr("dim"); //k,n  
  
  arma::mat transition(transitionMatrix.begin(),eDims[0],eDims[0]);
  arma::mat emission(emissionArray.begin(), eDims[0], eDims[1]);
  arma::Mat<int> obs(obsArray.begin(), oDims[0], oDims[1]);
  
  int q = coef_.nrow();
  arma::mat coef(coef_.begin(),q,eDims[0]);
  arma::mat X(X_.begin(),oDims[0],q);
  
  arma::mat init = exp(X*coef).t(); 
  init.each_row() /= sum(init,0);
  init = log(init); 
  
  double tmp;
  arma::vec alpha(eDims[0]); //m,n,k
  arma::vec alphatmp(eDims[0]); //m,n,k
  
  double ll=0.0;
  
  transition = log(transition); 
  emission = log(emission); 
  
  
  double sumtmp;  
  double neginf = -arma::math::inf();  
  
  for(int k = 0; k < oDims[0]; k++){    
    
    alpha = init.col(k)+emission.col(obs(k,0));
    
    for(int t = 1; t < oDims[1]; t++){  
      for(int i = 0; i < eDims[0]; i++){
        sumtmp = neginf;
        for(int j = 0; j < eDims[0]; j++){
          tmp = alpha(j) + transition(j,i);
          if(tmp > neginf){
            sumtmp = logSumExp(sumtmp,tmp);
          }
        }        
        alphatmp(i) = sumtmp + emission(i,obs(k,t));
      }
      alpha = alphatmp;
    }
    
    tmp = neginf;
    for(int i = 0; i < eDims[0]; i++){
      if(alpha(i)>neginf){
        tmp = logSumExp(alpha(i),tmp); 
      }
    }
    ll += tmp;
  }
  
  return ll;
}

