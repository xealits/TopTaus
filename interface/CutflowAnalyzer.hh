#ifndef _CutflowAnalyzer_hh
#define _CutflowAnalyzer_hh
/**                                                                                                                                                                              
																			 \class    CutflowAnalyzer CutflowAnalyzer.cc "UserCode/LIP/TopTaus/src/CutflowAnalyzer.cc"                                                                     
																			 \brief    class for building and analyzing the cutflow of a physics analysis
  
  \author   Pietro Vischia, pietro.vischia@gmail.com

  \version  $Id: CutflowAnalyzer.hh,v 0.1 2013/02/01 17:53:22 vischia Exp $                                                                                                       
*/

// System headers
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#endif

// ROOT headers


#include "LIP/TopTaus/interface/UncertaintyCalculator.hh"
#include "LIP/TopTaus/interface/AnalysisMonitoring.hh"
#include "LIP/TopTaus/interface/ObjectSelector.hh"

using namespace std;



class CutflowAnalyzer : public UncertaintyCalculator, public AnalysisMonitoring, public ObjectSelector {
public:
  
  CutflowAnalyzer();
  
  
  
  
};


#endif
