
#ifndef _TauDileptonPDFBuilderFitter_hh
#define _TauDileptonPDFBuilderFitter_hh

/**                                                                                                                                                                              
																						 \class    likelihoodFitter::tauDileptonPDFBuilderFitter tauDileptonPDFBuilderFitter.cc "UserCode/LIP/TopTaus/tauDileptonPDFBuilderFitter.cc"                                                                     
  \brief    executable for performing multivariable likelihood fit in order to improve estimation of N_fakes
  
  \author   Pietro Vischia

  \version  $Id: TauDileptonPDFBuilderFitter.hh,v 1.6 2012/09/18 17:12:33 vischia Exp $                                                                                                       
*/

#if !defined(__CINT__) || defined(__MAKECINT__)

#ifndef __CINT__
#include "RooGlobalFunc.h"
#endif

// System headers
#include <string>
#include <fstream>

// RooFit headers
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooDataHist.h"
#include "RooHistPdf.h"
#include "RooKeysPdf.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooProdPdf.h"
#include "RooFitResult.h"
#include "RooPlot.h"
#include "RooNLLVar.h"
#include "RooAddition.h"
//#include "RooMinuit.h"


// ROOT headers
#include "TString.h"
#include "TStyle.h"
#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TLegend.h"

#include "LIP/TopTaus/interface/FitVar.hh"

#endif


class TauDileptonPDFBuilderFitter{
  
public :
  typedef enum {SM2BKG=0, SM3BKG, HIGGS2BKG, HIGGS3BKG, N_FITTYPES} FITTYPES;
  
  // Constructor
  TauDileptonPDFBuilderFitter(string);
  // Destructor
  ~TauDileptonPDFBuilderFitter();
  
  /**
     @short performs the variable fit to a given set of variables
  */
  void DoFit();
  
private:
  void Init();
  void SetOptions();
  void InitFitSettings(size_t);
  void InitPerVariableAmbient(size_t);
  void BuildDatasets(size_t);
  void BuildPDFs(size_t);
  void DrawTemplates(size_t);
  void BuildConstrainedModels(size_t);
  void DoPerVariableFit(size_t);
  void DrawPerVariableFit(size_t);
  void DoPerVariableLikelihoodFit(size_t);
  void DoCombinedLikelihoodFit();
  // Data members
  
  // Parameter set
  string parSet_;

  // Output paths
  string outFolder_;
  string resultsFileName_;
  ofstream resultsFile_;
  streambuf* streamToFile_;

  // Style
  TStyle* myStyle_;

  // Fit settings
  vector<FITTYPES> fitType_;
  bool includeSignal_;
  bool standaloneTTbar_;
  string baseIdentifier_;
  double signalStatError_;
  double ddbkgEstimate_;
  double ddbkgStatError_;
  double ttbarmcbkgStatError_;
  double mcbkgStatError_;

  // Input paths
  TString   baseMCDir_;
  TString   baseDataDir_;

  TString   signalFileNameWH_;
  TString   signalFileNameHH_;
  TString   dataFileName_;
  TString   ddBkgFileName_;
  TString   ttbarmcBkgFileName_;
  TString   mcBkgFileName_;

  TString minitreeSelected_;
  TString minitreeDataDriven_;
  
  // Input files
  TFile * signalFileWH_;
  TFile * signalFileHH_;
  TFile * ddBkgFile_;
  TFile * ttbarmcBkgFile_;
  TFile * mcBkgFile_;
  TFile * dataFile_;

  // Input trees
  TTree* signalTreeWH_;
  TTree* signalTreeHH_; 
  TTree* ddBkgTree_ ;
  TTree* ttbarmcBkgTree_;
  TTree* mcBkgTree_;
  TTree* dataTree_;
  
  // Variables parameters
  size_t nVars_;
  vector<FitVar> fitVars_;  
  vector<string> vars_;
  vector<double> mins_;
  vector<double> maxs_;
  vector<int> bins_;
  vector<double> hmin_;
  vector<double> hmax_;
  vector<Int_t> unbinned_;
  vector<Int_t> smoothOrder_;

