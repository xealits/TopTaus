/**   
      \class	    physicsAnalysis physicsAnalysis.cc "UserCode/LIP/TopTaus/bin/physicsAnalysis.cc"                                                                     
      \brief    executable for performing the cutflow analysis and control plots for the various channels and data samples. Must expand to calling other classes
      
      \author   Pietro Vischia
      
      \version  $Id: physicsAnalysis.cc,v 0.1 2013/02/08 14:40:14 vischia Exp $                                                                                                       
*/

#include "LIP/TopTaus/interface/CutflowAnalyzer.hh"

// System includes	
#include <string>

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// ROOT includes
#include "TSystem.h"




/*
  
@run me like this

tauDileptonPDFBuilderFitter mutau HIGGS2BKG
tauDileptonPDFBuilderFitter mutau HIGGS2BKG

tauDileptonPDFBuilderFitter mutau HIGGS2BKG
tauDileptonPDFBuilderFitter mutau HIGGS3BKG

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
  const edm::ParameterSet &pSet = edm::readPSetsFrom(parSet)->getParameter<edm::ParameterSet>("PhysicsAnalysisParSet");
  
  double tauPtCut = pSet.getParameter<double>("tauPtCut");


  CutflowAnalyzer* analyzer = new CutflowAnalyzer( tauPtCut/*parSet*/ );
  
  std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;
  
  //  myProducer->Produce();
  //  cout << "Shapes producer reached its natural end" << endl;
  
  return 0;
  
}
//  // creates the physics analyzer
//  PhysicsAnalyzer analyzer(20);
//
//  
//  //analyzer.process_ttbar();  
//  //analyzer.process_data();
//  //analyzer.process_ttbar_bychannel();
//
//  // Table processing starts here
//  // include all errors -> (xx,xx,false,false)
//  bool onlyhiggs(true), sm(false), doNotPrintAllErrors(false), printAllErrors(true), includeSoverB(true), doNotincludeSoverB(false); 
//  int detailed(2), notDetailed(1);      
//analyzer.mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "yields-mc-", false, false, false); 
//analyzer.mcTable(notDetailed, includeSoverB, printAllErrors, onlyhiggs, "PFlow", "yields-mc-", false, false, false); 
////analyzer.mcTable(notDetailed, includeSoverB, doNotPrintAllErrors, onlyhiggs, "PFlow", "yields-mc-", false, false, false); 
//  //DEBUG (include all systematics )
//  //analyzer.mcTable(notDetailed, includeSoverB, doNotPrintAllErrors, sm, "PFlow", "yields-mc-", false, false, false);  
//  
//  //
// analyzer.summaryTable( notDetailed, false, false, false, false);
// analyzer.summaryTable( notDetailed, true, false, false, false);
//
// analyzer.summaryTable( notDetailed, false, true, false, false);
// analyzer.summaryTable( notDetailed, false, false, true, false);
// analyzer.summaryTable( notDetailed, false, true, true, false);
// analyzer.summaryTable( notDetailed, false, true, true, true);
//
// analyzer.summaryTable( notDetailed, true, true, false, false);
// analyzer.summaryTable( notDetailed, true, false, true, false);
// analyzer.summaryTable( notDetailed, true, true, true, false);
// analyzer.summaryTable( notDetailed, true, true, true, true);
//
//// analyzer.summaryTable( notDetailed, true, false, false, false);
//
//*/
