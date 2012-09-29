#ifndef  _SampleProcessor_hh
#define  _SampleProcessor_hh

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "LIP/TopTaus/interface/CommonDefinitions.hh"

// System headers
#include <iostream>
#include <vector>

#endif

using namespace commondefinitions;

// FIXME: clean code


//using namespace PhysicsTools;

typedef unsigned int uint;

class SampleProcessor {
  
public :
  /*
    Fills default X sections 
  */
  SampleProcessor( double tauPtCut){
    
    outFile_=0;
    
    checkleptonsFromTauDecay_ = false;
    //checkleptonsFromTauDecay_ = true;  // used to compute tables with leptons from tau decays (tt->WbWb->tauhtaul)

    //pdfweights_=true;
    pdfweights_=false; // this should allways be false, it will be overwritten by the process_ttbar

    sampleTauPtCut_ = tauPtCut;
    
    jetpgid_=JETPGID_OLD;

    fast_=true;

    //trigEffStudy_= true;
    trigEffStudy_= false;
   
    pu_ = NOPU;

    //pu_ = PUPLUS;
    //pu_ = PUPLUS;

//    keys_.push_back("TC");
//    keys_.push_back("PF");  
//    keys_.push_back("TaNC0");  // tanc FR    1  %
//    keys_.push_back("TaNC1");  // tanc FR  0.5  %
//    keys_.push_back("TaNC2");  // tanc FR  0.25 %
//    keys_.push_back("TaNC3");  // tanc FR  0.1  %
//    keys_.push_back("HPS0");   // HPS loose iso
//    keys_.push_back("HPS1");   // HPS medium iso
//    keys_.push_back("HPS2");   // HPS high iso
      keys_.push_back("PFlow");  // PF HPS loose iso
 
    char data[10]; sprintf(data,"%dGeV",(int)sampleTauPtCut_);
    
    // output folder and input folder
    
	// Folders at LIP
	
    //oFolder_ = TString("/soft/lip-sw/cmssw/users/nalmeida/new/nalmeida/sw424/Physics/UserCode/nalmeida/TopTauDileptons2012/");  
    //iFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/");  
    oFolder_ = TString("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/");
    iFolder_ = TString("/lustre/data3/cmslocal/samples/CMSSW_4_4_4/mc/minitrees-muon-Fall11/");

    // Folders at CERN
    //oFolder_ = TString("/afs/cern.ch/work/n/nalmeida/private/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/test/");   
	
    //iFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/");
    if( run2012_ ) iFolder_ = TString("/lustre/lip.pt/data/cmslocal/fnguyen/");   


    if( pu_ == PUPLUS  ) oFolder_ += TString("puplus-"); else if ( pu_ == PUMINUS ) oFolder_ += TString("puminus-");

    TString mtStr("nomt-");    if ( APPLY_MT_CUT_)    { mtStr  = TString("mt-");}
    TString lepStr("EL-");     if (!eChONmuChOFF_)    { lepStr = TString("MU-");}
    TString bwStr("");         if (!bwStr)            { bwStr  = TString("bw-");}
    TString vers("2011-V1-");

   
    if( run2012_ ) vers = TString("2012-V1-");


    oFolder_ += mtStr+vers+TString("mc-")+lepStr+bwStr+TString(data)+TString("/");
    

    if( eChONmuChOFF_ ) iDFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3-B/");  
    //else              iDFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/");  
    else                iDFolder_ = TString("/lustre/data3/cmslocal/samples/CMSSW_4_4_4/data/minitrees-muon-2011/");  
   


    // LIP
    //if( run2012_ ) iDFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_8_patch4b/data/minitrees-muon-2011-newTau/");  
    //oDFolder_  = TString("/afs/cern.ch/work/n/nalmeida/private/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/test");

    if( run2012_ ) iDFolder_ = TString("/lustre/lip.pt/data/cmslocal/fnguyen/");  
    //if( run2012_ ) iDFolder_ = TString("/lip-sw/cmssw/users/nalmeida/new/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/mt-2012-V1-data-MU-20GeV/");  
    oDFolder_  = TString("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/");






// Fall 2010 samples   : https://twiki.cern.ch/twiki/bin/viewauth/CMS/ProductionFall2010
// NLO cross sections  : https://twiki.cern.ch/twiki/bin/view/CMS/CrossSections_3XSeries#crosssections
// NNLO cross sections : https://twiki.cern.ch/twiki/pub/CMS/GeneratorMain/ShortXsec.pdf


// Summer11
//http://cms.cern.ch/iCMS/prep/requestmanagement?status=Done&pwg=HIG&campid=Summer11_R1&dsn=
//http://cms.cern.ch/iCMS/prep/requestmanagement?pwg=TOP&campid=Summer11_R1
//https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSections




    oDFolder_ += mtStr+vers+TString("data-")+lepStr+TString(data)+TString("/");
    
    
    for (int i=0; i<200; i++ ){ defaultXSections_.push_back(0); }

    defaultXSections_[DATA_URL]      = 0.;         
    defaultXSections_[TTBAR_URL]     = 165;  //NNLO aproximation +- 10 old was 157.5 

    defaultXSections_[W_ENU_URL]     = 10438; //NNLO 31314/3 =10438 //NLO 28000/3 = 9333 // xsec 7899 WToENu_TuneZ2_7TeV-pythia6/Fall10-START38_V12-v1                 
    defaultXSections_[W_MUNU_URL]    = 10438;
    defaultXSections_[W_TAUNU_URL]   = 10438; 


    // ALPGEN SAMPLES
    /*
     W0Jets           /W0Jets_TuneD6T_7TeV-alpgen-tauola                                                    2.003e+04
     W1Jets_0to100    /W1Jets_ptW-0to100_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO 	    3.693e+03
     W1Jets_100to300  /W1Jets_ptW-100to300_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO     7.197e+01
     W2Jets_0to100    /W2Jets_ptW-0to100_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO 	    9.434e+02
     W2Jets_100to300  /W2Jets_ptW-100to300_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO     6.718e+01
     W3Jets_0to100    /W3Jets_ptW-0to100_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO 	    2.087e+02
     W3Jets_100to300  /W3Jets_ptW-100to300_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO     3.243e+01
     W4Jets_0to100    /W4Jets_ptW-0to100_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO 	    4.446e+01
     W4Jets_100to300  /W4Jets_ptW-100to300_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO     1.138e+01
     W5Jets_0to100    /W5Jets_ptW-0to100_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO 	    1.111e+01
     W5Jets_100to300  /W5Jets_ptW-100to300_TuneZ2_7TeV-alpgen-tauola/Fall10-START38_V12-v1/GEN-SIM-RECO     3.789e+00 
    */

    // total 
    // w0 ->      2.024e+04 //Z2
    // W0 ->D6    2.003e+04

    defaultXSections_[W0J_URL]         = 20030.*1.3;
    defaultXSections_[W1JA_URL]        = 3693.*1.3;
    defaultXSections_[W1JB_URL]        = 71.97*1.3;
    defaultXSections_[W2JA_URL]        = 943.4*1.3;
    defaultXSections_[W2JB_URL]        = 67.18*1.3;
    defaultXSections_[W3JA_URL]        = 208.7*1.3;
    defaultXSections_[W3JB_URL]        = 32.43*1.3;
    defaultXSections_[W4JA_URL]        = 44.46*1.3;
    defaultXSections_[W4JB_URL]        = 11.38*1.3;
    defaultXSections_[W5JA_URL]        = 11.11*1.3;
    defaultXSections_[W5JB_URL]        = 3.789*1.3;

    defaultXSections_[WJMADGRAPH_URL]  = 31314;

    defaultXSections_[TT0J_URL]        = 49.17*2.1768;
    defaultXSections_[TT1J_URL]        = 17.97*2.1768 ;
    defaultXSections_[TT2J_URL]        = 4.279*2.1768 ;
    defaultXSections_[TT3J_URL]        = 0.7951*2.1768 ;
    defaultXSections_[TT4J_URL]        = 0.1404*2.1768 ;


    /*
    defaultXSections_[DY_HIGHLL_URL]     = 3048;
    defaultXSections_[DY_EE_URL]         = 650;
    defaultXSections_[DY_MM_URL]         = 650;
    defaultXSections_[DY_TAUTAU_URL]     = 650;
    */

    //defaultXSections_[DY_10TO50_URL]          = 310;  //NLO 310 // xsec = 310  eff=0.18 DYJetsToLL_TuneD6T_M-10To50_7TeV-madgraph-tauola (on madgraph we do not apply eff)

   // the number is LO cross section (3x310 pb) with a k-factor to correct for NLO (1.33). The actual source of this - I don't know.
    defaultXSections_[DY_10TO50_URL]          = 12369;
    defaultXSections_[DY_FROM50_URL]          = 3048; //NNLO 3048 //NLO 2800// xsec= 2321  eff=0.44 DYJetsToLL_TuneZ2_M-50_7TeV-madgraph-tauola 
  

   /* Fall 2010 samples
    defaultXSections_[S_URL]                  = 1.49;     
    defaultXSections_[T_URL]                  = 20.93 ;    
    defaultXSections_[W_URL]                  = 10.6;   
   */




/*

t+ (s-channel) 	MCFM 	-- 	NLO 	Inclusive 	2.72 	+0.07-0.06 (±0.08), Total = +0.11 - 0.10 	Inclusive top decay, PDF errors also includes alphas
t- (s-channel) 	MCFM 	-- 	NLO 	Inclusive 	1.49 	+0.04-0.03 (±0.08), Total = +0.09-0.08 	Inclusive top decay, PDF errors also includes alphas 

t+ (t-channel) 	MCFM 	-- 	NLO 	Inclusive 	42.6 	+0.9-0.8 (±2.2), Total = +2.4-2.3 	Inclusive top decay, PDF errors also includes alphas
t- (t-channel) 	MCFM 	-- 	NLO 	Inclusive 	22.0 	+0.6-0.3 (±0.8), Total = +0.10-0.8 	Inclusive top decay, PDF errors also includes alphas 

W+tbar   	MCFM 	-- 	NLO 	Inclusive 	5.3 	±0.6 	--
W-t 	        MCFM 	-- 	NLO 	Inclusive 	5.3 	±0.6 	-- 
*/




    defaultXSections_[S_URL]                  = 2.72;     
    defaultXSections_[T_URL]                  = 42.6;    
    //defaultXSections_[W_URL]                = 5.3;   //WARNING :: This is old
    defaultXSections_[W_URL]                  = 7.87;  

    
    defaultXSections_[A_S_URL]                = 1.49;    
    defaultXSections_[A_T_URL]                = 22;    
    //defaultXSections_[A_W_URL]              = 5.3;   //WARNING :: use the new value
    defaultXSections_[A_W_URL]                = 7.87;  


   

	                          
    defaultXSections_[QCD_EM20TO30_URL]       = 2454400.;     //xsec =236000000 eff=0.0104 QCD_Pt-20to30_EMEnriched_TuneZ2_7TeV-pythia6
    defaultXSections_[QCD_EM30TO80_URL]       = 3866200.;     //xsec =59480000  eff=0.065  QCD_Pt-30to80_EMEnriched_TuneZ2_7TeV-pythia6 
    defaultXSections_[QCD_EM80TO170_URL]      = 139500.;      //xsec =900000    eff=0.155  QCD_Pt-80to170_EMEnriched_TuneZ2_7TeV-pythia6 

    defaultXSections_[QCD_BCTOE20TO30_URL]    = 132160;       //xsec = 236000000 eff=0.00056 QCD_Pt-20to30_BCtoE_TuneZ2_7TeV-pythia6 
    defaultXSections_[QCD_BCTOE30TO80_URL]    = 136804;       //xsec = 59480000  eff=0.00230 QCD_Pt-30to80_BCtoE_TuneZ2_7TeV-pythia6 
    defaultXSections_[QCD_BCTOE80TO170_URL]   = 9360;         //xsec = 900000    eff=0.0104  QCD_Pt-80to170_BCtoE_TuneZ2_7TeV-pythia6   

    defaultXSections_[QCD_PHOTON30TO50_URL]   = 16690;        //xsec = 1.669e+04   G_Pt_30to50_TuneZ2_7TeV_pythia6                 
    defaultXSections_[QCD_PHOTON50TO80_URL]   = 2722;         //xsec = 2.722e+03   G_Pt_50to80_TuneZ2_7TeV_pythia6                         
    defaultXSections_[QCD_PHOTON80TO120_URL]  = 447.2;        //xsec = 4.472e+02   G_Pt_80to120_TuneZ2_7TeV_pythia6
    defaultXSections_[QCD_PHOTON120TO170_URL] = 84.1;	        //xsec = 8.417e+01   G_Pt_120to170_TuneZ2_7TeV_pythia6
                                        
    //defaultXSections_[QCD_MU_URL]           = 349988;       //xsec=296600000 	eff=0.00118  QCD_Pt-20_MuEnrichedPt-10_TuneZ2_7TeV-pythia6 //79688.;                                    
    defaultXSections_[QCD_MU_URL]             = 84679;        //xsec=296600000 	eff=0.002855 QCD_Pt-20_MuEnrichedPt-15_TuneZ2_7TeV-pythia6      


    // xsections taken from CMS-AN-10-337
    defaultXSections_[WW_URL]                 = 43;
    defaultXSections_[WZ_URL]                 = 18.2;
    defaultXSections_[ZZ_URL]                 = 5.9;

    defaultXSections_[CTRLWW_URL]             = 165; 
    defaultXSections_[WH80_URL]               = 165;            
    defaultXSections_[HH80_URL]               = 165;                                           
    defaultXSections_[WH100_URL]              = 165;                                          
    defaultXSections_[HH100_URL]              = 165;                                        
    defaultXSections_[WH120_URL]              = 165;                                          
    defaultXSections_[HH120_URL]              = 165;                                          
    defaultXSections_[WH140_URL]              = 165;                                          
    defaultXSections_[HH140_URL]              = 165; 
    defaultXSections_[WH150_URL]              = 165; 
    defaultXSections_[HH150_URL]              = 165; 
    defaultXSections_[WH155_URL]              = 165; 
    defaultXSections_[HH155_URL]              = 165; 
    defaultXSections_[WH160_URL]              = 165;                                           
    defaultXSections_[HH160_URL]              = 165;                                                                                 

    defaultXSections_[TBH250_URL]                = 165;           

  };