  TCanvas* canvas_;

  // RooFit stuff
  RooRealVar* sigVar_             ;              
  RooRealVar* sigMeanVar_         ;              
  RooRealVar* sigSigmaVar_        ;              
  RooRealVar* ttbarmcbkgVar_      ;
  RooRealVar* ttbarmcbkgMeanVar_  ;
  RooRealVar* ttbarmcbkgSigmaVar_ ;
  RooRealVar* mcbkgVar_           ;               
  RooRealVar* mcbkgMeanVar_       ;         
  RooRealVar* mcbkgSigmaVar_      ;         
  RooRealVar* ddbkgVar_           ;              
  RooRealVar* ddbkgMeanVar_       ;         
  RooRealVar* ddbkgSigmaVar_      ;        

  // Per-var stuff
  string identifier_;
  
  RooRealVar* myvar_         ;
  RooRealVar* myvar_weights_ ;
  RooRealVar* isOSvar_       ;
  
  string mySignalDSName_     ;
  string myDDBkgDSName_      ;
  string myTTBARMCBkgDSName_ ;
  string myMCBkgDSName_      ;
  string myDataDSName_       ;

  string signalModelName_           ;
  string signalConstrainedName_     ;
  string signalConstraintName_      ;
  string ddbkgModelName_            ;
  string ddbkgConstrainedName_      ;
  string ddbkgConstraintName_       ;
  string ttbarmcbkgModelName_       ;
  string ttbarmcbkgConstrainedName_ ;
  string ttbarmcbkgConstraintName_  ;
  string mcbkgModelName_            ;
  string mcbkgConstrainedName_      ;
  string mcbkgConstraintName_       ;

  string signalVarName_     ;
  string ddbkgVarName_      ;
  string ttbarmcbkgVarName_ ;
  string mcbkgVarName_      ;
  
  RooDataSet* mySignalDS_      ;
  RooDataSet* unrMyDDBkgDS_      ; // Unreduced datasets for OS cut
  RooDataSet* unrMyTTBARMCBkgDS_ ;
  RooDataSet* unrMyMCBkgDS_      ; 
  RooDataSet* unrMyDataDS_       ; 
  RooDataSet* myDDBkgDS_      ; // Reduced datasets
  RooDataSet* myTTBARMCBkgDS_ ;
  RooDataSet* myMCBkgDS_      ; 
  RooDataSet* myDataDS_       ; 

  RooDataHist* signalHisto_    ;
  RooDataHist* ttbarmcbkgHisto_;
  RooDataHist* mcbkgHisto_     ;
  RooDataHist* ddbkgHisto_     ;
  RooDataHist* dataHisto_      ;

  // Binned
  RooHistPdf* b_signalModel_    ;
  RooHistPdf* b_ddbkgModel_     ;
  RooHistPdf* b_ttbarmcbkgModel_;  
  RooHistPdf* b_mcbkgModel_     ;  
  
  
  // Unbinned	       
  RooKeysPdf* u_signalModel_    ; 
  RooKeysPdf* u_ddbkgModel_     ;  
  RooKeysPdf* u_ttbarmcbkgModel_;  
  RooKeysPdf* u_mcbkgModel_     ;  

  TH1* signalHist_;
  TH1* ddbkgHist_;
  TH1* ttbarmcbkgHist_;
  TH1* mcbkgHist_;
  TLegend* leg_;
  
  double sumWeights_;

  RooGaussian* signalConstraint_;
  RooGaussian* ttbarmcbkgConstraint_;
  RooGaussian* ddbkgConstraint_;
  RooGaussian* mcbkgConstraint_;

  RooAddPdf* sumModel_;
  RooProdPdf* model_;
  RooFitResult* constrainedModelFit_;
  
  RooPlot* myFrame_;

  RooNLLVar* nll_;
  vector<RooNLLVar* > likelihoodVector_;
  
  RooFitResult* myNllFitResult_;
  RooPlot* contourPlot_;
  
  RooAddition* combll_;
};

#endif //_TauDileptonPDFBuilderFitter_hh


