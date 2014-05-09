/**   
      \class	    physicsAnalysis physicsAnalysis.cc "UserCode/LIP/TopTaus/bin/physicsAnalysis.cc"                                                                     
      \brief    executable for performing the cutflow analysis and control plots for the various channels and data samples. Must expand to calling other classes
      
      \author   Pietro Vischia
      
      \version  $Id: physicsAnalysis.cc,v 1.15 2013/03/07 15:08:49 vischia Exp $                                                                                                       
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

  CutflowAnalyzer* analyzer = new CutflowAnalyzer( tauPtCut, noUncertainties, doWPlusJetsAnalysis, inputArea, outputArea, puFileName, runRange /*parSet*/ );
  
  std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;
  
  
  for(int i=0; i<400; ++i){
    stringstream sidx;
    sidx<<i;
    string idx=sidx.str();
    // 50 from 0
    if(runOn == "ww_"+idx)            analyzer->process_diboson(0, i);
    else if(runOn == "wz_"+idx)            analyzer->process_diboson(1, i);
    else if(runOn == "zz_"+idx)            analyzer->process_diboson(2, i);
    else if(runOn == "ttbar_etau_"+idx)         analyzer->process_ttbar_etau(i)    ;
    else if(runOn == "ttbar_mutau_"+idx)         analyzer->process_ttbar_mutau(i)    ;
//     else if(runOn == "ttbar_mumu_"+idx)          analyzer->process_ttbar_mumu(i)    ;
//     else if(runOn == "ttbar_emu_"+idx)           analyzer->process_ttbar_emu(i)    ;
    else if(runOn == "ttbar_ddbkg_"+idx)         analyzer->process_ttbar_ddbkg(i)    ;
    else if(runOn == "ttbar_mcbkg_"+idx)         analyzer->process_ttbar_mcbkg(i)    ;
    else if(runOn == "ttbar_"+idx)               analyzer->process_ttbar(i)              ;
    else if(runOn == "ttbar-169p5_"+idx)               analyzer->process_ttbar169p5(i)              ;
    else if(runOn == "ttbar-175p5_"+idx)               analyzer->process_ttbar175p5(i)              ;

    else if(runOn == "ttv3a_etau_"+idx)         analyzer->process_ttv3a_etau(i)    ;
    else if(runOn == "ttv3a_mutau_"+idx)         analyzer->process_ttv3a_mutau(i)    ;
    else if(runOn == "ttv3a_ddbkg_"+idx)         analyzer->process_ttv3a_ddbkg(i)    ;
    else if(runOn == "ttv3a_mcbkg_"+idx)         analyzer->process_ttv3a_mcbkg(i)    ;

    else if(runOn == "ttv3b_etau_"+idx)         analyzer->process_ttv3b_etau(i)    ;
    else if(runOn == "ttv3b_mutau_"+idx)         analyzer->process_ttv3b_mutau(i)    ;
    else if(runOn == "ttv3b_ddbkg_"+idx)         analyzer->process_ttv3b_ddbkg(i)    ;
    else if(runOn == "ttv3b_mcbkg_"+idx)         analyzer->process_ttv3b_mcbkg(i)    ;

    else if(runOn == "ttFullLeptMG_"+idx)               analyzer->process_ttFullLeptMG(i)              ;
    else if(runOn == "ttSemiLeptMG_"+idx)               analyzer->process_ttSemiLeptMG(i)              ;
    else if(runOn == "ttHadronicMG_"+idx)               analyzer->process_ttHadronicMG(i)              ;

    else if(runOn == "ttbar_scaleup_"+idx)               analyzer->process_ttbar_scaleup(i)              ;
    else if(runOn == "ttbar_scaledown_"+idx)               analyzer->process_ttbar_scaledown(i)              ;
    else if(runOn == "ttbar_matchup_"+idx)               analyzer->process_ttbar_matchup(i)              ;
    else if(runOn == "ttbar_matchdown_"+idx)               analyzer->process_ttbar_matchdown(i)              ;

    else if(runOn == "qcd_EM_Pt30to80_"+idx)           analyzer->process_qcd_EM_Pt30to80(i)       ;
    else if(runOn == "qcd_EM_Pt80to170_"+idx)           analyzer->process_qcd_EM_Pt80to170(i)       ;
    else if(runOn == "qcd_EM_Pt170to250_"+idx)           analyzer->process_qcd_EM_Pt170to250(i)       ;
    else if(runOn == "qcd_EM_Pt250to350_"+idx)           analyzer->process_qcd_EM_Pt250to350(i)       ;
    else if(runOn == "qcd_EM_Pt350toinf_"+idx)           analyzer->process_qcd_EM_Pt350toinf(i)       ;
    
    else if(runOn == "qcd_30to80_BCtoE_"+idx)           analyzer->process_qcd_30to80_BCtoE(i)       ;
    else if(runOn == "qcd_80to170_BCtoE_"+idx)           analyzer->process_qcd_80to170_BCtoE(i)       ;
    else if(runOn == "qcd_170to250_BCtoE_"+idx)           analyzer->process_qcd_170to250_BCtoE(i)       ;
    else if(runOn == "qcd_250to350_BCtoE_"+idx)           analyzer->process_qcd_250to350_BCtoE(i)       ;
    else if(runOn == "qcd_350toinf_BCtoE_"+idx)           analyzer->process_qcd_350toinf_BCtoE(i)       ;

    else if(runOn == "PhotonJets_15to30_"+idx)     analyzer->process_PhotonJets_15to30(i);
    else if(runOn == "PhotonJets_30to50_"+idx)     analyzer->process_PhotonJets_30to50(i);
    else if(runOn == "PhotonJets_50to80_"+idx)     analyzer->process_PhotonJets_50to80(i);
    else if(runOn == "PhotonJets_80to120_"+idx)     analyzer->process_PhotonJets_80to120(i);
    else if(runOn == "PhotonJets_120to170_"+idx)     analyzer->process_PhotonJets_120to170(i);
    else if(runOn == "PhotonJets_170to300_"+idx)     analyzer->process_PhotonJets_170to300(i);
    else if(runOn == "PhotonJets_300to470_"+idx)     analyzer->process_PhotonJets_300to470(i);
    
    else if(runOn == "qcd_1_"+idx)                 analyzer->process_qcd(1, i)                ;
    else if(runOn == "qcd_2_"+idx)                 analyzer->process_qcd(2, i)                ;
    else if(runOn == "qcd_3_"+idx)                 analyzer->process_qcd(3, i)                ;
    else if(runOn == "qcd_4_"+idx)                 analyzer->process_qcd(4, i)                ;
    else if(runOn == "qcd_5_"+idx)                 analyzer->process_qcd(5, i)                ;
    else if(runOn == "qcd_6_"+idx)                 analyzer->process_qcd(6, i)                ;
    else if(runOn == "qcd_7_"+idx)                 analyzer->process_qcd(7, i)                ;
    else if(runOn == "qcd_8_"+idx)                 analyzer->process_qcd(8, i)                ;
    else if(runOn == "qcd_9_"+idx)                 analyzer->process_qcd(9, i)                ;

    // 200 from 0
    else if(runOn == "zjets_from50_"+idx)        analyzer->process_zjets_from50(i)       ;
    // 100 from 0
    else if(runOn == "zjets_10to50_"+idx)        analyzer->process_zjets_10to50(i)       ;

    // 400 from 0
    else if(runOn == "wjets_"+idx)               analyzer->process_wjets(i)              ;

    // 20 from 0
    else if(runOn == "singletop_"+idx)           analyzer->process_singletop(i)          ;

//     // 30 from 0
//     else if(runOn == "htb_higgs_bychannel_180_"+idx) analyzer->process_htb_higgs_bychannel(180, i) ;
//     else if(runOn == "htb_higgs_bychannel_200_"+idx) analyzer->process_htb_higgs_bychannel(200, i) ;
//     else if(runOn == "htb_higgs_bychannel_220_"+idx) analyzer->process_htb_higgs_bychannel(220, i) ;
//     else if(runOn == "htb_higgs_bychannel_240_"+idx) analyzer->process_htb_higgs_bychannel(240, i) ;
//     else if(runOn == "htb_higgs_bychannel_250_"+idx) analyzer->process_htb_higgs_bychannel(250, i) ;
//     else if(runOn == "htb_higgs_bychannel_260_"+idx) analyzer->process_htb_higgs_bychannel(260, i) ;
//     else if(runOn == "htb_higgs_bychannel_280_"+idx) analyzer->process_htb_higgs_bychannel(280, i) ;
//     else if(runOn == "htb_higgs_bychannel_300_"+idx) analyzer->process_htb_higgs_bychannel(300, i) ;
//     else if(runOn == "htb_higgs_180_"+idx)           analyzer->process_htb_higgs(180, i)           ;
//     else if(runOn == "htb_higgs_200_"+idx)           analyzer->process_htb_higgs(200, i)           ;
//     else if(runOn == "htb_higgs_220_"+idx)           analyzer->process_htb_higgs(220, i)           ;
//     else if(runOn == "htb_higgs_240_"+idx)           analyzer->process_htb_higgs(240, i)           ;
//     else if(runOn == "htb_higgs_250_"+idx)           analyzer->process_htb_higgs(250, i)           ;
//     else if(runOn == "htb_higgs_260_"+idx)           analyzer->process_htb_higgs(260, i)           ;
//     else if(runOn == "htb_higgs_280_"+idx)           analyzer->process_htb_higgs(280, i)           ;
//     else if(runOn == "htb_higgs_300_"+idx)           analyzer->process_htb_higgs(300, i)           ;

    else if(runOn == "stopbar_t_"+idx)   analyzer->process_stopbar_t(i);

    else if(runOn == "stop_tW_"+idx)   analyzer->process_stop_tW(i);

    else if(runOn == "Electron_RunA_"+idx)  analyzer->process_electr_RunA(i);
    else if(runOn == "Electron_RunB_"+idx)  analyzer->process_electr_RunB(i);

    else if (runOn == "data_muonA_"+idx)  analyzer->process_muon_RunA( i);
    else if (runOn == "data_muonB_"+idx)  analyzer->process_muon_RunB( i);
    else if (runOn == "data_muonC1_"+idx) analyzer->process_muon_RunC1(i);
    else if (runOn == "data_muonC2_"+idx) analyzer->process_muon_RunC2(i);
    else if (runOn == "data_muonD1_"+idx) analyzer->process_muon_RunD1(i);
    else if (runOn == "data_muonD2_"+idx) analyzer->process_muon_RunD2(i);
    else if (runOn == "dataMissing_"+idx)  analyzer->process_muon_Miss(i);

  }  


  //  if(runOn == "Electron_RunA")     analyzer->process_electr_RunA();
//   else if(runOn == "Electron_RunB_10")  analyzer->process_electr_RunB(10);
//   else if(runOn == "Electron_RunB_11")  analyzer->process_electr_RunB(11);
//   else if(runOn == "Electron_RunB_12")  analyzer->process_electr_RunB(12);
//   else if(runOn == "Electron_RunB_13")  analyzer->process_electr_RunB(13);
//   else if(runOn == "Electron_RunB_14")  analyzer->process_electr_RunB(14);
//   else if(runOn == "Electron_RunB_15")  analyzer->process_electr_RunB(15);
//   else if(runOn == "Electron_RunB_16")  analyzer->process_electr_RunB(16);
//   else if(runOn == "Electron_RunB_17")  analyzer->process_electr_RunB(17);
//   else if(runOn == "Electron_RunB_18")  analyzer->process_electr_RunB(18);
//   else if(runOn == "Electron_RunB_19")  analyzer->process_electr_RunB(19);

