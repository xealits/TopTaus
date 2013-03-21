/**   
      \class	    doTauFakesStudy.cc doTauFakesStudy.cc.cc "UserCode/LIP/TopTaus/bin/doTauFakesStudy.cc.cc"                                                                     
      \brief    executable for performing all the tau fakes study
      
      \author   Pietro Vischia
      
      \version  $Id: doTauFakesStudy.cc.cc,v 0.1 2013/03/19 17:08:18 vischia Exp $                                                                                                       
*/

#include "LIP/TopTaus/interface/TauFakesHelper.hh"


// System includes

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// ROOT includes
#include "TSystem.h"



/*
  
@run me like this

doTauFakesStudy 

*/

//
int main(int argc, char* argv[])
{
  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();
  
  //check arguments
  if ( argc < 2 ) {
    std::cout << "Usage : " << argv[0] << " parameters_cfg.py" << std::endl;
    return 0;
  }
  
  string parSet(argv[1]);
  
  const edm::ParameterSet &pSet = edm::readPSetsFrom(parSet)->getParameter<edm::ParameterSet>("TauFakesStudyParSet");
  
  double tauPtCut = pSet.getParameter<double>("tauPtCut");
  TString inputArea = TString(pSet.getParameter<string>("inputArea"));
  TString outputArea = TString(pSet.getParameter<string>("outputArea"));
  
  
  
  //  TString puFileName = TString(pSet.getParameter<string>("puFileName"));
  
  
  cout << "Before"  << endl;
  FRDataQCDTree* qcdTreeCreator = new FRDataQCDTree( tauPtCut, inputArea, outputArea /*parSet*/ );
  qcdTreeCreator->processEvents();
  std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;
  
  
  cout << "physicsAnalysis reached its natural end" << endl;
  
  return 0;
  
}


/*

   FRDataQCDTree q(20);
   q.processEvents();

*/
 
