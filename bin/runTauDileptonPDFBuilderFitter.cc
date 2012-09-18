/**                                                                                                                                                                              
  \class    likelihoodFitter::tauDileptonPDFBuilderFitter tauDileptonPDFBuilderFitter.cc "UserCode/LIP/TopTaus/tauDileptonPDFBuilderFitter.cc"                                                                     
  \brief    executable for performing multivariable likelihood fit in order to improve estimation of N_fakes
  
  \author   Pietro Vischia

  \version  $Id: runTauDileptonPDFBuilderFitter.cc,v 1.2 2012/09/17 18:52:51 vischia Exp $                                                                                                       
*/

#include "LIP/TopTaus/interface/TauDileptonPDFBuilderFitter.hh"

// System includes
#include <string>

// ROOT includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
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

  TauDileptonPDFBuilderFitter* myFitter = new TauDileptonPDFBuilderFitter(parSet);
  myFitter->DoFit();
  cout << "Fitter reached natural end" << endl;

  return 0;
  
}