//   else if(runOn == "Electron_RunB_20")  analyzer->process_electr_RunB(20);
//   else if(runOn == "Electron_RunB_21")  analyzer->process_electr_RunB(21);
//   else if(runOn == "Electron_RunB_22")  analyzer->process_electr_RunB(22);
//   else if(runOn == "Electron_RunB_23")  analyzer->process_electr_RunB(23);
//   else if(runOn == "Electron_RunB_24")  analyzer->process_electr_RunB(24);
//   else if(runOn == "Electron_RunB_25")  analyzer->process_electr_RunB(25);
//   else if(runOn == "Electron_RunB_26")  analyzer->process_electr_RunB(26);
//   else if(runOn == "Electron_RunB_27")  analyzer->process_electr_RunB(27);
//   else if(runOn == "Electron_RunB_28")  analyzer->process_electr_RunB(28);
//   else if(runOn == "Electron_RunB_29")  analyzer->process_electr_RunB(29);

//   else if(runOn == "Electron_RunB_3")  analyzer->process_electr_RunB(3);
//   else if(runOn == "Electron_RunB_4")  analyzer->process_electr_RunB(4);
//   else if(runOn == "Electron_RunB_5")  analyzer->process_electr_RunB(5);
//   else if(runOn == "Electron_RunB_6")  analyzer->process_electr_RunB(6);
//   else if(runOn == "Electron_RunB_7")  analyzer->process_electr_RunB(7);
//   else if(runOn == "Electron_RunB_8")  analyzer->process_electr_RunB(8);
//   else if(runOn == "Electron_RunB_9")  analyzer->process_electr_RunB(9);
//   else if(runOn == "Electron_RunB_0")  analyzer->process_electr_RunB(0);

  if(runOn == "Electron_RunC_leg1_10")  analyzer->process_electr_RunC_leg1(10);
  else if(runOn == "Electron_RunC_leg1_11")  analyzer->process_electr_RunC_leg1(11);
  else if(runOn == "Electron_RunC_leg1_12")  analyzer->process_electr_RunC_leg1(12);
  else if(runOn == "Electron_RunC_leg1_13")  analyzer->process_electr_RunC_leg1(13);
  else if(runOn == "Electron_RunC_leg1_14")  analyzer->process_electr_RunC_leg1(14);
  else if(runOn == "Electron_RunC_leg1_15")  analyzer->process_electr_RunC_leg1(15);
  else if(runOn == "Electron_RunC_leg1_16")  analyzer->process_electr_RunC_leg1(16);
  else if(runOn == "Electron_RunC_leg1_17")  analyzer->process_electr_RunC_leg1(17);
  else if(runOn == "Electron_RunC_leg1_18")  analyzer->process_electr_RunC_leg1(18);
  else if(runOn == "Electron_RunC_leg1_19")  analyzer->process_electr_RunC_leg1(19);

  else if(runOn == "Electron_RunC_leg1_20")  analyzer->process_electr_RunC_leg1(20);
  else if(runOn == "Electron_RunC_leg1_21")  analyzer->process_electr_RunC_leg1(21);
  else if(runOn == "Electron_RunC_leg1_22")  analyzer->process_electr_RunC_leg1(22);
  else if(runOn == "Electron_RunC_leg1_23")  analyzer->process_electr_RunC_leg1(23);
  else if(runOn == "Electron_RunC_leg1_24")  analyzer->process_electr_RunC_leg1(24);
  else if(runOn == "Electron_RunC_leg1_25")  analyzer->process_electr_RunC_leg1(25);
  else if(runOn == "Electron_RunC_leg1_26")  analyzer->process_electr_RunC_leg1(26);
  else if(runOn == "Electron_RunC_leg1_27")  analyzer->process_electr_RunC_leg1(27);
  else if(runOn == "Electron_RunC_leg1_28")  analyzer->process_electr_RunC_leg1(28);
  else if(runOn == "Electron_RunC_leg1_29")  analyzer->process_electr_RunC_leg1(29);

  else if(runOn == "Electron_RunC_leg1_3")  analyzer->process_electr_RunC_leg1(3);
  else if(runOn == "Electron_RunC_leg1_4")  analyzer->process_electr_RunC_leg1(4);
  else if(runOn == "Electron_RunC_leg1_5")  analyzer->process_electr_RunC_leg1(5);
  else if(runOn == "Electron_RunC_leg1_6")  analyzer->process_electr_RunC_leg1(6);
  else if(runOn == "Electron_RunC_leg1_7")  analyzer->process_electr_RunC_leg1(7);
  else if(runOn == "Electron_RunC_leg1_8")  analyzer->process_electr_RunC_leg1(8);
  else if(runOn == "Electron_RunC_leg1_9")  analyzer->process_electr_RunC_leg1(9);
  else if(runOn == "Electron_RunC_leg1_0")  analyzer->process_electr_RunC_leg1(0);

  else if(runOn == "Electron_RunC_leg2_10")  analyzer->process_electr_RunC_leg2(10);
  else if(runOn == "Electron_RunC_leg2_11")  analyzer->process_electr_RunC_leg2(11);
  else if(runOn == "Electron_RunC_leg2_12")  analyzer->process_electr_RunC_leg2(12);
  else if(runOn == "Electron_RunC_leg2_13")  analyzer->process_electr_RunC_leg2(13);
  else if(runOn == "Electron_RunC_leg2_14")  analyzer->process_electr_RunC_leg2(14);
  else if(runOn == "Electron_RunC_leg2_15")  analyzer->process_electr_RunC_leg2(15);
  else if(runOn == "Electron_RunC_leg2_16")  analyzer->process_electr_RunC_leg2(16);
  else if(runOn == "Electron_RunC_leg2_17")  analyzer->process_electr_RunC_leg2(17);
  else if(runOn == "Electron_RunC_leg2_18")  analyzer->process_electr_RunC_leg2(18);
  else if(runOn == "Electron_RunC_leg2_19")  analyzer->process_electr_RunC_leg2(19);

  else if(runOn == "Electron_RunC_leg2_20")  analyzer->process_electr_RunC_leg2(20);
  else if(runOn == "Electron_RunC_leg2_21")  analyzer->process_electr_RunC_leg2(21);
  else if(runOn == "Electron_RunC_leg2_22")  analyzer->process_electr_RunC_leg2(22);
  else if(runOn == "Electron_RunC_leg2_23")  analyzer->process_electr_RunC_leg2(23);
  else if(runOn == "Electron_RunC_leg2_24")  analyzer->process_electr_RunC_leg2(24);
  else if(runOn == "Electron_RunC_leg2_25")  analyzer->process_electr_RunC_leg2(25);
  else if(runOn == "Electron_RunC_leg2_26")  analyzer->process_electr_RunC_leg2(26);
  else if(runOn == "Electron_RunC_leg2_27")  analyzer->process_electr_RunC_leg2(27);
  else if(runOn == "Electron_RunC_leg2_28")  analyzer->process_electr_RunC_leg2(28);
  else if(runOn == "Electron_RunC_leg2_29")  analyzer->process_electr_RunC_leg2(29);

  else if(runOn == "Electron_RunC_leg2_3")  analyzer->process_electr_RunC_leg2(3);
  else if(runOn == "Electron_RunC_leg2_4")  analyzer->process_electr_RunC_leg2(4);
  else if(runOn == "Electron_RunC_leg2_5")  analyzer->process_electr_RunC_leg2(5);
  else if(runOn == "Electron_RunC_leg2_6")  analyzer->process_electr_RunC_leg2(6);
  else if(runOn == "Electron_RunC_leg2_7")  analyzer->process_electr_RunC_leg2(7);
  else if(runOn == "Electron_RunC_leg2_8")  analyzer->process_electr_RunC_leg2(8);
  else if(runOn == "Electron_RunC_leg2_9")  analyzer->process_electr_RunC_leg2(9);
  else if(runOn == "Electron_RunC_leg2_0")  analyzer->process_electr_RunC_leg2(0);

  else if(runOn == "Electron_RunD_leg1_10")  analyzer->process_electr_RunD_leg1(10);
  else if(runOn == "Electron_RunD_leg1_11")  analyzer->process_electr_RunD_leg1(11);
  else if(runOn == "Electron_RunD_leg1_12")  analyzer->process_electr_RunD_leg1(12);
  else if(runOn == "Electron_RunD_leg1_13")  analyzer->process_electr_RunD_leg1(13);
  else if(runOn == "Electron_RunD_leg1_14")  analyzer->process_electr_RunD_leg1(14);
  else if(runOn == "Electron_RunD_leg1_15")  analyzer->process_electr_RunD_leg1(15);
  else if(runOn == "Electron_RunD_leg1_16")  analyzer->process_electr_RunD_leg1(16);
  else if(runOn == "Electron_RunD_leg1_17")  analyzer->process_electr_RunD_leg1(17);
  else if(runOn == "Electron_RunD_leg1_18")  analyzer->process_electr_RunD_leg1(18);
  else if(runOn == "Electron_RunD_leg1_19")  analyzer->process_electr_RunD_leg1(19);

  else if(runOn == "Electron_RunD_leg1_20")  analyzer->process_electr_RunD_leg1(20);
  else if(runOn == "Electron_RunD_leg1_21")  analyzer->process_electr_RunD_leg1(21);
  else if(runOn == "Electron_RunD_leg1_22")  analyzer->process_electr_RunD_leg1(22);
  else if(runOn == "Electron_RunD_leg1_23")  analyzer->process_electr_RunD_leg1(23);
  else if(runOn == "Electron_RunD_leg1_24")  analyzer->process_electr_RunD_leg1(24);
  else if(runOn == "Electron_RunD_leg1_25")  analyzer->process_electr_RunD_leg1(25);
  else if(runOn == "Electron_RunD_leg1_26")  analyzer->process_electr_RunD_leg1(26);
  else if(runOn == "Electron_RunD_leg1_27")  analyzer->process_electr_RunD_leg1(27);
  else if(runOn == "Electron_RunD_leg1_28")  analyzer->process_electr_RunD_leg1(28);
  else if(runOn == "Electron_RunD_leg1_29")  analyzer->process_electr_RunD_leg1(29);

  else if(runOn == "Electron_RunD_leg1_3")  analyzer->process_electr_RunD_leg1(3);
  else if(runOn == "Electron_RunD_leg1_4")  analyzer->process_electr_RunD_leg1(4);
  else if(runOn == "Electron_RunD_leg1_5")  analyzer->process_electr_RunD_leg1(5);
  else if(runOn == "Electron_RunD_leg1_6")  analyzer->process_electr_RunD_leg1(6);
  else if(runOn == "Electron_RunD_leg1_7")  analyzer->process_electr_RunD_leg1(7);
  else if(runOn == "Electron_RunD_leg1_8")  analyzer->process_electr_RunD_leg1(8);
  else if(runOn == "Electron_RunD_leg1_9")  analyzer->process_electr_RunD_leg1(9);
  else if(runOn == "Electron_RunD_leg1_0")  analyzer->process_electr_RunD_leg1(0);

  else if(runOn == "Electron_RunD_leg2_10")  analyzer->process_electr_RunD_leg2(10);
  else if(runOn == "Electron_RunD_leg2_11")  analyzer->process_electr_RunD_leg2(11);
  else if(runOn == "Electron_RunD_leg2_12")  analyzer->process_electr_RunD_leg2(12);
  else if(runOn == "Electron_RunD_leg2_13")  analyzer->process_electr_RunD_leg2(13);
  else if(runOn == "Electron_RunD_leg2_14")  analyzer->process_electr_RunD_leg2(14);
  else if(runOn == "Electron_RunD_leg2_15")  analyzer->process_electr_RunD_leg2(15);
  else if(runOn == "Electron_RunD_leg2_16")  analyzer->process_electr_RunD_leg2(16);
  else if(runOn == "Electron_RunD_leg2_17")  analyzer->process_electr_RunD_leg2(17);
  else if(runOn == "Electron_RunD_leg2_18")  analyzer->process_electr_RunD_leg2(18);
  else if(runOn == "Electron_RunD_leg2_19")  analyzer->process_electr_RunD_leg2(19);

  else if(runOn == "Electron_RunD_leg2_20")  analyzer->process_electr_RunD_leg2(20);
  else if(runOn == "Electron_RunD_leg2_21")  analyzer->process_electr_RunD_leg2(21);
  else if(runOn == "Electron_RunD_leg2_22")  analyzer->process_electr_RunD_leg2(22);
  else if(runOn == "Electron_RunD_leg2_23")  analyzer->process_electr_RunD_leg2(23);
  else if(runOn == "Electron_RunD_leg2_24")  analyzer->process_electr_RunD_leg2(24);
  else if(runOn == "Electron_RunD_leg2_25")  analyzer->process_electr_RunD_leg2(25);
  else if(runOn == "Electron_RunD_leg2_26")  analyzer->process_electr_RunD_leg2(26);
  else if(runOn == "Electron_RunD_leg2_27")  analyzer->process_electr_RunD_leg2(27);
  else if(runOn == "Electron_RunD_leg2_28")  analyzer->process_electr_RunD_leg2(28);
  else if(runOn == "Electron_RunD_leg2_29")  analyzer->process_electr_RunD_leg2(29);

  else if(runOn == "Electron_RunD_leg2_3")  analyzer->process_electr_RunD_leg2(3);
  else if(runOn == "Electron_RunD_leg2_4")  analyzer->process_electr_RunD_leg2(4);
  else if(runOn == "Electron_RunD_leg2_5")  analyzer->process_electr_RunD_leg2(5);
  else if(runOn == "Electron_RunD_leg2_6")  analyzer->process_electr_RunD_leg2(6);
  else if(runOn == "Electron_RunD_leg2_7")  analyzer->process_electr_RunD_leg2(7);
  else if(runOn == "Electron_RunD_leg2_8")  analyzer->process_electr_RunD_leg2(8);
  else if(runOn == "Electron_RunD_leg2_9")  analyzer->process_electr_RunD_leg2(9);
  else if(runOn == "Electron_RunD_leg2_0")  analyzer->process_electr_RunD_leg2(0);

  else if(runOn == "Electron_MisB_1")  analyzer->process_electr_MisB(1);
  else if(runOn == "Electron_MisB_2")  analyzer->process_electr_MisB(2);
  else if(runOn == "Electron_MisB_3")  analyzer->process_electr_MisB(3);
  else if(runOn == "Electron_MisB_4")  analyzer->process_electr_MisB(4);
  else if(runOn == "Electron_MisB_5")  analyzer->process_electr_MisB(5);
  else if(runOn == "Electron_MisB_6")  analyzer->process_electr_MisB(6);
  else if(runOn == "Electron_MisB_7")  analyzer->process_electr_MisB(7);
  else if(runOn == "Electron_MisB_8")  analyzer->process_electr_MisB(8);
  else if(runOn == "Electron_MisB_9")  analyzer->process_electr_MisB(9);


  else if(runOn == "Electron_RunC_mis1_10")  analyzer->process_electr_RunC_mis1(10);
  else if(runOn == "Electron_RunC_mis1_11")  analyzer->process_electr_RunC_mis1(11);
  else if(runOn == "Electron_RunC_mis1_12")  analyzer->process_electr_RunC_mis1(12);
  else if(runOn == "Electron_RunC_mis1_13")  analyzer->process_electr_RunC_mis1(13);
  else if(runOn == "Electron_RunC_mis1_14")  analyzer->process_electr_RunC_mis1(14);
  else if(runOn == "Electron_RunC_mis1_15")  analyzer->process_electr_RunC_mis1(15);
  else if(runOn == "Electron_RunC_mis1_16")  analyzer->process_electr_RunC_mis1(16);
  else if(runOn == "Electron_RunC_mis1_17")  analyzer->process_electr_RunC_mis1(17);
  else if(runOn == "Electron_RunC_mis1_18")  analyzer->process_electr_RunC_mis1(18);
  else if(runOn == "Electron_RunC_mis1_19")  analyzer->process_electr_RunC_mis1(19);

  else if(runOn == "Electron_RunC_mis1_2")  analyzer->process_electr_RunC_mis1(2);
  else if(runOn == "Electron_RunC_mis1_3")  analyzer->process_electr_RunC_mis1(3);
  else if(runOn == "Electron_RunC_mis1_4")  analyzer->process_electr_RunC_mis1(4);
  else if(runOn == "Electron_RunC_mis1_5")  analyzer->process_electr_RunC_mis1(5);
  else if(runOn == "Electron_RunC_mis1_6")  analyzer->process_electr_RunC_mis1(6);
  else if(runOn == "Electron_RunC_mis1_7")  analyzer->process_electr_RunC_mis1(7);
  else if(runOn == "Electron_RunC_mis1_8")  analyzer->process_electr_RunC_mis1(8);
  else if(runOn == "Electron_RunC_mis1_9")  analyzer->process_electr_RunC_mis1(9);


  else if(runOn == "Electron_RunC_mis2_10")  analyzer->process_electr_RunC_mis2(10);
  else if(runOn == "Electron_RunC_mis2_11")  analyzer->process_electr_RunC_mis2(11);
  else if(runOn == "Electron_RunC_mis2_12")  analyzer->process_electr_RunC_mis2(12);
  else if(runOn == "Electron_RunC_mis2_13")  analyzer->process_electr_RunC_mis2(13);
  else if(runOn == "Electron_RunC_mis2_14")  analyzer->process_electr_RunC_mis2(14);
  else if(runOn == "Electron_RunC_mis2_15")  analyzer->process_electr_RunC_mis2(15);
  else if(runOn == "Electron_RunC_mis2_16")  analyzer->process_electr_RunC_mis2(16);
  else if(runOn == "Electron_RunC_mis2_17")  analyzer->process_electr_RunC_mis2(17);
  else if(runOn == "Electron_RunC_mis2_18")  analyzer->process_electr_RunC_mis2(18);
  else if(runOn == "Electron_RunC_mis2_19")  analyzer->process_electr_RunC_mis2(19);

  else if(runOn == "Electron_RunC_mis2_2")  analyzer->process_electr_RunC_mis2(2);
  else if(runOn == "Electron_RunC_mis2_3")  analyzer->process_electr_RunC_mis2(3);
  else if(runOn == "Electron_RunC_mis2_4")  analyzer->process_electr_RunC_mis2(4);
  else if(runOn == "Electron_RunC_mis2_5")  analyzer->process_electr_RunC_mis2(5);
  else if(runOn == "Electron_RunC_mis2_6")  analyzer->process_electr_RunC_mis2(6);
  else if(runOn == "Electron_RunC_mis2_7")  analyzer->process_electr_RunC_mis2(7);
  else if(runOn == "Electron_RunC_mis2_8")  analyzer->process_electr_RunC_mis2(8);
  else if(runOn == "Electron_RunC_mis2_9")  analyzer->process_electr_RunC_mis2(9);


  else if(runOn == "Electron_RunD_mis1_10")  analyzer->process_electr_RunD_mis1(10);
  else if(runOn == "Electron_RunD_mis1_11")  analyzer->process_electr_RunD_mis1(11);
  else if(runOn == "Electron_RunD_mis1_12")  analyzer->process_electr_RunD_mis1(12);
  else if(runOn == "Electron_RunD_mis1_13")  analyzer->process_electr_RunD_mis1(13);
  else if(runOn == "Electron_RunD_mis1_14")  analyzer->process_electr_RunD_mis1(14);
  else if(runOn == "Electron_RunD_mis1_15")  analyzer->process_electr_RunD_mis1(15);
  else if(runOn == "Electron_RunD_mis1_16")  analyzer->process_electr_RunD_mis1(16);
  else if(runOn == "Electron_RunD_mis1_17")  analyzer->process_electr_RunD_mis1(17);
  else if(runOn == "Electron_RunD_mis1_18")  analyzer->process_electr_RunD_mis1(18);
  else if(runOn == "Electron_RunD_mis1_19")  analyzer->process_electr_RunD_mis1(19);

  else if(runOn == "Electron_RunD_mis1_2")  analyzer->process_electr_RunD_mis1(2);
  else if(runOn == "Electron_RunD_mis1_3")  analyzer->process_electr_RunD_mis1(3);
  else if(runOn == "Electron_RunD_mis1_4")  analyzer->process_electr_RunD_mis1(4);
  else if(runOn == "Electron_RunD_mis1_5")  analyzer->process_electr_RunD_mis1(5);
  else if(runOn == "Electron_RunD_mis1_6")  analyzer->process_electr_RunD_mis1(6);
  else if(runOn == "Electron_RunD_mis1_7")  analyzer->process_electr_RunD_mis1(7);
  else if(runOn == "Electron_RunD_mis1_8")  analyzer->process_electr_RunD_mis1(8);
  else if(runOn == "Electron_RunD_mis1_9")  analyzer->process_electr_RunD_mis1(9);

  else if(runOn == "Electron_RunD_mis2_10")  analyzer->process_electr_RunD_mis2(10);
  else if(runOn == "Electron_RunD_mis2_11")  analyzer->process_electr_RunD_mis2(11);
  else if(runOn == "Electron_RunD_mis2_12")  analyzer->process_electr_RunD_mis2(12);
  else if(runOn == "Electron_RunD_mis2_13")  analyzer->process_electr_RunD_mis2(13);
  else if(runOn == "Electron_RunD_mis2_14")  analyzer->process_electr_RunD_mis2(14);
  else if(runOn == "Electron_RunD_mis2_15")  analyzer->process_electr_RunD_mis2(15);
  else if(runOn == "Electron_RunD_mis2_16")  analyzer->process_electr_RunD_mis2(16);
  else if(runOn == "Electron_RunD_mis2_17")  analyzer->process_electr_RunD_mis2(17);
  else if(runOn == "Electron_RunD_mis2_18")  analyzer->process_electr_RunD_mis2(18);
  else if(runOn == "Electron_RunD_mis2_19")  analyzer->process_electr_RunD_mis2(19);

  else if(runOn == "Electron_RunD_mis2_2")  analyzer->process_electr_RunD_mis2(2);
  else if(runOn == "Electron_RunD_mis2_3")  analyzer->process_electr_RunD_mis2(3);
  else if(runOn == "Electron_RunD_mis2_4")  analyzer->process_electr_RunD_mis2(4);
  else if(runOn == "Electron_RunD_mis2_5")  analyzer->process_electr_RunD_mis2(5);
  else if(runOn == "Electron_RunD_mis2_6")  analyzer->process_electr_RunD_mis2(6);
  else if(runOn == "Electron_RunD_mis2_7")  analyzer->process_electr_RunD_mis2(7);
  else if(runOn == "Electron_RunD_mis2_8")  analyzer->process_electr_RunD_mis2(8);
  else if(runOn == "Electron_RunD_mis2_9")  analyzer->process_electr_RunD_mis2(9);
    
