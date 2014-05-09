#include "LIP/TopTaus/interface/SampleProcessor.hh"

// System headers
#include <iostream>
#include <iomanip>
#include <sstream>

// ROOT headers



SampleProcessor::SampleProcessor(double tauPtCut, TString inputArea, TString outputArea):
  sampleTauPtCut_(tauPtCut)
{
  
  commondefinitions::inputArea_ = inputArea;
  commondefinitions::outputArea_ = outputArea;
    
    outFile_=0;
    
    checkleptonsFromTauDecay_ = false; // true; // used to compute tables with leptons from tau decays (tt->WbWb->tauhtaul)

    pdfweights_=false; // this should allways be false, it will be overwritten by the process_ttbar

    jetpgid_=JETPGID_OLD;

    fast_=true;

    trigEffStudy_= false; // Trigger efficiency study for electron+jets trigger. Should always be false: obsolete!
   
    pu_ = NOPU; // PUPLUS PUMINUS now are dealt with by passing the pileup distributions computed with different minbias xsec

    keys_.push_back("PFlow"); // PF HPS taus 
    char data[10]; sprintf(data,"%dGeV",(int)sampleTauPtCut_);
    
    // I/O folders
    iFolder_ = TString(commondefinitions::inputArea_);    //    iFolder_ += TString("mc/");
    oFolder_ = TString(commondefinitions::outputArea_);

    
    if( pu_ == PUPLUS  ) oFolder_ += TString("puplus-"); else if ( pu_ == PUMINUS ) oFolder_ += TString("puminus-");

    TString mtStr("nomt-");    if ( APPLY_MT_CUT_)    { mtStr  = TString("mt-");}
    TString lepStr("EL-");     if (!eChONmuChOFF_)    { lepStr = TString("MU-");}
    TString bwStr("");         if (!bwStr)            { bwStr  = TString("bw-");}
    TString vers("2011-V1-");
   
    if( run2012_ ) vers = TString("BTAG-V1-");


    iDFolder_ = TString(commondefinitions::inputArea_); 
//     if( eChONmuChOFF_ ) iDFolder_ = TString(commondefinitions::inputArea_);  
//     else                iDFolder_ = TString(commondefinitions::inputArea_);
    //    iDFolder_ += TString("data/");

    oFolder_ += mtStr+vers+TString("mc-")+lepStr+bwStr+TString(data)+TString("/");
  
    oDFolder_  = TString(commondefinitions::outputArea_);
    oDFolder_ += mtStr+vers+TString("data-")+lepStr+TString(data)+TString("/");
    
    
    for (int i=0; i<200; i++ ){ defaultXSections_.push_back(0); }
    
    if( run2012_ ) {
      defaultXSections_[DATA_URL]      = 0.;         
      defaultXSections_[TTBAR_URL]     = 251.7; // Mitov et al. //234.;  //Kidonakis - approx. NNLO - Inclusive - Top mass: m(top)=173GeV, Default scales: muF=muR=mt, PDF: MSTW2008 NNLO 
      
      defaultXSections_[W_ENU_URL]     = 12503.0; //NNLO 37509.0  36257.2/3 = 12085.7 - NNLO - W->lv, l=e,m,t - Inclusive W production, BR(W->lv) included, l=e,m,t, PDF error also includes alphas 
      defaultXSections_[W_MUNU_URL]    = 12503.0;
      defaultXSections_[W_TAUNU_URL]   = 12503.0; 

      defaultXSections_[WJMADGRAPH_URL]  = 37509.; // 36257.2;// 30400; //36257.2;
      
      // for the _filter sample:     xsec: 11050.0 eff: 0.069 
      defaultXSections_[DY_10TO50_URL]          = 11050.; // from http://cms.cern.ch/iCMS/prep/requestmanagement?dsn=DYJetsToLL_M-10To50filter_8TeV-madgraph&campid=Summer12_DR53X
      //fn      defaultXSections_[DY_FROM50_URL]          = 2950.; // from http://cms.cern.ch/iCMS/prep/requestmanagement?dsn=DYJetsToLL_M-50_TuneZ2Star_8TeV-madgraph-tarball&campid=Summer12_DR53X

      defaultXSections_[DY_FROM50_URL]          = 3503.71; // from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV

      defaultXSections_[S_URL]                  = 3.79;     
      defaultXSections_[T_URL]                  = 56.4;    
      defaultXSections_[W_URL]                  = 11.1;  
      
      
      defaultXSections_[A_S_URL]                = 1.76;    
      defaultXSections_[A_T_URL]                = 30.7;    
      defaultXSections_[A_W_URL]                = 11.1;

      defaultXSections_[QCD_EM20TO30_URL]       = 2914860.;
      defaultXSections_[QCD_EM30TO80_URL]       = 4615893.; //5077482.3; //4615893.; 
      defaultXSections_[QCD_EM80TO170_URL]      = 277997.; //238283.37; //183294.9; 
      defaultXSections_[QCD_EM170TO250_URL]      = 5962.476; //4586.52; 
      defaultXSections_[QCD_EM250TO350_URL]      = 835.125; //556.75; 
      defaultXSections_[QCD_EM350TOINF_URL]      = 133.65; //89.1; 
      
      defaultXSections_[QCD_BCTOE20TO30_URL]    = 167388.; 
      defaultXSections_[QCD_BCTOE30TO80_URL]    = 183744.; //167040.; 
      defaultXSections_[QCD_BCTOE80TO170_URL]   =  19689.; //16876.47; //12981.9; 
      defaultXSections_[QCD_BCTOE170TO250_URL]   = 821.6; //632.; 
      defaultXSections_[QCD_BCTOE250TO350_URL]   = 154.95; //103.3; 
      defaultXSections_[QCD_BCTOE350TOINF_URL]   = 36.; //24.; 
      
      defaultXSections_[QCD_MU_URL]             = 134680.; //364000000. eff 3.7E-4 
      
      defaultXSections_[QCD_PHOTON15TO30_URL]   = 200061.7; //220067.87; //200061.7;        
      defaultXSections_[QCD_PHOTON30TO50_URL]   = 19931.62; //21924.78; //19931.62;        
      defaultXSections_[QCD_PHOTON50TO80_URL]   = 3322.309;         
      defaultXSections_[QCD_PHOTON80TO120_URL]  = 893.; //781.6; //614.115; //558.2865;        
      defaultXSections_[QCD_PHOTON120TO170_URL] = 172.81; //151.209; //108.0068;
      defaultXSections_[QCD_PHOTON170TO300_URL] = 48.195; //45.183;  //39.1586; //30.122;
      defaultXSections_[QCD_PHOTON300TO470_URL] = 3.42;  //3.21;  //2.14;
      
      //fn xsections from https://twiki.cern.ch/twiki/bin/viewauth/CMS/StandardModelCrossSectionsat8TeV
      defaultXSections_[WW_URL]                 = 54.838;
      defaultXSections_[WZ_URL]                 = 33.72;
      defaultXSections_[ZZ_URL]                 = 17.627;
      
      defaultXSections_[CTRLWW_URL]             = 234; 
      defaultXSections_[WH80_URL]               = 234;            
      defaultXSections_[HH80_URL]               = 234;                                           
      defaultXSections_[WH100_URL]              = 234;                                          
      defaultXSections_[HH100_URL]              = 234;                                        
      defaultXSections_[WH120_URL]              = 234;                                          
      defaultXSections_[HH120_URL]              = 234;                                          
      defaultXSections_[WH140_URL]              = 234;                                          
      defaultXSections_[HH140_URL]              = 234; 
      defaultXSections_[WH150_URL]              = 234; 
      defaultXSections_[HH150_URL]              = 234; 
      defaultXSections_[WH155_URL]              = 234; 
      defaultXSections_[HH155_URL]              = 234; 
      defaultXSections_[WH160_URL]              = 234;                                           
      defaultXSections_[HH160_URL]              = 234;                                                                                 
      
      defaultXSections_[TBH180_URL]             = 1.1; //234;  
      defaultXSections_[TBH190_URL]             = 1.1; //234;  
      defaultXSections_[TBH200_URL]             = 1.1; //234;  
      defaultXSections_[TBH220_URL]             = 1.1; //234; 
      defaultXSections_[TBH250_URL]             = 1.1; //234;
      defaultXSections_[TBH300_URL]             = 1.1; //234;

      defaultXSections_[TTFULL_URL]             = 24.57*2134/2084; //234;
      defaultXSections_[TTSEMI_URL]             = 102.49*1341/1082; //234;
      defaultXSections_[TTHADR_URL]             = 106.94; //234;

      
    } else {
      
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
	
      t+ (s-channel) 	MCFM 	-- 	NLO 	Inclusive 	2.72 	+0.07-0.06 (Å¬Å±0.08), Total = +0.11 - 0.10 	Inclusive top decay, PDF errors also includes alphas
      t- (s-channel) 	MCFM 	-- 	NLO 	Inclusive 	1.49 	+0.04-0.03 (Å¬Å±0.08), Total = +0.09-0.08 	Inclusive top decay, PDF errors also includes alphas 
      
      t+ (t-channel) 	MCFM 	-- 	NLO 	Inclusive 	42.6 	+0.9-0.8 (Å¬Å±2.2), Total = +2.4-2.3 	Inclusive top decay, PDF errors also includes alphas
      t- (t-channel) 	MCFM 	-- 	NLO 	Inclusive 	22.0 	+0.6-0.3 (Å¬Å±0.8), Total = +0.10-0.8 	Inclusive top decay, PDF errors also includes alphas 
      
      W+tbar   	MCFM 	-- 	NLO 	Inclusive 	5.3 	Å¬Å±0.6 	--
      W-t 	        MCFM 	-- 	NLO 	Inclusive 	5.3 	Å¬Å±0.6 	-- 
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
    }
  }




void SampleProcessor::init(vector<TString> listOfurls, vector<double> listOfXSections, bool fullStats, double lum, uint n_data){

    
  nToProcess_ = n_data;
 
  if(listOfurls.size()!= listOfXSections.size() ){ 
    cout<<endl<<"\n\n Number of files does not match number of xsections... \n\n\n"<<endl; 
    exit(0);
  }
  
  // Reset urls, xsections,files,histos, scales,reades,events ////////
  listOfurls_.clear();
  listOfXSections_.clear();
  listOfFiles_.clear();
  listOfHistos_.clear();
  listOfScales_.clear();
  listOfReaders_.clear();
  listOfReadersMC_.clear();
  //listOfSpies_.clear();

  listOfEventsToProcess_.clear();
  listOfEventsToProcessMC_.clear();

  listOfEvents_.clear();
 
  
  URLS_=listOfurls.size();

  for(unsigned int i=0; i< URLS_;i++){ 

    listOfurls_.push_back(listOfurls[i]); 
    listOfXSections_.push_back(listOfXSections[i]); 
    listOfFiles_.push_back(0); 
    listOfHistos_.push_back(0); 
    listOfScales_.push_back(0); 
    listOfReaders_.push_back(0);
    listOfReadersMC_.push_back(0);  
    //listOfSpies_.push_back(0);
    listOfEventsToProcess_.push_back(0); 
    listOfEventsToProcessMC_.push_back(0); 
    listOfEvents_.push_back(0); 
  }

  fullStats_ = fullStats;
  lum_       = lum;

  init();
  
}


