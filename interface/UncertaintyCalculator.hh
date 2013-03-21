#ifndef _UncertaintyCalculator_hh
#define _UncertaintyCalculator_hh
/**                                                                                                                                                                              
																			 \class    UncertaintyCalculator UncertaintyCalculator.cc "UserCode/LIP/TopTaus/src/UncertaintyCalculator.cc"                                                                     
																			 \brief    class for managing the calculation of uncertainties.
  
  \author   Pietro Vischia, pietro.vischia@gmail.com

  \version  $Id: UncertaintyCalculator.hh,v 1.2 2013/03/21 11:00:47 vischia Exp $                                                                                                       
*/

// System headers
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#endif

// ROOT headers


#include "LIP/TopTaus/interface/CommonDefinitions.hh" // Should be protected by the CINT "already defined" stuff or the global var is enough?


// CMSSW headers
#include "CondFormats/JetMETObjects/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

//Should comment the 
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"


#include "LIP/TopTaus/interface/SelectionMonitor.hh"
#include "LIP/TopTaus/interface/MiniEvent.hh"
// #include "LIP/TopTaus/interface/Utilities.hh"


using namespace std;


class UncertaintyCalculator {
public:
  double jetMETScaling( vector<double> & jerFactors, double jes,JetCorrectionUncertainty * junc, vector<PhysicsObject> & vJ , double missetX, double missetY);
  double jetMETUnclustered( vector<double> & jerFactors, PhysicsObject * obj, double unc, vector<PhysicsObject> & vJ, double missetX, double missetY);
  double jetMETResolution( vector<double> & jerFactors , vector<PhysicsObject> & vJ, double missetX, double missetY);
  double getJetPt( PhysicsObject &j ,JetCorrectionUncertainty * junc, double jerFactor,double jes);
  PhysicsObject smearedJet(const PhysicsObject &origJet, double genJetPt, int mode, double& scaleFactor); // Perhaps it would be better to create a class or a namespace METUtils
  enum UncertaintyVariations { JER, JER_UP, JER_DOWN, JES_UP, JES_DOWN,UMET_UP,UMET_DOWN,LES_UP,LES_DOWN};
  //void computeVariation(vector<PhysicsObject>& jets, 
  //			vector<PhysicsObject> &leptons, // not really needed for now
  //			PhysicsObject& met, // 
  //			vector<PhysicsObjectCollection>& jetsVar, 
  //			vector<PhysicsObject>& metsVar,
  //			JetCorrectionUncertainty *jecUnc);
  //
};


#endif