//   else if(runOn == "qcd_EM_Pt30to80_10")     analyzer->process_qcd_EM_Pt30to80(10);
//   else if(runOn == "qcd_EM_Pt30to80_11")     analyzer->process_qcd_EM_Pt30to80(11);
//   else if(runOn == "qcd_EM_Pt30to80_12")     analyzer->process_qcd_EM_Pt30to80(12);
//   else if(runOn == "qcd_EM_Pt30to80_13")     analyzer->process_qcd_EM_Pt30to80(13);
//   else if(runOn == "qcd_EM_Pt30to80_14")     analyzer->process_qcd_EM_Pt30to80(14);
//   else if(runOn == "qcd_EM_Pt30to80_15")     analyzer->process_qcd_EM_Pt30to80(15);
//   else if(runOn == "qcd_EM_Pt30to80_16")     analyzer->process_qcd_EM_Pt30to80(16);
//   else if(runOn == "qcd_EM_Pt30to80_17")     analyzer->process_qcd_EM_Pt30to80(17);
//   else if(runOn == "qcd_EM_Pt30to80_18")     analyzer->process_qcd_EM_Pt30to80(18);
//   else if(runOn == "qcd_EM_Pt30to80_19")     analyzer->process_qcd_EM_Pt30to80(19);
//   else if(runOn == "qcd_EM_Pt30to80_20")     analyzer->process_qcd_EM_Pt30to80(20);
//   else if(runOn == "qcd_EM_Pt30to80_21")     analyzer->process_qcd_EM_Pt30to80(21);
//   else if(runOn == "qcd_EM_Pt30to80_22")     analyzer->process_qcd_EM_Pt30to80(22);
//   else if(runOn == "qcd_EM_Pt30to80_23")     analyzer->process_qcd_EM_Pt30to80(23);
//   else if(runOn == "qcd_EM_Pt30to80_24")     analyzer->process_qcd_EM_Pt30to80(24);
//   else if(runOn == "qcd_EM_Pt30to80_25")     analyzer->process_qcd_EM_Pt30to80(25);
//   else if(runOn == "qcd_EM_Pt30to80_26")     analyzer->process_qcd_EM_Pt30to80(26);
//   else if(runOn == "qcd_EM_Pt30to80_27")     analyzer->process_qcd_EM_Pt30to80(27);
//   else if(runOn == "qcd_EM_Pt30to80_28")     analyzer->process_qcd_EM_Pt30to80(28);
//   else if(runOn == "qcd_EM_Pt30to80_29")     analyzer->process_qcd_EM_Pt30to80(29);
//   else if(runOn == "qcd_EM_Pt30to80_0")     analyzer->process_qcd_EM_Pt30to80(0);
//   else if(runOn == "qcd_EM_Pt30to80_3")     analyzer->process_qcd_EM_Pt30to80(3);
//   else if(runOn == "qcd_EM_Pt30to80_4")     analyzer->process_qcd_EM_Pt30to80(4);
//   else if(runOn == "qcd_EM_Pt30to80_5")     analyzer->process_qcd_EM_Pt30to80(5);
//   else if(runOn == "qcd_EM_Pt30to80_6")     analyzer->process_qcd_EM_Pt30to80(6);
//   else if(runOn == "qcd_EM_Pt30to80_7")     analyzer->process_qcd_EM_Pt30to80(7);
//   else if(runOn == "qcd_EM_Pt30to80_8")     analyzer->process_qcd_EM_Pt30to80(8);
//   else if(runOn == "qcd_EM_Pt30to80_9")     analyzer->process_qcd_EM_Pt30to80(9);

