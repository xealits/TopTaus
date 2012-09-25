
#ifndef _LandSShapesProducer_hh
#define _LandSShapesProducer_hh

/**                                                                                                                                                                              
  \class    LandSShapesProducer LandSShapesProducer.cc "UserCode/LIP/TopTaus/interface/LandSShapesProducer.hh"                                                                     
  \brief    executable for performing multivariable likelihood fit in order to improve estimation of N_fakes
  
  \author   Pietro Vischia

  \version  $Id: LandSShapesProducer.hh,v 1.6 2012/09/23 14:21:56 vischia Exp $                                                                                                       
*/


// FIXME: configure for all the different datacards 

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
#endif

#include "LIP/TopTaus/interface/FitVar.hh"

class LandSShapesProducer{
  
public :
  
  // Constructor
  LandSShapesProducer(string);
  // Destructor
  ~LandSShapesProducer();
  
  /**
     @short performs the variable fit to a given set of variables
  */
  void Produce();
  
private:
  void Init();
  void SetOptions();
  void InitMassPoint(size_t);
  void InitPerVariableAmbient(size_t);
  void BuildDatasets(size_t);
  //  void BuildPDFs(size_t);
  void DrawTemplates(size_t);
  // void BuildConstrainedModels(size_t);
  //  void DoPerVariableFit(size_t);
  //  void DrawPerVariableFit(size_t);
  //  void DoPerVariableLikelihoodFit(size_t);
  //  void DoCombinedLikelihoodFit();
  // Data members
  
  // Parameter set
  string parSet_;

  // Output paths
  string outFolder_;
  string outputFileName_;
  vector<string> outputFileNameSuffix_;
  string resultsFileName_; // not used. txt.
  ofstream resultsFile_;
  streambuf* streamToFile_;

  // Style
  TStyle* myStyle_;

  // Fit settings
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

  vector<string> signalFileNameWH_;
  vector<string> signalFileNameHH_;
  TString dataFileName_;
  TString ddBkgFileName_;
  vector<TString> mcBkgFileName_;

  string signalSampleNameWH_;
  string signalSampleNameHH_;
  string dataSampleName_;
  string ddBkgSampleName_;
  vector<string> mcBkgSampleName_;
  
  TString minitreeSelected_;
  TString minitreeDataDriven_;
  
  // Input files
  TFile* signalFileWH_;
  TFile* signalFileHH_;
  TFile* ddBkgFile_;
  vector<TFile*> mcBkgFile_;
  TFile* dataFile_;

  double osCutEff_;

  // Input trees
  TTree* signalTreeWH_;
  TTree* signalTreeHH_; 
  TTree* ddBkgTree_;
  vector<TTree*> mcBkgTree_;
  TTree* dataTree_;

  vector<string> uncSources_;
  size_t currentMassPoint_;
  size_t nMassPoints_;
  // Variables parameters
  size_t nVars_;
  size_t nMcSamples_;
  vector<FitVar*> fitVars_;  
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
  
  string mySignalDSNameWH_     ;
  string mySignalDSNameHH_     ;
  string myDDBkgDSName_      ;
  vector<string> myMCBkgDSName_ ;
  string myDataDSName_       ;

  string signalVarNameWH_     ;
  string signalVarNameHH_     ;
  vector<string> ddbkgVarName_      ;
  string mcbkgVarName_ ;
  
  RooDataHist* signalHistoWH_    ;
  RooDataHist* signalHistoHH_    ;
  RooDataHist* ddbkgHisto_     ;
  vector<RooDataHist*> mcbkgHisto_     ;
  RooDataHist* dataHisto_      ;
  
  RooDataSet* mySignalDSWH_      ;
  RooDataSet* mySignalDSHH_      ;
  RooDataSet* myDDBkgDS_         ; // Reduced datasets
  vector<RooDataSet*> myMCBkgDS_         ; 
  RooDataSet* myDataDS_          ; 

  TH1* signalHistWH_;
  TH1* signalHistHH_;
  TH1* ddbkgHist_;
  vector<TH1*> mcbkgHist_;
  TH1* dataHist_;
  

  TLegend* leg_;
  
  double sumWeights_;

};

#endif //_TauDileptonPDFBuilderFitter_hh


