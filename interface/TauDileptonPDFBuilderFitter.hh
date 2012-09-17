
#ifndef _TauDileptonPDFBuilderFitter_hh
#define _TauDileptonPDFBuilderFitter_hh

/**                                                                                                                                                                              
																						 \class    likelihoodFitter::tauDileptonPDFBuilderFitter tauDileptonPDFBuilderFitter.cc "UserCode/LIP/TopTaus/tauDileptonPDFBuilderFitter.cc"                                                                     
  \brief    executable for performing multivariable likelihood fit in order to improve estimation of N_fakes
  
  \author   Pietro Vischia

  \version  $Id: TauDileptonPDFBuilderFitter.hh,v 1.2 2012/09/14 14:42:04 vischia Exp $                                                                                                       
*/

#if !defined(__CINT__) || defined(__MAKECINT__)

#ifndef __CINT__
// #include "RooGlobalFunc.h"
#endif

// System headers

// RooFit headers
//#include "RooRealVar.h"
//#include "RooMinuit.h"
//#include "RooDataSet.h"
//#include "RooDataHist.h"

// ROOT headers
#include "TString.h"
#include "TTree.h"
//#include "TCanvas.h"
//#include "TH1F.h"

#endif

// perhaps not needed anymore
#define NVARS 6


class FitVar{
  
public:
  FitVar(string, double, double, double, double, double, Int_t, Int_t);
  string getVarName();
  double getMin();
  double getMax();
  int getBins();
  double getHmin();
  double getHmax();
  Int_t getUnbinned();
  Int_t getSmoothOrder();
private:
  string varName_;
  double min_;
  double max_;
  double bins_;
  double hmin_;
  double hmax_;
  Int_t unbinned_;
  Int_t smoothOrder_;
};

class TauDileptonPDFBuilderFitter{
  
public :
  enum FITTYPES {SM2BKG=0, SM3BKG, HIGGS2BKG, HIGGS3BKG};
  
  // Constructor
  TauDileptonPDFBuilderFitter(string);
  // Destructor
  ~TauDileptonPDFBuilderFitter();
  
  /**
     @short performs the variable fit to a given set of variables
  */
  void buildGenericMultiPDFModel(vector<string> & distVector,
				 vector<double> & minVector,
				 vector<double> & maxVector,
				 vector<double> & numbBinsVector,
				 vector<double> & hminVector,
				 vector<double> & hmaxVector,
				 vector<Int_t> & unbinned,
				 vector<Int_t> & smoothOrder,
				 unsigned int & fitType,
				 );
    
private:
  void Init();
  void SetOptions();
  void SetFitSettings();

  // Data members
  string parSet_;

  string outFolder_;
  string resultsFileName_;
  
  ofstream resultsFile_;
  bool includeSignal_;
  bool standaloneTTbar_;
  string baseIdentifier_;
  double signalStatError_;
  double ddbkgEstimate_;
  double ddbkgStatError_;
  double ttbarmcbkgStatError_;
  double mcbkgStatError_;

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
  
  TTree* signalTreeWH_;
  TTree* signalTreeHH_; 
  TTree* ddBkgTree_ ;
  TTree* ttbarmcBkgTree_;
  TTree* mcBkgTree_;
  TTree* dataTree_;
  
  TFile * signalFile;
  TFile * signalFileWH;
  TFile * signalFileHH;
  TFile * ddBkgFile;
  TFile * ttbarmcBkgFile;
  TFile * mcBkgFile;
  TFile * dataFile;

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
};

#endif //_TauDileptonPDFBuilderFitter_hh