//   else if(runOn == "qcd_EM_Pt80to170_10")     analyzer->process_qcd_EM_Pt80to170(10);
//   else if(runOn == "qcd_EM_Pt80to170_11")     analyzer->process_qcd_EM_Pt80to170(11);
//   else if(runOn == "qcd_EM_Pt80to170_12")     analyzer->process_qcd_EM_Pt80to170(12);
//   else if(runOn == "qcd_EM_Pt80to170_13")     analyzer->process_qcd_EM_Pt80to170(13);
//   else if(runOn == "qcd_EM_Pt80to170_14")     analyzer->process_qcd_EM_Pt80to170(14);
//   else if(runOn == "qcd_EM_Pt80to170_15")     analyzer->process_qcd_EM_Pt80to170(15);
//   else if(runOn == "qcd_EM_Pt80to170_16")     analyzer->process_qcd_EM_Pt80to170(16);
//   else if(runOn == "qcd_EM_Pt80to170_17")     analyzer->process_qcd_EM_Pt80to170(17);
//   else if(runOn == "qcd_EM_Pt80to170_18")     analyzer->process_qcd_EM_Pt80to170(18);
//   else if(runOn == "qcd_EM_Pt80to170_19")     analyzer->process_qcd_EM_Pt80to170(19);
//   else if(runOn == "qcd_EM_Pt80to170_20")     analyzer->process_qcd_EM_Pt80to170(20);
//   else if(runOn == "qcd_EM_Pt80to170_21")     analyzer->process_qcd_EM_Pt80to170(21);
//   else if(runOn == "qcd_EM_Pt80to170_22")     analyzer->process_qcd_EM_Pt80to170(22);
//   else if(runOn == "qcd_EM_Pt80to170_23")     analyzer->process_qcd_EM_Pt80to170(23);
//   else if(runOn == "qcd_EM_Pt80to170_24")     analyzer->process_qcd_EM_Pt80to170(24);
//   else if(runOn == "qcd_EM_Pt80to170_25")     analyzer->process_qcd_EM_Pt80to170(25);
//   else if(runOn == "qcd_EM_Pt80to170_26")     analyzer->process_qcd_EM_Pt80to170(26);
//   else if(runOn == "qcd_EM_Pt80to170_27")     analyzer->process_qcd_EM_Pt80to170(27);
//   else if(runOn == "qcd_EM_Pt80to170_28")     analyzer->process_qcd_EM_Pt80to170(28);
//   else if(runOn == "qcd_EM_Pt80to170_29")     analyzer->process_qcd_EM_Pt80to170(29);
//   else if(runOn == "qcd_EM_Pt80to170_0")     analyzer->process_qcd_EM_Pt80to170(0);
//   else if(runOn == "qcd_EM_Pt80to170_3")     analyzer->process_qcd_EM_Pt80to170(3);
//   else if(runOn == "qcd_EM_Pt80to170_4")     analyzer->process_qcd_EM_Pt80to170(4);
//   else if(runOn == "qcd_EM_Pt80to170_5")     analyzer->process_qcd_EM_Pt80to170(5);
//   else if(runOn == "qcd_EM_Pt80to170_6")     analyzer->process_qcd_EM_Pt80to170(6);
//   else if(runOn == "qcd_EM_Pt80to170_7")     analyzer->process_qcd_EM_Pt80to170(7);
//   else if(runOn == "qcd_EM_Pt80to170_8")     analyzer->process_qcd_EM_Pt80to170(8);
//   else if(runOn == "qcd_EM_Pt80to170_9")     analyzer->process_qcd_EM_Pt80to170(9);
 
//   else if(runOn == "qcd_EM_Pt170to250_10")     analyzer->process_qcd_EM_Pt170to250(10);
//   else if(runOn == "qcd_EM_Pt170to250_11")     analyzer->process_qcd_EM_Pt170to250(11);
//   else if(runOn == "qcd_EM_Pt170to250_12")     analyzer->process_qcd_EM_Pt170to250(12);
//   else if(runOn == "qcd_EM_Pt170to250_13")     analyzer->process_qcd_EM_Pt170to250(13);
//   else if(runOn == "qcd_EM_Pt170to250_14")     analyzer->process_qcd_EM_Pt170to250(14);
//   else if(runOn == "qcd_EM_Pt170to250_15")     analyzer->process_qcd_EM_Pt170to250(15);
//   else if(runOn == "qcd_EM_Pt170to250_16")     analyzer->process_qcd_EM_Pt170to250(16);
//   else if(runOn == "qcd_EM_Pt170to250_17")     analyzer->process_qcd_EM_Pt170to250(17);
//   else if(runOn == "qcd_EM_Pt170to250_18")     analyzer->process_qcd_EM_Pt170to250(18);
//   else if(runOn == "qcd_EM_Pt170to250_19")     analyzer->process_qcd_EM_Pt170to250(19);
//   else if(runOn == "qcd_EM_Pt170to250_20")     analyzer->process_qcd_EM_Pt170to250(20);
//   else if(runOn == "qcd_EM_Pt170to250_21")     analyzer->process_qcd_EM_Pt170to250(21);
//   else if(runOn == "qcd_EM_Pt170to250_22")     analyzer->process_qcd_EM_Pt170to250(22);
//   else if(runOn == "qcd_EM_Pt170to250_23")     analyzer->process_qcd_EM_Pt170to250(23);
//   else if(runOn == "qcd_EM_Pt170to250_24")     analyzer->process_qcd_EM_Pt170to250(24);
//   else if(runOn == "qcd_EM_Pt170to250_25")     analyzer->process_qcd_EM_Pt170to250(25);
//   else if(runOn == "qcd_EM_Pt170to250_26")     analyzer->process_qcd_EM_Pt170to250(26);
//   else if(runOn == "qcd_EM_Pt170to250_27")     analyzer->process_qcd_EM_Pt170to250(27);
//   else if(runOn == "qcd_EM_Pt170to250_28")     analyzer->process_qcd_EM_Pt170to250(28);
//   else if(runOn == "qcd_EM_Pt170to250_29")     analyzer->process_qcd_EM_Pt170to250(29);
//   else if(runOn == "qcd_EM_Pt170to250_0")     analyzer->process_qcd_EM_Pt170to250(0);
//   else if(runOn == "qcd_EM_Pt170to250_3")     analyzer->process_qcd_EM_Pt170to250(3);
//   else if(runOn == "qcd_EM_Pt170to250_4")     analyzer->process_qcd_EM_Pt170to250(4);
//   else if(runOn == "qcd_EM_Pt170to250_5")     analyzer->process_qcd_EM_Pt170to250(5);
//   else if(runOn == "qcd_EM_Pt170to250_6")     analyzer->process_qcd_EM_Pt170to250(6);
//   else if(runOn == "qcd_EM_Pt170to250_7")     analyzer->process_qcd_EM_Pt170to250(7);
//   else if(runOn == "qcd_EM_Pt170to250_8")     analyzer->process_qcd_EM_Pt170to250(8);
//   else if(runOn == "qcd_EM_Pt170to250_9")     analyzer->process_qcd_EM_Pt170to250(9);