  /*
    Initialize sample processor with files urls , associated x sections and processed luminosity
  */
  void virtual  init( vector<TString> listOfFiles, vector<double> listOfXSections, bool fullStats, double lum, uint n_data =0);

  /*
    Requires : initialized list of urls, xsections and luminosity 
    Performs : opens TFiles and computes scales
  */
  void init();


  /*
    Opens list of processed files with default urls
  */
  void initProcessedFiles();

  /*
    Gets url code / selector code and returns code in this analyzer
  */
  int codeChannel(int i, int urlCode);

  /*
   translates analysis code in specific tau dilepton channel
  */
  int tdChannel(int channel);
  
  /*
   Process the different samples
  */

  // FIXME: parametrize with hadd on a per-10 basis (quicker in lip batch)
  void process_data_RunA();
  void process_data_RunB();
  void process_ttbar();
  void process_ttbar_bychannel();
  void process_singletop();
  void process_qcd();
  void process_wjets();
  void process_zjets();
  void process_hh_higgs();
  void process_hh_higgs_bychannel();
  void process_wh_higgs();
  void process_wh_higgs_bychannel();
  void process_dibosons();
  void process_tbh_higgs();
  void process_tbh_higgs_bychannel();
  void process_ttbar_unc();

  void process_triggersamples_mht();
  void process_triggersamples_mhtjets();
  

