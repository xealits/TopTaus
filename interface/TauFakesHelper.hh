#ifndef _TauFakesHelper_hh
#define _TauFakesHelper_hh
/**   
      \class	    TauFakesHelper TauFakesHelper.hh "UserCode/LIP/TopTaus/interface/TauFakesHelper.hh"                                                                     
      \brief    Class for managing the operations needed in order to estimate tauFakes :)
      
      \author   Pietro Vischia
      
      \version  $Id: TauFakesHelper.hh,v 1.2 2013/04/18 12:51:36 vischia Exp $                                                                                                       
*/


// System headers
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <string>
#include <iostream>
#include <iomanip>
#endif

// ROOT headers
#include "TRandom2.h"
#include "TMatrixD.h"
#include "TF1.h"
#include "TProfile.h"
#include "TObjArray.h"
#include "TMatrixD.h"
#include "TH1.h"
#include "TTimeStamp.h"


// User headers
#include "LIP/TopTaus/interface/PlotStyle.hh"
#include "LIP/TopTaus/interface/SelectionMonitor.hh"
#include "LIP/TopTaus/interface/MiniEvent.hh"
#include "LIP/TopTaus/interface/ObjectSelector.hh"
  
#include "CondFormats/JetMETObjects/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

#include "PhysicsTools/Utilities/interface/LumiReweightingStandAlone.h"

// Forward declarations
namespace PhysicsTools{
  class MVAComputer;
}

using namespace std;

// FIXME: have a single class for all the four tree producing

class TauFakesHelper : public ObjectSelector {
public:
  TauFakesHelper(double, TString, TString, TString, TString, TString, TString, TString);
  ~TauFakesHelper(){}

  enum qualif {WMUDATA, WMUMC, QCDDATA, QCDMC};
  
  void ProcessEvents(unsigned int);
  void ComputeFakeRate(TString myKey="PF", bool passing = true, bool isAntiBTag = false);  

  void Trainer(unsigned int);
  void Monitor(unsigned int);

  void PrepareFiles(unsigned int);
  void ComputeTauFake(string type , double TauPt = 20., int option =1);
  void TauFakeEstimate(TString, TString, PhysicsTools::MVAComputer*, TH1F*, double*, double*);

  void ProduceDataDrivenDistributions();

private:
  void SetParameters();

  void AddFakeRate(TH1*, TH1*, TH1*);
  double GetErrorFraction(double&, double&);
  void GetReweightedPlots(TH1D*, TH1D*, TString, TString);
  // FIXME: condensate into one
  void GetPlotsFRPt(TH1D* data, TH1D* mc, TString name, const char* xaxis_title="Jet p_{T} [GeV]", float ymin=0.0001, float ymax=1.0, float xmin=0., float xmax=100.0);
  void GetPlotsFRAbsEta(TH1D* data, TH1D* mc, TString name, const char* xaxis_title="Jet |#eta|", float ymin=0.0001, float ymax=1.0, float xmin=0.0, float xmax=3.0);
  void GetPlotsFRR(TH1D* data, TH1D* mc, TString name,  const char* xaxis_title="Jet Width", float ymin=0.0001, float ymax=1.0, float xmin=0, float xmax=0.32);

  // Data members
  double tauPtCut_;
  TString inputArea_;
  TString trainingTreesArea_; // = routines outputArea_
  TString configArea_;
  TString trainingOutputArea_;
  TString outputArea_;
  TString puFileName_;
  TString ntuplesArea_;

  unsigned int qualifier_;
  TString infix_;  

  int TAUPRONGS_;
  double DRMIN_JET_E_, DRMIN_JET_M_, DRMIN_T_E_, DRMIN_T_M_, DRMIN_T_J_,  MIN_JET_PT_CUT_, MIN_TAU_PT_CUT_;
  double MET_CUT_; // WJetsTree only
  double JES_;
  double JetPt, AbsJetEta, JetWidth, __WEIGHT__;
  int __TARGET__;

  TFile* fPassing_;
  TFile* fFailing_;
  
  TTree* tPassing_;
  TTree* tFailing_;

  event::Reader *evReader;

  reweight::LumiReWeighting LumiWeights_; // WJetsTree only

};


#endif