//   else if(runOn == "qcd_EM_Pt250to350_10")     analyzer->process_qcd_EM_Pt250to350(10);
//   else if(runOn == "qcd_EM_Pt250to350_11")     analyzer->process_qcd_EM_Pt250to350(11);
//   else if(runOn == "qcd_EM_Pt250to350_12")     analyzer->process_qcd_EM_Pt250to350(12);
//   else if(runOn == "qcd_EM_Pt250to350_13")     analyzer->process_qcd_EM_Pt250to350(13);
//   else if(runOn == "qcd_EM_Pt250to350_14")     analyzer->process_qcd_EM_Pt250to350(14);
//   else if(runOn == "qcd_EM_Pt250to350_15")     analyzer->process_qcd_EM_Pt250to350(15);
//   else if(runOn == "qcd_EM_Pt250to350_16")     analyzer->process_qcd_EM_Pt250to350(16);
//   else if(runOn == "qcd_EM_Pt250to350_17")     analyzer->process_qcd_EM_Pt250to350(17);
//   else if(runOn == "qcd_EM_Pt250to350_18")     analyzer->process_qcd_EM_Pt250to350(18);
//   else if(runOn == "qcd_EM_Pt250to350_19")     analyzer->process_qcd_EM_Pt250to350(19);
//   else if(runOn == "qcd_EM_Pt250to350_20")     analyzer->process_qcd_EM_Pt250to350(20);
//   else if(runOn == "qcd_EM_Pt250to350_21")     analyzer->process_qcd_EM_Pt250to350(21);
//   else if(runOn == "qcd_EM_Pt250to350_22")     analyzer->process_qcd_EM_Pt250to350(22);
//   else if(runOn == "qcd_EM_Pt250to350_23")     analyzer->process_qcd_EM_Pt250to350(23);
//   else if(runOn == "qcd_EM_Pt250to350_24")     analyzer->process_qcd_EM_Pt250to350(24);
//   else if(runOn == "qcd_EM_Pt250to350_25")     analyzer->process_qcd_EM_Pt250to350(25);
//   else if(runOn == "qcd_EM_Pt250to350_26")     analyzer->process_qcd_EM_Pt250to350(26);
//   else if(runOn == "qcd_EM_Pt250to350_27")     analyzer->process_qcd_EM_Pt250to350(27);
//   else if(runOn == "qcd_EM_Pt250to350_28")     analyzer->process_qcd_EM_Pt250to350(28);
//   else if(runOn == "qcd_EM_Pt250to350_29")     analyzer->process_qcd_EM_Pt250to350(29);
//   else if(runOn == "qcd_EM_Pt250to350_0")     analyzer->process_qcd_EM_Pt250to350(0);
//   else if(runOn == "qcd_EM_Pt250to350_3")     analyzer->process_qcd_EM_Pt250to350(3);
//   else if(runOn == "qcd_EM_Pt250to350_4")     analyzer->process_qcd_EM_Pt250to350(4);
//   else if(runOn == "qcd_EM_Pt250to350_5")     analyzer->process_qcd_EM_Pt250to350(5);
//   else if(runOn == "qcd_EM_Pt250to350_6")     analyzer->process_qcd_EM_Pt250to350(6);
//   else if(runOn == "qcd_EM_Pt250to350_7")     analyzer->process_qcd_EM_Pt250to350(7);
//   else if(runOn == "qcd_EM_Pt250to350_8")     analyzer->process_qcd_EM_Pt250to350(8);
//   else if(runOn == "qcd_EM_Pt250to350_9")     analyzer->process_qcd_EM_Pt250to350(9);

//   else if(runOn == "qcd_EM_Pt350toinf_10")     analyzer->process_qcd_EM_Pt350toinf(10);
//   else if(runOn == "qcd_EM_Pt350toinf_11")     analyzer->process_qcd_EM_Pt350toinf(11);
//   else if(runOn == "qcd_EM_Pt350toinf_12")     analyzer->process_qcd_EM_Pt350toinf(12);
//   else if(runOn == "qcd_EM_Pt350toinf_13")     analyzer->process_qcd_EM_Pt350toinf(13);
//   else if(runOn == "qcd_EM_Pt350toinf_14")     analyzer->process_qcd_EM_Pt350toinf(14);
//   else if(runOn == "qcd_EM_Pt350toinf_15")     analyzer->process_qcd_EM_Pt350toinf(15);
//   else if(runOn == "qcd_EM_Pt350toinf_16")     analyzer->process_qcd_EM_Pt350toinf(16);
//   else if(runOn == "qcd_EM_Pt350toinf_17")     analyzer->process_qcd_EM_Pt350toinf(17);
//   else if(runOn == "qcd_EM_Pt350toinf_18")     analyzer->process_qcd_EM_Pt350toinf(18);
//   else if(runOn == "qcd_EM_Pt350toinf_19")     analyzer->process_qcd_EM_Pt350toinf(19);
//   else if(runOn == "qcd_EM_Pt350toinf_20")     analyzer->process_qcd_EM_Pt350toinf(20);
//   else if(runOn == "qcd_EM_Pt350toinf_21")     analyzer->process_qcd_EM_Pt350toinf(21);
//   else if(runOn == "qcd_EM_Pt350toinf_22")     analyzer->process_qcd_EM_Pt350toinf(22);
//   else if(runOn == "qcd_EM_Pt350toinf_23")     analyzer->process_qcd_EM_Pt350toinf(23);
//   else if(runOn == "qcd_EM_Pt350toinf_24")     analyzer->process_qcd_EM_Pt350toinf(24);
//   else if(runOn == "qcd_EM_Pt350toinf_25")     analyzer->process_qcd_EM_Pt350toinf(25);
//   else if(runOn == "qcd_EM_Pt350toinf_26")     analyzer->process_qcd_EM_Pt350toinf(26);
//   else if(runOn == "qcd_EM_Pt350toinf_27")     analyzer->process_qcd_EM_Pt350toinf(27);
//   else if(runOn == "qcd_EM_Pt350toinf_28")     analyzer->process_qcd_EM_Pt350toinf(28);
//   else if(runOn == "qcd_EM_Pt350toinf_29")     analyzer->process_qcd_EM_Pt350toinf(29);
//   else if(runOn == "qcd_EM_Pt350toinf_0")     analyzer->process_qcd_EM_Pt350toinf(0);
//   else if(runOn == "qcd_EM_Pt350toinf_3")     analyzer->process_qcd_EM_Pt350toinf(3);
//   else if(runOn == "qcd_EM_Pt350toinf_4")     analyzer->process_qcd_EM_Pt350toinf(4);
//   else if(runOn == "qcd_EM_Pt350toinf_5")     analyzer->process_qcd_EM_Pt350toinf(5);
//   else if(runOn == "qcd_EM_Pt350toinf_6")     analyzer->process_qcd_EM_Pt350toinf(6);
//   else if(runOn == "qcd_EM_Pt350toinf_7")     analyzer->process_qcd_EM_Pt350toinf(7);
//   else if(runOn == "qcd_EM_Pt350toinf_8")     analyzer->process_qcd_EM_Pt350toinf(8);
//   else if(runOn == "qcd_EM_Pt350toinf_9")     analyzer->process_qcd_EM_Pt350toinf(9);

//   else if(runOn == "qcd_30to80_BCtoE_1")     analyzer->process_qcd_30to80_BCtoE(1);
//   else if(runOn == "qcd_30to80_BCtoE_2")     analyzer->process_qcd_30to80_BCtoE(2);
//   else if(runOn == "qcd_30to80_BCtoE_3")     analyzer->process_qcd_30to80_BCtoE(3);
//   else if(runOn == "qcd_30to80_BCtoE_4")     analyzer->process_qcd_30to80_BCtoE(4);
//   else if(runOn == "qcd_30to80_BCtoE_5")     analyzer->process_qcd_30to80_BCtoE(5);
//   else if(runOn == "qcd_30to80_BCtoE_6")     analyzer->process_qcd_30to80_BCtoE(6);
//   else if(runOn == "qcd_30to80_BCtoE_7")     analyzer->process_qcd_30to80_BCtoE(7);
//   else if(runOn == "qcd_30to80_BCtoE_8")     analyzer->process_qcd_30to80_BCtoE(8);
//   else if(runOn == "qcd_30to80_BCtoE_9")     analyzer->process_qcd_30to80_BCtoE(9);

//   else if(runOn == "qcd_80to170_BCtoE_1")     analyzer->process_qcd_80to170_BCtoE(1);
//   else if(runOn == "qcd_80to170_BCtoE_2")     analyzer->process_qcd_80to170_BCtoE(2);
//   else if(runOn == "qcd_80to170_BCtoE_3")     analyzer->process_qcd_80to170_BCtoE(3);
//   else if(runOn == "qcd_80to170_BCtoE_4")     analyzer->process_qcd_80to170_BCtoE(4);
//   else if(runOn == "qcd_80to170_BCtoE_5")     analyzer->process_qcd_80to170_BCtoE(5);
//   else if(runOn == "qcd_80to170_BCtoE_6")     analyzer->process_qcd_80to170_BCtoE(6);
//   else if(runOn == "qcd_80to170_BCtoE_7")     analyzer->process_qcd_80to170_BCtoE(7);
//   else if(runOn == "qcd_80to170_BCtoE_8")     analyzer->process_qcd_80to170_BCtoE(8);
//   else if(runOn == "qcd_80to170_BCtoE_9")     analyzer->process_qcd_80to170_BCtoE(9);

//   else if(runOn == "qcd_170to250_BCtoE_1")     analyzer->process_qcd_170to250_BCtoE(1);
//   else if(runOn == "qcd_170to250_BCtoE_2")     analyzer->process_qcd_170to250_BCtoE(2);
//   else if(runOn == "qcd_170to250_BCtoE_3")     analyzer->process_qcd_170to250_BCtoE(3);
//   else if(runOn == "qcd_170to250_BCtoE_4")     analyzer->process_qcd_170to250_BCtoE(4);
//   else if(runOn == "qcd_170to250_BCtoE_5")     analyzer->process_qcd_170to250_BCtoE(5);
//   else if(runOn == "qcd_170to250_BCtoE_6")     analyzer->process_qcd_170to250_BCtoE(6);
//   else if(runOn == "qcd_170to250_BCtoE_7")     analyzer->process_qcd_170to250_BCtoE(7);
//   else if(runOn == "qcd_170to250_BCtoE_8")     analyzer->process_qcd_170to250_BCtoE(8);
//   else if(runOn == "qcd_170to250_BCtoE_9")     analyzer->process_qcd_170to250_BCtoE(9);

//   else if(runOn == "qcd_250to350_BCtoE_1")     analyzer->process_qcd_250to350_BCtoE(1);
//   else if(runOn == "qcd_250to350_BCtoE_2")     analyzer->process_qcd_250to350_BCtoE(2);
//   else if(runOn == "qcd_250to350_BCtoE_3")     analyzer->process_qcd_250to350_BCtoE(3);
//   else if(runOn == "qcd_250to350_BCtoE_4")     analyzer->process_qcd_250to350_BCtoE(4);
//   else if(runOn == "qcd_250to350_BCtoE_5")     analyzer->process_qcd_250to350_BCtoE(5);
//   else if(runOn == "qcd_250to350_BCtoE_6")     analyzer->process_qcd_250to350_BCtoE(6);
//   else if(runOn == "qcd_250to350_BCtoE_7")     analyzer->process_qcd_250to350_BCtoE(7);
//   else if(runOn == "qcd_250to350_BCtoE_8")     analyzer->process_qcd_250to350_BCtoE(8);
//   else if(runOn == "qcd_250to350_BCtoE_9")     analyzer->process_qcd_250to350_BCtoE(9);

