#ifndef _TauFakesHelper_hh
#define _TauFakesHelper_hh
/**   
      \class	    TauFakesHelper TauFakesHelper.hh "UserCode/LIP/TopTaus/interface/TauFakesHelper.hh"                                                                     
      \brief    Class for managing the operations needed in order to estimate tauFakes :)
      
      \author   Pietro Vischia
      
      \version  $Id: TauFakesHelper.hh,v 0.1 2013/03/19 17:08:18 vischia Exp $                                                                                                       
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


/*

   FRDataQCDTree q(20);
   q.processEvents();

*/
 


using namespace std;

// FIXME: have a single class for all the four tree producing

class FRDataQCDTree : public ObjectSelector {
public:
  FRDataQCDTree(double, TString, TString);
  ~FRDataQCDTree(){}

  void processEvents();
  void ComputeFakeRate(TString url,  TString myKey="PF", TString histoFiles_="TauFRData", bool passing = true, bool isAntiBTag = false);

  TString inputArea_; // FIXME: mutuate from commondefinitions
  TString outputArea_; 
  int    TAUPRONGS_;
  double DRMIN_JET_E_, DRMIN_JET_M_, DRMIN_T_E_, DRMIN_T_M_, DRMIN_T_J_, MIN_JET_PT_CUT_, MIN_TAU_PT_CUT_;

  double JES_;
  
  double JetPt, AbsJetEta, JetWidth, __WEIGHT__;
  int __TARGET__;
  event::Reader *evReader;
};


#endif
