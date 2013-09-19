/**   
      \class	    physicsAnalysis physicsAnalysis.cc "UserCode/LIP/TopTaus/bin/physicsAnalysis.cc"                                                                     
      \brief    executable for performing the cutflow analysis and control plots for the various channels and data samples. Must expand to calling other classes
      
      \author   Pietro Vischia
      
      \version  $Id: physicsAnalysis.cc,v 1.22 2013/04/19 15:13:00 vischia Exp $                                                                                                       
*/

#include "LIP/TopTaus/interface/CutflowAnalyzer.hh"
#include "LIP/TopTaus/interface/HistogramPlotter.hh"

// System includes	
#include <string>
#include <sstream>

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// ROOT includes
#include "TSystem.h"



/*
  
@run me like this

physicsAnalysis test/physicsAnalysisParSets_cfg.py sample --> analyze sample

physicsAnalysis test/physicsAnalysisParSets_cfg.py doPlots --> draw plots

physicsAnalysis test/physicsAnalysisParSets_cfg.py doTables --> draw tables

*/

//
int main(int argc, char* argv[])
{
  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();
  
  //check arguments
  if ( argc < 3 ) {
    std::cout << "Usage : " << argv[0] << " parameters_cfg.py action" << std::endl;
    return 0;
  }
  
  
  //  // Get input arguments
  //  string sDoDatacards("");
  //  bool doDatacards(false);
  
  //  for(int i=3;i<argc;i++){
  //    string arg(argv[i]);
  //    //    if(arg.find("--help")        !=string::npos) { printHelp(); return -1;}                                                                                                                                  
  //    if(arg.find("--doDatacards") !=string::npos) { sDoDatacards = argv[i+1];}
  //    //check arguments // FIXME: implement --blah bih                             
  //  }
  //  if(sDoDatacards == "true")
  //    doDatacards=true;
  //  else if(sDoDatacards == "false")
  //    doDatacards=false;
  //  else{
  //    cout << "Error. DoDatacards value not defined. Defaulting to false (tables mode)" << endl;
  //    doDatacards=false;
  //  }
  
  
  
  string parSet(argv[1]);
  string runOn(argv[2]);
  
  const edm::ParameterSet &pSet = edm::readPSetsFrom(parSet)->getParameter<edm::ParameterSet>("PhysicsAnalysisParSet");
  
  double tauPtCut = pSet.getParameter<double>("tauPtCut");
  bool noUncertainties = pSet.getParameter<bool>("noUncertainties");
  bool doWPlusJetsAnalysis = pSet.getParameter<bool>("doWPlusJetsAnalysis");
  TString inputArea = TString(pSet.getParameter<string>("inputArea"));
  TString outputArea = TString(pSet.getParameter<string>("outputArea"));
  TString puFileName = TString(pSet.getParameter<string>("puFileName"));
  TString runRange   = TString(pSet.getParameter<string>("runRange"));
  std::vector<double> brHtaunu = pSet.getParameter<std::vector<double> >("brHtaunu");
  std::vector<double> brHtb    = pSet.getParameter<std::vector<double> >("brHtb");
  
  CutflowAnalyzer* analyzer = new CutflowAnalyzer( tauPtCut, noUncertainties, doWPlusJetsAnalysis, inputArea, outputArea, puFileName, runRange, brHtaunu, brHtb /*parSet*/ );
  
  std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;


  for(int i=0; i<400; ++i){
    stringstream sidx;
    sidx<<i;
    string idx=sidx.str();
    // 50 from 0
    if      (runOn == "data_muonA_"+idx)  analyzer->process_data_RunA( i);
    else if (runOn == "data_muonB_"+idx)  analyzer->process_data_RunB( i);
    else if (runOn == "data_muonC1_"+idx) analyzer->process_data_RunC1(i);
    else if (runOn == "data_muonC2_"+idx) analyzer->process_data_RunC2(i);
    else if (runOn == "data_muonD1_"+idx) analyzer->process_data_RunD1(i);
    else if (runOn == "data_muonD2_"+idx) analyzer->process_data_RunD2(i);
    else if(runOn == "ww_"+idx)            analyzer->process_diboson(0, i);
    else if(runOn == "wz_"+idx)            analyzer->process_diboson(1, i);
    else if(runOn == "zz_"+idx)            analyzer->process_diboson(2, i);
    else if(runOn == "ttbar_mutau_"+idx)         analyzer->process_ttbar_mutau(i)    ;
    else if(runOn == "ttbar_mumu_"+idx)          analyzer->process_ttbar_mumu(i)    ;
    else if(runOn == "ttbar_emu_"+idx)           analyzer->process_ttbar_emu(i)    ;
    else if(runOn == "ttbar_ddbkg_"+idx)         analyzer->process_ttbar_ddbkg(i)    ;
    else if(runOn == "ttbar_mcbkg_"+idx)         analyzer->process_ttbar_mcbkg(i)    ;
    else if(runOn == "ttbar_"+idx)               analyzer->process_ttbar(i)              ;
    else if(runOn == "qcd_1_"+idx)                 analyzer->process_qcd(1, i)                ;
    else if(runOn == "qcd_2_"+idx)                 analyzer->process_qcd(2, i)                ;
    else if(runOn == "qcd_3_"+idx)                 analyzer->process_qcd(3, i)                ;
    else if(runOn == "qcd_4_"+idx)                 analyzer->process_qcd(4, i)                ;
    else if(runOn == "qcd_5_"+idx)                 analyzer->process_qcd(5, i)                ;
    else if(runOn == "qcd_6_"+idx)                 analyzer->process_qcd(6, i)                ;
    else if(runOn == "qcd_7_"+idx)                 analyzer->process_qcd(7, i)                ;
    else if(runOn == "qcd_8_"+idx)                 analyzer->process_qcd(8, i)                ;
    else if(runOn == "qcd_9_"+idx)                 analyzer->process_qcd(9, i)                ;

    // 100 from 0
    else if(runOn == "zjets_from50_"+idx)        analyzer->process_zjets_from50(i)       ;
    else if(runOn == "zjets_10to50_"+idx)        analyzer->process_zjets_10to50(i)       ;

    // 400 from 0
    else if(runOn == "wjets_"+idx)               analyzer->process_wjets(i)              ;

    // 20 from 0
    else if(runOn == "singletop_"+idx)           analyzer->process_singletop(i)          ;

    // 30 from 0
    else if(runOn == "htb_higgs_bychannel_180_"+idx) analyzer->process_htb_higgs_bychannel(180, i) ;
    else if(runOn == "htb_higgs_bychannel_200_"+idx) analyzer->process_htb_higgs_bychannel(200, i) ;
    else if(runOn == "htb_higgs_bychannel_220_"+idx) analyzer->process_htb_higgs_bychannel(220, i) ;
    else if(runOn == "htb_higgs_bychannel_240_"+idx) analyzer->process_htb_higgs_bychannel(240, i) ;
    else if(runOn == "htb_higgs_bychannel_250_"+idx) analyzer->process_htb_higgs_bychannel(250, i) ;
    else if(runOn == "htb_higgs_bychannel_260_"+idx) analyzer->process_htb_higgs_bychannel(260, i) ;
    else if(runOn == "htb_higgs_bychannel_280_"+idx) analyzer->process_htb_higgs_bychannel(280, i) ;
    else if(runOn == "htb_higgs_bychannel_300_"+idx) analyzer->process_htb_higgs_bychannel(300, i) ;
    else if(runOn == "htb_higgs_180_"+idx)           analyzer->process_htb_higgs(180, i)           ;
    else if(runOn == "htb_higgs_200_"+idx)           analyzer->process_htb_higgs(200, i)           ;
    else if(runOn == "htb_higgs_220_"+idx)           analyzer->process_htb_higgs(220, i)           ;
    else if(runOn == "htb_higgs_240_"+idx)           analyzer->process_htb_higgs(240, i)           ;
    else if(runOn == "htb_higgs_250_"+idx)           analyzer->process_htb_higgs(250, i)           ;
    else if(runOn == "htb_higgs_260_"+idx)           analyzer->process_htb_higgs(260, i)           ;
    else if(runOn == "htb_higgs_280_"+idx)           analyzer->process_htb_higgs(280, i)           ;
    else if(runOn == "htb_higgs_300_"+idx)           analyzer->process_htb_higgs(300, i)           ;

  }  

    
  if     (runOn == "hh_higgs_bychannel")  analyzer->process_hh_higgs_bychannel() ;
  else if(runOn == "hh_higgs")            analyzer->process_hh_higgs()           ;
  else if(runOn == "ttbar_unc")           analyzer->process_ttbar_unc()          ;
  
  else if(runOn == "wh_higgs_bychannel")  analyzer->process_wh_higgs_bychannel() ;
  else if(runOn == "wh_higgs")            analyzer->process_wh_higgs()           ;
  else if(runOn == "tbh_higgs_bychannel") analyzer->process_tbh_higgs_bychannel() ;
  else if(runOn == "tbh_higgs")           analyzer->process_tbh_higgs()           ;



  // FIXME: manage with --blah for having parameters like "withHiggs" and so on
  else if(runOn == "doTables"){
    cout << "Doing tables" << endl;
    bool onlyhiggs(true), heavyhiggs(false), sm(false), doNotPrintAllErrors(false), printAllErrors(true), includeSoverB(true), doNotincludeSoverB(false), produceDatacards(false), withShapes(true), withStatShapes(false), unsplit(false);
    int detailed(2), notDetailed(1);      
    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, heavyhiggs, sm, "PFlow", "yields-mc-", false, false, false); 
    cout << "Done SM table" << endl;
    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, onlyhiggs, heavyhiggs, "PFlow", "yields-mc-", false, false, false); 
    cout << "Done TBH table" << endl;
    analyzer->summaryTable( notDetailed, true, heavyhiggs, false, false, false, produceDatacards, withShapes, withStatShapes, unsplit);
    cout << "Done summary table" << endl;
  }
  else if(runOn == "doDatacards"){
    bool produceDatacards(true), withShapes(true), withStatShapes(false), unsplit(true), heavyhiggs(false);
    int detailed(2), notDetailed(1); 
    analyzer->summaryTable( notDetailed, true, heavyhiggs, false, false, false, produceDatacards, withShapes, withStatShapes, unsplit);
  }
  else if(runOn == "doPlots"){
//    PlotStyle sty();
//    sty.setTDRStyle();
      
    TString samples("data/plotter/samples.xml");
    TString outFolder("plots/"); // move to input line
    TString cmd = "mkdir -p "+outFolder+"; cp data/plotter/index.html+"+outFolder+";";
    gSystem->Exec(cmd);
    TString limits("data/plotter/limits.xml");
    TString leptons("data/plotter/leptons.xml");
    TString met("data/plotter/met.xml");
    TString mt("data/plotter/mt.xml");
    TString wjets("data/plotter/wjets.xml");
    TString wtmass("data/plotter/wtmass.xml");
    TString jets("data/plotter/jets.xml");
    TString yields("data/plotter/yields.xml");
    TString taus("data/plotter/taus.xml");
    TString vertex("data/plotter/vertex.xml");
    TString afterR("data/plotter/afterR.xml");
    TString test("data/plotter/test.xml");
    TString debug("data/plotter/debug.xml");

    HistogramPlotter a; // Move to input line or cfg file the choice of what to plot
    a.parse(samples,vertex,outFolder);
    a.parse(samples,met,outFolder);      
    a.parse(samples,leptons,outFolder);  
    a.parse(samples,mt,outFolder);      
    a.parse(samples,jets,outFolder); 
    a.parse(samples,yields,outFolder);

    //a.parse(samples,limits,outFolder);      
    //a.parse(samples,debug,outFolder);  
    //a.parse(samples,afterR,outFolder);
    //
    //  //a.parse(samples,wjets,outFolder);
    //  //a.parse(samples,wtmass,outFolder);
    //  //a.parse(higgssamples,taus,outFolder);
    //  //a.parse(samples,taus,outFolder);
    //  //a.parse(samples,test,outFolder);
    
  }
  else
    cout << "Sample does not exist" << endl;
  


  
  cout << "physicsAnalysis reached its natural end" << endl;
  
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