//   else if(runOn == "qcd_350toinf_BCtoE_1")     analyzer->process_qcd_350toinf_BCtoE(1);
//   else if(runOn == "qcd_350toinf_BCtoE_2")     analyzer->process_qcd_350toinf_BCtoE(2);
//   else if(runOn == "qcd_350toinf_BCtoE_3")     analyzer->process_qcd_350toinf_BCtoE(3);
//   else if(runOn == "qcd_350toinf_BCtoE_4")     analyzer->process_qcd_350toinf_BCtoE(4);
//   else if(runOn == "qcd_350toinf_BCtoE_5")     analyzer->process_qcd_350toinf_BCtoE(5);
//   else if(runOn == "qcd_350toinf_BCtoE_6")     analyzer->process_qcd_350toinf_BCtoE(6);
//   else if(runOn == "qcd_350toinf_BCtoE_7")     analyzer->process_qcd_350toinf_BCtoE(7);
//   else if(runOn == "qcd_350toinf_BCtoE_8")     analyzer->process_qcd_350toinf_BCtoE(8);
//   else if(runOn == "qcd_350toinf_BCtoE_9")     analyzer->process_qcd_350toinf_BCtoE(9);

//   else if(runOn == "PhotonJets_15to30_1")     analyzer->process_PhotonJets_15to30(1);
//   else if(runOn == "PhotonJets_15to30_2")     analyzer->process_PhotonJets_15to30(2);
//   else if(runOn == "PhotonJets_15to30_3")     analyzer->process_PhotonJets_15to30(3);
//   else if(runOn == "PhotonJets_15to30_4")     analyzer->process_PhotonJets_15to30(4);
//   else if(runOn == "PhotonJets_15to30_5")     analyzer->process_PhotonJets_15to30(5);
//   else if(runOn == "PhotonJets_15to30_6")     analyzer->process_PhotonJets_15to30(6);
//   else if(runOn == "PhotonJets_15to30_7")     analyzer->process_PhotonJets_15to30(7);
//   else if(runOn == "PhotonJets_15to30_8")     analyzer->process_PhotonJets_15to30(8);
//   else if(runOn == "PhotonJets_15to30_9")     analyzer->process_PhotonJets_15to30(9);

//   else if(runOn == "PhotonJets_30to50_1")     analyzer->process_PhotonJets_30to50(1);
//   else if(runOn == "PhotonJets_30to50_2")     analyzer->process_PhotonJets_30to50(2);
//   else if(runOn == "PhotonJets_30to50_3")     analyzer->process_PhotonJets_30to50(3);
//   else if(runOn == "PhotonJets_30to50_4")     analyzer->process_PhotonJets_30to50(4);
//   else if(runOn == "PhotonJets_30to50_5")     analyzer->process_PhotonJets_30to50(5);
//   else if(runOn == "PhotonJets_30to50_6")     analyzer->process_PhotonJets_30to50(6);
//   else if(runOn == "PhotonJets_30to50_7")     analyzer->process_PhotonJets_30to50(7);
//   else if(runOn == "PhotonJets_30to50_8")     analyzer->process_PhotonJets_30to50(8);
//   else if(runOn == "PhotonJets_30to50_9")     analyzer->process_PhotonJets_30to50(9);

//   else if(runOn == "PhotonJets_50to80_1")     analyzer->process_PhotonJets_50to80(1);
//   else if(runOn == "PhotonJets_50to80_2")     analyzer->process_PhotonJets_50to80(2);
//   else if(runOn == "PhotonJets_50to80_3")     analyzer->process_PhotonJets_50to80(3);
//   else if(runOn == "PhotonJets_50to80_4")     analyzer->process_PhotonJets_50to80(4);
//   else if(runOn == "PhotonJets_50to80_5")     analyzer->process_PhotonJets_50to80(5);
//   else if(runOn == "PhotonJets_50to80_6")     analyzer->process_PhotonJets_50to80(6);
//   else if(runOn == "PhotonJets_50to80_7")     analyzer->process_PhotonJets_50to80(7);
//   else if(runOn == "PhotonJets_50to80_8")     analyzer->process_PhotonJets_50to80(8);
//   else if(runOn == "PhotonJets_50to80_9")     analyzer->process_PhotonJets_50to80(9);

//   else if(runOn == "PhotonJets_80to120_1")     analyzer->process_PhotonJets_80to120(1);
//   else if(runOn == "PhotonJets_80to120_2")     analyzer->process_PhotonJets_80to120(2);
//   else if(runOn == "PhotonJets_80to120_3")     analyzer->process_PhotonJets_80to120(3);
//   else if(runOn == "PhotonJets_80to120_4")     analyzer->process_PhotonJets_80to120(4);
//   else if(runOn == "PhotonJets_80to120_5")     analyzer->process_PhotonJets_80to120(5);
//   else if(runOn == "PhotonJets_80to120_6")     analyzer->process_PhotonJets_80to120(6);
//   else if(runOn == "PhotonJets_80to120_7")     analyzer->process_PhotonJets_80to120(7);
//   else if(runOn == "PhotonJets_80to120_8")     analyzer->process_PhotonJets_80to120(8);
//   else if(runOn == "PhotonJets_80to120_9")     analyzer->process_PhotonJets_80to120(9);

//   else if(runOn == "PhotonJets_120to170_1")     analyzer->process_PhotonJets_120to170(1);
//   else if(runOn == "PhotonJets_120to170_2")     analyzer->process_PhotonJets_120to170(2);
//   else if(runOn == "PhotonJets_120to170_3")     analyzer->process_PhotonJets_120to170(3);
//   else if(runOn == "PhotonJets_120to170_4")     analyzer->process_PhotonJets_120to170(4);
//   else if(runOn == "PhotonJets_120to170_5")     analyzer->process_PhotonJets_120to170(5);
//   else if(runOn == "PhotonJets_120to170_6")     analyzer->process_PhotonJets_120to170(6);
//   else if(runOn == "PhotonJets_120to170_7")     analyzer->process_PhotonJets_120to170(7);
//   else if(runOn == "PhotonJets_120to170_8")     analyzer->process_PhotonJets_120to170(8);
//   else if(runOn == "PhotonJets_120to170_9")     analyzer->process_PhotonJets_120to170(9);

//   else if(runOn == "PhotonJets_170to300_1")     analyzer->process_PhotonJets_170to300(1);
//   else if(runOn == "PhotonJets_170to300_2")     analyzer->process_PhotonJets_170to300(2);
//   else if(runOn == "PhotonJets_170to300_3")     analyzer->process_PhotonJets_170to300(3);
//   else if(runOn == "PhotonJets_170to300_4")     analyzer->process_PhotonJets_170to300(4);
//   else if(runOn == "PhotonJets_170to300_5")     analyzer->process_PhotonJets_170to300(5);
//   else if(runOn == "PhotonJets_170to300_6")     analyzer->process_PhotonJets_170to300(6);
//   else if(runOn == "PhotonJets_170to300_7")     analyzer->process_PhotonJets_170to300(7);
//   else if(runOn == "PhotonJets_170to300_8")     analyzer->process_PhotonJets_170to300(8);
//   else if(runOn == "PhotonJets_170to300_9")     analyzer->process_PhotonJets_170to300(9);

//   else if(runOn == "PhotonJets_300to470_1")     analyzer->process_PhotonJets_300to470(1);
//   else if(runOn == "PhotonJets_300to470_2")     analyzer->process_PhotonJets_300to470(2);
//   else if(runOn == "PhotonJets_300to470_3")     analyzer->process_PhotonJets_300to470(3);
//   else if(runOn == "PhotonJets_300to470_4")     analyzer->process_PhotonJets_300to470(4);
//   else if(runOn == "PhotonJets_300to470_5")     analyzer->process_PhotonJets_300to470(5);
//   else if(runOn == "PhotonJets_300to470_6")     analyzer->process_PhotonJets_300to470(6);
//   else if(runOn == "PhotonJets_300to470_7")     analyzer->process_PhotonJets_300to470(7);
//   else if(runOn == "PhotonJets_300to470_8")     analyzer->process_PhotonJets_300to470(8);
//   else if(runOn == "PhotonJets_300to470_9")     analyzer->process_PhotonJets_300to470(9);

  else if(runOn == "stop_s")              analyzer->process_stop_s();

//   else if(runOn == "stop_tW_1")              analyzer->process_stop_tW(1);
//   else if(runOn == "stop_tW_2")              analyzer->process_stop_tW(2);

  else if(runOn == "stop_t_1")              analyzer->process_stop_t(1);
  else if(runOn == "stop_t_2")              analyzer->process_stop_t(2);
  else if(runOn == "stop_t_3")              analyzer->process_stop_t(3);
  else if(runOn == "stop_t_4")              analyzer->process_stop_t(4);
  else if(runOn == "stop_t_5")              analyzer->process_stop_t(5);
  else if(runOn == "stop_t_6")              analyzer->process_stop_t(6);
  else if(runOn == "stop_t_7")              analyzer->process_stop_t(7);
  else if(runOn == "stop_t_8")              analyzer->process_stop_t(8);
  else if(runOn == "stop_t_9")              analyzer->process_stop_t(9);
  else if(runOn == "stop_t_10")              analyzer->process_stop_t(10);
  else if(runOn == "stop_t_11")              analyzer->process_stop_t(11);
  //  //  else if(runOn == "stop_t_12")              analyzer->process_stop_t(12);

//   else if(runOn == "ttbar_etau_1")         analyzer->process_ttbar_etau(1)    ;
//   else if(runOn == "ttbar_etau_2")         analyzer->process_ttbar_etau(2)    ;
//   else if(runOn == "ttbar_etau_3")         analyzer->process_ttbar_etau(3)    ;
//   else if(runOn == "ttbar_etau_4")         analyzer->process_ttbar_etau(4)    ;
//   else if(runOn == "ttbar_etau_5")         analyzer->process_ttbar_etau(5)    ;
//   else if(runOn == "ttbar_etau_6")         analyzer->process_ttbar_etau(6)    ;
//   else if(runOn == "ttbar_etau_7")         analyzer->process_ttbar_etau(7)    ;
//   else if(runOn == "ttbar_etau_8")         analyzer->process_ttbar_etau(8)    ;
//   else if(runOn == "ttbar_etau_9")         analyzer->process_ttbar_etau(9)    ;
//   else if(runOn == "ttbar_etau_10")         analyzer->process_ttbar_etau(10)    ;

//   else if(runOn == "ttbar_ddbkg_1")         analyzer->process_ttbar_ddbkg(1)    ;
//   else if(runOn == "ttbar_ddbkg_2")         analyzer->process_ttbar_ddbkg(2)    ;
//   else if(runOn == "ttbar_ddbkg_3")         analyzer->process_ttbar_ddbkg(3)    ;
//   else if(runOn == "ttbar_ddbkg_4")         analyzer->process_ttbar_ddbkg(4)    ;
//   else if(runOn == "ttbar_ddbkg_5")         analyzer->process_ttbar_ddbkg(5)    ;
//   else if(runOn == "ttbar_ddbkg_6")         analyzer->process_ttbar_ddbkg(6)    ;
//   else if(runOn == "ttbar_ddbkg_7")         analyzer->process_ttbar_ddbkg(7)    ;
//   else if(runOn == "ttbar_ddbkg_8")         analyzer->process_ttbar_ddbkg(8)    ;
//   else if(runOn == "ttbar_ddbkg_9")         analyzer->process_ttbar_ddbkg(9)    ;
//   else if(runOn == "ttbar_ddbkg_10")         analyzer->process_ttbar_ddbkg(10)    ;