void SampleProcessor::initProcessedFiles(){

  TString dataFolder  ("xxx"); 	 	 
  TString mcFolder    ("xxx");   

  listOfurls_.clear(); 
  listOfurls_.push_back(dataFolder+TString("out-data.root"));
  listOfurls_.push_back(mcFolder+TString("out-ttbar.root"));
  listOfurls_.push_back(mcFolder+TString("out-singletop.root"));  
  listOfurls_.push_back(mcFolder+TString("out-wjets.root"));  
  listOfurls_.push_back(mcFolder+TString("out-zjets.root"));  
  listOfurls_.push_back(mcFolder+TString("out-dibosons.root"));  
  listOfurls_.push_back(mcFolder+TString("out-qcd.root"));  

  listOfProcessedFiles_.clear();

  // open files ///////////////////////////////////////////////////////////////////////////////////////////////
  for(unsigned int i=0;i <listOfurls_.size();i++){
    listOfProcessedFiles_.push_back( TFile::Open(listOfurls_[i]) );
    if(! listOfProcessedFiles_[i] ){ cout <<endl<<"\n Can't find file : "<<(listOfurls_[i])<<endl; continue;}  
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////

}



void SampleProcessor::init(){


  // data location in the tree ///////////////////////
  TString cutflow("myEventSelector/Selection/cutflow");
  TString tag("data"); 
  TString id("myEventSelector");
  ////////////////////////////////////////////////////


  // open files ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(unsigned int i=0;i <URLS_;i++){
    listOfFiles_[i] = TFile::Open(listOfurls_[i]);  
    if(! listOfFiles_[i] ){ cout <<endl<<"\n Can't find file : "<<(listOfurls_[i])<<endl; continue;}

    listOfHistos_[i] = (TH1D *) (listOfFiles_[i])->Get(cutflow);
    listOfEvents_[i] = (listOfHistos_[i])->GetBinContent(1);

    //    double unsplitNumber(1.);                                                                                                                                                      

    //if     (listOfurls_[i].Contains("ttbar_") )           /*unsplitNumber*/ listOfEvents_[i] = 53938617.; // 8791198; //fn == 8272517/0.941 //8272517.;
  
    /* ttbar_v3a = 22 969 600.; ttbar_v3b = 22 696 500.; */ 

    if(listOfurls_[i].Contains("FullLeptMG_") )      /*unsplitNumber*/ listOfEvents_[i] = 12011400.; 
    else if(listOfurls_[i].Contains("SemiLeptMG_") )      /*unsplitNumber*/ listOfEvents_[i] = 24937400.; 
    else if(listOfurls_[i].Contains("HadronicMG_") )      /*unsplitNumber*/ listOfEvents_[i] = 10537400.;

    else if(listOfurls_[i].Contains("PowThia_") )   /*unsplitNumber*/ listOfEvents_[i] =  25610100.;
    else if(listOfurls_[i].Contains("PowRwig_") )   /*unsplitNumber*/ listOfEvents_[i] =  27348200.;
 
    else if(listOfurls_[i].Contains("161p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5369214.; 
    else if(listOfurls_[i].Contains("163p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5365348.;
    else if(listOfurls_[i].Contains("166p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  4469095.; 
    else if(listOfurls_[i].Contains("169p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5202810.; //5543751; //fn == 5.20281E+06/0.9385
    else if(listOfurls_[i].Contains("175p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5178490.; //5491506; //fn == 5.17849E+06/0.943
    else if(listOfurls_[i].Contains("178p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  4725470.; ///4996268; //fn == 4.72547E+06/0.9458
    else if(listOfurls_[i].Contains("181p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5145140.; 
    else if(listOfurls_[i].Contains("184p5_") )      /*unsplitNumber*/ listOfEvents_[i] =  5249525.; 

    else if(listOfurls_[i].Contains("scaledown_") )      /*unsplitNumber*/ listOfEvents_[i] = 32844600.; // w/o SpCor 5371170.;
    else if(listOfurls_[i].Contains("scaleup_") )      /*unsplitNumber*/ listOfEvents_[i] =  32887000.; // w/o SpCor 4969480.;
    else if(listOfurls_[i].Contains("matchdown_") )      /*unsplitNumber*/ listOfEvents_[i] = 32879600.; // w/o SpCor  5388720.;
    else if(listOfurls_[i].Contains("matchup_") )      /*unsplitNumber*/ listOfEvents_[i] =  32869700.; // w/o SpCor 5359000.;

    else if(listOfurls_[i].Contains("WJetsToLNu_") )      /*unsplitNumber*/ listOfEvents_[i] = 75337700.;
    else if(listOfurls_[i].Contains("dy_from50_") )       /*unsplitNumber*/ listOfEvents_[i] = 30405400.;
    else if(listOfurls_[i].Contains("dy_10_50_") )        /*unsplitNumber*/ listOfEvents_[i] =  8944070.;//Pietro's is different
    //    else if(listOfurls_[i].Contains("stop_t_v") )        /*unsplitNumber*/ listOfEvents_[i] =  3741874.;

    else if(listOfurls_[i].Contains("stop_s_"             ))    listOfEvents_[i] =  259960    ;
    else if(listOfurls_[i].Contains("stop_t_"             ))    listOfEvents_[i] =  3741874   ;
    else if(listOfurls_[i].Contains("stop_tW-DR_"         ))    listOfEvents_[i] =  497657    ;
    else if(listOfurls_[i].Contains("stop_tWCat_"         ))    listOfEvents_[i] =  497657    ;
    else if(listOfurls_[i].Contains("stopbar_s_"          ))    listOfEvents_[i] =  139974    ;
    else if(listOfurls_[i].Contains("stopbar_t_"          ))    listOfEvents_[i] =  1915066   ;
    else if(listOfurls_[i].Contains("stopbar_tW-DR_"      ))    listOfEvents_[i] =  493458    ;
    else if(listOfurls_[i].Contains("stopbar_tWCat_"      ))    listOfEvents_[i] =  493458    ;

    else if(listOfurls_[i].Contains("EM_Pt20to30_") )     /*unsplitNumber*/ listOfEvents_[i] =  25919000.;
    else if(listOfurls_[i].Contains("EM_Pt30to80_") )     /*unsplitNumber*/ listOfEvents_[i] =  19784000.;//Pietro's is different but I checked mine once again!!!
    else if(listOfurls_[i].Contains("EM_Pt80to170_") )     /*unsplitNumber*/ listOfEvents_[i] =  19945900.;
    else if(listOfurls_[i].Contains("EM_Pt170to250_") )     /*unsplitNumber*/ listOfEvents_[i] =  24900900.;
    else if(listOfurls_[i].Contains("EM_Pt250to350_") )     /*unsplitNumber*/ listOfEvents_[i] =  24900900.;
    else if(listOfurls_[i].Contains("EM_Pt350toinf_") )     /*unsplitNumber*/ listOfEvents_[i] =  24802000.;

    else if(listOfurls_[i].Contains("WW_") )     /*unsplitNumber*/ listOfEvents_[i] =  9969490.;
    else if(listOfurls_[i].Contains("WZ_") )     /*unsplitNumber*/ listOfEvents_[i] =  9919520.;
    else if(listOfurls_[i].Contains("ZZ_") )     /*unsplitNumber*/ listOfEvents_[i] =  9743980.;

    else if(listOfurls_[i].Contains("30to80_BCtoE_") )     /*unsplitNumber*/ listOfEvents_[i] =  2043150.;
    else if(listOfurls_[i].Contains("80to170_BCtoE_") )     /*unsplitNumber*/ listOfEvents_[i] = 1945520.;
    else if(listOfurls_[i].Contains("170to250_BCtoE_") )     /*unsplitNumber*/ listOfEvents_[i] = 1948110.;
    else if(listOfurls_[i].Contains("250to350_BCtoE_") )     /*unsplitNumber*/ listOfEvents_[i] = 2016520.;
    else if(listOfurls_[i].Contains("350toinf_BCtoE_") )     /*unsplitNumber*/ listOfEvents_[i] = 1943530.;

    else if(listOfurls_[i].Contains("PhotonJets_15to30_") )     /*unsplitNumber*/ listOfEvents_[i] = 1970740.;
    else if(listOfurls_[i].Contains("PhotonJets_30to50_") )     /*unsplitNumber*/ listOfEvents_[i] = 1984320.;
    else if(listOfurls_[i].Contains("PhotonJets_50to80_") )     /*unsplitNumber*/ listOfEvents_[i] = 1986060.;
    else if(listOfurls_[i].Contains("PhotonJets_80to120_") )     /*unsplitNumber*/ listOfEvents_[i] = 1983630.;
    else if(listOfurls_[i].Contains("PhotonJets_120to170_") )     /*unsplitNumber*/ listOfEvents_[i] = 1960040.;
    else if(listOfurls_[i].Contains("PhotonJets_170to300_") )     /*unsplitNumber*/ listOfEvents_[i] = 1990070.;
    else if(listOfurls_[i].Contains("PhotonJets_300to470_") )     /*unsplitNumber*/ listOfEvents_[i] = 2000130.;

    else if(listOfurls_[i].Contains("qcdmu15_20toinf_"    ))    listOfEvents_[i] =  20920569  ;

    listOfScales_[i] = (lum_*listOfXSections_[i])/listOfEvents_[i];   

    if( listOfXSections_[i] == 0){ listOfScales_[i]=1;}  // Data

    listOfReaders_[i]     = new event::Reader(); 
    listOfReadersMC_[i]   = new event::Reader();

    listOfEventsToProcess_[i]   = (listOfReaders_[i])->AssignMiniEventTreeFrom(listOfFiles_[i],tag,id);
    listOfEventsToProcessMC_[i] = (listOfReadersMC_[i])->AssignMiniEventTreeFrom(listOfFiles_[i],"mc",id);


//     //SPY ///////////////////////////////////////////////////////////
    
//     TString extra;
//     if(eChONmuChOFF_) extra=TString("_spy_eltau.root");
//     else extra=TString("_spy_mutau.root");
//     TString spyName = outFileName_ + extra;
//     TTree * spyTree = listOfReaders_[i]->PrepareToSpyEvents(spyName);
//     cout<<endl<<"Spy Events ... on file ... "<<spyName<<endl; 
   
    //////////////////////////////////////////////////////////////////
    
    if(fullStats_){
      cout<<endl<<"\n processing file  : "<<(listOfurls_[i])
          <<"\n Tau pt cut was         : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<sampleTauPtCut_
          <<"\n sample -> scale        : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<(listOfScales_[i])
          <<"\n evt processed          : "<<setw(8)<<setprecision(1)<<(listOfEvents_[i])
          <<"\n evt in minitree        : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i])
          <<"\n MC evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcessMC_[i])
          <<"\n Xsection employed      : "<<(listOfXSections_[i])
          <<"\n Normalized to lumi (pb): "<<LUM_<<endl;
      
      infoFile_<<endl<<"\n processing file : "<<(listOfurls_[i])
	       <<"\n Tau pt cut was      : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<sampleTauPtCut_
	       <<"\n sample -> scale     : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<(listOfScales_[i])
	       <<"\n evt processed       : "<<setw(8)<<setprecision(1)<<(listOfEvents_[i])
	       <<"\n evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i])
	       <<"\n MC evt in minitree  : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcessMC_[i])
	       <<"\n Xsection employed : "<<(listOfXSections_[i])
	       <<"\n Normalized to lumi (pb): "<<LUM_<<endl;
      
      if(trigEffStudy_)   {  infoFile_<<" Trigger efficiency is ON  "; cout<<" Trigger efficiency is ON ";  }
      else                {  infoFile_<<" Trigger efficiency is OFF "; cout<<" Trigger efficiency is OFF "; }
      if(applybtagweight_){  infoFile_<<" BTAG weights are ON  ";      cout<<" BTAG weights are ON ";       }
      else                {  infoFile_<<" BTAG weights are OFF ";      cout<<" BTAG weights are OFF";       }



    }else{ 

      cout<<endl<<"\n processing file : "<<(listOfurls_[i])<<"\n evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i]);
      infoFile_<<"\n processing file : "<<(listOfurls_[i])<<"\n evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i]);
 
      if( nToProcess_ > listOfEventsToProcess_[i] ){ 
        cout<<endl<<" WARNING :: required number of events too large : "<<nToProcess_<<" sample will be fully processed..."<<endl;
        infoFile_<<"  WARNING :: required number of events too large : "<<nToProcess_<<" sample will be fully processed..."<<endl;
   
      }else {      
        cout<<" but only "<<nToProcess_<<" will be processed... "<<endl;
        infoFile_<<" but only "<<nToProcess_<<" will be processed... "<<endl;

        listOfScales_[i] = (lum_*listOfXSections_[i])/nToProcess_;   
        listOfEventsToProcess_[i] = nToProcess_;
      }

      if(trigEffStudy_)   {  infoFile_<<" Trigger efficiency is ON ";  cout<<" Trigger efficiency is ON ";  }
      else                {  infoFile_<<" Trigger efficiency is OFF "; cout<<" Trigger efficiency is OFF "; } 
      if(commondefinitions::applybtagweight_){  infoFile_<<" BTAG weights are ON  ";      cout<<" BTAG weights are ON ";       }
      else                {  infoFile_<<" BTAG weights are OFF ";      cout<<" BTAG weights are OFF";       }

    }
  }// finish processing urls /////////////////////////////////////////////////////////////////////////////////////////////////////////
  

}



int SampleProcessor::codeChannel(int i, int urlCode){

  int ret(666);
  if( 
    urlCode == WH80_URL  || urlCode == HH80_URL  ||
    urlCode == WH100_URL || urlCode == HH100_URL ||
    urlCode == WH120_URL || urlCode == HH120_URL || 
    urlCode == WH140_URL || urlCode == HH140_URL || 
    urlCode == WH150_URL || urlCode == HH150_URL || 
    urlCode == WH155_URL || urlCode == HH155_URL ||
    urlCode == WH160_URL || urlCode == HH160_URL 
  )                                          {  ret = HIGGS_CH;     }
  else if( 
	  urlCode == TBH180_URL || 
	  urlCode == TBH190_URL ||
	  urlCode == TBH200_URL ||
	  urlCode == TBH220_URL ||
	  urlCode == TBH250_URL ||
	  urlCode == TBH300_URL
	  ){  ret = TBH_CH;       }                   
  else if( i == ETAU                        ){  ret = ETAU_CH;      } 
  else if( i == MUTAU                       ){  ret = MUTAU_CH;     } 
  else if( i == EJETS                       ){  ret = EJETS_CH;     }
  else if( i == MUJETS                      ){  ret = MUJETS_CH;    }
  else if( i == EE                          ){  ret = EE_CH;        }
  else if( i == MUMU                        ){  ret = MUMU_CH;      }
  else if( i == EMU                         ){  ret = EMU_CH;       }
  else if( i == TAUJETS                     ){  ret = TAUJETS_CH;   }
  else if( i == TAUTAU                      ){  ret = TAUTAU_CH;    }
  else if( i == ALLJETS                     ){  ret = ALLJETS_CH;   }
  else if( i == WW || i == WZ || i== ZZ     ){  ret = EWKDI_CH;     }
  else if( i == WQQ                         ){  ret = WQQ_CH;       }
  else if( i == WENU                        ){  ret = WENU_CH;      }
  else if( i == WMUNU                       ){  ret = WMUNU_CH;     }
  else if( i == WTAUNU                      ){  ret = WTAUNU_CH;    }  
  else if( i == ZJETS                       ){  ret = ZJETS_CH;     } 
  else if( i == ZTAUTAU                     ){  ret = ZTAUTAU_CH;   } 
  else if( i == TOPS                        ){  ret = SINGLETOP_CH; } 
  else if( i == TOPT                        ){  ret = SINGLETOP_CH; } 
  else if( i == TOPW                        ){  ret = SINGLETOP_CH; } 
  else if( i == QCD                         ){  ret = QCD_CH;       } 

  return ret;

}


 
int SampleProcessor::tdChannel(int i){
  
  int ret(666);
  if     ( i == ETAU_CH                              ){  ret = ETAU_TDCH;      } 
  else if( i == MUTAU_CH                             ){  ret = MUTAU_TDCH;     } 
  else if( i == EJETS_CH || i == MUJETS_CH           ){  ret = LEPJETS_TDCH;   }
  //  else if( i == EE_CH || i == EMU_CH || i == MUMU_CH ){  ret = LEPLEP_TDCH;    }
  else if( i == EE_CH                                ){  ret = EE_TDCH;        }
  else if( i == EMU_CH                               ){  ret = EMU_TDCH;       }
  else if( i == MUMU_CH                              ){  ret = MUMU_TDCH;      }
  else if( i == TAUJETS_CH                           ){  ret = TAUJETS_TDCH;   }
  else if( i == TAUTAU_CH                            ){  ret = TAUTAU_TDCH;    }
  else if( i == ALLJETS_CH                           ){  ret = ALLJETS_TDCH;   }
  else if( i == EWKDI_CH                             ){  ret = EWKDI_TDCH;     }
  else if( i == WQQ_CH                               ){  ret = WQQ_TDCH;       }
  else if( i == WENU_CH                              ){  ret = WENU_TDCH;      }
  else if( i == WMUNU_CH                             ){  ret = WMUNU_TDCH;     }
  else if( i == WTAUNU_CH                            ){  ret = WTAUNU_TDCH;    }  
  else if( i == WJETS_CH                             ){  ret = WJETS_TDCH;     }
  else if( i == ZJETS_CH                             ){  ret = ZJETS_TDCH;     } 
  else if( i == ZTAUTAU_CH                           ){  ret = ZTAUTAU_TDCH;   } 
  else if( i == SINGLETOP_CH                         ){  ret = SINGLETOP_TDCH; } 
  else if( i == QCD_CH                               ){  ret = QCD_TDCH;       } 

  return ret;

}


void SampleProcessor::process_ttbar_mcnlo_mutau(){
  url_= TTBAR_URL;
  process(false, url_, iFolder_ + TString("ttbar_mcnlo_1.root"), oFolder_+TString("out-ttbar-mcnlo_mutau.root"),keys_, MUTAU_ ); 
}



void SampleProcessor::process_ttbar(int i){

  //NORMAL SAMPLE 
  url_= TTBAR_URL; 

  if(i>49){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  //fn  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/ttbar_"+idx+".root"),
  process(false, url_, iFolder_+TString("ttbar_"+idx+".root"),
	  oFolder_+TString("out-ttbar_"+idx+".root"),keys_);
 
  //SAMPLE WITH WEIGHTS
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf-nofilter-B.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  ///lustre/data3/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/ttbar-pdf-nofilter-B.root

//   switch(i){
//   case 1: 
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_1.root"), oFolder_+TString("out-ttbar_v1_1.root"),keys_);
//     break;
//   case 2:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_2.root"), oFolder_+TString("out-ttbar_v1_2.root"),keys_);
//     break;
//   case 3:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_3.root"), oFolder_+TString("out-ttbar_v1_3.root"),keys_);
//     break;
//   case 4:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_4.root"), oFolder_+TString("out-ttbar_v1_4.root"),keys_);
//     break;
//   case 5:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_5.root"), oFolder_+TString("out-ttbar_v1_5.root"),keys_);
//     break;
//   case 6:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_6.root"), oFolder_+TString("out-ttbar_v1_6.root"),keys_);
//     break;
//   case 7:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_7.root"), oFolder_+TString("out-ttbar_v1_7.root"),keys_);
//     break;
//   case 8:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_8.root"), oFolder_+TString("out-ttbar_v1_8.root"),keys_);
//     break;
//   case 9:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_9.root"), oFolder_+TString("out-ttbar_v1_9.root"),keys_);
//     break;
//   case 10:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v2.root"), oFolder_+TString("out-ttbar_v2.root"),keys_);
//     break;
//   }

}


void SampleProcessor::process_ttbar169p5(int i){

  //NORMAL SAMPLE 
  url_= TTBAR_URL; 

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_166p5_"+idx+".root"),
	  oFolder_+TString("out-ttbar-166p5_"+idx+".root"),keys_);
 
  //SAMPLE WITH WEIGHTS
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf-nofilter-B.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  ///lustre/data3/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/ttbar-pdf-nofilter-B.root

}

void SampleProcessor::process_ttbar175p5(int i){

  //NORMAL SAMPLE 
  url_= TTBAR_URL; 

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar-175p5_"+idx+".root"),
	  oFolder_+TString("out-ttbar-175p5_"+idx+".root"),keys_);
 
  //SAMPLE WITH WEIGHTS
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf-nofilter-B.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  ///lustre/data3/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/ttbar-pdf-nofilter-B.root

}

void SampleProcessor::process_ttbar_unc(){

  url_= TTBAR_URL;

  process(false, url_, iFolder_ + TString("ttbar_scaleup.root"),   oFolder_+TString("out-ttbar-scaleup.root"),keys_);
  process(false, url_, iFolder_ + TString("ttbar_scaledown.root"), oFolder_+TString("out-ttbar-scaledown.root"),keys_);
  process(false, url_, iFolder_ + TString("ttbar_matchup.root"),   oFolder_+TString("out-ttbar-matchup.root"),keys_);
  process(false, url_, iFolder_ + TString("ttbar_matchdown.root"), oFolder_+TString("out-ttbar-matchdown.root"),keys_);
  
/*
  process(false, url_, iFolder_ + TString("ttbar-scaleup_424.root"),   oFolder_+TString("out-ttbar-scaleup.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-scaledown_424.root"), oFolder_+TString("out-ttbar-scaledown.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-matchup_424.root"),   oFolder_+TString("out-ttbar-pdfup.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-matchdown_424.root"), oFolder_+TString("out-ttbar-pdfdown.root"),keys_);   

  process(false, url_, iFolder_ + TString("ttbar-m161p5_424.root"),   oFolder_+TString("out-ttbar-m161p5.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-m166p5_424.root"),   oFolder_+TString("out-ttbar-m166p5.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-m169p5_424.root"),   oFolder_+TString("out-ttbar-m169p5.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-m175p5_424.root"),   oFolder_+TString("out-ttbar-m175p5.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-m178p5_424.root"),   oFolder_+TString("out-ttbar-m178p5.root"),keys_);    
  process(false, url_, iFolder_ + TString("ttbar-m184p5_424.root"),   oFolder_+TString("out-ttbar-m184p5.root"),keys_);  

*/

  // this is negleted for now 
/*
  process(false, url_, iFolder_ + TString("ttbar-m161p5_424.root"),   oFolder_+TString("out-ttbar-m161p5_etau.root"),keys_,ETAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m166p5_424.root"),   oFolder_+TString("out-ttbar-m166p5_etau.root"),keys_,ETAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m169p5_424.root"),   oFolder_+TString("out-ttbar-m169p5_etau.root"),keys_,ETAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m175p5_424.root"),   oFolder_+TString("out-ttbar-m175p5_etau.root"),keys_,ETAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m178p5_424.root"),   oFolder_+TString("out-ttbar-m178p5_etau.root"),keys_,ETAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m184p5_424.root"),   oFolder_+TString("out-ttbar-m184p5_etau.root"),keys_,ETAU_);  
  
  process(false, url_, iFolder_ + TString("ttbar-m161p5_424.root"),   oFolder_+TString("out-ttbar-m161p5_mutau.root"),keys_,MUTAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m166p5_424.root"),   oFolder_+TString("out-ttbar-m166p5_mutau.root"),keys_,MUTAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m169p5_424.root"),   oFolder_+TString("out-ttbar-m169p5_mutau.root"),keys_,MUTAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m175p5_424.root"),   oFolder_+TString("out-ttbar-m175p5_mutau.root"),keys_,MUTAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m178p5_424.root"),   oFolder_+TString("out-ttbar-m178p5_mutau.root"),keys_,MUTAU_);    
  process(false, url_, iFolder_ + TString("ttbar-m184p5_424.root"),   oFolder_+TString("out-ttbar-m184p5_mutau.root"),keys_,MUTAU_);    

  process(false, url_, iFolder_ + TString("ttbar-m161p5_424.root"),   oFolder_+TString("out-ttbar-m161p5_other.root"),keys_,TTBAR_OTHER_);    
  process(false, url_, iFolder_ + TString("ttbar-m166p5_424.root"),   oFolder_+TString("out-ttbar-m166p5_other.root"),keys_,TTBAR_OTHER_);    
  process(false, url_, iFolder_ + TString("ttbar-m169p5_424.root"),   oFolder_+TString("out-ttbar-m169p5_other.root"),keys_,TTBAR_OTHER_);    
  process(false, url_, iFolder_ + TString("ttbar-m175p5_424.root"),   oFolder_+TString("out-ttbar-m175p5_other.root"),keys_,TTBAR_OTHER_);    
  process(false, url_, iFolder_ + TString("ttbar-m178p5_424.root"),   oFolder_+TString("out-ttbar-m178p5_other.root"),keys_,TTBAR_OTHER_);    
  process(false, url_, iFolder_ + TString("ttbar-m184p5_424.root"),   oFolder_+TString("out-ttbar-m184p5_other.root"),keys_,TTBAR_OTHER_);    

*/
}


void SampleProcessor::process_ttbar_scaleup(int i){

  url_= TTBAR_URL;

  if(i>99){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  //  process(false, url_, iFolder_ + TString("ttbar_scaleup.root"),   oFolder_+TString("out-ttbar_scaleup.root"),keys_);
  //  process(false, url_, iFolder_ + TString("ttbar_scaleup.root"), oFolder_+TString("out-ttbar-scaleup_mutau.root"),keys_, MUTAU_ ); 
  //fn  process(false, url_, iFolder_ + TString("ttbar_scaleup_"+idx+".root"),
  //fn	  oFolder_+TString("out-ttbar-scaleup_"+idx+".root"),keys_); 
  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttMSC_scaleup_"+idx+".root"),
	  oFolder_+TString("out-ttbar-scaleup_"+idx+".root"),keys_); 
}


void SampleProcessor::process_ttbar_scaledown(int i){

  url_= TTBAR_URL;

  if(i>99){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  //  process(false, url_, iFolder_ + TString("ttbar_scaledown.root"), oFolder_+TString("out-ttbar_scaledown.root"),keys_);
  //  process(false, url_, iFolder_ + TString("ttbar_scaledown.root"), oFolder_+TString("out-ttbar-scaledown_mutau.root"),keys_, MUTAU_ ); 
  //fn  process(false, url_, iFolder_ + TString("ttbar_scaledown_"+idx+".root"),
  //fn	  oFolder_+TString("out-ttbar-scaledown_"+idx+".root"),keys_); 
  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttMSC_scaledown_"+idx+".root"),
	  oFolder_+TString("out-ttbar-scaledown_"+idx+".root"),keys_); 
}


void SampleProcessor::process_ttbar_matchup(int i){

  url_= TTBAR_URL;

  if(i>99){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  //  process(false, url_, iFolder_ + TString("ttbar_matchup.root"),   oFolder_+TString("out-ttbar_matchup.root"),keys_);
  //  process(false, url_, iFolder_ + TString("ttbar_matchup.root"), oFolder_+TString("out-ttbar-matchup_mutau.root"),keys_, MUTAU_ ); 
  //fn   process(false, url_, iFolder_ + TString("ttbar_matchup_"+idx+".root"),
  //fn	  oFolder_+TString("out-ttbar-matchup_"+idx+".root"),keys_); 
  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttMSC_matchup_"+idx+".root"),
	  oFolder_+TString("out-ttbar-matchup_"+idx+".root"),keys_); 
}


void SampleProcessor::process_ttbar_matchdown(int i){

  url_= TTBAR_URL;

  if(i>99){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  //  process(false, url_, iFolder_ + TString("ttbar_matchdown.root"), oFolder_+TString("out-ttbar_matchdown.root"),keys_);
  //  process(false, url_, iFolder_ + TString("ttbar_matchdown.root"), oFolder_+TString("out-ttbar-matchdown_mutau.root"),keys_, MUTAU_ ); 
  //fn  process(false, url_, iFolder_ + TString("ttbar_matchdown_"+idx+".root"),
  //fn	  oFolder_+TString("out-ttbar-matchdown_"+idx+".root"),keys_); 
  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttMSC_matchdown_"+idx+".root"),
	  oFolder_+TString("out-ttbar-matchdown_"+idx+".root"),keys_); 
}

void SampleProcessor::process_ttFullLeptMG(int i){

  url_= TTFULL_URL;

  if(i>89){
    cout << "ERROR CODE - must be in the range [0,89]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttFullLeptMG_"+idx+".root"),
	  //	  oFolder_+TString("out-ttFullLept-etau_"+idx+".root"),keys_, ETAU_ ); 
	  oFolder_+TString("out-ttFullLept-mutau_"+idx+".root"),keys_, MUTAU_ ); 
	  //  	  oFolder_+TString("out-ttFullLept-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_ ); 
}
void SampleProcessor::process_ttSemiLeptMG(int i){

  url_= TTSEMI_URL;

  if(i>299){
    cout << "ERROR CODE - must be in the range [0,299]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttSemiLeptMG_"+idx+".root"),
	  oFolder_+TString("out-ttSemiLept_"+idx+".root"),keys_ ); 
}
void SampleProcessor::process_ttHadronicMG(int i){

  url_= TTHADR_URL;

  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttHadronicMG_"+idx+".root"),
	  oFolder_+TString("out-ttHadronic_"+idx+".root"),keys_ ); 
}



void SampleProcessor::process_ttv3a_etau(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3a_"+idx+".root"),
	  oFolder_+TString("out-ttv3a-etau_"+idx+".root"),keys_,ETAU_); 
}


void SampleProcessor::process_ttv3b_etau(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3b_"+idx+".root"),
	  oFolder_+TString("out-ttv3b-etau_"+idx+".root"),keys_,ETAU_); 
}


void SampleProcessor::process_ttbar_etau(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>149){
    cout << "ERROR CODE - must be in the range [0,149]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  

  //  process(false, url_,iFolder_+TString("ttbar_"+idx+".root"),oFolder_+TString("out-ttbar-etau_"+idx+".root"),keys_, ETAU_ ); 

//   process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttFullLeptMG_"+idx+".root"),
// 	  oFolder_+TString("out-ttFullLept-etau_"+idx+".root"),keys_,ETAU_); 

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttPowThia_"+idx+".root"),
	  oFolder_+TString("out-ttPowThia_"+idx+".root"),keys_ ); 


//   switch(i){
//   case 1:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_1.root"), oFolder_+TString("out-ttbar-etau_v1_1.root"),keys_,ETAU_);
//     break;
//   case 2:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_2.root"), oFolder_+TString("out-ttbar-etau_v1_2.root"),keys_,ETAU_);
//     break;
//   case 3:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_3.root"), oFolder_+TString("out-ttbar-etau_v1_3.root"),keys_,ETAU_);
//     break;
//   case 4:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_4.root"), oFolder_+TString("out-ttbar-etau_v1_4.root"),keys_,ETAU_);
//     break;
//   case 5:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_5.root"), oFolder_+TString("out-ttbar-etau_v1_5.root"),keys_,ETAU_);
//     break;
//   case 6:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_6.root"), oFolder_+TString("out-ttbar-etau_v1_6.root"),keys_,ETAU_);
//     break;
//   case 7:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_7.root"), oFolder_+TString("out-ttbar-etau_v1_7.root"),keys_,ETAU_);
//     break;
//   case 8:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_8.root"), oFolder_+TString("out-ttbar-etau_v1_8.root"),keys_,ETAU_);
//     break;
//   case 9:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v1_9.root"), oFolder_+TString("out-ttbar-etau_v1_9.root"),keys_,ETAU_);
//     break;
//   case 10:
//     process(false, url_, iFolder_ + TString("mc/ttbar_v2.root"), oFolder_+TString("out-ttbar-etau_v2.root"),keys_,ETAU_);
//     break;
//   }

  //  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar-etau.root"),keys_,  ETAU_  );    
  //  process(false, url_, iFolder_ + TString("ttbar_v1.root"), oFolder_+TString("out-ttbar-etau.root"),keys_, ETAU_ ); 
}


void SampleProcessor::process_ttv3a_mutau(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3a_"+idx+".root"),
	  oFolder_+TString("out-ttv3a-mutau_"+idx+".root"),keys_,MUTAU_); 
}


void SampleProcessor::process_ttv3b_mutau(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3b_"+idx+".root"),
	  oFolder_+TString("out-ttv3b-mutau_"+idx+".root"),keys_,MUTAU_); 
}


void SampleProcessor::process_ttbar_mutau(int i){

  url_= TTBAR_URL; //fn url_= TTFULL_URL; 

  if(i>149){
    cout << "ERROR CODE - must be in the range [0,149]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  //  process(false, url_, iFolder_ + TString("ttbar_v1.root"), oFolder_+TString("out-ttbar_v1_mutau.root"),keys_, MUTAU_ ); 
  //  process(false, url_, iFolder_ + TString("ttbar_v2.root"), oFolder_+TString("out-ttbar_v2_mutau.root"),keys_, MUTAU_ ); 

  //  process(false, url_,iFolder_+TString("ttbar_"+idx+".root"),oFolder_+TString("out-ttbar-mutau_"+idx+".root"),keys_, MUTAU_ );

//   process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttFullLeptMG_"+idx+".root"),
// 	  oFolder_+TString("out-ttFullLept-mutau_"+idx+".root"),keys_,MUTAU_); 

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttPowThia_"+idx+".root"),
	  oFolder_+TString("out-ttPowThia_"+idx+".root"),keys_ ); 


}


void SampleProcessor::process_ttbar_mumu(int i){

  url_= TTBAR_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/ttbar_"+idx+".root"), oFolder_+TString("out-ttbar-mumu_"+idx+".root"),keys_, MUMU_ ); 
}


void SampleProcessor::process_ttbar_emu(int i){

  url_= TTBAR_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/ttbar_"+idx+".root"), oFolder_+TString("out-ttbar-emu_"+idx+".root"),keys_, EMU_ ); 

}


void SampleProcessor::process_ttv3a_ddbkg(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3a_"+idx+".root"),
	  oFolder_+TString("out-ttv3a-ddbkg_"+idx+".root"),keys_, TTBAR_DDBKG_); 
}


void SampleProcessor::process_ttv3b_ddbkg(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3b_"+idx+".root"),
	  oFolder_+TString("out-ttv3b-ddbkg_"+idx+".root"),keys_, TTBAR_DDBKG_); 
}

void SampleProcessor::process_ttbar_ddbkg(int i){

  url_= TTBAR_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [1,18]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  // dd bkg
  //  process(false, url_, iFolder_ + TString("ttbar_v1.root"), oFolder_+TString("out-ttbar_v1_ddbkg.root"),keys_, TTBAR_DDBKG_);   
  //  process(false, url_, iFolder_ + TString("ttbar_v2.root"), oFolder_+TString("out-ttbar_v2_ddbkg.root"),keys_, TTBAR_DDBKG_);   
  //fn  process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/ttbar_"+idx+".root"),
  process(false, url_,iFolder_+TString("ttbar_"+idx+".root"),
	  oFolder_+TString("out-ttbar-ddbkg_"+idx+".root"),keys_, TTBAR_DDBKG_);   

}

void SampleProcessor::process_ttv3a_mcbkg(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3a_"+idx+".root"),
	  oFolder_+TString("out-ttv3a-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_); 
}


void SampleProcessor::process_ttv3b_mcbkg(int i){
  
  url_= TTBAR_URL; //fn url_= TTFULL_URL; 
  
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,99]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttbar_v3b_"+idx+".root"),
	  oFolder_+TString("out-ttv3b-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_); 
}

void SampleProcessor::process_ttbar_mcbkg(int i){

  url_= TTBAR_URL; //fn url_= TTFULL_URL; 

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  // mc bkg
  //  process(false, url_, iFolder_ + TString("ttbar_v1.root"), oFolder_+TString("out-ttbar_v1_mcbkg.root"),keys_, TTBAR_MCBKG_ );
  //  process(false, url_, iFolder_ + TString("ttbar_v2.root"), oFolder_+TString("out-ttbar_v2_mcbkg.root"),keys_, TTBAR_MCBKG_ );

  process(false, url_,iFolder_+TString("ttbar_"+idx+".root"),oFolder_+TString("out-ttbar-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_ );
  // process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/ttFullLeptMG_"+idx+".root"),
  //  oFolder_+TString("out-ttFullLept-mcbkg_"+idx+".root"),keys_,TTBAR_MCBKG_); 

}


void SampleProcessor::process_singletop(int i){

  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  if(i<20){
  //SINGLE TOP
  url_= S_URL; process(false, url_,iFolder_+TString("stop_s_"+idx+".root"),oFolder_ + TString("out-stop_s_"+idx+".root"),keys_);   
  url_= T_URL; process(false, url_,iFolder_+TString("stop_t_"+idx+".root"),oFolder_ + TString("out-stop_t_"+idx+".root"),keys_);  
  //  url_= W_URL; process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/stop_tW-DR_"+idx+".root"), oFolder_ + TString("out-stop-DR_tW_"+idx+".root"),keys_); 

  //ANTI SINGLE TOP
  url_= A_S_URL; process(false, url_,iFolder_+TString("stopbar_s_"+idx+".root"),oFolder_ + TString("out-stopbar_s_"+idx+".root"),keys_);   
  }
  //  url_= A_T_URL; process(false, url_, iFolder_ + TString("stopbar_t_"+idx+".root"),     oFolder_ + TString("out-stopbar_t_"+idx+".root"),keys_);  
  //  url_= A_W_URL; process(false, url_, iFolder_ + TString("stopbar_tW-DR_"+idx+".root"), oFolder_ + TString("out-stopbar-DR_tW_"+idx+".root"),keys_);     

}


void SampleProcessor::process_stop_s(){

  url_= S_URL; process(false, url_, iFolder_ + TString("mc/stop_s.root"),     oFolder_ + TString("out-stop_s.root"),keys_);   
  url_= A_S_URL; process(false, url_, iFolder_ + TString("mc/stopbar_s.root"),     oFolder_ + TString("out-stopbar_s.root"),keys_);   

}


void SampleProcessor::process_stop_tW(int i){
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  if (i<10) {
//     url_= W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stop_tW-DR_"+idx+".root"),
//  			 oFolder_ + TString("out-stop-DR_tW_"+idx+".root"),keys_);
//     url_= A_W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stopbar_tW-DR_"+idx+".root"), 
// 			   oFolder_ + TString("out-stopbar-DR_tW_"+idx+".root"),keys_);

    url_= W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stop_tWCat_"+idx+".root"),
 			 oFolder_ + TString("out-stop-tWCat-mutau_"+idx+".root"),keys_, MUTAU_);
    url_= A_W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stopbar_tWCat_"+idx+".root"), 
			   oFolder_ + TString("out-stopbar-tWCat-mutau_"+idx+".root"),keys_, MUTAU_);

    url_= W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stop_tWCat_"+idx+".root"),
 			 oFolder_ + TString("out-stop-tWCat-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_);
    url_= A_W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stopbar_tWCat_"+idx+".root"), 
			   oFolder_ + TString("out-stopbar-tWCat-mcbkg_"+idx+".root"),keys_, TTBAR_MCBKG_);

//     url_= W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stop_tWCat_"+idx+".root"),
//  			 oFolder_ + TString("out-stop-tWCat_"+idx+".root"),keys_);
//     url_= A_W_URL; process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stopbar_tWCat_"+idx+".root"), 
// 			   oFolder_ + TString("out-stopbar-tWCat_"+idx+".root"),keys_);

    /* TString("/lustre/ncg.ingrid.pt/cmslocal/fnguyen/tau_dilepton/Monitor_16_1_S5Z.root"), */
  }

//   switch(i){
//   case 1:
//     url_= W_URL; process(false, url_, iFolder_ + TString("mc/stop_tW-DR.root"), oFolder_ + TString("out-stop-DR_tW.root"),keys_);
//     break;
//   case 2:
//     url_= A_W_URL; process(false, url_, iFolder_ + TString("mc/stopbar_tW-DR.root"), oFolder_ + TString("out-stopbar-DR_tW.root"),keys_);   
//     break;
//   }

}

void SampleProcessor::process_stop_t(int j){

  url_= T_URL;

  switch(j){
  case 1:
    process(false, url_, iFolder_ + TString("mc/stop_t_v2.root"),     oFolder_ + TString("out-stop_t_v2.root"),keys_);  
    break;
  case 2:
    process(false, url_, iFolder_ + TString("mc/stop_t_v3.root"),     oFolder_ + TString("out-stop_t_v3.root"),keys_);  
    break;
  case 3:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_1.root"),     oFolder_ + TString("out-stop_t_v1_1.root"),keys_);  
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_2.root"),     oFolder_ + TString("out-stop_t_v1_2.root"),keys_);  
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_3.root"),     oFolder_ + TString("out-stop_t_v1_3.root"),keys_);  
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_4.root"),     oFolder_ + TString("out-stop_t_v1_4.root"),keys_);  
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_5.root"),     oFolder_ + TString("out-stop_t_v1_5.root"),keys_);  
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_6.root"),     oFolder_ + TString("out-stop_t_v1_6.root"),keys_);  
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_7.root"),     oFolder_ + TString("out-stop_t_v1_7.root"),keys_);  
    break;
  case 10:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_8.root"),     oFolder_ + TString("out-stop_t_v1_8.root"),keys_);  
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/stop_t_v1_9.root"),     oFolder_ + TString("out-stop_t_v1_9.root"),keys_);  
    break;
//   case 12:
//     url_= A_T_URL;
//     process(false, url_, iFolder_ + TString("mc/stopbar_t_1.root"),     oFolder_ + TString("out-stopbar_t.root"),keys_);  
//     break;

  }
}

void SampleProcessor::process_stopbar_t(int i){

  url_= A_T_URL;

  if(i>9){
    cout << "ERROR CODE - must be in the range [0,9]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_,TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9/mc/stopbar_t_"+idx+".root"), oFolder_ + TString("out-stopbar_t_"+idx+".root"),keys_);  

}


void SampleProcessor::process_wjets(int i){

  // WARNING W+jets with new pgid info
  url_= WJMADGRAPH_URL;

  if(i>399){
    cout << "ERROR CODE - must be in the range [0,399]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();


  process(false, url_,iFolder_+TString("WJetsToLNu_"+idx+".root"), oFolder_+TString("out-wjets_"+idx+".root"),keys_);

}


void SampleProcessor::process_wjets_v1(int j){

  // WARNING W+jets with new pgid info
  url_= WJMADGRAPH_URL;

  switch(j){
  case 10:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_10.root"), oFolder_+TString("out-wjets_v1_10.root"),keys_);
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_11.root"), oFolder_+TString("out-wjets_v1_11.root"),keys_);
    break;
  case 12:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_12.root"), oFolder_+TString("out-wjets_v1_12.root"),keys_);
    break;
  case 13:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_13.root"), oFolder_+TString("out-wjets_v1_13.root"),keys_);
    break;
  case 14:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_14.root"), oFolder_+TString("out-wjets_v1_14.root"),keys_);
    break;
  case 15:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_15.root"), oFolder_+TString("out-wjets_v1_15.root"),keys_);
    break;
  case 16:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_16.root"), oFolder_+TString("out-wjets_v1_16.root"),keys_);
    break;
  case 17:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_17.root"), oFolder_+TString("out-wjets_v1_17.root"),keys_);
    break;
  case 18:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_18.root"), oFolder_+TString("out-wjets_v1_18.root"),keys_);
    break;
  case 19:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_19.root"), oFolder_+TString("out-wjets_v1_19.root"),keys_);
    break;
  case 2:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_2.root"), oFolder_+TString("out-wjets_v1_2.root"),keys_);
    break;
  case 3:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_3.root"), oFolder_+TString("out-wjets_v1_3.root"),keys_);
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_4.root"), oFolder_+TString("out-wjets_v1_4.root"),keys_);
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_5.root"), oFolder_+TString("out-wjets_v1_5.root"),keys_);
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_6.root"), oFolder_+TString("out-wjets_v1_6.root"),keys_);
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_7.root"), oFolder_+TString("out-wjets_v1_7.root"),keys_);
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_8.root"), oFolder_+TString("out-wjets_v1_8.root"),keys_);
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v1_9.root"), oFolder_+TString("out-wjets_v1_9.root"),keys_);
    break;
  }
}



void SampleProcessor::process_wjets_v2(int j){

  // WARNING W+jets with new pgid info
  url_= WJMADGRAPH_URL;

  switch(j){
  case 10:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_10.root"), oFolder_+TString("out-wjets_v2_10.root"),keys_);
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_11.root"), oFolder_+TString("out-wjets_v2_11.root"),keys_);
    break;
  case 12:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_12.root"), oFolder_+TString("out-wjets_v2_12.root"),keys_);
    break;
  case 13:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_13.root"), oFolder_+TString("out-wjets_v2_13.root"),keys_);
    break;
  case 14:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_14.root"), oFolder_+TString("out-wjets_v2_14.root"),keys_);
    break;
  case 15:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_15.root"), oFolder_+TString("out-wjets_v2_15.root"),keys_);
    break;
  case 16:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_16.root"), oFolder_+TString("out-wjets_v2_16.root"),keys_);
    break;
  case 17:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_17.root"), oFolder_+TString("out-wjets_v2_17.root"),keys_);
    break;
  case 18:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_18.root"), oFolder_+TString("out-wjets_v2_18.root"),keys_);
    break;
  case 19:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_19.root"), oFolder_+TString("out-wjets_v2_19.root"),keys_);
    break;

  case 20:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_20.root"), oFolder_+TString("out-wjets_v2_20.root"),keys_);
    break;
  case 21:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_21.root"), oFolder_+TString("out-wjets_v2_21.root"),keys_);
    break;
  case 22:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_22.root"), oFolder_+TString("out-wjets_v2_22.root"),keys_);
    break;
  case 23:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_23.root"), oFolder_+TString("out-wjets_v2_23.root"),keys_);
    break;
  case 24:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_24.root"), oFolder_+TString("out-wjets_v2_24.root"),keys_);
    break;
  case 25:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_25.root"), oFolder_+TString("out-wjets_v2_25.root"),keys_);
    break;
  case 26:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_26.root"), oFolder_+TString("out-wjets_v2_26.root"),keys_);
    break;
  case 27:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_27.root"), oFolder_+TString("out-wjets_v2_27.root"),keys_);
    break;
  case 28:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_28.root"), oFolder_+TString("out-wjets_v2_28.root"),keys_);
    break;
  case 29:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_29.root"), oFolder_+TString("out-wjets_v2_29.root"),keys_);
    break;

  case 30:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_30.root"), oFolder_+TString("out-wjets_v2_30.root"),keys_);
    break;
  case 31:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_31.root"), oFolder_+TString("out-wjets_v2_31.root"),keys_);
    break;
  case 32:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_32.root"), oFolder_+TString("out-wjets_v2_32.root"),keys_);
    break;
  case 33:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_33.root"), oFolder_+TString("out-wjets_v2_33.root"),keys_);
    break;
  case 34:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_34.root"), oFolder_+TString("out-wjets_v2_34.root"),keys_);
    break;
  case 35:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_35.root"), oFolder_+TString("out-wjets_v2_35.root"),keys_);
    break;
  case 36:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_36.root"), oFolder_+TString("out-wjets_v2_36.root"),keys_);
    break;
  case 37:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_37.root"), oFolder_+TString("out-wjets_v2_37.root"),keys_);
    break;
  case 38:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_38.root"), oFolder_+TString("out-wjets_v2_38.root"),keys_);
    break;
  case 39:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_39.root"), oFolder_+TString("out-wjets_v2_39.root"),keys_);
    break;

  case 0:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_0.root"), oFolder_+TString("out-wjets_v2_0.root"),keys_);
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_4.root"), oFolder_+TString("out-wjets_v2_4.root"),keys_);
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_5.root"), oFolder_+TString("out-wjets_v2_5.root"),keys_);
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_6.root"), oFolder_+TString("out-wjets_v2_6.root"),keys_);
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_7.root"), oFolder_+TString("out-wjets_v2_7.root"),keys_);
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_8.root"), oFolder_+TString("out-wjets_v2_8.root"),keys_);
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/WJetsToLNu_v2_9.root"), oFolder_+TString("out-wjets_v2_9.root"),keys_);
    break;

  }

}




void SampleProcessor::process_zjets_from50(int i){

  // MADGRAPH Z+JETS
  url_ = DY_FROM50_URL; 
  if(i>199){
    cout << "ERROR CODE - must be in the range [0,199]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false, url_,iFolder_+TString("dy_from50_"+idx+".root"), oFolder_+TString("out-dy_from50_"+idx+".root"),keys_); 
}

void SampleProcessor::process_zjets_10to50(int i){
  
  // MADGRAPH Z+JETS
  url_ = DY_10TO50_URL;
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  process(false, url_,iFolder_+TString("dy_10_50_"+idx+".root"), oFolder_+TString("out-dy_10to50_"+idx+".root"),keys_);

}

// void SampleProcessor::process_zjets_from50(int j){

//   // MADGRAPH Z+JETS
//   url_ = DY_FROM50_URL; 

//   switch(j){
//   case 10:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_10.root"), oFolder_+TString("out-dy_from50_10.root"),keys_);
//     break;
//   case 11:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_11.root"), oFolder_+TString("out-dy_from50_11.root"),keys_);
//     break;
//   case 12:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_12.root"), oFolder_+TString("out-dy_from50_12.root"),keys_);
//     break;
//   case 13:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_13.root"), oFolder_+TString("out-dy_from50_13.root"),keys_);
//     break;
//   case 14:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_14.root"), oFolder_+TString("out-dy_from50_14.root"),keys_);
//     break;
//   case 15:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_15.root"), oFolder_+TString("out-dy_from50_15.root"),keys_);
//     break;
//   case 16:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_16.root"), oFolder_+TString("out-dy_from50_16.root"),keys_);
//     break;
//   case 17:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_17.root"), oFolder_+TString("out-dy_from50_17.root"),keys_);
//     break;
//   case 18:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_18.root"), oFolder_+TString("out-dy_from50_18.root"),keys_);
//     break;
//   case 19:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_19.root"), oFolder_+TString("out-dy_from50_19.root"),keys_);
//     break;

//   case 20:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_20.root"), oFolder_+TString("out-dy_from50_20.root"),keys_);
//     break;
//   case 21:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_21.root"), oFolder_+TString("out-dy_from50_21.root"),keys_);
//     break;
//   case 22:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_22.root"), oFolder_+TString("out-dy_from50_22.root"),keys_);
//     break;
//   case 23:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_23.root"), oFolder_+TString("out-dy_from50_23.root"),keys_);
//     break;
//   case 24:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_24.root"), oFolder_+TString("out-dy_from50_24.root"),keys_);
//     break;
//   case 25:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_25.root"), oFolder_+TString("out-dy_from50_25.root"),keys_);
//     break;
//   case 26:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_26.root"), oFolder_+TString("out-dy_from50_26.root"),keys_);
//     break;
//   case 27:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_27.root"), oFolder_+TString("out-dy_from50_27.root"),keys_);
//     break;
//   case 28:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_28.root"), oFolder_+TString("out-dy_from50_28.root"),keys_);
//     break;
//   case 29:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_29.root"), oFolder_+TString("out-dy_from50_29.root"),keys_);
//     break;

//   case 30:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_30.root"), oFolder_+TString("out-dy_from50_30.root"),keys_);
//     break;
//   case 31:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_31.root"), oFolder_+TString("out-dy_from50_31.root"),keys_);
//     break;
//   case 32:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_32.root"), oFolder_+TString("out-dy_from50_32.root"),keys_);
//     break;
//   case 33:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_33.root"), oFolder_+TString("out-dy_from50_33.root"),keys_);
//     break;
//   case 34:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_34.root"), oFolder_+TString("out-dy_from50_34.root"),keys_);
//     break;
//   case 35:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_35.root"), oFolder_+TString("out-dy_from50_35.root"),keys_);
//     break;
//   case 36:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_36.root"), oFolder_+TString("out-dy_from50_36.root"),keys_);
//     break;
//   case 37:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_37.root"), oFolder_+TString("out-dy_from50_37.root"),keys_);
//     break;
//   case 38:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_38.root"), oFolder_+TString("out-dy_from50_38.root"),keys_);
//     break;
//   case 39:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_39.root"), oFolder_+TString("out-dy_from50_39.root"),keys_);
//     break;

//   case 0:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_0.root"), oFolder_+TString("out-dy_from50_0.root"),keys_);
//     break;
//   case 4:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_4.root"), oFolder_+TString("out-dy_from50_4.root"),keys_);
//     break;
//   case 5:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_5.root"), oFolder_+TString("out-dy_from50_5.root"),keys_);
//     break;
//   case 6:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_6.root"), oFolder_+TString("out-dy_from50_6.root"),keys_);
//     break;
//   case 7:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_7.root"), oFolder_+TString("out-dy_from50_7.root"),keys_);
//     break;
//   case 8:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_8.root"), oFolder_+TString("out-dy_from50_8.root"),keys_);
//     break;
//   case 9:
//     process(false, url_, iFolder_ + TString("mc/dy_from50_9.root"), oFolder_+TString("out-dy_from50_9.root"),keys_);
//     break;
//   }

//   //  process(false, url_, iFolder_ + TString("mc/dy_from50.root"), oFolder_+TString("out-dy_from50.root"),keys_); 

// }

// void SampleProcessor::process_zjets_10to50(int i){
  
//   // MADGRAPH Z+JETS
//   url_ = DY_10TO50_URL;

//   switch(i){
//   case 10:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_10.root"), oFolder_+TString("out-dy_10to50_10.root"),keys_);
//     break;
//   case 11:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_11.root"), oFolder_+TString("out-dy_10to50_11.root"),keys_);
//     break;
//   case 12:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_12.root"), oFolder_+TString("out-dy_10to50_12.root"),keys_);
//     break;
//   case 13:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_13.root"), oFolder_+TString("out-dy_10to50_13.root"),keys_);
//     break;
//   case 14:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_14.root"), oFolder_+TString("out-dy_10to50_14.root"),keys_);
//     break;
//   case 15:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_15.root"), oFolder_+TString("out-dy_10to50_15.root"),keys_);
//     break;
//   case 16:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_16.root"), oFolder_+TString("out-dy_10to50_16.root"),keys_);
//     break;
//   case 17:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_17.root"), oFolder_+TString("out-dy_10to50_17.root"),keys_);
//     break;
//   case 18:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_18.root"), oFolder_+TString("out-dy_10to50_18.root"),keys_);
//     break;
//   case 19:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_19.root"), oFolder_+TString("out-dy_10to50_19.root"),keys_);
//     break;
//   case 20:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_20.root"), oFolder_+TString("out-dy_10to50_20.root"),keys_);
//     break;
//   case 21:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_21.root"), oFolder_+TString("out-dy_10to50_21.root"),keys_);
//     break;
//   case 22:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_22.root"), oFolder_+TString("out-dy_10to50_22.root"),keys_);
//     break;
//   case 23:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_23.root"), oFolder_+TString("out-dy_10to50_23.root"),keys_);
//     break;
//   case 24:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_24.root"), oFolder_+TString("out-dy_10to50_24.root"),keys_);
//     break;
//   case 25:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_25.root"), oFolder_+TString("out-dy_10to50_25.root"),keys_);
//     break;
//   case 26:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_26.root"), oFolder_+TString("out-dy_10to50_26.root"),keys_);
//     break;
//   case 27:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_27.root"), oFolder_+TString("out-dy_10to50_27.root"),keys_);
//     break;
//   case 28:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_28.root"), oFolder_+TString("out-dy_10to50_28.root"),keys_);
//     break;
//   case 29:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_29.root"), oFolder_+TString("out-dy_10to50_29.root"),keys_);
//     break;
//   case 3:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_3.root"), oFolder_+TString("out-dy_10to50_3.root"),keys_);
//     break;
//   case 4:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_4.root"), oFolder_+TString("out-dy_10to50_4.root"),keys_);
//     break;
//   case 5:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_5.root"), oFolder_+TString("out-dy_10to50_5.root"),keys_);
//     break;
//   case 6:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_6.root"), oFolder_+TString("out-dy_10to50_6.root"),keys_);
//     break;
//   case 7:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_7.root"), oFolder_+TString("out-dy_10to50_7.root"),keys_);
//     break;
//   case 8:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_8.root"), oFolder_+TString("out-dy_10to50_8.root"),keys_);
//     break;
//   case 9:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_9.root"), oFolder_+TString("out-dy_10to50_9.root"),keys_);
//     break;
//   case 0:
//     process(false, url_, iFolder_ + TString("mc/dy_10_50_0.root"), oFolder_+TString("out-dy_10to50_0.root"),keys_);
//     break;
//   }

// }


void SampleProcessor::process_hh_higgs(){

  url_ = HH80_URL;  process(false, url_, iFolder_ + TString("hh-pythia-m80.root"),  oFolder_+TString("out-hh-pythia-m80.root"),keys_);  
  url_ = HH100_URL; process(false, url_, iFolder_ + TString("hh-pythia-m100.root"), oFolder_+TString("out-hh-pythia-m100.root"),keys_); 
  url_ = HH120_URL; process(false, url_, iFolder_ + TString("hh-pythia-m120.root"), oFolder_+TString("out-hh-pythia-m120.root"),keys_); 
  url_ = HH140_URL; process(false, url_, iFolder_ + TString("hh-pythia-m140.root"), oFolder_+TString("out-hh-pythia-m140.root"),keys_); 
  url_ = HH150_URL; process(false, url_, iFolder_ + TString("hh-pythia-m150.root"), oFolder_+TString("out-hh-pythia-m150.root"),keys_); 
  //  url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m155.root"), oFolder_+TString("out-hh-pythia-m155.root"),keys_); 
  url_ = HH160_URL; process(false, url_, iFolder_ + TString("hh-pythia-m160.root"), oFolder_+TString("out-hh-pythia-m160.root"),keys_); 


}

void SampleProcessor::process_hh_higgs_bychannel(){

//  url_ = HH80_URL;  process(false, url_, iFolder_ + TString("hh-pythia-m80.root"),  oFolder_+TString("out-hh-pythia-m80_etau.root"), keys_, ETAU_);  
//  url_ = HH100_URL; process(false, url_, iFolder_ + TString("hh-pythia-m100.root"), oFolder_+TString("out-hh-pythia-m100_etau.root"),keys_, ETAU_); 
//  url_ = HH120_URL; process(false, url_, iFolder_ + TString("hh-pythia-m120.root"), oFolder_+TString("out-hh-pythia-m120_etau.root"),keys_, ETAU_); 
//  url_ = HH140_URL; process(false, url_, iFolder_ + TString("hh-pythia-m140.root"), oFolder_+TString("out-hh-pythia-m140_etau.root"),keys_, ETAU_); 
//  url_ = HH150_URL; process(false, url_, iFolder_ + TString("hh-pythia-m150.root"), oFolder_+TString("out-hh-pythia-m150_etau.root"),keys_, ETAU_); 
//  url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m155.root"), oFolder_+TString("out-hh-pythia-m155_etau.root"),keys_, ETAU_); 
//  url_ = HH160_URL; process(false, url_, iFolder_ + TString("hh-pythia-m160.root"), oFolder_+TString("out-hh-pythia-m160_etau.root"),keys_, ETAU_); 

  
  url_ = HH80_URL;  process(false, url_, iFolder_ + TString("hh-pythia-m80.root"),  oFolder_+TString("out-hh-pythia-m80_mutau.root"), keys_, MUTAU_);  
  url_ = HH100_URL; process(false, url_, iFolder_ + TString("hh-pythia-m100.root"), oFolder_+TString("out-hh-pythia-m100_mutau.root"),keys_, MUTAU_); 
  url_ = HH120_URL; process(false, url_, iFolder_ + TString("hh-pythia-m120.root"), oFolder_+TString("out-hh-pythia-m120_mutau.root"),keys_, MUTAU_); 
  url_ = HH140_URL; process(false, url_, iFolder_ + TString("hh-pythia-m140.root"), oFolder_+TString("out-hh-pythia-m140_mutau.root"),keys_, MUTAU_); 
  url_ = HH150_URL; process(false, url_, iFolder_ + TString("hh-pythia-m150.root"), oFolder_+TString("out-hh-pythia-m150_mutau.root"),keys_, MUTAU_); 
  // url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m155.root"), oFolder_+TString("out-hh-pythia-m155_mutau.root"),keys_, MUTAU_); 
  url_ = HH160_URL; process(false, url_, iFolder_ + TString("hh-pythia-m160.root"), oFolder_+TString("out-hh-pythia-m160_mutau.root"),keys_, MUTAU_); 

}


void SampleProcessor::process_wh_higgs(){

  url_ = WH80_URL;  process(false, url_, iFolder_ + TString("wh-pythia-m80.root"), oFolder_+TString("out-wh-pythia-m80.root"),keys_  ); 
  // For 90 must add url keys. Still, useless w/out hh, which does not exist.
  //url_ = WH90_URL;  process(false, url_, iFolder_ + TString("wh-pythia-m90.root"), oFolder_+TString("out-wh-pythia-m90.root"),keys_  ); 
  url_ = WH100_URL; process(false, url_, iFolder_ + TString("wh-pythia-m100.root"), oFolder_+TString("out-wh-pythia-m100.root"),keys_); 
  url_ = WH120_URL; process(false, url_, iFolder_ + TString("wh-pythia-m120.root"), oFolder_+TString("out-wh-pythia-m120.root"),keys_); 
  url_ = WH140_URL; process(false, url_, iFolder_ + TString("wh-pythia-m140.root"), oFolder_+TString("out-wh-pythia-m140.root"),keys_); 
  url_ = WH150_URL; process(false, url_, iFolder_ + TString("wh-pythia-m150.root"), oFolder_+TString("out-wh-pythia-m150.root"),keys_); 
  url_ = WH155_URL; process(false, url_, iFolder_ + TString("wh-pythia-m155.root"), oFolder_+TString("out-wh-pythia-m155.root"),keys_); 
  url_ = WH160_URL; process(false, url_, iFolder_ + TString("wh-pythia-m160.root"), oFolder_+TString("out-wh-pythia-m160.root"),keys_); 


}


void SampleProcessor::process_wh_higgs_bychannel(){

//  url_ = WH80_URL;  process(false, url_, iFolder_ + TString("wh-pythia-m80.root"),  oFolder_+TString("out-wh-pythia-m80_etau.root"),keys_,  ETAU_); 
//  url_ = WH100_URL; process(false, url_, iFolder_ + TString("wh-pythia-m100.root"), oFolder_+TString("out-wh-pythia-m100_etau.root"),keys_, ETAU_); 
//  url_ = WH120_URL; process(false, url_, iFolder_ + TString("wh-pythia-m120.root"), oFolder_+TString("out-wh-pythia-m120_etau.root"),keys_, ETAU_); 
//  url_ = WH140_URL; process(false, url_, iFolder_ + TString("wh-pythia-m140.root"), oFolder_+TString("out-wh-pythia-m140_etau.root"),keys_, ETAU_); 
//
//  url_ = WH150_URL; process(false, url_, iFolder_ + TString("wh-pythia-m150.root"), oFolder_+TString("out-wh-pythia-m150_etau.root"),keys_, ETAU_); 
//  url_ = WH155_URL; process(false, url_, iFolder_ + TString("wh-pythia-m155.root"), oFolder_+TString("out-wh-pythia-m155_etau.root"),keys_, ETAU_); 
//  url_ = WH160_URL; process(false, url_, iFolder_ + TString("wh-pythia-m160.root"), oFolder_+TString("out-wh-pythia-m160_etau.root"),keys_, ETAU_); 

  url_ = WH80_URL;  process(false, url_, iFolder_ + TString("wh-pythia-m80.root"),  oFolder_+TString("out-wh-pythia-m80_mutau.root"),keys_,  MUTAU_); 
  url_ = WH100_URL; process(false, url_, iFolder_ + TString("wh-pythia-m100.root"), oFolder_+TString("out-wh-pythia-m100_mutau.root"),keys_, MUTAU_); 
  url_ = WH120_URL; process(false, url_, iFolder_ + TString("wh-pythia-m120.root"), oFolder_+TString("out-wh-pythia-m120_mutau.root"),keys_, MUTAU_); 
  url_ = WH140_URL; process(false, url_, iFolder_ + TString("wh-pythia-m140.root"), oFolder_+TString("out-wh-pythia-m140_mutau.root"),keys_, MUTAU_); 
  url_ = WH150_URL; process(false, url_, iFolder_ + TString("wh-pythia-m150.root"), oFolder_+TString("out-wh-pythia-m150_mutau.root"),keys_, MUTAU_); 
  url_ = WH155_URL; process(false, url_, iFolder_ + TString("wh-pythia-m155.root"), oFolder_+TString("out-wh-pythia-m155_mutau.root"),keys_, MUTAU_); 
  url_ = WH160_URL; process(false, url_, iFolder_ + TString("wh-pythia-m160.root"), oFolder_+TString("out-wh-pythia-m160_mutau.root"),keys_, MUTAU_); 
  

}


void SampleProcessor::process_tbh_higgs_bychannel(){
  url_ = TBH180_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m180.root"), oFolder_+TString("out-tbh-pythia-m180_mutau.root"),keys_, MUTAU_); 
  url_ = TBH190_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m190.root"), oFolder_+TString("out-tbh-pythia-m190_mutau.root"),keys_, MUTAU_); 
  url_ = TBH200_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m200.root"), oFolder_+TString("out-tbh-pythia-m200_mutau.root"),keys_, MUTAU_); 
  url_ = TBH220_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m220.root"), oFolder_+TString("out-tbh-pythia-m220_mutau.root"),keys_, MUTAU_); 
  url_ = TBH250_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m250.root"), oFolder_+TString("out-tbh-pythia-m250_mutau.root"),keys_, MUTAU_); 
  url_ = TBH300_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m300.root"), oFolder_+TString("out-tbh-pythia-m300_mutau.root"),keys_, MUTAU_); 
  
}


void SampleProcessor::process_tbh_higgs(){
  url_ = TBH180_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m180.root"), oFolder_+TString("out-tbh-pythia-m180.root"),keys_); 
  url_ = TBH190_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m190.root"), oFolder_+TString("out-tbh-pythia-m190.root"),keys_); 
  url_ = TBH200_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m200.root"), oFolder_+TString("out-tbh-pythia-m200.root"),keys_); 
  url_ = TBH220_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m220.root"), oFolder_+TString("out-tbh-pythia-m220.root"),keys_); 
  url_ = TBH250_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m250.root"), oFolder_+TString("out-tbh-pythia-m250.root"),keys_); 
  url_ = TBH300_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m300.root"), oFolder_+TString("out-tbh-pythia-m300.root"),keys_); 


}


void SampleProcessor::process_qcd(int sample, int i){

  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  switch(sample){
  case 1:
    url_= QCD_EM30TO80_URL;     process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/qcd_EM_Pt30to80_"+idx+".root"),     oFolder_ + TString("out-qcd_EM_Pt30to80_"+idx+".root"),keys_); 
    break;
  case 2:
    url_= QCD_EM80TO170_URL;    process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/qcd_EM_Pt80to170_"+idx+".root"),    oFolder_ + TString("out-qcd_EM_Pt80to170_"+idx+".root"),keys_);    
    break;
  case 3:
    url_= QCD_BCTOE30TO80_URL;  process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/qcd_30to80_BCtoE_"+idx+".root"),    oFolder_ + TString("out-qcd_30to80_BCtoE_"+idx+".root"),keys_);  
    break;
  case 4:
    url_= QCD_BCTOE80TO170_URL; process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/qcd_80to170_BCtoE_"+idx+".root"),   oFolder_ + TString("out-qcd_80to170_BCtoE_"+idx+".root"),keys_);  
    break;
  case 5:
    url_= QCD_PHOTON30TO50_URL;   process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/PhotonJets_30to50_"+idx+".root"),   oFolder_ + TString("out-PhotonJets_30to50_"+idx+".root"),keys_);  
    break;
  case 6:
    url_= QCD_PHOTON50TO80_URL;   process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/PhotonJets_50to80_"+idx+".root"),   oFolder_ + TString("out-PhotonJets_50to80_"+idx+".root"),keys_);  
    break;
  case 7:
    url_= QCD_PHOTON80TO120_URL;  process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/PhotonJets_80to120_"+idx+".root"),  oFolder_ + TString("out-PhotonJets_80to120_"+idx+".root"),keys_); 
    break;
  case 8:
    url_= QCD_PHOTON120TO170_URL; process(false,url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/PhotonJets_120to170_"+idx+".root"), oFolder_ + TString("out-PhotonJets_120to170_"+idx+".root"),keys_); 
    break;
  case 9:
    // mu + jets specific samples
    url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf_"+idx+".root"), oFolder_ + TString("out-qcdmu15_20toinf_"+idx+".root"),keys_);   
    //fn    url_= QCD_MU_URL; process(false, url_, TString("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/qcdmu15_20toinf_"+idx+".root"),
    //fn			      oFolder_ + TString("out-qcdmu15_20toinf_"+idx+".root"),keys_);   
    break;
  default:
    cout << "No Sample" << endl;
    break;
  }
  
}


void SampleProcessor::process_diboson(int sample, int i){

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  
  switch(sample){
  case 0:
    url_= WW_URL; process(false, url_,iFolder_+TString("WW_"+idx+".root"), oFolder_+TString("out-ww_"+idx+".root"),keys_); 
    break;
  case 1:
    url_= WZ_URL; process(false, url_,iFolder_+TString("WZ_"+idx+".root"), oFolder_+TString("out-wz_"+idx+".root"),keys_);  
    break;
  case 2:
    url_= ZZ_URL; process(false, url_,iFolder_+TString("ZZ_"+idx+".root"), oFolder_+TString("out-zz_"+idx+".root"),keys_);
    break;
  default:
    cout << "No Sample" << endl;
    break;
  }
  
}


void SampleProcessor::process_qcd_EM_Pt20to30(int i){
   
  url_= QCD_EM20TO30_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt20to30.root"),     oFolder_ + TString("out-qcd_EM_Pt20to30.root"),keys_); 
 
  switch(i){
  case 10:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_10.root"),oFolder_+TString("out-qcd_EM_Pt20to30_10.root"),keys_);
    break;
  case 11:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_11.root"),oFolder_+TString("out-qcd_EM_Pt20to30_11.root"),keys_);
    break;
  case 12:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_12.root"),oFolder_+TString("out-qcd_EM_Pt20to30_12.root"),keys_);
    break;
  case 13:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_13.root"),oFolder_+TString("out-qcd_EM_Pt20to30_13.root"),keys_);
    break;
  case 14:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_14.root"),oFolder_+TString("out-qcd_EM_Pt20to30_14.root"),keys_);
    break;
  case 15:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_15.root"),oFolder_+TString("out-qcd_EM_Pt20to30_15.root"),keys_);
    break;
  case 16:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_16.root"),oFolder_+TString("out-qcd_EM_Pt20to30_16.root"),keys_);
    break;
  case 17:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_17.root"),oFolder_+TString("out-qcd_EM_Pt20to30_17.root"),keys_);
    break;
  case 18:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_18.root"),oFolder_+TString("out-qcd_EM_Pt20to30_18.root"),keys_);
    break;
  case 19:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_19.root"),oFolder_+TString("out-qcd_EM_Pt20to30_19.root"),keys_);
    break;
    
  case 20:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_20.root"),oFolder_+TString("out-qcd_EM_Pt20to30_20.root"),keys_);
    break;
  case 21:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_21.root"),oFolder_+TString("out-qcd_EM_Pt20to30_21.root"),keys_);
    break;
  case 22:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_22.root"),oFolder_+TString("out-qcd_EM_Pt20to30_22.root"),keys_);
    break;
  case 23:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_23.root"),oFolder_+TString("out-qcd_EM_Pt20to30_23.root"),keys_);
    break;
  case 24:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_24.root"),oFolder_+TString("out-qcd_EM_Pt20to30_24.root"),keys_);
    break;
  case 25:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_25.root"),oFolder_+TString("out-qcd_EM_Pt20to30_25.root"),keys_);
    break;
  case 26:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_26.root"),oFolder_+TString("out-qcd_EM_Pt20to30_26.root"),keys_);
    break;
  case 27:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_27.root"),oFolder_+TString("out-qcd_EM_Pt20to30_27.root"),keys_);
    break;
  case 28:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_28.root"),oFolder_+TString("out-qcd_EM_Pt20to30_28.root"),keys_);
    break;
  case 29:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_29.root"),oFolder_+TString("out-qcd_EM_Pt20to30_29.root"),keys_);
    break;
    
    
  case 0:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_0.root"),oFolder_+TString("out-qcd_EM_Pt20to30_0.root"),keys_);
    break;
  case 3:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_3.root"),oFolder_+TString("out-qcd_EM_Pt20to30_3.root"),keys_);
    break;
  case 4:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_4.root"),oFolder_+TString("out-qcd_EM_Pt20to30_4.root"),keys_);
    break;
  case 5:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_5.root"),oFolder_+TString("out-qcd_EM_Pt20to30_5.root"),keys_);
    break;
  case 6:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_6.root"),oFolder_+TString("out-qcd_EM_Pt20to30_6.root"),keys_);
    break;
  case 7:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_7.root"),oFolder_+TString("out-qcd_EM_Pt20to30_7.root"),keys_);
    break;
  case 8:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_8.root"),oFolder_+TString("out-qcd_EM_Pt20to30_8.root"),keys_);
    break;
  case 9:
    process(false,url_,iFolder_+TString("mc/qcd_EM_Pt20to30_9.root"),oFolder_+TString("out-qcd_EM_Pt20to30_9.root"),keys_);
    break;
  }
  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_EM_Pt30to80(int i){
   
  url_= QCD_EM30TO80_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt30to80.root"),     oFolder_ + TString("out-qcd_EM_Pt30to80.root"),keys_); 
 
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_"+idx+".root"),oFolder_+TString("out-qcd_EM_Pt30to80_"+idx+".root"),keys_);

//   switch(i){
//   case 10:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_10.root"),oFolder_+TString("out-qcd_EM_Pt30to80_10.root"),keys_);
//     break;
//   case 11:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_11.root"),oFolder_+TString("out-qcd_EM_Pt30to80_11.root"),keys_);
//     break;
//   case 12:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_12.root"),oFolder_+TString("out-qcd_EM_Pt30to80_12.root"),keys_);
//     break;
//   case 13:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_13.root"),oFolder_+TString("out-qcd_EM_Pt30to80_13.root"),keys_);
//     break;
//   case 14:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_14.root"),oFolder_+TString("out-qcd_EM_Pt30to80_14.root"),keys_);
//     break;
//   case 15:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_15.root"),oFolder_+TString("out-qcd_EM_Pt30to80_15.root"),keys_);
//     break;
//   case 16:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_16.root"),oFolder_+TString("out-qcd_EM_Pt30to80_16.root"),keys_);
//     break;
//   case 17:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_17.root"),oFolder_+TString("out-qcd_EM_Pt30to80_17.root"),keys_);
//     break;
//   case 18:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_18.root"),oFolder_+TString("out-qcd_EM_Pt30to80_18.root"),keys_);
//     break;
//   case 19:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_19.root"),oFolder_+TString("out-qcd_EM_Pt30to80_19.root"),keys_);
//     break;
    
//   case 20:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_20.root"),oFolder_+TString("out-qcd_EM_Pt30to80_20.root"),keys_);
//     break;
//   case 21:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_21.root"),oFolder_+TString("out-qcd_EM_Pt30to80_21.root"),keys_);
//     break;
//   case 22:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_22.root"),oFolder_+TString("out-qcd_EM_Pt30to80_22.root"),keys_);
//     break;
//   case 23:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_23.root"),oFolder_+TString("out-qcd_EM_Pt30to80_23.root"),keys_);
//     break;
//   case 24:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_24.root"),oFolder_+TString("out-qcd_EM_Pt30to80_24.root"),keys_);
//     break;
//   case 25:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_25.root"),oFolder_+TString("out-qcd_EM_Pt30to80_25.root"),keys_);
//     break;
//   case 26:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_26.root"),oFolder_+TString("out-qcd_EM_Pt30to80_26.root"),keys_);
//     break;
//   case 27:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_27.root"),oFolder_+TString("out-qcd_EM_Pt30to80_27.root"),keys_);
//     break;
//   case 28:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_28.root"),oFolder_+TString("out-qcd_EM_Pt30to80_28.root"),keys_);
//     break;
//   case 29:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_29.root"),oFolder_+TString("out-qcd_EM_Pt30to80_29.root"),keys_);
//     break;
    
    
//   case 0:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_0.root"),oFolder_+TString("out-qcd_EM_Pt30to80_0.root"),keys_);
//     break;
//   case 3:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_3.root"),oFolder_+TString("out-qcd_EM_Pt30to80_3.root"),keys_);
//     break;
//   case 4:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_4.root"),oFolder_+TString("out-qcd_EM_Pt30to80_4.root"),keys_);
//     break;
//   case 5:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_5.root"),oFolder_+TString("out-qcd_EM_Pt30to80_5.root"),keys_);
//     break;
//   case 6:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_6.root"),oFolder_+TString("out-qcd_EM_Pt30to80_6.root"),keys_);
//     break;
//   case 7:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_7.root"),oFolder_+TString("out-qcd_EM_Pt30to80_7.root"),keys_);
//     break;
//   case 8:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_8.root"),oFolder_+TString("out-qcd_EM_Pt30to80_8.root"),keys_);
//     break;
//   case 9:
//     process(false,url_,iFolder_+TString("qcd_EM_Pt30to80_9.root"),oFolder_+TString("out-qcd_EM_Pt30to80_9.root"),keys_);
//     break;
//   }

  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_EM_Pt80to170(int i){
   
  url_= QCD_EM80TO170_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt80to170.root"),     oFolder_ + TString("out-qcd_EM_Pt80to170.root"),keys_); 
 
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_EM_Pt80to170_"+idx+".root"),oFolder_+TString("out-qcd_EM_Pt80to170_"+idx+".root"),keys_);

//   switch(i){
//   case 10:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_10.root"),oFolder_+TString("out-qcd_EM_Pt80to170_10.root"),keys_);
//     break;
//   case 11:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_11.root"),oFolder_+TString("out-qcd_EM_Pt80to170_11.root"),keys_);
//     break;
//   case 12:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_12.root"),oFolder_+TString("out-qcd_EM_Pt80to170_12.root"),keys_);
//     break;
//   case 13:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_13.root"),oFolder_+TString("out-qcd_EM_Pt80to170_13.root"),keys_);
//     break;
//   case 14:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_14.root"),oFolder_+TString("out-qcd_EM_Pt80to170_14.root"),keys_);
//     break;
//   case 15:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_15.root"),oFolder_+TString("out-qcd_EM_Pt80to170_15.root"),keys_);
//     break;
//   case 16:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_16.root"),oFolder_+TString("out-qcd_EM_Pt80to170_16.root"),keys_);
//     break;
//   case 17:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_17.root"),oFolder_+TString("out-qcd_EM_Pt80to170_17.root"),keys_);
//     break;
//   case 18:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_18.root"),oFolder_+TString("out-qcd_EM_Pt80to170_18.root"),keys_);
//     break;
//   case 19:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_19.root"),oFolder_+TString("out-qcd_EM_Pt80to170_19.root"),keys_);
//     break;
    
//   case 20:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_20.root"),oFolder_+TString("out-qcd_EM_Pt80to170_20.root"),keys_);
//     break;
//   case 21:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_21.root"),oFolder_+TString("out-qcd_EM_Pt80to170_21.root"),keys_);
//     break;
//   case 22:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_22.root"),oFolder_+TString("out-qcd_EM_Pt80to170_22.root"),keys_);
//     break;
//   case 23:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_23.root"),oFolder_+TString("out-qcd_EM_Pt80to170_23.root"),keys_);
//     break;
//   case 24:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_24.root"),oFolder_+TString("out-qcd_EM_Pt80to170_24.root"),keys_);
//     break;
//   case 25:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_25.root"),oFolder_+TString("out-qcd_EM_Pt80to170_25.root"),keys_);
//     break;
//   case 26:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_26.root"),oFolder_+TString("out-qcd_EM_Pt80to170_26.root"),keys_);
//     break;
//   case 27:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_27.root"),oFolder_+TString("out-qcd_EM_Pt80to170_27.root"),keys_);
//     break;
//   case 28:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_28.root"),oFolder_+TString("out-qcd_EM_Pt80to170_28.root"),keys_);
//     break;
//   case 29:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_29.root"),oFolder_+TString("out-qcd_EM_Pt80to170_29.root"),keys_);
//     break;
    
    
//   case 0:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_0.root"),oFolder_+TString("out-qcd_EM_Pt80to170_0.root"),keys_);
//     break;
//   case 3:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_3.root"),oFolder_+TString("out-qcd_EM_Pt80to170_3.root"),keys_);
//     break;
//   case 4:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_4.root"),oFolder_+TString("out-qcd_EM_Pt80to170_4.root"),keys_);
//     break;
//   case 5:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_5.root"),oFolder_+TString("out-qcd_EM_Pt80to170_5.root"),keys_);
//     break;
//   case 6:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_6.root"),oFolder_+TString("out-qcd_EM_Pt80to170_6.root"),keys_);
//     break;
//   case 7:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_7.root"),oFolder_+TString("out-qcd_EM_Pt80to170_7.root"),keys_);
//     break;
//   case 8:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_8.root"),oFolder_+TString("out-qcd_EM_Pt80to170_8.root"),keys_);
//     break;
//   case 9:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt80to170_9.root"),oFolder_+TString("out-qcd_EM_Pt80to170_9.root"),keys_);
//     break;
//   }

  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_EM_Pt170to250(int i){
   
  url_= QCD_EM170TO250_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt170to250.root"),     oFolder_ + TString("out-qcd_EM_Pt170to250.root"),keys_); 
 
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_EM_Pt170to250_"+idx+".root"),oFolder_+TString("out-qcd_EM_Pt170to250_"+idx+".root"),keys_);

//   switch(i){
//   case 10:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_10.root"),oFolder_+TString("out-qcd_EM_Pt170to250_10.root"),keys_);
//     break;
//   case 11:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_11.root"),oFolder_+TString("out-qcd_EM_Pt170to250_11.root"),keys_);
//     break;
//   case 12:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_12.root"),oFolder_+TString("out-qcd_EM_Pt170to250_12.root"),keys_);
//     break;
//   case 13:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_13.root"),oFolder_+TString("out-qcd_EM_Pt170to250_13.root"),keys_);
//     break;
//   case 14:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_14.root"),oFolder_+TString("out-qcd_EM_Pt170to250_14.root"),keys_);
//     break;
//   case 15:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_15.root"),oFolder_+TString("out-qcd_EM_Pt170to250_15.root"),keys_);
//     break;
//   case 16:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_16.root"),oFolder_+TString("out-qcd_EM_Pt170to250_16.root"),keys_);
//     break;
//   case 17:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_17.root"),oFolder_+TString("out-qcd_EM_Pt170to250_17.root"),keys_);
//     break;
//   case 18:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_18.root"),oFolder_+TString("out-qcd_EM_Pt170to250_18.root"),keys_);
//     break;
//   case 19:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_19.root"),oFolder_+TString("out-qcd_EM_Pt170to250_19.root"),keys_);
//     break;
    
//   case 20:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_20.root"),oFolder_+TString("out-qcd_EM_Pt170to250_20.root"),keys_);
//     break;
//   case 21:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_21.root"),oFolder_+TString("out-qcd_EM_Pt170to250_21.root"),keys_);
//     break;
//   case 22:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_22.root"),oFolder_+TString("out-qcd_EM_Pt170to250_22.root"),keys_);
//     break;
//   case 23:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_23.root"),oFolder_+TString("out-qcd_EM_Pt170to250_23.root"),keys_);
//     break;
//   case 24:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_24.root"),oFolder_+TString("out-qcd_EM_Pt170to250_24.root"),keys_);
//     break;
//   case 25:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_25.root"),oFolder_+TString("out-qcd_EM_Pt170to250_25.root"),keys_);
//     break;
//   case 26:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_26.root"),oFolder_+TString("out-qcd_EM_Pt170to250_26.root"),keys_);
//     break;
//   case 27:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_27.root"),oFolder_+TString("out-qcd_EM_Pt170to250_27.root"),keys_);
//     break;
//   case 28:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_28.root"),oFolder_+TString("out-qcd_EM_Pt170to250_28.root"),keys_);
//     break;
//   case 29:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_29.root"),oFolder_+TString("out-qcd_EM_Pt170to250_29.root"),keys_);
//     break;
    
    
//   case 0:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_0.root"),oFolder_+TString("out-qcd_EM_Pt170to250_0.root"),keys_);
//     break;
//   case 3:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_3.root"),oFolder_+TString("out-qcd_EM_Pt170to250_3.root"),keys_);
//     break;
//   case 4:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_4.root"),oFolder_+TString("out-qcd_EM_Pt170to250_4.root"),keys_);
//     break;
//   case 5:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_5.root"),oFolder_+TString("out-qcd_EM_Pt170to250_5.root"),keys_);
//     break;
//   case 6:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_6.root"),oFolder_+TString("out-qcd_EM_Pt170to250_6.root"),keys_);
//     break;
//   case 7:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_7.root"),oFolder_+TString("out-qcd_EM_Pt170to250_7.root"),keys_);
//     break;
//   case 8:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_8.root"),oFolder_+TString("out-qcd_EM_Pt170to250_8.root"),keys_);
//     break;
//   case 9:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt170to250_9.root"),oFolder_+TString("out-qcd_EM_Pt170to250_9.root"),keys_);
//     break;
//   }

  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_EM_Pt250to350(int i){
   
  url_= QCD_EM250TO350_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt250to350.root"),     oFolder_ + TString("out-qcd_EM_Pt250to350.root"),keys_); 
 
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_EM_Pt250to350_"+idx+".root"),oFolder_+TString("out-qcd_EM_Pt250to350_"+idx+".root"),keys_);

//   switch(i){
//   case 10:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_10.root"),oFolder_+TString("out-qcd_EM_Pt250to350_10.root"),keys_);
//     break;
//   case 11:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_11.root"),oFolder_+TString("out-qcd_EM_Pt250to350_11.root"),keys_);
//     break;
//   case 12:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_12.root"),oFolder_+TString("out-qcd_EM_Pt250to350_12.root"),keys_);
//     break;
//   case 13:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_13.root"),oFolder_+TString("out-qcd_EM_Pt250to350_13.root"),keys_);
//     break;
//   case 14:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_14.root"),oFolder_+TString("out-qcd_EM_Pt250to350_14.root"),keys_);
//     break;
//   case 15:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_15.root"),oFolder_+TString("out-qcd_EM_Pt250to350_15.root"),keys_);
//     break;
//   case 16:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_16.root"),oFolder_+TString("out-qcd_EM_Pt250to350_16.root"),keys_);
//     break;
//   case 17:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_17.root"),oFolder_+TString("out-qcd_EM_Pt250to350_17.root"),keys_);
//     break;
//   case 18:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_18.root"),oFolder_+TString("out-qcd_EM_Pt250to350_18.root"),keys_);
//     break;
//   case 19:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_19.root"),oFolder_+TString("out-qcd_EM_Pt250to350_19.root"),keys_);
//     break;
    
//   case 20:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_20.root"),oFolder_+TString("out-qcd_EM_Pt250to350_20.root"),keys_);
//     break;
//   case 21:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_21.root"),oFolder_+TString("out-qcd_EM_Pt250to350_21.root"),keys_);
//     break;
//   case 22:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_22.root"),oFolder_+TString("out-qcd_EM_Pt250to350_22.root"),keys_);
//     break;
//   case 23:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_23.root"),oFolder_+TString("out-qcd_EM_Pt250to350_23.root"),keys_);
//     break;
//   case 24:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_24.root"),oFolder_+TString("out-qcd_EM_Pt250to350_24.root"),keys_);
//     break;
//   case 25:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_25.root"),oFolder_+TString("out-qcd_EM_Pt250to350_25.root"),keys_);
//     break;
//   case 26:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_26.root"),oFolder_+TString("out-qcd_EM_Pt250to350_26.root"),keys_);
//     break;
//   case 27:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_27.root"),oFolder_+TString("out-qcd_EM_Pt250to350_27.root"),keys_);
//     break;
//   case 28:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_28.root"),oFolder_+TString("out-qcd_EM_Pt250to350_28.root"),keys_);
//     break;
//   case 29:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_29.root"),oFolder_+TString("out-qcd_EM_Pt250to350_29.root"),keys_);
//     break;
    
    
//   case 0:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_0.root"),oFolder_+TString("out-qcd_EM_Pt250to350_0.root"),keys_);
//     break;
//   case 3:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_3.root"),oFolder_+TString("out-qcd_EM_Pt250to350_3.root"),keys_);
//     break;
//   case 4:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_4.root"),oFolder_+TString("out-qcd_EM_Pt250to350_4.root"),keys_);
//     break;
//   case 5:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_5.root"),oFolder_+TString("out-qcd_EM_Pt250to350_5.root"),keys_);
//     break;
//   case 6:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_6.root"),oFolder_+TString("out-qcd_EM_Pt250to350_6.root"),keys_);
//     break;
//   case 7:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_7.root"),oFolder_+TString("out-qcd_EM_Pt250to350_7.root"),keys_);
//     break;
//   case 8:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_8.root"),oFolder_+TString("out-qcd_EM_Pt250to350_8.root"),keys_);
//     break;
//   case 9:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt250to350_9.root"),oFolder_+TString("out-qcd_EM_Pt250to350_9.root"),keys_);
//     break;
//   }

  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_EM_Pt350toinf(int i){
   
  url_= QCD_EM350TOINF_URL;     //process(false,url_, iFolder_ + TString("qcd_EM_Pt350toinf.root"),     oFolder_ + TString("out-qcd_EM_Pt350toinf.root"),keys_); 
 
  if(i>99){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_EM_Pt350toinf_"+idx+".root"),oFolder_+TString("out-qcd_EM_Pt350toinf_"+idx+".root"),keys_);

//   switch(i){
//   case 10:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_10.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_10.root"),keys_);
//     break;
//   case 11:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_11.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_11.root"),keys_);
//     break;
//   case 12:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_12.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_12.root"),keys_);
//     break;
//   case 13:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_13.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_13.root"),keys_);
//     break;
//   case 14:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_14.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_14.root"),keys_);
//     break;
//   case 15:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_15.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_15.root"),keys_);
//     break;
//   case 16:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_16.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_16.root"),keys_);
//     break;
//   case 17:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_17.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_17.root"),keys_);
//     break;
//   case 18:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_18.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_18.root"),keys_);
//     break;
//   case 19:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_19.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_19.root"),keys_);
//     break;
    
//   case 20:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_20.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_20.root"),keys_);
//     break;
//   case 21:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_21.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_21.root"),keys_);
//     break;
//   case 22:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_22.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_22.root"),keys_);
//     break;
//   case 23:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_23.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_23.root"),keys_);
//     break;
//   case 24:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_24.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_24.root"),keys_);
//     break;
//   case 25:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_25.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_25.root"),keys_);
//     break;
//   case 26:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_26.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_26.root"),keys_);
//     break;
//   case 27:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_27.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_27.root"),keys_);
//     break;
//   case 28:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_28.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_28.root"),keys_);
//     break;
//   case 29:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_29.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_29.root"),keys_);
//     break;
    
    
//   case 0:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_0.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_0.root"),keys_);
//     break;
//   case 3:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_3.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_3.root"),keys_);
//     break;
//   case 4:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_4.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_4.root"),keys_);
//     break;
//   case 5:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_5.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_5.root"),keys_);
//     break;
//   case 6:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_6.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_6.root"),keys_);
//     break;
//   case 7:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_7.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_7.root"),keys_);
//     break;
//   case 8:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_8.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_8.root"),keys_);
//     break;
//   case 9:
//     process(false,url_,iFolder_+TString("mc/qcd_EM_Pt350toinf_9.root"),oFolder_+TString("out-qcd_EM_Pt350toinf_9.root"),keys_);
//     break;
//   }

  // mu + jets specific samples
  //fn  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   
 
}


void SampleProcessor::process_qcd_20to30_BCtoE(){
  url_= QCD_BCTOE20TO30_URL;     process(false,url_, iFolder_ + TString("mc/qcd_20to30_BCtoE.root"),     oFolder_ + TString("out-qcd_20to30_BCtoE.root"),keys_);  
}

void SampleProcessor::process_qcd_30to80_BCtoE(int i){
   
  url_= QCD_BCTOE30TO80_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_30to80_BCtoE_"+idx+".root"),oFolder_+TString("out-qcd_30to80_BCtoE_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_1.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_2.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_3.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_4.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_5.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_6.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_7.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_8.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/qcd_30to80_BCtoE_9.root"),     oFolder_ + TString("out-qcd_30to80_BCtoE_9.root"),keys_); 
//     break;

//   }

}

void SampleProcessor::process_qcd_80to170_BCtoE(int i){
   
  url_= QCD_BCTOE80TO170_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_80to170_BCtoE_"+idx+".root"),oFolder_+TString("out-qcd_80to170_BCtoE_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_1.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_2.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_3.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_4.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_5.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_6.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_7.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_8.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/qcd_80to170_BCtoE_9.root"),     oFolder_ + TString("out-qcd_80to170_BCtoE_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_qcd_170to250_BCtoE(int i){
   
  url_= QCD_BCTOE170TO250_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_170to250_BCtoE_"+idx+".root"),oFolder_+TString("out-qcd_170to250_BCtoE_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_1.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_2.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_3.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_4.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_5.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_6.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_7.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_8.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/qcd_170to250_BCtoE_9.root"),     oFolder_ + TString("out-qcd_170to250_BCtoE_9.root"),keys_); 
//     break;
//   }

}

void SampleProcessor::process_qcd_250to350_BCtoE(int i){
   
  url_= QCD_BCTOE250TO350_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_250to350_BCtoE_"+idx+".root"),oFolder_+TString("out-qcd_250to350_BCtoE_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_1.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_2.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_3.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_4.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_5.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_6.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_7.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_8.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/qcd_250to350_BCtoE_9.root"),     oFolder_ + TString("out-qcd_250to350_BCtoE_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_qcd_350toinf_BCtoE(int i){
   
  url_= QCD_BCTOE350TOINF_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("qcd_350toinf_BCtoE_"+idx+".root"),oFolder_+TString("out-qcd_350toinf_BCtoE_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_1.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_2.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_3.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_4.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_5.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_6.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_7.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_8.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/qcd_350toinf_BCtoE_9.root"),     oFolder_ + TString("out-qcd_350toinf_BCtoE_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_PhotonJets_15to30(int i){
   
  url_= QCD_PHOTON15TO30_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_15to30_"+idx+".root"),oFolder_+TString("out-PhotonJets_15to30_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_1.root"),     oFolder_ + TString("out-PhotonJets_15to30_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_2.root"),     oFolder_ + TString("out-PhotonJets_15to30_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_3.root"),     oFolder_ + TString("out-PhotonJets_15to30_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_4.root"),     oFolder_ + TString("out-PhotonJets_15to30_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_5.root"),     oFolder_ + TString("out-PhotonJets_15to30_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_6.root"),     oFolder_ + TString("out-PhotonJets_15to30_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_7.root"),     oFolder_ + TString("out-PhotonJets_15to30_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_8.root"),     oFolder_ + TString("out-PhotonJets_15to30_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_15to30_9.root"),     oFolder_ + TString("out-PhotonJets_15to30_9.root"),keys_); 
//     break;
//   }

}

void SampleProcessor::process_PhotonJets_30to50(int i){
   
  url_= QCD_PHOTON30TO50_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_30to50_"+idx+".root"),oFolder_+TString("out-PhotonJets_30to50_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_1.root"),     oFolder_ + TString("out-PhotonJets_30to50_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_2.root"),     oFolder_ + TString("out-PhotonJets_30to50_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_3.root"),     oFolder_ + TString("out-PhotonJets_30to50_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_4.root"),     oFolder_ + TString("out-PhotonJets_30to50_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_5.root"),     oFolder_ + TString("out-PhotonJets_30to50_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_6.root"),     oFolder_ + TString("out-PhotonJets_30to50_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_7.root"),     oFolder_ + TString("out-PhotonJets_30to50_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_8.root"),     oFolder_ + TString("out-PhotonJets_30to50_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_30to50_9.root"),     oFolder_ + TString("out-PhotonJets_30to50_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_PhotonJets_50to80(int i){
   
  url_= QCD_PHOTON50TO80_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_50to80_"+idx+".root"),oFolder_+TString("out-PhotonJets_50to80_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_1.root"),     oFolder_ + TString("out-PhotonJets_50to80_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_2.root"),     oFolder_ + TString("out-PhotonJets_50to80_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_3.root"),     oFolder_ + TString("out-PhotonJets_50to80_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_4.root"),     oFolder_ + TString("out-PhotonJets_50to80_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_5.root"),     oFolder_ + TString("out-PhotonJets_50to80_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_6.root"),     oFolder_ + TString("out-PhotonJets_50to80_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_7.root"),     oFolder_ + TString("out-PhotonJets_50to80_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_8.root"),     oFolder_ + TString("out-PhotonJets_50to80_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_50to80_9.root"),     oFolder_ + TString("out-PhotonJets_50to80_9.root"),keys_); 
//     break;
//   }

}



void SampleProcessor::process_PhotonJets_80to120(int i){
   
  url_= QCD_PHOTON80TO120_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_80to120_"+idx+".root"),oFolder_+TString("out-PhotonJets_80to120_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_1.root"),     oFolder_ + TString("out-PhotonJets_80to120_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_2.root"),     oFolder_ + TString("out-PhotonJets_80to120_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_3.root"),     oFolder_ + TString("out-PhotonJets_80to120_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_4.root"),     oFolder_ + TString("out-PhotonJets_80to120_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_5.root"),     oFolder_ + TString("out-PhotonJets_80to120_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_6.root"),     oFolder_ + TString("out-PhotonJets_80to120_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_7.root"),     oFolder_ + TString("out-PhotonJets_80to120_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_8.root"),     oFolder_ + TString("out-PhotonJets_80to120_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_80to120_9.root"),     oFolder_ + TString("out-PhotonJets_80to120_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_PhotonJets_120to170(int i){
   
  url_= QCD_PHOTON120TO170_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_120to170_"+idx+".root"),oFolder_+TString("out-PhotonJets_120to170_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_1.root"),     oFolder_ + TString("out-PhotonJets_120to170_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_2.root"),     oFolder_ + TString("out-PhotonJets_120to170_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_3.root"),     oFolder_ + TString("out-PhotonJets_120to170_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_4.root"),     oFolder_ + TString("out-PhotonJets_120to170_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_5.root"),     oFolder_ + TString("out-PhotonJets_120to170_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_6.root"),     oFolder_ + TString("out-PhotonJets_120to170_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_7.root"),     oFolder_ + TString("out-PhotonJets_120to170_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_8.root"),     oFolder_ + TString("out-PhotonJets_120to170_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_120to170_9.root"),     oFolder_ + TString("out-PhotonJets_120to170_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_PhotonJets_170to300(int i){
   
  url_= QCD_PHOTON170TO300_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_170to300_"+idx+".root"),oFolder_+TString("out-PhotonJets_170to300_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_1.root"),     oFolder_ + TString("out-PhotonJets_170to300_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_2.root"),     oFolder_ + TString("out-PhotonJets_170to300_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_3.root"),     oFolder_ + TString("out-PhotonJets_170to300_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_4.root"),     oFolder_ + TString("out-PhotonJets_170to300_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_5.root"),     oFolder_ + TString("out-PhotonJets_170to300_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_6.root"),     oFolder_ + TString("out-PhotonJets_170to300_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_7.root"),     oFolder_ + TString("out-PhotonJets_170to300_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_8.root"),     oFolder_ + TString("out-PhotonJets_170to300_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_170to300_9.root"),     oFolder_ + TString("out-PhotonJets_170to300_9.root"),keys_); 
//     break;
//   }

}



void SampleProcessor::process_PhotonJets_300to470(int i){
   
  url_= QCD_PHOTON300TO470_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(false,url_,iFolder_+TString("PhotonJets_300to470_"+idx+".root"),oFolder_+TString("out-PhotonJets_300to470_"+idx+".root"),keys_);

//   switch(j){
//   case 1:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_1.root"),     oFolder_ + TString("out-PhotonJets_300to470_1.root"),keys_); 
//     break;
//   case 2:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_2.root"),     oFolder_ + TString("out-PhotonJets_300to470_2.root"),keys_); 
//     break;
//   case 3:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_3.root"),     oFolder_ + TString("out-PhotonJets_300to470_3.root"),keys_); 
//     break;
//   case 4:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_4.root"),     oFolder_ + TString("out-PhotonJets_300to470_4.root"),keys_); 
//     break;
//   case 5:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_5.root"),     oFolder_ + TString("out-PhotonJets_300to470_5.root"),keys_); 
//     break;
//   case 6:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_6.root"),     oFolder_ + TString("out-PhotonJets_300to470_6.root"),keys_); 
//     break;
//   case 7:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_7.root"),     oFolder_ + TString("out-PhotonJets_300to470_7.root"),keys_); 
//     break;
//   case 8:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_8.root"),     oFolder_ + TString("out-PhotonJets_300to470_8.root"),keys_); 
//     break;
//   case 9:
//     process(false,url_, iFolder_ + TString("mc/PhotonJets_300to470_9.root"),     oFolder_ + TString("out-PhotonJets_300to470_9.root"),keys_); 
//     break;
//   }

}


void SampleProcessor::process_WW(int i){

  url_= WW_URL; 
  //  process(false, url_, iFolder_ + TString("WW.root"), oFolder_+TString("out-ww.root"),keys_); 

  switch(i){
  case 10:
    process(false, url_, iFolder_ + TString("mc/WW_10.root"), oFolder_+TString("out-ww_10.root"),keys_);
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/WW_11.root"), oFolder_+TString("out-ww_11.root"),keys_);
    break;
  case 12:
    process(false, url_, iFolder_ + TString("mc/WW_12.root"), oFolder_+TString("out-ww_12.root"),keys_);
    break;
  case 13:
    process(false, url_, iFolder_ + TString("mc/WW_13.root"), oFolder_+TString("out-ww_13.root"),keys_);
    break;
  case 14:
    process(false, url_, iFolder_ + TString("mc/WW_14.root"), oFolder_+TString("out-ww_14.root"),keys_);
    break;
  case 15:
    process(false, url_, iFolder_ + TString("mc/WW_15.root"), oFolder_+TString("out-ww_15.root"),keys_);
    break;
  case 16:
    process(false, url_, iFolder_ + TString("mc/WW_16.root"), oFolder_+TString("out-ww_16.root"),keys_);
    break;
  case 17:
    process(false, url_, iFolder_ + TString("mc/WW_17.root"), oFolder_+TString("out-ww_17.root"),keys_);
    break;
  case 18:
    process(false, url_, iFolder_ + TString("mc/WW_18.root"), oFolder_+TString("out-ww_18.root"),keys_);
    break;
  case 19:
    process(false, url_, iFolder_ + TString("mc/WW_19.root"), oFolder_+TString("out-ww_19.root"),keys_);
    break;
  case 2:
    process(false, url_, iFolder_ + TString("mc/WW_2.root"), oFolder_+TString("out-ww_2.root"),keys_);
    break;
  case 3:
    process(false, url_, iFolder_ + TString("mc/WW_3.root"), oFolder_+TString("out-ww_3.root"),keys_);
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/WW_4.root"), oFolder_+TString("out-ww_4.root"),keys_);
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/WW_5.root"), oFolder_+TString("out-ww_5.root"),keys_);
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/WW_6.root"), oFolder_+TString("out-ww_6.root"),keys_);
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/WW_7.root"), oFolder_+TString("out-ww_7.root"),keys_);
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/WW_8.root"), oFolder_+TString("out-ww_8.root"),keys_);
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/WW_9.root"), oFolder_+TString("out-ww_9.root"),keys_);
    break;
  }

}


void SampleProcessor::process_WZ(int i){

  url_= WZ_URL; 
  //  process(false, url_, iFolder_ + TString("WZ.root"), oFolder_+TString("out-wz.root"),keys_); 

  switch(i){
  case 10:
    process(false, url_, iFolder_ + TString("mc/WZ_10.root"), oFolder_+TString("out-wz_10.root"),keys_);
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/WZ_11.root"), oFolder_+TString("out-wz_11.root"),keys_);
    break;
  case 12:
    process(false, url_, iFolder_ + TString("mc/WZ_12.root"), oFolder_+TString("out-wz_12.root"),keys_);
    break;
  case 13:
    process(false, url_, iFolder_ + TString("mc/WZ_13.root"), oFolder_+TString("out-wz_13.root"),keys_);
    break;
  case 14:
    process(false, url_, iFolder_ + TString("mc/WZ_14.root"), oFolder_+TString("out-wz_14.root"),keys_);
    break;
  case 15:
    process(false, url_, iFolder_ + TString("mc/WZ_15.root"), oFolder_+TString("out-wz_15.root"),keys_);
    break;
  case 16:
    process(false, url_, iFolder_ + TString("mc/WZ_16.root"), oFolder_+TString("out-wz_16.root"),keys_);
    break;
  case 17:
    process(false, url_, iFolder_ + TString("mc/WZ_17.root"), oFolder_+TString("out-wz_17.root"),keys_);
    break;
  case 18:
    process(false, url_, iFolder_ + TString("mc/WZ_18.root"), oFolder_+TString("out-wz_18.root"),keys_);
    break;
  case 19:
    process(false, url_, iFolder_ + TString("mc/WZ_19.root"), oFolder_+TString("out-wz_19.root"),keys_);
    break;
  case 2:
    process(false, url_, iFolder_ + TString("mc/WZ_2.root"), oFolder_+TString("out-wz_2.root"),keys_);
    break;
  case 3:
    process(false, url_, iFolder_ + TString("mc/WZ_3.root"), oFolder_+TString("out-wz_3.root"),keys_);
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/WZ_4.root"), oFolder_+TString("out-wz_4.root"),keys_);
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/WZ_5.root"), oFolder_+TString("out-wz_5.root"),keys_);
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/WZ_6.root"), oFolder_+TString("out-wz_6.root"),keys_);
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/WZ_7.root"), oFolder_+TString("out-wz_7.root"),keys_);
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/WZ_8.root"), oFolder_+TString("out-wz_8.root"),keys_);
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/WZ_9.root"), oFolder_+TString("out-wz_9.root"),keys_);
    break;
  }
}


void SampleProcessor::process_ZZ(int i){

  url_= ZZ_URL; 
  //  process(false, url_, iFolder_ + TString("ZZ.root"), oFolder_+TString("out-zz.root"),keys_); 

  switch(i){
  case 10:
    process(false, url_, iFolder_ + TString("mc/ZZ_10.root"), oFolder_+TString("out-zz_10.root"),keys_);
    break;
  case 11:
    process(false, url_, iFolder_ + TString("mc/ZZ_11.root"), oFolder_+TString("out-zz_11.root"),keys_);
    break;
  case 12:
    process(false, url_, iFolder_ + TString("mc/ZZ_12.root"), oFolder_+TString("out-zz_12.root"),keys_);
    break;
  case 13:
    process(false, url_, iFolder_ + TString("mc/ZZ_13.root"), oFolder_+TString("out-zz_13.root"),keys_);
    break;
  case 14:
    process(false, url_, iFolder_ + TString("mc/ZZ_14.root"), oFolder_+TString("out-zz_14.root"),keys_);
    break;
  case 15:
    process(false, url_, iFolder_ + TString("mc/ZZ_15.root"), oFolder_+TString("out-zz_15.root"),keys_);
    break;
  case 16:
    process(false, url_, iFolder_ + TString("mc/ZZ_16.root"), oFolder_+TString("out-zz_16.root"),keys_);
    break;
  case 17:
    process(false, url_, iFolder_ + TString("mc/ZZ_17.root"), oFolder_+TString("out-zz_17.root"),keys_);
    break;
  case 18:
    process(false, url_, iFolder_ + TString("mc/ZZ_18.root"), oFolder_+TString("out-zz_18.root"),keys_);
    break;
  case 19:
    process(false, url_, iFolder_ + TString("mc/ZZ_19.root"), oFolder_+TString("out-zz_19.root"),keys_);
    break;
  case 2:
    process(false, url_, iFolder_ + TString("mc/ZZ_2.root"), oFolder_+TString("out-zz_2.root"),keys_);
    break;
  case 3:
    process(false, url_, iFolder_ + TString("mc/ZZ_3.root"), oFolder_+TString("out-zz_3.root"),keys_);
    break;
  case 4:
    process(false, url_, iFolder_ + TString("mc/ZZ_4.root"), oFolder_+TString("out-zz_4.root"),keys_);
    break;
  case 5:
    process(false, url_, iFolder_ + TString("mc/ZZ_5.root"), oFolder_+TString("out-zz_5.root"),keys_);
    break;
  case 6:
    process(false, url_, iFolder_ + TString("mc/ZZ_6.root"), oFolder_+TString("out-zz_6.root"),keys_);
    break;
  case 7:
    process(false, url_, iFolder_ + TString("mc/ZZ_7.root"), oFolder_+TString("out-zz_7.root"),keys_);
    break;
  case 8:
    process(false, url_, iFolder_ + TString("mc/ZZ_8.root"), oFolder_+TString("out-zz_8.root"),keys_);
    break;
  case 9:
    process(false, url_, iFolder_ + TString("mc/ZZ_9.root"), oFolder_+TString("out-zz_9.root"),keys_);
    break;
  }
}


void SampleProcessor::process_muon_RunA(int i){

  url_ = DATA_URL;
  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunA_"+idx+".root"),oDFolder_+TString("out-Muon_RunA_"+idx+".root"),keys_);

}



void SampleProcessor::process_muon_RunB(int i){

  url_ = DATA_URL;
  
  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunB_"+idx+".root"),oDFolder_+TString("out-Muon_RunB_"+idx+".root"),keys_);
  
}



void SampleProcessor::process_muon_RunC1(int i){

  url_ = DATA_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunC_1_"+idx+".root"),oDFolder_+TString("out-Muon_RunC_1_"+idx+".root"),keys_);  

}



void SampleProcessor::process_muon_RunC2(int i){

  url_ = DATA_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunC_2_"+idx+".root"),oDFolder_+TString("out-Muon_RunC_2_"+idx+".root"),keys_);
  
}



void SampleProcessor::process_muon_RunD1(int i){

  url_ = DATA_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunD_1_"+idx+".root"),oDFolder_+TString("out-Muon_RunD_1_"+idx+".root"),keys_);
  

}




void SampleProcessor::process_muon_RunD2(int i){

  url_ = DATA_URL;

  if(i>49){
    cout << "ERROR CODE - must be in the range [0,49]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("Muon_RunD_2_"+idx+".root"),oDFolder_+TString("out-Muon_RunD_2_"+idx+".root"),keys_);
  

}

void SampleProcessor::process_muon_Miss(int i){

  url_ = DATA_URL;

  if(i>9){
    cout << "ERROR CODE - must be in the range [0,9]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();

  process(true,url_,iDFolder_+TString("MuonMissing_RunB_"+idx+".root"),oDFolder_+TString("out-MuonMiss_RunB_"+idx+".root"),keys_);
  process(true,url_,iDFolder_+TString("MuonMissing_RunC_1_"+idx+".root"),oDFolder_+TString("out-MuonMiss_RunC_1_"+idx+".root"),keys_);
  process(true,url_,iDFolder_+TString("MuonMissing_RunC_2_"+idx+".root"),oDFolder_+TString("out-MuonMiss_RunC_2_"+idx+".root"),keys_);
  process(true,url_,iDFolder_+TString("MuonMissing_RunD_1_"+idx+".root"),oDFolder_+TString("out-MuonMiss_RunD_1_"+idx+".root"),keys_);
  process(true,url_,iDFolder_+TString("MuonMissing_RunD_2_"+idx+".root"),oDFolder_+TString("out-MuonMiss_RunD_2_"+idx+".root"),keys_);

}


void SampleProcessor::process_electr_RunA(int i){

  url_ = DATA_URL;

  if(i>9){
    cout << "ERROR CODE - must be in the range [0,9]" << endl;
    return;
  }

  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  process(true,url_,iDFolder_+TString("Electron_RunA_"+idx+".root"),oDFolder_+TString("out-Electron_RunA_"+idx+".root"),keys_);  
}


void SampleProcessor::process_electr_RunB(int i){

  url_ = DATA_URL;
  
  if(i>39){
    cout << "ERROR CODE - must be in the range [0,39]" << endl;
    return;
  }
  
  stringstream sidx;
  sidx<<i;
  string idx=sidx.str();
  process(true,url_,iDFolder_+TString("Electron_RunB_"+idx+".root"),oDFolder_+TString("out-Electron_RunB_"+idx+".root"),keys_);    
}


void SampleProcessor::process_electr_MisB(int i){

    url_ = DATA_URL;

    switch(i){

    case 1:
      process(true,url_,iDFolder_+TString("Electron_MisB_1.root"),oDFolder_+TString("out-Electron_MisB_1.root"),keys_);
      break;
    case 2:
      process(true,url_,iDFolder_+TString("Electron_MisB_2.root"),oDFolder_+TString("out-Electron_MisB_2.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_MisB_3.root"),oDFolder_+TString("out-Electron_MisB_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_MisB_4.root"),oDFolder_+TString("out-Electron_MisB_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_MisB_5.root"),oDFolder_+TString("out-Electron_MisB_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_MisB_6.root"),oDFolder_+TString("out-Electron_MisB_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_MisB_7.root"),oDFolder_+TString("out-Electron_MisB_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_MisB_8.root"),oDFolder_+TString("out-Electron_MisB_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_MisB_9.root"),oDFolder_+TString("out-Electron_MisB_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }



void SampleProcessor::process_electr_RunC_leg1(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_10.root"),oDFolder_+TString("out-Electron_RunC_leg1_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_11.root"),oDFolder_+TString("out-Electron_RunC_leg1_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_12.root"),oDFolder_+TString("out-Electron_RunC_leg1_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_13.root"),oDFolder_+TString("out-Electron_RunC_leg1_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_14.root"),oDFolder_+TString("out-Electron_RunC_leg1_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_15.root"),oDFolder_+TString("out-Electron_RunC_leg1_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_16.root"),oDFolder_+TString("out-Electron_RunC_leg1_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_17.root"),oDFolder_+TString("out-Electron_RunC_leg1_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_18.root"),oDFolder_+TString("out-Electron_RunC_leg1_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_19.root"),oDFolder_+TString("out-Electron_RunC_leg1_19.root"),keys_);
      break;

    case 20:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_20.root"),oDFolder_+TString("out-Electron_RunC_leg1_20.root"),keys_);
      break;
    case 21:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_21.root"),oDFolder_+TString("out-Electron_RunC_leg1_21.root"),keys_);
      break;
    case 22:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_22.root"),oDFolder_+TString("out-Electron_RunC_leg1_22.root"),keys_);
      break;
    case 23:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_23.root"),oDFolder_+TString("out-Electron_RunC_leg1_23.root"),keys_);
      break;
    case 24:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_24.root"),oDFolder_+TString("out-Electron_RunC_leg1_24.root"),keys_);
      break;
    case 25:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_25.root"),oDFolder_+TString("out-Electron_RunC_leg1_25.root"),keys_);
      break;
    case 26:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_26.root"),oDFolder_+TString("out-Electron_RunC_leg1_26.root"),keys_);
      break;
    case 27:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_27.root"),oDFolder_+TString("out-Electron_RunC_leg1_27.root"),keys_);
      break;
    case 28:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_28.root"),oDFolder_+TString("out-Electron_RunC_leg1_28.root"),keys_);
      break;
    case 29:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_29.root"),oDFolder_+TString("out-Electron_RunC_leg1_29.root"),keys_);
      break;


    case 0:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_0.root"),oDFolder_+TString("out-Electron_RunC_leg1_0.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_3.root"),oDFolder_+TString("out-Electron_RunC_leg1_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_4.root"),oDFolder_+TString("out-Electron_RunC_leg1_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_5.root"),oDFolder_+TString("out-Electron_RunC_leg1_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_6.root"),oDFolder_+TString("out-Electron_RunC_leg1_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_7.root"),oDFolder_+TString("out-Electron_RunC_leg1_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_8.root"),oDFolder_+TString("out-Electron_RunC_leg1_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg1_9.root"),oDFolder_+TString("out-Electron_RunC_leg1_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunC_leg2(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_10.root"),oDFolder_+TString("out-Electron_RunC_leg2_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_11.root"),oDFolder_+TString("out-Electron_RunC_leg2_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_12.root"),oDFolder_+TString("out-Electron_RunC_leg2_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_13.root"),oDFolder_+TString("out-Electron_RunC_leg2_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_14.root"),oDFolder_+TString("out-Electron_RunC_leg2_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_15.root"),oDFolder_+TString("out-Electron_RunC_leg2_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_16.root"),oDFolder_+TString("out-Electron_RunC_leg2_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_17.root"),oDFolder_+TString("out-Electron_RunC_leg2_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_18.root"),oDFolder_+TString("out-Electron_RunC_leg2_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_19.root"),oDFolder_+TString("out-Electron_RunC_leg2_19.root"),keys_);
      break;

    case 20:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_20.root"),oDFolder_+TString("out-Electron_RunC_leg2_20.root"),keys_);
      break;
    case 21:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_21.root"),oDFolder_+TString("out-Electron_RunC_leg2_21.root"),keys_);
      break;
    case 22:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_22.root"),oDFolder_+TString("out-Electron_RunC_leg2_22.root"),keys_);
      break;
    case 23:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_23.root"),oDFolder_+TString("out-Electron_RunC_leg2_23.root"),keys_);
      break;
    case 24:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_24.root"),oDFolder_+TString("out-Electron_RunC_leg2_24.root"),keys_);
      break;
    case 25:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_25.root"),oDFolder_+TString("out-Electron_RunC_leg2_25.root"),keys_);
      break;
    case 26:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_26.root"),oDFolder_+TString("out-Electron_RunC_leg2_26.root"),keys_);
      break;
    case 27:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_27.root"),oDFolder_+TString("out-Electron_RunC_leg2_27.root"),keys_);
      break;
    case 28:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_28.root"),oDFolder_+TString("out-Electron_RunC_leg2_28.root"),keys_);
      break;
    case 29:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_29.root"),oDFolder_+TString("out-Electron_RunC_leg2_29.root"),keys_);
      break;


    case 0:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_0.root"),oDFolder_+TString("out-Electron_RunC_leg2_0.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_3.root"),oDFolder_+TString("out-Electron_RunC_leg2_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_4.root"),oDFolder_+TString("out-Electron_RunC_leg2_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_5.root"),oDFolder_+TString("out-Electron_RunC_leg2_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_6.root"),oDFolder_+TString("out-Electron_RunC_leg2_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_7.root"),oDFolder_+TString("out-Electron_RunC_leg2_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_8.root"),oDFolder_+TString("out-Electron_RunC_leg2_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunC_leg2_9.root"),oDFolder_+TString("out-Electron_RunC_leg2_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunD_leg1(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_10.root"),oDFolder_+TString("out-Electron_RunD_leg1_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_11.root"),oDFolder_+TString("out-Electron_RunD_leg1_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_12.root"),oDFolder_+TString("out-Electron_RunD_leg1_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_13.root"),oDFolder_+TString("out-Electron_RunD_leg1_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_14.root"),oDFolder_+TString("out-Electron_RunD_leg1_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_15.root"),oDFolder_+TString("out-Electron_RunD_leg1_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_16.root"),oDFolder_+TString("out-Electron_RunD_leg1_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_17.root"),oDFolder_+TString("out-Electron_RunD_leg1_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_18.root"),oDFolder_+TString("out-Electron_RunD_leg1_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_19.root"),oDFolder_+TString("out-Electron_RunD_leg1_19.root"),keys_);
      break;

    case 20:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_20.root"),oDFolder_+TString("out-Electron_RunD_leg1_20.root"),keys_);
      break;
    case 21:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_21.root"),oDFolder_+TString("out-Electron_RunD_leg1_21.root"),keys_);
      break;
    case 22:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_22.root"),oDFolder_+TString("out-Electron_RunD_leg1_22.root"),keys_);
      break;
    case 23:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_23.root"),oDFolder_+TString("out-Electron_RunD_leg1_23.root"),keys_);
      break;
    case 24:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_24.root"),oDFolder_+TString("out-Electron_RunD_leg1_24.root"),keys_);
      break;
    case 25:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_25.root"),oDFolder_+TString("out-Electron_RunD_leg1_25.root"),keys_);
      break;
    case 26:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_26.root"),oDFolder_+TString("out-Electron_RunD_leg1_26.root"),keys_);
      break;
    case 27:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_27.root"),oDFolder_+TString("out-Electron_RunD_leg1_27.root"),keys_);
      break;
    case 28:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_28.root"),oDFolder_+TString("out-Electron_RunD_leg1_28.root"),keys_);
      break;
    case 29:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_29.root"),oDFolder_+TString("out-Electron_RunD_leg1_29.root"),keys_);
      break;


    case 0:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_0.root"),oDFolder_+TString("out-Electron_RunD_leg1_0.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_3.root"),oDFolder_+TString("out-Electron_RunD_leg1_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_4.root"),oDFolder_+TString("out-Electron_RunD_leg1_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_5.root"),oDFolder_+TString("out-Electron_RunD_leg1_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_6.root"),oDFolder_+TString("out-Electron_RunD_leg1_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_7.root"),oDFolder_+TString("out-Electron_RunD_leg1_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_8.root"),oDFolder_+TString("out-Electron_RunD_leg1_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg1_9.root"),oDFolder_+TString("out-Electron_RunD_leg1_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunD_leg2(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_10.root"),oDFolder_+TString("out-Electron_RunD_leg2_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_11.root"),oDFolder_+TString("out-Electron_RunD_leg2_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_12.root"),oDFolder_+TString("out-Electron_RunD_leg2_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_13.root"),oDFolder_+TString("out-Electron_RunD_leg2_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_14.root"),oDFolder_+TString("out-Electron_RunD_leg2_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_15.root"),oDFolder_+TString("out-Electron_RunD_leg2_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_16.root"),oDFolder_+TString("out-Electron_RunD_leg2_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_17.root"),oDFolder_+TString("out-Electron_RunD_leg2_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_18.root"),oDFolder_+TString("out-Electron_RunD_leg2_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_19.root"),oDFolder_+TString("out-Electron_RunD_leg2_19.root"),keys_);
      break;

    case 20:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_20.root"),oDFolder_+TString("out-Electron_RunD_leg2_20.root"),keys_);
      break;
    case 21:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_21.root"),oDFolder_+TString("out-Electron_RunD_leg2_21.root"),keys_);
      break;
    case 22:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_22.root"),oDFolder_+TString("out-Electron_RunD_leg2_22.root"),keys_);
      break;
    case 23:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_23.root"),oDFolder_+TString("out-Electron_RunD_leg2_23.root"),keys_);
      break;
    case 24:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_24.root"),oDFolder_+TString("out-Electron_RunD_leg2_24.root"),keys_);
      break;
    case 25:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_25.root"),oDFolder_+TString("out-Electron_RunD_leg2_25.root"),keys_);
      break;
    case 26:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_26.root"),oDFolder_+TString("out-Electron_RunD_leg2_26.root"),keys_);
      break;
    case 27:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_27.root"),oDFolder_+TString("out-Electron_RunD_leg2_27.root"),keys_);
      break;
    case 28:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_28.root"),oDFolder_+TString("out-Electron_RunD_leg2_28.root"),keys_);
      break;
    case 29:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_29.root"),oDFolder_+TString("out-Electron_RunD_leg2_29.root"),keys_);
      break;


    case 0:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_0.root"),oDFolder_+TString("out-Electron_RunD_leg2_0.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_3.root"),oDFolder_+TString("out-Electron_RunD_leg2_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_4.root"),oDFolder_+TString("out-Electron_RunD_leg2_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_5.root"),oDFolder_+TString("out-Electron_RunD_leg2_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_6.root"),oDFolder_+TString("out-Electron_RunD_leg2_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_7.root"),oDFolder_+TString("out-Electron_RunD_leg2_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_8.root"),oDFolder_+TString("out-Electron_RunD_leg2_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunD_leg2_9.root"),oDFolder_+TString("out-Electron_RunD_leg2_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunC_mis1(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_10.root"),oDFolder_+TString("out-Electron_RunC_mis1_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_11.root"),oDFolder_+TString("out-Electron_RunC_mis1_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_12.root"),oDFolder_+TString("out-Electron_RunC_mis1_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_13.root"),oDFolder_+TString("out-Electron_RunC_mis1_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_14.root"),oDFolder_+TString("out-Electron_RunC_mis1_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_15.root"),oDFolder_+TString("out-Electron_RunC_mis1_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_16.root"),oDFolder_+TString("out-Electron_RunC_mis1_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_17.root"),oDFolder_+TString("out-Electron_RunC_mis1_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_18.root"),oDFolder_+TString("out-Electron_RunC_mis1_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_19.root"),oDFolder_+TString("out-Electron_RunC_mis1_19.root"),keys_);
      break;

    case 2:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_2.root"),oDFolder_+TString("out-Electron_RunC_mis1_2.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_3.root"),oDFolder_+TString("out-Electron_RunC_mis1_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_4.root"),oDFolder_+TString("out-Electron_RunC_mis1_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_5.root"),oDFolder_+TString("out-Electron_RunC_mis1_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_6.root"),oDFolder_+TString("out-Electron_RunC_mis1_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_7.root"),oDFolder_+TString("out-Electron_RunC_mis1_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_8.root"),oDFolder_+TString("out-Electron_RunC_mis1_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis1_9.root"),oDFolder_+TString("out-Electron_RunC_mis1_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunC_mis2(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_10.root"),oDFolder_+TString("out-Electron_RunC_mis2_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_11.root"),oDFolder_+TString("out-Electron_RunC_mis2_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_12.root"),oDFolder_+TString("out-Electron_RunC_mis2_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_13.root"),oDFolder_+TString("out-Electron_RunC_mis2_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_14.root"),oDFolder_+TString("out-Electron_RunC_mis2_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_15.root"),oDFolder_+TString("out-Electron_RunC_mis2_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_16.root"),oDFolder_+TString("out-Electron_RunC_mis2_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_17.root"),oDFolder_+TString("out-Electron_RunC_mis2_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_18.root"),oDFolder_+TString("out-Electron_RunC_mis2_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_19.root"),oDFolder_+TString("out-Electron_RunC_mis2_19.root"),keys_);
      break;

    case 2:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_2.root"),oDFolder_+TString("out-Electron_RunC_mis2_2.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_3.root"),oDFolder_+TString("out-Electron_RunC_mis2_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_4.root"),oDFolder_+TString("out-Electron_RunC_mis2_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_5.root"),oDFolder_+TString("out-Electron_RunC_mis2_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_6.root"),oDFolder_+TString("out-Electron_RunC_mis2_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_7.root"),oDFolder_+TString("out-Electron_RunC_mis2_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_8.root"),oDFolder_+TString("out-Electron_RunC_mis2_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunC_mis2_9.root"),oDFolder_+TString("out-Electron_RunC_mis2_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunD_mis1(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_10.root"),oDFolder_+TString("out-Electron_RunD_mis1_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_11.root"),oDFolder_+TString("out-Electron_RunD_mis1_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_12.root"),oDFolder_+TString("out-Electron_RunD_mis1_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_13.root"),oDFolder_+TString("out-Electron_RunD_mis1_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_14.root"),oDFolder_+TString("out-Electron_RunD_mis1_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_15.root"),oDFolder_+TString("out-Electron_RunD_mis1_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_16.root"),oDFolder_+TString("out-Electron_RunD_mis1_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_17.root"),oDFolder_+TString("out-Electron_RunD_mis1_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_18.root"),oDFolder_+TString("out-Electron_RunD_mis1_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_19.root"),oDFolder_+TString("out-Electron_RunD_mis1_19.root"),keys_);
      break;

    case 2:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_2.root"),oDFolder_+TString("out-Electron_RunD_mis1_2.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_3.root"),oDFolder_+TString("out-Electron_RunD_mis1_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_4.root"),oDFolder_+TString("out-Electron_RunD_mis1_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_5.root"),oDFolder_+TString("out-Electron_RunD_mis1_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_6.root"),oDFolder_+TString("out-Electron_RunD_mis1_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_7.root"),oDFolder_+TString("out-Electron_RunD_mis1_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_8.root"),oDFolder_+TString("out-Electron_RunD_mis1_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis1_9.root"),oDFolder_+TString("out-Electron_RunD_mis1_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


void SampleProcessor::process_electr_RunD_mis2(int i){

    url_ = DATA_URL;

    switch(i){
    case 10:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_10.root"),oDFolder_+TString("out-Electron_RunD_mis2_10.root"),keys_);
      break;
    case 11:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_11.root"),oDFolder_+TString("out-Electron_RunD_mis2_11.root"),keys_);
      break;
    case 12:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_12.root"),oDFolder_+TString("out-Electron_RunD_mis2_12.root"),keys_);
      break;
    case 13:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_13.root"),oDFolder_+TString("out-Electron_RunD_mis2_13.root"),keys_);
      break;
    case 14:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_14.root"),oDFolder_+TString("out-Electron_RunD_mis2_14.root"),keys_);
      break;
    case 15:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_15.root"),oDFolder_+TString("out-Electron_RunD_mis2_15.root"),keys_);
      break;
    case 16:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_16.root"),oDFolder_+TString("out-Electron_RunD_mis2_16.root"),keys_);
      break;
    case 17:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_17.root"),oDFolder_+TString("out-Electron_RunD_mis2_17.root"),keys_);
      break;
    case 18:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_18.root"),oDFolder_+TString("out-Electron_RunD_mis2_18.root"),keys_);
      break;
    case 19:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_19.root"),oDFolder_+TString("out-Electron_RunD_mis2_19.root"),keys_);
      break;

    case 2:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_2.root"),oDFolder_+TString("out-Electron_RunD_mis2_2.root"),keys_);
      break;
    case 3:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_3.root"),oDFolder_+TString("out-Electron_RunD_mis2_3.root"),keys_);
      break;
    case 4:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_4.root"),oDFolder_+TString("out-Electron_RunD_mis2_4.root"),keys_);
      break;
    case 5:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_5.root"),oDFolder_+TString("out-Electron_RunD_mis2_5.root"),keys_);
      break;
    case 6:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_6.root"),oDFolder_+TString("out-Electron_RunD_mis2_6.root"),keys_);
      break;
    case 7:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_7.root"),oDFolder_+TString("out-Electron_RunD_mis2_7.root"),keys_);
      break;
    case 8:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_8.root"),oDFolder_+TString("out-Electron_RunD_mis2_8.root"),keys_);
      break;
    case 9:
      process(true,url_,iDFolder_+TString("Electron_RunD_mis2_9.root"),oDFolder_+TString("out-Electron_RunD_mis2_9.root"),keys_);
      break;
    default:
      cout << "ERROR CODE - must be in the correct range" << endl;
      break;
    }
  }


/*
  else{ 
    // inclusive electron trigger
    process(true,url_,iDFolder_+TString("Electron_1.root"), oDFolder_+TString("out-Electron_1.root"),keys_);
  }
*/
/*
  else if(! eChONmuChOFF_ && ! run2012_ ) {
  electron+2jets+mht samples
  process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_4.root"), oDFolder_+TString("out-eJTrigMatch_4.root"),keys_);
  }
*/  











void SampleProcessor::process_triggersamples_mht(){

  url_ = DATA_URL;

  iDFolder_=TString("/lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/");


  process(true,url_,iDFolder_+TString("Ele_MetTrig_4.root"),oDFolder_+TString("out-Ele_MetTrig_4.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_MetTrig_5.root"),oDFolder_+TString("out-Ele_MetTrig_5.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_MetTrig_6.root"),oDFolder_+TString("out-Ele_MetTrig_6.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_MetTrig_7.root"),oDFolder_+TString("out-Ele_MetTrig_7.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_MetTrig_8.root"),oDFolder_+TString("out-Ele_MetTrig_8.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_MetTrig_9.root"),oDFolder_+TString("out-Ele_MetTrig_9.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_MetTrig_10.root"),oDFolder_+TString("out-Ele_MetTrig_10.root"),keys_);




}


void SampleProcessor::process_triggersamples_mhtjets(){

  url_ = DATA_URL;

  iDFolder_=TString("/lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3-B/");
/*
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_4.root"),oDFolder_+TString("out-Ele_JetMetTrig_4.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_5.root"),oDFolder_+TString("out-Ele_JetMetTrig_5.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_6.root"),oDFolder_+TString("out-Ele_JetMetTrig_6.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_7.root"),oDFolder_+TString("out-Ele_JetMetTrig_7.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_8.root"),oDFolder_+TString("out-Ele_JetMetTrig_8.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_9.root"),oDFolder_+TString("out-Ele_JetMetTrig_9.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_10.root"),oDFolder_+TString("out-Ele_JetMetTrig_10.root"),keys_);
*/

  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_4.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_4.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_5.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_5.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_6.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_6.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_7.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_7.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_8.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_8.root"),keys_); 
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_9.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_9.root"),keys_);
  process(true,url_,iDFolder_+TString("Ele_JetMetTrig_matched_10.root"),oDFolder_+TString("out-Ele_JetMetTrig_matched_10.root"),keys_);

}


