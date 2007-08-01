#include <BCModelEfficiency.h>
#include <BCLog.h> 
#include <BCH1D.h> 

#include <TCanvas.h> 
#include <TGraphErrors.h>
#include <TH1D.h> 
#include <TH2D.h> 
#include <TF1.h> 

#include "style.c"

// ---------------------------------------------------------
  
int main()
{
  // ---------------------------------------------------------
  // open log file 
  // ---------------------------------------------------------

  BCLog::OpenLog(); 

  // ---------------------------------------------------------
  // set personal root style
  // ---------------------------------------------------------

  SetStyle();

  // ---------------------------------------------------------
  // model definition 
  // ---------------------------------------------------------

  BCModelEfficiency* fModelEfficiency = new BCModelEfficiency("ModelEfficiency"); 

  // set integration parameters 

  //  fModelEfficiency -> SetNIterationsMax(1000000); 
  //  fModelEfficiency -> SetRelativePrecision(1e-3); 
  fModelEfficiency -> SetNiterationsPerDimension(500); 

  // ---------------------------------------------------------
  // read data from file 
  // ---------------------------------------------------------

  BCDataSet* fDataSet = new BCDataSet(); 

  if (fDataSet -> ReadDataFromFileTxt("./data/data.txt", 2) != 0)
    return -1; 

  // assign data set to model 

  fModelEfficiency -> SetDataSet(fDataSet); 

  // ---------------------------------------------------------
  // normalize  
  // ---------------------------------------------------------

  fModelEfficiency -> FindMode(); 

  // ---------------------------------------------------------
  // marginalize 
  // ---------------------------------------------------------

  BCH1D* bch1d_epsilon = fModelEfficiency -> MarginalizeProbability("epsilon"); 
  bch1d_epsilon -> Print("modelefficiency_epsilon.ps", 1); 
  BCH1D* bch1d_lambda    = fModelEfficiency -> MarginalizeProbability("lambda");
  bch1d_lambda -> Print("modelefficiency_lambda.ps", 1); 
  fModelEfficiency -> MarginalizeProbability("epsilon", "lambda") -> Print("modelefficiency_epsilon_lambda.ps", 2); 

  // ---------------------------------------------------------
  // summarize
  // ---------------------------------------------------------

  fModelEfficiency -> PrintSummary(); 

  // ---------------------------------------------------------
  // close log file 
  // ---------------------------------------------------------

  BCLog::CloseLog(); 

  return 0; 

}

// ---------------------------------------------------------
  