  /*
    virtual process method handled by the analyzer
  */
  virtual void process(bool isData, urlCodes code, TString dataPath, TString histoFiles, vector<TString> & keys, uint ttbarCode =0){ 
     cout<<endl<<" Sample processor virtual processor call "<<endl;
  }
 
 
  // My lists /////////////////////////////////////////////////////////////////////////////////////////////
  vector< TString >         listOfurls_;             // list of input file urls
  vector< TFile * >         listOfFiles_;            // list of files
  vector< TH1D * >          listOfHistos_;           // list of input histograms
  vector< double >          listOfXSections_;        // list of cross sections
  vector< double >          listOfScales_;           // list of scale factors to be applied in the analysis
  vector< uint >            listOfEvents_;           // list of input events 
  vector< uint >            listOfEventsToProcess_;  // list of events stored in the mini tree
  vector< uint >            listOfEventsToProcessMC_;  // list of events stored in the mini tree
  vector< event::Reader * > listOfReaders_;          // list of minitrees readers
  vector< event::Reader * > listOfReadersMC_;        // list of minitrees readers
  vector< event::Reader * > listOfSpies_;            // list of minitrees readers
  //////////////////////////////////////////////////////////////////////////////////////////////////////////


  // My List ///////////////////////////////////////////////////////////
  vector<TString>   listOfProcessedUrls_;     //list of processed urls
  vector<TFile *>   listOfProcessedFiles_;    //list of processed files
  vector<double>    listOfProcessedScales_;   //list of processed scales
  int qcdProcessedInitSample_,qcdProcessedFinalSample_;
  //////////////////////////////////////////////////////////////////////

  vector<double> defaultXSections_;  


  bool fullStats_;
  double lum_;
  uint nToProcess_;
  uint URLS_;
  urlCodes url_; // corrent url being processed

  TString iFolder_,iDFolder_;  // inputFolder for mc and data
  TString oFolder_,oDFolder_;  // outputFoler for mc and data

  ofstream infoFile_;          // debug info file

  vector<TString> keys_;

  bool checkleptonsFromTauDecay_;

  bool fast_;

  int jetpgid_; 

  double sampleTauPtCut_;  

  bool pdfweights_;

  double w0b_,w1b_,w2b_;

  int pu_;

  TFile * outFile_;  TString outFileName_;                       // file to be written with all histogram info


//  // QCD MVA
//  MVAComputer * qcdMVA_;  
//  
//  // W plus jets MVA
//  MVAComputer * wjetsMVA_;
  
  

public:
  bool trigEffStudy_;

  //  TFile * f_mva_;


 
};


#endif // _SampleProcessor_hh

