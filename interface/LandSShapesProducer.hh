
#ifndef _LandSShapesProducer_hh
#define _LandSShapesProducer_hh

/**                                                                                                                                                                              
  \class    LandSShapesProducer LandSShapesProducer.cc "UserCode/LIP/TopTaus/interface/LandSShapesProducer.hh"                                                                     
  \brief    executable for performing multivariable likelihood fit in order to improve estimation of N_fakes
  
  \author   Pietro Vischia

  \version  $Id: LandSShapesProducer.hh,v 1.8 2012/11/07 16:27:19 vischia Exp $                                                                                                       

  TODO List:
  - generalize to array of samples, simplifying the approach and maintainability
  - de-zero the zeroed bins (Pedro suggestion: essentially RooFit (->combine tool) does not like zero bins -> set them to 1E-06
  - multidimensional shapes
  - MVA shapes (creating an external reweighter and not this code)
            

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
#endif

#include "LIP/TopTaus/interface/Utilities.hh"

#include "LIP/TopTaus/interface/FitVar.hh"

class LandSShapesProducer: protected utilities::EditorialUtils, utilities::StatUtils {
  
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

  void StorePerMassSignalShapes();
  void DrawSignalShapesComparison();

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
  bool produceOnly_;
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

  string signalFancySampleNameWH_;
  string dataFancySampleName_;
  string ddBkgFancySampleName_;
  vector<string> mcBkgFancySampleName_;

  Int_t signalSampleColor_;
  Int_t ddBkgSampleColor_;
  vector<Int_t> mcBkgSampleColor_;

  TString minitreeSelected_;
  TString minitreeDataDriven_;

  vector<TString> systComponents_;
  vector<TString> systFancyComponents_;
  
  // Input files
  TFile* signalFileWH_;
  TFile* signalFileHH_;
  TFile* ddBkgFile_;
  vector<TFile*> mcBkgFile_;
  TFile* dataFile_;

  double osCutEff_;

  // Input base trees
  TTree* signalTreeWH_;
  TTree* signalTreeHH_; 
  TTree* ddBkgTree_;
  vector<TTree*> mcBkgTree_;
  TTree* dataTree_;

  // Input syst trees
  vector<TTree*> signalSystTreeWH_;
  vector<TTree*> signalSystTreeHH_; 
  vector<TTree*> ddBkgSystTree_;
  vector<vector<TTree*> > mcBkgSystTree_;

  vector<string> uncSources_;
  size_t currentMassPoint_;
  size_t nMassPoints_;
  // Variables parameters
  size_t nVars_;
  size_t nMcSamples_;
  size_t nSysts_;

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

  vector<string> mySignalSystDSNameWH_     ;
  vector<string> mySignalSystDSNameHH_     ;
  vector<string> myDDBkgSystDSName_      ;
  vector<vector<string> > myMCBkgSystDSName_ ;

  string signalVarNameWH_     ;
  string signalVarNameHH_     ;
  vector<string> ddbkgVarName_      ;
  string mcbkgVarName_ ;
  
  RooDataHist* signalHistoWH_    ;
  RooDataHist* signalHistoHH_    ;
  RooDataHist* ddbkgHisto_     ;
  vector<RooDataHist*> mcbkgHisto_     ;
  RooDataHist* dataHisto_      ;

  vector<RooDataHist*> signalSystHistoWH_    ;
  vector<RooDataHist*> signalSystHistoHH_    ;
  vector<RooDataHist*> ddbkgSystHisto_     ;
  vector<vector<RooDataHist*> > mcbkgSystHisto_     ;
 
  
  RooDataSet* mySignalDSWH_      ;
  RooDataSet* mySignalDSHH_      ;
  RooDataSet* myDDBkgDS_         ; // Reduced datasets
  vector<RooDataSet*> myMCBkgDS_         ; 
  RooDataSet* myDataDS_          ; 

  vector<RooDataSet*> mySignalSystDSWH_      ;
  vector<RooDataSet*> mySignalSystDSHH_      ;
  vector<RooDataSet*> myDDBkgSystDS_         ; // Reduced datasets
  vector<vector<RooDataSet*> >myMCBkgSystDS_         ; 


  vector<vector<TH1*> > signalShapesToCompare_;
  vector<vector<TH1*> > signalShapesToCompareHH_;
  vector<vector<TH1*> > signalShapesToCompareWH_;

  vector<TH1*> perMassPointSignalShapesToCompare_;
  vector<TH1*> perMassPointSignalShapesToCompareHH_;
  vector<TH1*> perMassPointSignalShapesToCompareWH_;

  TH1* signalHistWH_;
  TH1* signalHistHH_;
  TH1* ddbkgHist_;
  TH1* ddbkgHistUp_;  // These are for showing the error bands. Not for LandS
  TH1* ddbkgHistDown_;// These are for showing the error bands. Not for LandS
  vector<TH1*> mcbkgHist_;
  TH1* dataHist_;

  TH1* signalHistWHStatUp_;
  TH1* signalHistHHStatUp_;
  TH1* ddbkgHistStatUp_;
  vector<TH1*> mcbkgHistStatUp_;
  TH1* dataHistStatUp_;

  TH1* signalHistWHStatDown_;
  TH1* signalHistHHStatDown_;
  TH1* ddbkgHistStatDown_;
  vector<TH1*> mcbkgHistStatDown_;
  TH1* dataHistStatDown_;

  
  vector<TH1*> signalSystHistWH_;
  vector<TH1*> signalSystHistHH_;
  vector<TH1*> ddbkgSystHist_;
  vector<vector<TH1*> > mcbkgSystHist_;

  TLegend* leg_;
  
  double sumWeights_;

};

#endif //_TauDileptonPDFBuilderFitter_hh