//   else if(runOn == "ttbar_mcbkg_1")         analyzer->process_ttbar_mcbkg(1)    ;
//   else if(runOn == "ttbar_mcbkg_2")         analyzer->process_ttbar_mcbkg(2)    ;
//   else if(runOn == "ttbar_mcbkg_3")         analyzer->process_ttbar_mcbkg(3)    ;
//   else if(runOn == "ttbar_mcbkg_4")         analyzer->process_ttbar_mcbkg(4)    ;
//   else if(runOn == "ttbar_mcbkg_5")         analyzer->process_ttbar_mcbkg(5)    ;
//   else if(runOn == "ttbar_mcbkg_6")         analyzer->process_ttbar_mcbkg(6)    ;
//   else if(runOn == "ttbar_mcbkg_7")         analyzer->process_ttbar_mcbkg(7)    ;
//   else if(runOn == "ttbar_mcbkg_8")         analyzer->process_ttbar_mcbkg(8)    ;
//   else if(runOn == "ttbar_mcbkg_9")         analyzer->process_ttbar_mcbkg(9)    ;
//   else if(runOn == "ttbar_mcbkg_10")         analyzer->process_ttbar_mcbkg(10)    ;

//   else if(runOn == "ttbar_1")         analyzer->process_ttbar(1)    ;
//   else if(runOn == "ttbar_2")         analyzer->process_ttbar(2)    ;
//   else if(runOn == "ttbar_3")         analyzer->process_ttbar(3)    ;
//   else if(runOn == "ttbar_4")         analyzer->process_ttbar(4)    ;
//   else if(runOn == "ttbar_5")         analyzer->process_ttbar(5)    ;
//   else if(runOn == "ttbar_6")         analyzer->process_ttbar(6)    ;
//   else if(runOn == "ttbar_7")         analyzer->process_ttbar(7)    ;
//   else if(runOn == "ttbar_8")         analyzer->process_ttbar(8)    ;
//   else if(runOn == "ttbar_9")         analyzer->process_ttbar(9)    ;
//   else if(runOn == "ttbar_10")         analyzer->process_ttbar(10)    ;

  else if(runOn == "wjets_v1_10")               analyzer->process_wjets_v1(10)              ;
  else if(runOn == "wjets_v1_11")               analyzer->process_wjets_v1(11)              ;
  else if(runOn == "wjets_v1_12")               analyzer->process_wjets_v1(12)              ;
  else if(runOn == "wjets_v1_13")               analyzer->process_wjets_v1(13)              ;
  else if(runOn == "wjets_v1_14")               analyzer->process_wjets_v1(14)              ;
  else if(runOn == "wjets_v1_15")               analyzer->process_wjets_v1(15)              ;
  else if(runOn == "wjets_v1_16")               analyzer->process_wjets_v1(16)              ;
  else if(runOn == "wjets_v1_17")               analyzer->process_wjets_v1(17)              ;
  else if(runOn == "wjets_v1_18")               analyzer->process_wjets_v1(18)              ;
  else if(runOn == "wjets_v1_19")               analyzer->process_wjets_v1(19)              ;
  else if(runOn == "wjets_v1_2")               analyzer->process_wjets_v1(2)              ;
  else if(runOn == "wjets_v1_3")               analyzer->process_wjets_v1(3)              ;
  else if(runOn == "wjets_v1_4")               analyzer->process_wjets_v1(4)              ;
  else if(runOn == "wjets_v1_5")               analyzer->process_wjets_v1(5)              ;
  else if(runOn == "wjets_v1_6")               analyzer->process_wjets_v1(6)              ;
  else if(runOn == "wjets_v1_7")               analyzer->process_wjets_v1(7)              ;
  else if(runOn == "wjets_v1_8")               analyzer->process_wjets_v1(8)              ;
  else if(runOn == "wjets_v1_9")               analyzer->process_wjets_v1(9)              ;

  else if(runOn == "wjets_v2_10")               analyzer->process_wjets_v2(10)              ;
  else if(runOn == "wjets_v2_11")               analyzer->process_wjets_v2(11)              ;
  else if(runOn == "wjets_v2_12")               analyzer->process_wjets_v2(12)              ;
  else if(runOn == "wjets_v2_13")               analyzer->process_wjets_v2(13)              ;
  else if(runOn == "wjets_v2_14")               analyzer->process_wjets_v2(14)              ;
  else if(runOn == "wjets_v2_15")               analyzer->process_wjets_v2(15)              ;
  else if(runOn == "wjets_v2_16")               analyzer->process_wjets_v2(16)              ;
  else if(runOn == "wjets_v2_17")               analyzer->process_wjets_v2(17)              ;
  else if(runOn == "wjets_v2_18")               analyzer->process_wjets_v2(18)              ;
  else if(runOn == "wjets_v2_19")               analyzer->process_wjets_v2(19)              ;

  else if(runOn == "wjets_v2_20")               analyzer->process_wjets_v2(20)              ;
  else if(runOn == "wjets_v2_21")               analyzer->process_wjets_v2(21)              ;
  else if(runOn == "wjets_v2_22")               analyzer->process_wjets_v2(22)              ;
  else if(runOn == "wjets_v2_23")               analyzer->process_wjets_v2(23)              ;
  else if(runOn == "wjets_v2_24")               analyzer->process_wjets_v2(24)              ;
  else if(runOn == "wjets_v2_25")               analyzer->process_wjets_v2(25)              ;
  else if(runOn == "wjets_v2_26")               analyzer->process_wjets_v2(26)              ;
  else if(runOn == "wjets_v2_27")               analyzer->process_wjets_v2(27)              ;
  else if(runOn == "wjets_v2_28")               analyzer->process_wjets_v2(28)              ;
  else if(runOn == "wjets_v2_29")               analyzer->process_wjets_v2(29)              ;

  else if(runOn == "wjets_v2_30")               analyzer->process_wjets_v2(30)              ;
  else if(runOn == "wjets_v2_31")               analyzer->process_wjets_v2(31)              ;
  else if(runOn == "wjets_v2_32")               analyzer->process_wjets_v2(32)              ;
  else if(runOn == "wjets_v2_33")               analyzer->process_wjets_v2(33)              ;
  else if(runOn == "wjets_v2_34")               analyzer->process_wjets_v2(34)              ;
  else if(runOn == "wjets_v2_35")               analyzer->process_wjets_v2(35)              ;
  else if(runOn == "wjets_v2_36")               analyzer->process_wjets_v2(36)              ;
  else if(runOn == "wjets_v2_37")               analyzer->process_wjets_v2(37)              ;
  else if(runOn == "wjets_v2_38")               analyzer->process_wjets_v2(38)              ;
  else if(runOn == "wjets_v2_39")               analyzer->process_wjets_v2(39)              ;

  else if(runOn == "wjets_v2_0")               analyzer->process_wjets_v2(0)              ;
  else if(runOn == "wjets_v2_4")               analyzer->process_wjets_v2(4)              ;
  else if(runOn == "wjets_v2_5")               analyzer->process_wjets_v2(5)              ;
  else if(runOn == "wjets_v2_6")               analyzer->process_wjets_v2(6)              ;
  else if(runOn == "wjets_v2_7")               analyzer->process_wjets_v2(7)              ;
  else if(runOn == "wjets_v2_8")               analyzer->process_wjets_v2(8)              ;
  else if(runOn == "wjets_v2_9")               analyzer->process_wjets_v2(9)              ;

//   else if(runOn == "zjets_from50_10")               analyzer->process_zjets_from50(10)              ;
//   else if(runOn == "zjets_from50_11")               analyzer->process_zjets_from50(11)              ;
//   else if(runOn == "zjets_from50_12")               analyzer->process_zjets_from50(12)              ;
//   else if(runOn == "zjets_from50_13")               analyzer->process_zjets_from50(13)              ;
//   else if(runOn == "zjets_from50_14")               analyzer->process_zjets_from50(14)              ;
//   else if(runOn == "zjets_from50_15")               analyzer->process_zjets_from50(15)              ;
//   else if(runOn == "zjets_from50_16")               analyzer->process_zjets_from50(16)              ;
//   else if(runOn == "zjets_from50_17")               analyzer->process_zjets_from50(17)              ;
//   else if(runOn == "zjets_from50_18")               analyzer->process_zjets_from50(18)              ;
//   else if(runOn == "zjets_from50_19")               analyzer->process_zjets_from50(19)              ;

//   else if(runOn == "zjets_from50_20")               analyzer->process_zjets_from50(20)              ;
//   else if(runOn == "zjets_from50_21")               analyzer->process_zjets_from50(21)              ;
//   else if(runOn == "zjets_from50_22")               analyzer->process_zjets_from50(22)              ;
//   else if(runOn == "zjets_from50_23")               analyzer->process_zjets_from50(23)              ;
//   else if(runOn == "zjets_from50_24")               analyzer->process_zjets_from50(24)              ;
//   else if(runOn == "zjets_from50_25")               analyzer->process_zjets_from50(25)              ;
//   else if(runOn == "zjets_from50_26")               analyzer->process_zjets_from50(26)              ;
//   else if(runOn == "zjets_from50_27")               analyzer->process_zjets_from50(27)              ;
//   else if(runOn == "zjets_from50_28")               analyzer->process_zjets_from50(28)              ;
//   else if(runOn == "zjets_from50_29")               analyzer->process_zjets_from50(29)              ;

//   else if(runOn == "zjets_from50_30")               analyzer->process_zjets_from50(30)              ;
//   else if(runOn == "zjets_from50_31")               analyzer->process_zjets_from50(31)              ;
//   else if(runOn == "zjets_from50_32")               analyzer->process_zjets_from50(32)              ;
//   else if(runOn == "zjets_from50_33")               analyzer->process_zjets_from50(33)              ;
//   else if(runOn == "zjets_from50_34")               analyzer->process_zjets_from50(34)              ;
//   else if(runOn == "zjets_from50_35")               analyzer->process_zjets_from50(35)              ;
//   else if(runOn == "zjets_from50_36")               analyzer->process_zjets_from50(36)              ;
//   else if(runOn == "zjets_from50_37")               analyzer->process_zjets_from50(37)              ;
//   else if(runOn == "zjets_from50_38")               analyzer->process_zjets_from50(38)              ;
//   else if(runOn == "zjets_from50_39")               analyzer->process_zjets_from50(39)              ;

//   else if(runOn == "zjets_from50_0")               analyzer->process_zjets_from50(0)              ;
//   else if(runOn == "zjets_from50_4")               analyzer->process_zjets_from50(4)              ;
//   else if(runOn == "zjets_from50_5")               analyzer->process_zjets_from50(5)              ;
//   else if(runOn == "zjets_from50_6")               analyzer->process_zjets_from50(6)              ;
//   else if(runOn == "zjets_from50_7")               analyzer->process_zjets_from50(7)              ;
//   else if(runOn == "zjets_from50_8")               analyzer->process_zjets_from50(8)              ;
//   else if(runOn == "zjets_from50_9")               analyzer->process_zjets_from50(9)              ;

