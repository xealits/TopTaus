#ifndef _HistogramBuilder_hh
#define _HistogramBuilder_hh
/**                                                                                                                                                                              
																			 \class    HistogramBuilder HistogramBuilder.cc "UserCode/LIP/TopTaus/src/HistogramBuilder.cc"                                                                     
																			 \brief    class for managing the creation of histograms starting from events
  
  \author   Pietro Vischia, pietro.vischia@gmail.com

  \version  $Id: HistogramBuilder.hh,v 0.1 2013/01/31 19:40:14 vischia Exp $                                                                                                       
*/

// System headers
#if !defined(__CINT__) || defined(__MAKECINT__)
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#endif

// ROOT headers
#include "TFile.h"
#include "TPolyMarker3D.h"

#include "LIP/TopTaus/interface/SelectionMonitor.hh"
#include "LIP/TopTaus/interface/Utilities.hh"
#include "LIP/TopTaus/interface/CommonDefinitions.hh"




using namespace std;



class HistogramBuilder {
  public:

    HistogramBuilder(){}

    void buildHistograms( 
      TFile * outFile, 
      map< TString, SelectionMonitor * >  & stepsMons , 
      map< TString, SelectionMonitor * > & yieldsMons , 
      map< TString, SelectionMonitor * > & mcMons,  
      map< TString, SelectionMonitor * > & bkgMons,
      map< TString, SelectionMonitor * > & debugMons,  
      map<TString,TDirectory *> & dirsMaps , vector<TString> & keys 
    );

    void buildRecoStepHistograms(   TFile * outFile, map< TString, SelectionMonitor * >  & stepsMons  , map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void buildYieldsHistograms(     TFile * outFile, map< TString, SelectionMonitor * >  & yieldsMons , map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void buildMCYieldsHistograms(   TFile * outFile, map< TString, SelectionMonitor * >  & yieldsMons , map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void buildBkgHistograms(        TFile * outFile, map< TString, SelectionMonitor * >  & bkgMons    , map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void buildDebugHistograms(      TFile * outFile, map< TString, SelectionMonitor * >  & debugMons  , map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void buildNewPhysicsHistograms( TFile * outFile,  map<TString,TDirectory *> & dirsMaps , vector<TString> & keys );
    void addHistoToMonitors( TH1 * h, TString name, map<TString, SelectionMonitor * > & v, vector<TString> & keys);


   bool wplusjets_;


   map< TString, SelectionMonitor * >  newPhysicsMons_;           // yield monitors at different steps for tau inv mass studies

   

};


#endif