//   else if(runOn == "zjets_10to50_10")     analyzer->process_zjets_10to50(10);
//   else if(runOn == "zjets_10to50_11")     analyzer->process_zjets_10to50(11);
//   else if(runOn == "zjets_10to50_12")     analyzer->process_zjets_10to50(12);
//   else if(runOn == "zjets_10to50_13")     analyzer->process_zjets_10to50(13);
//   else if(runOn == "zjets_10to50_14")     analyzer->process_zjets_10to50(14);
//   else if(runOn == "zjets_10to50_15")     analyzer->process_zjets_10to50(15);
//   else if(runOn == "zjets_10to50_16")     analyzer->process_zjets_10to50(16);
//   else if(runOn == "zjets_10to50_17")     analyzer->process_zjets_10to50(17);
//   else if(runOn == "zjets_10to50_18")     analyzer->process_zjets_10to50(18);
//   else if(runOn == "zjets_10to50_19")     analyzer->process_zjets_10to50(19);
//   else if(runOn == "zjets_10to50_20")     analyzer->process_zjets_10to50(20);
//   else if(runOn == "zjets_10to50_21")     analyzer->process_zjets_10to50(21);
//   else if(runOn == "zjets_10to50_22")     analyzer->process_zjets_10to50(22);
//   else if(runOn == "zjets_10to50_23")     analyzer->process_zjets_10to50(23);
//   else if(runOn == "zjets_10to50_24")     analyzer->process_zjets_10to50(24);
//   else if(runOn == "zjets_10to50_25")     analyzer->process_zjets_10to50(25);
//   else if(runOn == "zjets_10to50_26")     analyzer->process_zjets_10to50(26);
//   else if(runOn == "zjets_10to50_27")     analyzer->process_zjets_10to50(27);
//   else if(runOn == "zjets_10to50_28")     analyzer->process_zjets_10to50(28);
//   else if(runOn == "zjets_10to50_29")     analyzer->process_zjets_10to50(29);
//   else if(runOn == "zjets_10to50_0")     analyzer->process_zjets_10to50(0);
//   else if(runOn == "zjets_10to50_3")     analyzer->process_zjets_10to50(3);
//   else if(runOn == "zjets_10to50_4")     analyzer->process_zjets_10to50(4);
//   else if(runOn == "zjets_10to50_5")     analyzer->process_zjets_10to50(5);
//   else if(runOn == "zjets_10to50_6")     analyzer->process_zjets_10to50(6);
//   else if(runOn == "zjets_10to50_7")     analyzer->process_zjets_10to50(7);
//   else if(runOn == "zjets_10to50_8")     analyzer->process_zjets_10to50(8);
//   else if(runOn == "zjets_10to50_9")     analyzer->process_zjets_10to50(9);

  else if(runOn == "WW_10")            analyzer->process_WW(10)           ;
  else if(runOn == "WW_11")            analyzer->process_WW(11)           ;
  else if(runOn == "WW_12")            analyzer->process_WW(12)           ;
  else if(runOn == "WW_13")            analyzer->process_WW(13)           ;
  else if(runOn == "WW_14")            analyzer->process_WW(14)           ;
  else if(runOn == "WW_15")            analyzer->process_WW(15)           ;
  else if(runOn == "WW_16")            analyzer->process_WW(16)           ;
  else if(runOn == "WW_17")            analyzer->process_WW(17)           ;
  else if(runOn == "WW_18")            analyzer->process_WW(18)           ;
  else if(runOn == "WW_19")            analyzer->process_WW(19)           ;
  else if(runOn == "WW_2")            analyzer->process_WW(2)           ;
  else if(runOn == "WW_3")            analyzer->process_WW(3)           ;
  else if(runOn == "WW_4")            analyzer->process_WW(4)           ;
  else if(runOn == "WW_5")            analyzer->process_WW(5)           ;
  else if(runOn == "WW_6")            analyzer->process_WW(6)           ;
  else if(runOn == "WW_7")            analyzer->process_WW(7)           ;
  else if(runOn == "WW_8")            analyzer->process_WW(8)           ;
  else if(runOn == "WW_9")            analyzer->process_WW(9)           ;
  
  else if(runOn == "WZ_10")            analyzer->process_WZ(10)           ;
  else if(runOn == "WZ_11")            analyzer->process_WZ(11)           ;
  else if(runOn == "WZ_12")            analyzer->process_WZ(12)           ;
  else if(runOn == "WZ_13")            analyzer->process_WZ(13)           ;
  else if(runOn == "WZ_14")            analyzer->process_WZ(14)           ;
  else if(runOn == "WZ_15")            analyzer->process_WZ(15)           ;
  else if(runOn == "WZ_16")            analyzer->process_WZ(16)           ;
  else if(runOn == "WZ_17")            analyzer->process_WZ(17)           ;
  else if(runOn == "WZ_18")            analyzer->process_WZ(18)           ;
  else if(runOn == "WZ_19")            analyzer->process_WZ(19)           ;
  else if(runOn == "WZ_2")            analyzer->process_WZ(2)           ;
  else if(runOn == "WZ_3")            analyzer->process_WZ(3)           ;
  else if(runOn == "WZ_4")            analyzer->process_WZ(4)           ;
  else if(runOn == "WZ_5")            analyzer->process_WZ(5)           ;
  else if(runOn == "WZ_6")            analyzer->process_WZ(6)           ;
  else if(runOn == "WZ_7")            analyzer->process_WZ(7)           ;
  else if(runOn == "WZ_8")            analyzer->process_WZ(8)           ;
  else if(runOn == "WZ_9")            analyzer->process_WZ(9)           ;
  
  else if(runOn == "ZZ_10")            analyzer->process_ZZ(10)           ;
  else if(runOn == "ZZ_11")            analyzer->process_ZZ(11)           ;
  else if(runOn == "ZZ_12")            analyzer->process_ZZ(12)           ;
  else if(runOn == "ZZ_13")            analyzer->process_ZZ(13)           ;
  else if(runOn == "ZZ_14")            analyzer->process_ZZ(14)           ;
  else if(runOn == "ZZ_15")            analyzer->process_ZZ(15)           ;
  else if(runOn == "ZZ_16")            analyzer->process_ZZ(16)           ;
  else if(runOn == "ZZ_17")            analyzer->process_ZZ(17)           ;
  else if(runOn == "ZZ_18")            analyzer->process_ZZ(18)           ;
  else if(runOn == "ZZ_19")            analyzer->process_ZZ(19)           ;
  else if(runOn == "ZZ_2")            analyzer->process_ZZ(2)           ;
  else if(runOn == "ZZ_3")            analyzer->process_ZZ(3)           ;
  else if(runOn == "ZZ_4")            analyzer->process_ZZ(4)           ;
  else if(runOn == "ZZ_5")            analyzer->process_ZZ(5)           ;
  else if(runOn == "ZZ_6")            analyzer->process_ZZ(6)           ;
  else if(runOn == "ZZ_7")            analyzer->process_ZZ(7)           ;
  else if(runOn == "ZZ_8")            analyzer->process_ZZ(8)           ;
  else if(runOn == "ZZ_9")            analyzer->process_ZZ(9)           ;




//   if     (runOn == "hh_higgs_bychannel")  analyzer->process_hh_higgs_bychannel() ;
//   else if(runOn == "hh_higgs")            analyzer->process_hh_higgs()           ;
//   else if(runOn == "ttbar_unc")           analyzer->process_ttbar_unc()          ;
  
//   else if(runOn == "wh_higgs_bychannel")  analyzer->process_wh_higgs_bychannel() ;
//   else if(runOn == "wh_higgs")            analyzer->process_wh_higgs()           ;
//   else if(runOn == "tbh_higgs_bychannel") analyzer->process_tbh_higgs_bychannel() ;
//   else if(runOn == "tbh_higgs")           analyzer->process_tbh_higgs()           ;



 // FIXME: manage with --blah for having parameters like "withHiggs" and so on
  else if(runOn == "doTable"){
    cout << "Doing table" << endl;
    bool onlyhiggs(true), sm(false), doNotPrintAllErrors(false), printAllErrors(true), includeSoverB(true), doNotincludeSoverB(false); 
    int detailed(2), notDetailed(1);
    //fn    analyzer->summaryTable( detailed, false, false, false, false);
    //    analyzer->summaryTable( detailed, false, true, false, false);
    //    analyzer->summaryTable( detailed, false, false, true, false);
    //    analyzer->summaryTable( detailed, false, false, false, true);
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "yields-mc-", true, true, true); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "yields-mc-", false, false, false); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "yields-mc-", true, false, false); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-EL-yields-mc-", true, true, true, true); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-EL-yields-mc-", true, false, false, false, false); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-EL-yields-mc-", false, true, false, false, false); 
    //fn QUELLO SENZA ERRORI   
    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-MU-TauEf-mc-", false, false, true, false,false); 
//     analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-MU-FulMGsc-mc-", false, false, false, true,false); 
//     analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-MU-FulMGsc-mc-", false, false, false, false,true); 
//     analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-MU-FulMGsc-mc-", true, false, false, false,false); 
//     analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-MU-FulMGsc-mc-", false, true, false, false,false); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-EL-yields-mc-", false, false, false, true, false); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "NEWJ-EL-yields-mc-", false, false, false, false, true); 
    //    analyzer->mcTable(notDetailed, includeSoverB, printAllErrors, sm, "PFlow", "yields-mc-", false, false, true); 
    //analyzer.mcTable(notDetailed, includeSoverB, printAllErrors, onlyhiggs, "PFlow", "yields-mc-", false, false, false); 
  }
  else if(runOn == "doPlots"){
//    PlotStyle sty();
//    sty.setTDRStyle();
      
    TString samples("data/plotter/samples_el.xml");
    TString outFolder("temp23Mar14/3hitHIGNewSFuncertns70300p02XfxxPaper/"); // move to input line

//     TString limits("data/plotter/limits.xml");
//     TString leptons("data/plotter/leptons.xml");
//     TString met("data/plotter/met.xml");
//     TString mt("data/plotter/mt.xml");
//     TString wjets("data/plotter/wjets.xml");
//     TString wtmass("data/plotter/wtmass.xml");
    TString jets("data/plotter/jets_el.xml");
//     TString yields("data/plotter/yields_mu.xml");
//     TString taus("data/plotter/taus.xml");
//     TString vertex("data/plotter/vertex.xml");
//     TString afterR("data/plotter/afterR.xml");
//     TString test("data/plotter/test.xml");
//     TString debug("data/plotter/debug.xml");

//    TString mtop("data/plotter/mtop_el.xml");
//    TString plotsDD("data/plotter/plotsDD_mu.xml");


    HistogramPlotter a; // Move to input line or cfg file the choice of what to plot
    
    //    a.parse(samples,met,outFolder);

//     a.parse(samples,vertex,outFolder);
    a.parse(samples,jets,outFolder); 
//     a.parse(samples,met,outFolder);      
//     a.parse(samples,leptons,outFolder);

// // // // //    a.parse(samples,mt,outFolder);      
//    a.parse(samples,yields,outFolder);
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




