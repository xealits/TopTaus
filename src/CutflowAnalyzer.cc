#include "LIP/TopTaus/interface/CutflowAnalyzer.hh"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>

#include "LIP/TopTaus/interface/BTagSFUtil.hh"

#include "PhysicsTools/Utilities/interface/LumiReweightingStandAlone.h"

using namespace std;

CutflowAnalyzer::CutflowAnalyzer( double tauPtCut, bool noUncertainties, bool doWPlusJetsAnalysis, TString inputArea, TString outputArea, TString puFileName, TString runRange, vector<double> brHtaunu, vector<double> brHtb) :
  UncertaintyCalculator(),  
  AnalysisMonitoring(tauPtCut, inputArea, outputArea), 
  ObjectSelector(tauPtCut),
  noUncertainties_(noUncertainties),
  doWPlusJetsAnalysis_(doWPlusJetsAnalysis),
  testMe_(0),
  testMe_Nev_(0)

{    
  brHtaunu_ = brHtaunu;
  brHtb_    = brHtb;

  cout << "------- BR(H+->taunu) -------" << endl;
  for(size_t i=0; i<brHtaunu_.size(); ++i)
    cout<< brHtaunu_[i] << endl;

  cout << "------- BR(H+->tb) -------" << endl;
  for(size_t i=0; i<brHtb_.size(); ++i)
    cout<< brHtb_[i] << endl;

  puFileName_ = puFileName ;

  // Default is ABCD in CommonDefinitions.cc
  if(runRange=="ABCD")     LUM_ = 19696.; // 2012 ABCD 539 ReReco.
  //  if(runRange=="ABCD")     LUM_ = 18282; // 2012 BCD 539 ReReco.

  // RunA: 876.225
  // RunB: 4311
  // RunC1: 3598
  // RunC2: 3283
  // RunD1: 3061
  // RunD2: 4129
  else if(runRange=="AB")  LUM_ = 0.; // Must update
  else if(runRange=="ABC") LUM_ = 0.; // Must update
  else if(runRange=="A")   LUM_ = 0.; // Must update
  else if(runRange=="B")   LUM_ = 0.; // Must update
  else if(runRange=="C1")  LUM_ = 0.; // Must update
  else if(runRange=="C2")  LUM_ = 0.; // Must update
  else if(runRange=="D")   LUM_ = 0.; // Must update
  
  // Acquire pileup weights
  float dataDist[100] = {0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
			 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.}; 
  TFile* dataWeightsFile =0;
  if(run2012_) dataWeightsFile = new TFile(puFileName_);
  if(!dataWeightsFile){ cout<<endl<<"File : " << puFileName_ << " not found or run2012_ not set."<<endl; exit(0);}
  TH1D* hist = (TH1D*) dataWeightsFile->Get("pileup");
  for(int i=0; i<50;++i)
    dataDist[i]=hist->GetBinContent(i+1);
  for(int i = 0; i < 50; ++i )
    DataPUDist_.push_back(dataDist[i]);
  /////////////////////////  


  // lepton efficiencies assumed to be ~100%.
  electrontriggerefficiency_= 1;

  muontriggerefficiency_    = 1;   
  leptontriggerefficiency_  = 1;
  ////////////////////////////////////////
  
  w_=1.; // Event weight defaults to 1
  is_os_=0.; // Stored in tree for fitters

  // Minimum distance b/ween objects ////////////////////////////////////////////////////////////////
  DRMIN_JET_E_ = 0.4; 
  DRMIN_JET_M_ = 0.4; 
  DRMIN_T_E_   = 0.4; 
  DRMIN_T_M_   = 0.4; 
  DRMIN_T_J_   = 0.3;
  ///////////////////////////////////////////////////////////////////////////////////////////////////    
  
  MT_CUT_    = 40;  
  R_CUT_     = 0.9;
  
  TAUPRONGS_ = 1;    
  
  // Uncertainties //////////////////////////////////////////////////// FIXME: update this
  JES_ = 0.05; UNC_ = 0.10; JER_ = 1; BTAGUNC_= 0.10; UNBTAGUNC_=0.10; TES_ = 0.03; TOPPTUNC_ = 1.;
  ////////////////////////////////////////////////////////////////////
  
  
  
  string cmsswFolder      = get_env_var("CMSSW_BASE");
  string jerFolder        = cmsswFolder+string("/src/CondFormats/JetMETObjects/data/");
  
  string lipcmsBaseFolder = cmsswFolder + string("/src/LIP/TopTaus/");// get_env_var("LIPCMS_BASE");
  string analysisFolder   = lipcmsBaseFolder+string("data/");
  
  // Files for jet correction uncertainties /////////////////////////////////////////////////////////////////////
  // How to instantiate uncertainty sources: https://twiki.cern.ch/twiki/bin/view/CMS/JECUncertaintySources#Example_implementation
  cout<<endl<<" FILE 1 : "<<(analysisFolder+string("/Summer13_V4_DATA_UncertaintySources_AK5PFchs.txt"))<<endl;
  cout<<endl<<" FILE 2 : "<<(jerFolder+string("/Spring10_PtResolution_AK5PF.txt"))<<endl;
  
  JetCorrectorParameters* jcp = new JetCorrectorParameters(analysisFolder+string("/Summer13_V4_DATA_UncertaintySources_AK5PFchs.txt"), "Total"); // Must implement splitting by sources
  jecUnc_ak5_pf_ = new JetCorrectionUncertainty( *jcp );
  cout<<endl<<"JetCorrectorParameters acquired"<<endl;
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  //Not used //////////////////////////
  jecUnc_ak5_jpt_      = jecUnc_ak5_pf_;
  jecUnc_data_ak5_pf_  = jecUnc_ak5_pf_;
  jecUnc_data_ak5_jpt_ = jecUnc_ak5_pf_;
  //////////////////////////////////////
  
  jerUnc_ak5_pf_pt_  = new JetResolution(jerFolder+string("/Spring10_PtResolution_AK5PF.txt"),true); // Temporarily disabled
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Poisson shifter for number of vertices ///////// Not used anymore. Shifting minbias xsec in the pileup distributions given as an input
  PShiftDown_ = reweight::PoissonMeanShifter(-0.6); // FIXME: remember to recalculate (10% of the mean number of vertices))
  PShiftUp_   = reweight::PoissonMeanShifter(0.6);
  ///////////////////////////////////////////////////
  
  // New BTAG uncertainty method /////////////////////////////////
  // New BTAG: >= Moriond 2013
  BTagSF_     = 0.966; err_BTagSF_     = 0.025;
  LightJetSF_ = 1.16;  err_LightJetSF_ = sqrt(0.02*0.02+0.014*0.014);
  
}

void CutflowAnalyzer::process(bool isData, urlCodes urlCode, TString path, TString outhistograms, vector<TString> & keys, uint ttbarLike ){

  isData_             = isData;
  ttbarLike_          = ttbarLike;
  outFileName_        = outhistograms;
  urlCode_            = urlCode;
  
  
  // prepare url and xsection file ///////////////////////////////
  vector<TString> urls; urls.push_back(path);
  vector<double>  xsec; xsec.push_back(defaultXSections_[urlCode_]);
  
  // tells AnalyzerMonitoring to initialize histogram plots and trees
  buildStructures();
  
  // Initializes output info debug txt
  infoFile_.open (outFileName_ + TString("_debug.txt")); 
  
  // Builds event readers and open files and fills debug info if full processing is enabled ////////////////////////////////////////
  // third parameter is isFullStatistics. If it's set to true, the last parameter (number of events to be processed) is ignored.
  init(urls,xsec,true,LUM_,1); // No need to differentiate between channels. Will differentiate channels in the code (faster: only one access to event vars per event)
  ///  if(     !eChONmuChOFF_                                       ) init(urls,xsec,true,LUM_,10000);
  ///  else if( eChONmuChOFF_ && MODE_ == STARTING_AT_LJETS_        ) init(urls,xsec,true,LUM_,10000);   //single lepton trigger
  ///  else if( eChONmuChOFF_ && MODE_ == STARTING_AT_LJETSPLUSMET_ ) init(urls,xsec,true,LUM_,100);     //electron+2jets+met trigger

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  
  scale_=listOfScales_[0];

  // gets the reader for this sample and the number of events to be processed
  evR_   = listOfReaders_[0];   nevents_   = listOfEventsToProcess_[0];
  evRMC_ = listOfReadersMC_[0]; neventsMC_ = listOfEventsToProcessMC_[0];
  ///////////////////////////////////////////////////////////////////////////

  
  // values for mu + tau///
  if( TAU_PT_MIN_== 20 ){
    
    //Muon with MT. FIXME: check these values 
    //BTAG_eff_R_ = 0.81376;         
    //BTAG_eff_F_ = 0.163441;


    // Muons without mt cut, 2013-08-24. Following twiki, gluon contribution not considered for ratios.
    // passage ntuples //     BTAG_eff_R_ = 0.656;         
    // passage ntuples //     BTAG_eff_F_ = 0.054;
    
    // new ntuples
    BTAG_eff_R_ = 0.396856;
    BTAG_eff_F_ = 0.031894;
    

    
  }else { // Zeroed
    BTAG_eff_R_ = 0.00000; //811112;
    BTAG_eff_F_ = 0.00000; //172388;
  }
  /////////////////////////
  
  
  MCPUDist_.clear(); 
  
  // Acquire scale factor for event weight according to pileup distribution ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(!isData_){
    TH1 * mcPileupHisto = (TH1*) listOfFiles_[0]->Get("myEventSelector/Selection/pileup");
    
    int histoEntries(51);
    
    if( mcPileupHisto ){  for( int s=1 /* avoid underflow bin */; s < mcPileupHisto->GetNbinsX() && s<histoEntries; ++s ){ MCPUDist_.push_back( mcPileupHisto->GetBinContent(s)); } }
    LumiWeights_ = reweight::LumiReWeighting(MCPUDist_, DataPUDist_);
    originalPDFEvents_=0; 
    PDFSelectedEvents_=0;
    weightedPDFSelectedEvents_.clear();
    weighted2PDFSelectedEvents_.clear();
    weightedPDFEvents_.clear();
    myWeights_.clear();
    for(int i=0;i<100;++i){myWeights_.push_back(0);} 
    //if( pdfweights_ && nevents_ != neventsMC_ ){ cout<<endl<<" ERROR :: nevents is : "<<nevents_<<" and in MC is "<<neventsMC_<<endl; return;}
  }
  ///////////////////////////////////7///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  // Main event loop ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(i_=0; i_< nevents_; ++i_){
    
    //    event::MiniEvent_t*  ev   = evR_->GetNewMiniEvent(i_,"data");  if( ev == 0 ) continue;
    event::MiniEvent_t*  ev(0);
    evR_->GetNewMiniEvent_try(ev, i_,"data");  if( ev == 0 ) continue;



    is_os_=0.; // Stored in tree for fitters // Reset for each event
    tauR_=-1; // Reset for each event
    if( ! (i_% 1000 ) ){cout<<" -> Processing ... : "<<(i_+1)<<endl;}

    // TAU DILEPTON ANALYZER ////////////////////////////////////////////////////////
    vector<TString>::iterator it;
    for(it=keys.begin();it!=keys.end();++it){   
      // tau dilepton analysis (newphys,tau algo, miniEvent, jes, unc, jer )
      myKey_ = (*it);
      eventAnalysis( false, (*it),ev,         0 ,        0,        0  ,             0,                0,         0, 0); 
      if(!isData_)
	if(!noUncertainties_){
	  eventAnalysis( false, (*it),ev,      JES_ ,        0,        0  ,             0,                0,           0,          0);        
	  eventAnalysis( false, (*it),ev, (-1)*JES_ ,        0,        0  ,             0,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,     UNC_,        0  ,             0,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,(-1)*UNC_,        0  ,             0,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,      JER_ ,             0,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0, (-1)*JER_ ,             0,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,      BTAGUNC_,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  , (-1)*BTAGUNC_,                0,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,       UNBTAGUNC_,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,  (-1)*UNBTAGUNC_,           0,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,                0,        TES_,          0);
	  eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,                0,   (-1)*TES_,          0);

	  if( url_ == TTBAR_URL){
	    eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,                0,        0,          TOPPTUNC_);
	    eventAnalysis( false, (*it),ev,        0  ,        0,        0  ,             0,                0,        0,     (-1)*TOPPTUNC_);
	  }
	}
      
      // WARNING : we only store WplusJets info for PFlow (HPS Pftopat based jets) 
      if( (*it) == TString("PFlow") && doWPlusJetsAnalysis_ ){  wPlusJetAnalysis( (*it), ev, 0,0,0,0,0,0); }
    }
    //////////////////////////////////////////////////////////////////////////////////
  }

  //SPY /////////////// FIXME: examine this
  //evR_->EndSpyEvents();
  /////////////////////


 
  /*
  evaluatePDFUncertainty();
  UNCOMMENT */ // FIXME: configure code for evaluation (and switch from conf file)


  if(outFile_){ 
     map< TString, TTree * >::iterator it2;
     outFile_->cd();
     for(it2 = treeMap_.begin(); it2!=treeMap_.end(); ++it2){ (*it2).second->Write(); }            
       
     cout<<endl<<"\n Going to write file ... "<<endl;
     outFile_->Write(); outFile_->Close(); 
     cout<<endl<<"\n done..."<<endl;            
  }

  
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void CutflowAnalyzer::eventAnalysis(bool newPhys,
				    TString myKey,
				    event::MiniEvent_t *ev,
				    double jes ,
				    double unc, 
				    double jer, 
				    double btagunc, 
				    double unbtagunc,
				    double tes, 
				    double topptunc
				    ){


  jes_=jes;
  unc_=unc;
  jer_=jer;
  btagunc_=btagunc; 
  unbtagunc_=unbtagunc;
  tes_=tes;
  topptunc_=topptunc;


  // see if we run with systematics...////////////////////////////////////////// 
  if( jes_|| unc_|| jer_|| btagunc_|| unbtagunc_ || tes_ || topptunc_) sys_ = true; else sys_= false;
  ///////////////////////////////////////////////////////////////////////////////


  // Set default lepton + jet selection
  SetLeptonPlusJetsSelection();
  setSelectionParameters();
  //////////////////////////////////////

  //trigger ///////////////////////////////////////////////////////
  TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
  bool hasMutrig = ((*trig)[0]>0); bool hasEGtrig = ((*trig)[1]>0);
  /////////////////////////////////////////////////////////////////

  //cout<<" trigger bits : "<<hasMutrig<<endl;

  
  unsigned int jetAlgo,tauType, leptonType;
  TString mcTag("");

  JetCorrectionUncertainty * junc(0);   JetResolution  * jerc(0);
  //   unsigned int metAlgo; // unused
  
  if( myKey.Contains("PFlow")   ) { // Moved up for optimization
    jetAlgo=event::AK5PFLOW, leptonType=event::PFLOWLEPTON; tauType = PFLOWTAU; if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PFLOWMET;  
  } 
  else if( myKey=="PF"               ) { 
    jetAlgo=event::AK5PF;  leptonType=event::STDLEPTON;    tauType = PFTAU;    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;   
  }
  else if( myKey.Contains("TaNC")    ) { 
    jetAlgo=event::AK5PF;   leptonType=event::STDLEPTON;   tauType = PFTAU;    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;  
  }
  else if( myKey.Contains("HPS")     ) { 
    jetAlgo=event::AK5PF,    leptonType=event::STDLEPTON;   tauType = HPSTAU;   if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;  
  } 
  

  unsigned int jetCorr;
  if(isData_){ jetCorr = event::Reader::VTXCONSTRAINED | event::Reader::RESJECCORR; }
  else       { jetCorr = event::Reader::VTXCONSTRAINED;                             }
  

  // get physics objects ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<PhysicsObject> vertices                 = evR_->GetVertexCollection(ev);
  std::vector<PhysicsObject> jets_without_arbitration = evR_->GetPhysicsObjectsFrom(ev,event::JET, jetAlgo  ); 
  std::vector<PhysicsObject> jets                     = evR_->GetJetsWithArbitration( jets_without_arbitration, jetCorr); 
  std::vector<PhysicsObject> muons                    = evR_->GetPhysicsObjectsFrom(ev,event::MUON, leptonType);     
  std::vector<PhysicsObject> electrons                = evR_->GetPhysicsObjectsFrom(ev,event::ELECTRON, leptonType); 
  std::vector<PhysicsObject> tausColl                 = evR_->GetPhysicsObjectsFrom(ev,event::TAU);
  std::vector<PhysicsObject> taus; 
  
  for(size_t iorigtau=0; iorigtau<tausColl.size(); ++iorigtau){ // Get taus from leptons collection
    if(tausColl[iorigtau][17] == tauType ){ taus.push_back(tausColl[iorigtau]); }
  }

  if(i_ == 45){ // Debug event
    cout<<endl<< "JETS IN EVENT " << i_ << endl;
    for(size_t ijet=0; ijet<jets.size(); ++ijet){
      cout << setprecision(6) << "\t\t jet " << ijet << ", pt " << jets[ijet].Pt() <<", eta " << jets[ijet].Eta() << ", phi " << jets[ijet].Phi() << endl;
      
    }
  }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  
  

  if(vertices.size()==0){ cout<<endl<<" Vertex was zero ???????"<<endl; return; }

  //  if(vertices.size()>30 || vertices.size()<5) return; // Vertex cut

  PhysicsObject & primaryVertex = vertices[0];

  TVectorD *classif = (TVectorD *)ev->eventClassif->At(0);



  if( (url_ == EMBEDDED_DATA_URL || url_ == EMBEDDED_TTBAR_URL) && i_ == 45){ // Debug event
    // Debugging temporary lines
    cout << "Event: " << i_ << ", New embedded weight: " << (*classif)[9] << "           old embedded weight: " << (*classif)[10] << endl;
  }



  if(! isData_ ) if(classif==0) return;  

  // pileup reweighting /////////////////////////////////////////////////////////////////////////
  if(! isData_ ){ 
    outtimepuWeight_= 1;     /*outtimepuWeight_ = eventPUWeight( (*classif)[3] );*/ 
    int npv = (*classif)[2]; //8 True number of interactions // 2]; //intime pu vertices
    intimepuWeight_ = LumiWeights_.ITweight(npv);

    //int ave_nvtx = int((*classif)[2] + (*classif)[3])/3; // OOT pileup
    int ave_nvtx = npv;
    if     ( pu_ == PUPLUS  ) intimepuWeight_ = intimepuWeight_*PShiftUp_.ShiftWeight( ave_nvtx );
    else if( pu_ == PUMINUS ) intimepuWeight_ = intimepuWeight_*PShiftDown_.ShiftWeight( ave_nvtx );
  } 
  else  { intimepuWeight_ = 1;  outtimepuWeight_= 1; }
  //////////////////////////////////////////////////////////////////////////////////////////////////



  //jet resolution corrections ////////////////////////////////////////////////////////////////////////////////

  // Add here allinone met propagation and do stuff here for that yaya.
  uint metAlgo;
  if( myKey.Contains("PFlow")   ) { metAlgo=event::PFLOWMET;  } 
  else if( myKey=="PF"               ) { metAlgo=event::PF;        }
  else if( myKey.Contains("TaNC")    ) { metAlgo=event::PF;        }
  else if( myKey.Contains("HPS")     ) { metAlgo=event::PF;        } 
  
  // HERE
  PhysicsObjectCollection mets = evR_->GetPhysicsObjectsFrom(ev,event::MET,metAlgo);           
  if(mets.size()==0) { cout << "No met available for " << myKey <<" analyis type ! "<< endl;  return;}
  PhysicsObject met = mets[0]; 

  if(i_ == 45) // Debug event
    cout<<endl<< "MET: pt " << met.Pt() << ", eta " << met.Eta() << ", phi " << met.Phi() << endl;
  vector<double> jerFactors;
  doPropagations( jerFactors, jes, jer, junc, jets , met, isData_);
  if(i_ == 45){ // Debug event
    cout<<endl<< "RESCALED JETS IN EVENT " << i_ << endl;
    for(size_t ijet=0; ijet<jets.size(); ++ijet){
      cout << setprecision(6) << "\t\t jet " << ijet << ", pt " << jets[ijet].Pt() <<", eta " << jets[ijet].Eta() << ", phi " << jets[ijet].Phi() << endl;
      
    }
    cout<<endl<< "PROPAGATED MET: pt " << met.Pt() << ", eta " << met.Eta() << ", phi " << met.Phi() << endl;
  }

  mets[0] = met;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  //preselect low energy jets (this is used for MHT computation in the electron channel) /////////
  if(hasEGtrig){
    DisableLtkCutOnJets(); Pt_Jet(15); 
    jetsForTrigger_.clear();
    PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&jetsForTrigger_,jets);
  } else jetsForTrigger_.clear();
  ///////////////////////////////////////////////////////////////////////////
  
  
  // preselect main objects ///////////////////////////////////////////////
  vector<int> e_init, m_init, j_init, t_init;
  e_init.clear();
  m_init.clear();
  j_init.clear();
  t_init.clear();

  PreSelectMuons(     evR_, &m_init, muons    , primaryVertex ); 
  PreSelectElectrons( evR_, &e_init, electrons, primaryVertex );
  DisableLtkCutOnJets(); 
  Pt_Jet( JET_PT_CUT_ );     
  PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&j_init,jets );
  PreSelectTaus( &t_init,taus,TAUPRONGS_, myKey, primaryVertex, tes );
  ////////////////////////////////////////////////////////////////////////

  
  // only accept jets if dr > drmin in respect to electrons and muons //////////////////////////////////////////////////
  vector<int> emptyColl, j_toRemove; 
  vector<int> j_afterLeptonRemoval;
  emptyColl.clear();
  j_toRemove.clear();
  j_afterLeptonRemoval.clear();

  ProcessCleaning(&j_init, &j_toRemove, &e_init, &emptyColl, jets, electrons, DRMIN_JET_E_ );
  ProcessCleaning(&j_init, &j_toRemove, &m_init, &emptyColl, jets, muons,     DRMIN_JET_M_ );
  ApplyCleaning(  &j_init, &j_toRemove, &j_afterLeptonRemoval);
  // do the same cleaning for jets that will be used in MHT computation 
  vector<int> jetsForMHT_emptyColl, jetsForMHT_toRemove; 
  vector<int> jetsForMHT_afterLeptonRemoval;
  jetsForMHT_emptyColl.clear();
  jetsForMHT_toRemove.clear();
  jetsForMHT_afterLeptonRemoval.clear();

  if(hasEGtrig){ // only for electron channel
    ProcessCleaning(&jetsForTrigger_, &jetsForMHT_toRemove, &e_init, &jetsForMHT_emptyColl, jets, electrons, DRMIN_JET_E_ );
    ProcessCleaning(&jetsForTrigger_, &jetsForMHT_toRemove, &m_init, &jetsForMHT_emptyColl, jets, muons,     DRMIN_JET_M_ );
    ApplyCleaning(  &jetsForTrigger_, &jetsForMHT_toRemove, &jetsForMHT_afterLeptonRemoval);  
  } else{
    jetsForMHT_emptyColl.clear();
    jetsForMHT_toRemove.clear(); 
    jetsForMHT_afterLeptonRemoval.clear();
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  // only accept taus if dr > drmin in respect to electrons and muons /////////////////////
  vector<int> t_toRemove;
  vector<int> t_afterLeptonRemoval; 
  t_toRemove.clear();
  t_afterLeptonRemoval.clear();
  ProcessCleaning(&t_init, &t_toRemove, &e_init, &emptyColl, taus, electrons, DRMIN_T_E_ );
  ProcessCleaning(&t_init, &t_toRemove, &m_init, &emptyColl, taus, muons,     DRMIN_T_M_ );
  ApplyCleaning(&t_init, &t_toRemove, &t_afterLeptonRemoval);
  /////////////////////////////////////////////////////////////////////////////////////////
  
  
  // remove jets if dr < drmin in respect to taus ////////////////////////////////////////////////////////////
  j_toRemove.clear(); t_toRemove.clear();
  vector<int> j_final;
  j_final.clear();
  ProcessCleaning(&j_afterLeptonRemoval, &j_toRemove, &t_afterLeptonRemoval, &t_toRemove, jets, taus, DRMIN_T_J_ );
  ApplyCleaning(&j_afterLeptonRemoval, &j_toRemove, &j_final);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // extra jet collection //////////////////////////////////////////////////////////////////////////////////////
  Pt_Jet(TAU_PT_MIN_); 
  vector<int> j_init2;
  j_init2.clear();
  PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&j_init2,jets);
  // only accept jets if dr > drmin in respect to electrons and muons 
  vector<int> emptyColl2, j_toRemove2; 
  vector<int> j_afterLeptonRemoval2;
  emptyColl2.clear();
  j_toRemove2.clear();
  j_afterLeptonRemoval2.clear();
  ProcessCleaning(&j_init2, &j_toRemove2, &e_init, &emptyColl2, jets, electrons, DRMIN_JET_E_ );
  ProcessCleaning(&j_init2, &j_toRemove2, &m_init, &emptyColl2, jets, muons,     DRMIN_JET_M_ );
  ApplyCleaning(&j_init2, &j_toRemove2, &j_afterLeptonRemoval2);
  int numbJets(0);
  // number of jets between TAU_PT_MIN_ and jet pt cut....
  if( j_afterLeptonRemoval2.size() > j_final.size()){ numbJets = j_afterLeptonRemoval2.size() - j_final.size(); }
  if(TAU_PT_MIN_< JET_PT_CUT_ ) Pt_Jet(JET_PT_CUT_); 
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // DEBUG *****************************************************************************
  /*
  cout<<"\n Physics object debug     : "<<endl; 
  cout<<"\n vertices                 : "<<vertices.size()                         <<endl; 
  cout<<"\n jets_without_arbitration : "<<jets_without_arbitration.size()         <<endl;
  cout<<"\n jets                     : "<<jets.size()                             <<" jets after Lepton Removal --> "<<j_afterLeptonRemoval.size()<<endl;
  cout<<"\n muons                    : "<<muons.size()                            <<" selected muons            --> "<<m_init.size()<<endl;
  cout<<"\n electrons                : "<<electrons.size()                        <<" selected electrons        --> "<<e_init.size()<<endl;
  cout<<"\n tausColl                 : "<<tausColl.size()                         <<endl;
  cout<<"\n taus                     : "<<taus.size()                             <<" taus after Lepton removal --> "<<t_afterLeptonRemoval.size()<<endl;
  */
  //////////////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  int numb_e(0), numb_m(0);
  bool lepReq(true);

  // To measure ttbar efficiencies we force EGTrig and hasMuTrig to true (it will be applied later on) // FIXME: what the fuck?? Must check why, whenever doing that
  if(trigEffStudy_ ){ hasEGtrig = true; hasMutrig = true; }
  ////////////////////////////////////////////////////////////////////////////////////////////////////

  if( hasEGtrig ){ numb_e = e_init.size(); if(numb_e){ evType_ = ETAU_ ; if(!isData_) w_ = intimepuWeight_*scale_; }}
  if( hasMutrig ){ numb_m = m_init.size(); if(numb_m){ evType_ = MUTAU_; if(!isData_) w_ = intimepuWeight_*scale_; }}
  

  // Tau embedding weight
  if(url_ == EMBEDDED_DATA_URL || url_ == EMBEDDED_TTBAR_URL) w_ *= (*classif)[9];

  TVectorD * classifMC(0);
  // PDF Uncertainties ////////////////////////////////////////////////////////////////////////////////////////////////////
  // fill weighted events and reset selected events vector weights
  if( !isData_ && pdfweights_ && !jes && !unc && !jer && !btagunc && !tes){
    event::MiniEvent_t *evMC = evRMC_->GetNewMiniEvent(i_,"mc");  if( evMC == 0 ){ cout<<"\n empty event"<<endl; return;}
    classifMC = (TVectorD *)evMC->eventClassif->At(0);
    //originalPDFEvents_ += 1.*w_;
    originalPDFEvents_ += 1.;  
    if( weightedPDFEvents_.size() == 0 ){
      for(int i=14; i<classifMC->GetNoElements();++i ){
	//weightedPDFEvents_.push_back( ( (*classifMC)[i])*w_  );
        weightedPDFEvents_.push_back( ( (*classifMC)[i]) );
	weightedPDFSelectedEvents_.push_back(0);
        weighted2PDFSelectedEvents_.push_back(0);
      }
    }
    else{ for(int i=14; i<classifMC->GetNoElements();++i ){ weightedPDFEvents_[i-14] += (*classifMC)[i] ; } }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // see if we are processing electron channel or muon channel /////////////////////////////////
  if (  ( !eChONmuChOFF_ && evType_ == ETAU_) || (eChONmuChOFF_ && evType_ == MUTAU_  ) ) return;
  //////////////////////////////////////////////////////////////////////////////////////////////
  
  
  // are triggers exclusive ?///////////////////////////////////////////////////
  //if( hasEGtrig && hasMuTrig ){ if( num_e==1 && numb_m== 1 ){ evType_ = EMU_; }
  //////////////////////////////////////////////////////////////////////////////
  
  // is muon/electron veto satisfied? ///////////////////////////////////////////////////////////////////
  if( numb_e + numb_m != 1){  lepReq=false; }
  if(      lepReq && evType_ == MUTAU_){
    if( LooseMuonVeto( m_init[0], muons )           ){ lepReq=false; }   //see if we have loose muons
    if( LooseElectronVeto(evR_,-1,electrons)        ){ lepReq=false; }   //see if we have loose electrons
  }   
  else if( lepReq && evType_ == ETAU_){
    //see if we have loose muons
    if( LooseMuonVeto(-1,muons) )                    { lepReq=false; }   //see if we have loose muons
    if( LooseElectronVeto(evR_,e_init[0],electrons) ){ lepReq=false; }   //see if we have loose electrons
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  
  // MHT computation ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mht_=-2; mhtb_=-2; //initial value 
  if     (   lepReq && evType_ == MUTAU_ ){ computeMHT( jets, jetsForMHT_afterLeptonRemoval, muons[0]);      computeMHTb( jets, jetsForMHT_afterLeptonRemoval); }
  else if(   lepReq && evType_ == ETAU_  ){ computeMHT( jets, jetsForMHT_afterLeptonRemoval, electrons[0]);  computeMHTb( jets, jetsForMHT_afterLeptonRemoval); }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  

  if(newPhys){ newPhysics(vertices, muons,m_init,electrons,e_init,taus,t_afterLeptonRemoval,jets,j_final,numbJets,junc,jerFactors,myKey,ev);}
  
  tauDileptonSelection(lepReq, vertices, muons,m_init,electrons,e_init,taus,t_afterLeptonRemoval,jets,j_final,numbJets,junc,jerFactors,myKey,ev, mets);

}



// to be removed //void CutflowAnalyzer::dileptonAnalysis(bool newPhys, TString myKey, event::MiniEvent_t *ev, double jes , double unc, double jer, double btagunc, double unbtagunc, double tes){
// to be removed //
// to be removed //
// to be removed //  jes_=jes; unc_=unc; jer_=jer, btagunc_=btagunc; unbtagunc_=unbtagunc; tes_=tes;
// to be removed //
// to be removed //
// to be removed //  // see if we run with systematics...////////////////////////////////////////// 
// to be removed //  if( jes_|| unc_|| jer_|| btagunc_|| unbtagunc_) sys_ = true; else sys_= false;
// to be removed //  ///////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //
// to be removed //  // Set default lepton + jet selection (inherited from ObjectSelector)
// to be removed //  // SetLeptonPlusJetsSelection();
// to be removed //  // Set default dilepton selection (inherited from ObjectSelector)
// to be removed //  SetDileptonSelection();
// to be removed //  setSelectionParameters();
// to be removed //  //////////////////////////////////////
// to be removed //
// to be removed //  //trigger ///////////////////////////////////////////////////////
// to be removed //  TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
// to be removed //  bool hasMutrig = ((*trig)[0]>0); bool hasEGtrig = ((*trig)[1]>0);
// to be removed //  /////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //  //cout<<" trigger bits : "<<hasMutrig<<endl;
// to be removed //
// to be removed //  
// to be removed //  unsigned int jetAlgo,tauType, leptonType;
// to be removed //  TString mcTag("");
// to be removed //  
// to be removed //  JetCorrectionUncertainty * junc(0);   JetResolution  * jerc(0);
// to be removed //  //  unsigned int metAlgo;  // unused
// to be removed //  
// to be removed //  if( myKey.Contains("PFlow")   ) { // Moved up for optimization
// to be removed //    jetAlgo=event::AK5PFLOW, leptonType=event::PFLOWLEPTON; tauType = PFLOWTAU; if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PFLOWMET;  
// to be removed //  } 
// to be removed //  else if( myKey=="PF"               ) { 
// to be removed //    jetAlgo=event::AK5PF;  leptonType=event::STDLEPTON;    tauType = PFTAU;    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;   
// to be removed //  }
// to be removed //  else if( myKey.Contains("TaNC")    ) { 
// to be removed //    jetAlgo=event::AK5PF;   leptonType=event::STDLEPTON;   tauType = PFTAU;    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;  
// to be removed //  }
// to be removed //  else if( myKey.Contains("HPS")     ) { 
// to be removed //    jetAlgo=event::AK5PF,    leptonType=event::STDLEPTON;   tauType = HPSTAU;   if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{junc=jecUnc_data_ak5_pf_ ;}// metAlgo=event::PF;  
// to be removed //  } 
// to be removed //  
// to be removed //  
// to be removed //  unsigned int jetCorr;
// to be removed //  if(isData_){ jetCorr = event::Reader::VTXCONSTRAINED | event::Reader::RESJECCORR; }
// to be removed //  else       { jetCorr = event::Reader::VTXCONSTRAINED;                             }
// to be removed //  
// to be removed //  
// to be removed //  // get physics objects ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  std::vector<PhysicsObject> vertices                 = evR_->GetVertexCollection(ev);
// to be removed //  std::vector<PhysicsObject> jets_without_arbitration = evR_->GetPhysicsObjectsFrom(ev,event::JET, jetAlgo  ); 
// to be removed //  std::vector<PhysicsObject> jets                     = evR_->GetJetsWithArbitration( jets_without_arbitration, jetCorr); 
// to be removed //  std::vector<PhysicsObject> muons                    = evR_->GetPhysicsObjectsFrom(ev,event::MUON, leptonType);     
// to be removed //  std::vector<PhysicsObject> electrons                = evR_->GetPhysicsObjectsFrom(ev,event::ELECTRON, leptonType); 
// to be removed //  std::vector<PhysicsObject> tausColl                 = evR_->GetPhysicsObjectsFrom(ev,event::TAU);
// to be removed //  std::vector<PhysicsObject> taus; 
// to be removed //  //  cout << "PFLOWTAU: " << PFLOWTAU << ", PFTAU: " << PFTAU << "HPSTAU: " << HPSTAU << endl; // Debug
// to be removed //  for(size_t iorigtau=0; iorigtau<tausColl.size(); ++iorigtau){ // Get taus from leptons collection
// to be removed //    //cout << tausColl[iorigtau][17] << ", " << tauType << endl; // Debug 
// to be removed //    if(tausColl[iorigtau][17] == tauType ){ taus.push_back(tausColl[iorigtau]); }
// to be removed //  }
// to be removed //  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  
// to be removed //  
// to be removed //
// to be removed //  if(vertices.size()==0){ cout<<endl<<" Vertex was zero ???????"<<endl; return; }
// to be removed //  PhysicsObject & primaryVertex = vertices[0];
// to be removed //  
// to be removed //  TVectorD *classif = (TVectorD *)ev->eventClassif->At(0);
// to be removed //  
// to be removed //  if(! isData_ ) if(classif==0) return;  
// to be removed //  
// to be removed //  // pileup reweighting /////////////////////////////////////////////////////////////////////////
// to be removed //  if(! isData_ ){ 
// to be removed //    outtimepuWeight_= 1;     /*outtimepuWeight_ = eventPUWeight( (*classif)[3] );*/ 
// to be removed //    int npv = (*classif)[2]; //intime pu vertices
// to be removed //    intimepuWeight_ = LumiWeights_.ITweight(npv);
// to be removed //    
// to be removed //    // why outtime pu? why this formula
// to be removed //    //int ave_nvtx = int((*classif)[2] + (*classif)[3])/3;
// to be removed //    int ave_nvtx = npv;
// to be removed //    if     ( pu_ == PUPLUS  ) intimepuWeight_ = intimepuWeight_*PShiftUp_.ShiftWeight( ave_nvtx );
// to be removed //    else if( pu_ == PUMINUS ) intimepuWeight_ = intimepuWeight_*PShiftDown_.ShiftWeight( ave_nvtx );
// to be removed //  } 
// to be removed //  else  { intimepuWeight_ = 1;  outtimepuWeight_= 1; }
// to be removed //  //////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //  //jet energy corrections ////////////////////////////////////////////////////////////////////////////////
// to be removed //  vector<double> jerFactors;
// to be removed //  vector<PhysicsObject> newJets;
// to be removed //  
// to be removed //  jerFactors.clear();
// to be removed //  newJets.clear();
// to be removed //  for(unsigned int i=0;i<jets.size();++i){ 
// to be removed //    double corr_jer(1.);
// to be removed //    if(!isData_){
// to be removed //      if(jer_== 0) newJets.push_back( smearedJet(jets[i], jets[i][34], 0/* 0=genpt, 1=random */, 0 /* 0=base, 1=jerup, 2=jerdown*/, corr_jer) );
// to be removed //      if(jer_> 0)  newJets.push_back( smearedJet(jets[i], jets[i][34], 0/* 0=genpt, 1=random */, 1 /* 0=base, 1=jerup, 2=jerdown*/, corr_jer) );
// to be removed //      if(jer_< 0)  newJets.push_back( smearedJet(jets[i], jets[i][34], 0/* 0=genpt, 1=random */, 2 /* 0=base, 1=jerup, 2=jerdown*/, corr_jer) );
// to be removed //    }
// to be removed //    jerFactors.push_back(1.);
// to be removed //  }
// to be removed //  jets=newJets;
// to be removed //
// to be removed //
// to be removed //  ///////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  //preselect low energy jets (this is used for MHT computation in the electron channel) /////////
// to be removed //  if(hasEGtrig){
// to be removed //    DisableLtkCutOnJets(); Pt_Jet(15); 
// to be removed //    jetsForTrigger_.clear();
// to be removed //    PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&jetsForTrigger_,jets);
// to be removed //  } else jetsForTrigger_.clear();
// to be removed //  ///////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  // preselect main objects ///////////////////////////////////////////////
// to be removed //  vector<int> e_init, m_init, j_init, t_init;
// to be removed //  PreSelectMuons(     evR_, &m_init, muons    , primaryVertex ); 
// to be removed //  PreSelectElectrons( evR_, &e_init, electrons, primaryVertex );
// to be removed //  DisableLtkCutOnJets(); 
// to be removed //  Pt_Jet( JET_PT_CUT_ );     
// to be removed //  PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&j_init,jets );
// to be removed //  PreSelectTaus( &t_init,taus,TAUPRONGS_, myKey, primaryVertex, tes );
// to be removed //  ////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //
// to be removed //  // only accept jets if dr > drmin in respect to electrons and muons //////////////////////////////////////////////////
// to be removed //  vector<int> emptyColl, j_toRemove; 
// to be removed //  vector<int> j_afterLeptonRemoval;
// to be removed //  ProcessCleaning(&j_init, &j_toRemove, &e_init, &emptyColl, jets, electrons, DRMIN_JET_E_ );
// to be removed //  ProcessCleaning(&j_init, &j_toRemove, &m_init, &emptyColl, jets, muons,     DRMIN_JET_M_ );
// to be removed //  ApplyCleaning(  &j_init, &j_toRemove, &j_afterLeptonRemoval);
// to be removed //  // do the same cleaning for jets that will be used in MHT computation 
// to be removed //  vector<int> jetsForMHT_emptyColl, jetsForMHT_toRemove; 
// to be removed //  vector<int> jetsForMHT_afterLeptonRemoval;
// to be removed //  if(hasEGtrig){ // only for electron channel
// to be removed //    ProcessCleaning(&jetsForTrigger_, &jetsForMHT_toRemove, &e_init, &jetsForMHT_emptyColl, jets, electrons, DRMIN_JET_E_ );
// to be removed //    ProcessCleaning(&jetsForTrigger_, &jetsForMHT_toRemove, &m_init, &jetsForMHT_emptyColl, jets, muons,     DRMIN_JET_M_ );
// to be removed //    ApplyCleaning(  &jetsForTrigger_, &jetsForMHT_toRemove, &jetsForMHT_afterLeptonRemoval);  
// to be removed //  } else{
// to be removed //    jetsForMHT_emptyColl.clear();
// to be removed //    jetsForMHT_toRemove.clear(); 
// to be removed //    jetsForMHT_afterLeptonRemoval.clear();
// to be removed //  }
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  // only accept taus if dr > drmin in respect to electrons and muons /////////////////////
// to be removed //  vector<int> t_toRemove;
// to be removed //  vector<int> t_afterLeptonRemoval; 
// to be removed //  ProcessCleaning(&t_init, &t_toRemove, &e_init, &emptyColl, taus, electrons, DRMIN_T_E_ );
// to be removed //  ProcessCleaning(&t_init, &t_toRemove, &m_init, &emptyColl, taus, muons,     DRMIN_T_M_ );
// to be removed //  ApplyCleaning(&t_init, &t_toRemove, &t_afterLeptonRemoval);
// to be removed //  /////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  // remove jets if dr < drmin in respect to taus ////////////////////////////////////////////////////////////
// to be removed //  j_toRemove.clear(); t_toRemove.clear();
// to be removed //  vector<int> j_final;
// to be removed //  ProcessCleaning(&j_afterLeptonRemoval, &j_toRemove, &t_afterLeptonRemoval, &t_toRemove, jets, taus, DRMIN_T_J_ );
// to be removed //  ApplyCleaning(&j_afterLeptonRemoval, &j_toRemove, &j_final);
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //
// to be removed //
// to be removed //  // extra jet collection //////////////////////////////////////////////////////////////////////////////////////
// to be removed //  Pt_Jet(TAU_PT_MIN_); 
// to be removed //  vector<int> j_init2;
// to be removed //  PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&j_init2,jets);
// to be removed //  // only accept jets if dr > drmin in respect to electrons and muons 
// to be removed //  vector<int> emptyColl2, j_toRemove2; 
// to be removed //  vector<int> j_afterLeptonRemoval2;
// to be removed //  ProcessCleaning(&j_init2, &j_toRemove2, &e_init, &emptyColl2, jets, electrons, DRMIN_JET_E_ );
// to be removed //  ProcessCleaning(&j_init2, &j_toRemove2, &m_init, &emptyColl2, jets, muons,     DRMIN_JET_M_ );
// to be removed //  ApplyCleaning(&j_init2, &j_toRemove2, &j_afterLeptonRemoval2);
// to be removed //  int numbJets(0);
// to be removed //  // number of jets between TAU_PT_MIN_ and jet pt cut....
// to be removed //  if( j_afterLeptonRemoval2.size() > j_final.size()){ numbJets = j_afterLeptonRemoval2.size() - j_final.size(); }
// to be removed //  if(TAU_PT_MIN_< JET_PT_CUT_ ) Pt_Jet(JET_PT_CUT_); 
// to be removed //  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //  // DEBUG *****************************************************************************
// to be removed //  /*
// to be removed //  cout<<"\n Physics object debug     : "<<endl; 
// to be removed //  cout<<"\n vertices                 : "<<vertices.size()                         <<endl; 
// to be removed //  cout<<"\n jets_without_arbitration : "<<jets_without_arbitration.size()         <<endl;
// to be removed //  cout<<"\n jets                     : "<<jets.size()                             <<" jets after Lepton Removal --> "<<j_afterLeptonRemoval.size()<<endl;
// to be removed //  cout<<"\n muons                    : "<<muons.size()                            <<" selected muons            --> "<<m_init.size()<<endl;
// to be removed //  cout<<"\n electrons                : "<<electrons.size()                        <<" selected electrons        --> "<<e_init.size()<<endl;
// to be removed //  cout<<"\n tausColl                 : "<<tausColl.size()                         <<endl;
// to be removed //  cout<<"\n taus                     : "<<taus.size()                             <<" taus after Lepton removal --> "<<t_afterLeptonRemoval.size()<<endl;
// to be removed //  */
// to be removed //  //////////////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  int numb_e(0), numb_m(0);
// to be removed //  bool lepReq(true);
// to be removed //
// to be removed //  // To measure ttbar efficiencies we force EGTrig and hasMuTrig to true (it will be applied later on) // FIXME: what the fuck?? Must check why, whenever doing that
// to be removed //  if(trigEffStudy_ ){ hasEGtrig = true; hasMutrig = true; }
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //
// to be removed //  if( hasEGtrig ){ numb_e = e_init.size(); if(numb_e){ evType_ = ETAU_ ; if(!isData_) w_ = intimepuWeight_*scale_; }}
// to be removed //  if( hasMutrig ){ numb_m = m_init.size(); if(numb_m){ evType_ = MUTAU_; if(!isData_) w_ = intimepuWeight_*scale_; }}
// to be removed //  
// to be removed //  
// to be removed //  TVectorD * classifMC(0);
// to be removed //  // PDF Uncertainties ////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  // fill weighted events and reset selected events vector weights
// to be removed //  if( !isData_ && pdfweights_ && !jes && !unc && !jer && !btagunc){
// to be removed //    event::MiniEvent_t *evMC = evRMC_->GetNewMiniEvent(i_,"mc");  if( evMC == 0 ){ cout<<"\n empty event"<<endl; return;}
// to be removed //    classifMC = (TVectorD *)evMC->eventClassif->At(0);
// to be removed //    //originalPDFEvents_ += 1.*w_;
// to be removed //    originalPDFEvents_ += 1.;  
// to be removed //    if( weightedPDFEvents_.size() == 0 ){
// to be removed //      for(int i=14; i<classifMC->GetNoElements();++i ){
// to be removed //	//weightedPDFEvents_.push_back( ( (*classifMC)[i])*w_  );
// to be removed //        weightedPDFEvents_.push_back( ( (*classifMC)[i]) );
// to be removed //	weightedPDFSelectedEvents_.push_back(0);
// to be removed //        weighted2PDFSelectedEvents_.push_back(0);
// to be removed //      }
// to be removed //    }
// to be removed //    else{ for(int i=14; i<classifMC->GetNoElements();++i ){ weightedPDFEvents_[i-14] += (*classifMC)[i] ; } }
// to be removed //  }
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  // see if we are processing electron channel or muon channel /////////////////////////////////
// to be removed //  if (  ( !eChONmuChOFF_ && evType_ == ETAU_) || (eChONmuChOFF_ && evType_ == MUTAU_  ) ) return;
// to be removed //  //////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  // are triggers exclusive ?///////////////////////////////////////////////////
// to be removed //  //if( hasEGtrig && hasMuTrig ){ if( num_e==1 && numb_m== 1 ){ evType_ = EMU_; }
// to be removed //  //////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  // is muon/electron veto satisfied? ///////////////////////////////////////////////////////////////////
// to be removed //  if( numb_e + numb_m != 1){  lepReq=false; }
// to be removed //  if(      lepReq && evType_ == MUTAU_){
// to be removed //    if( LooseMuonVeto( m_init[0], muons )           ){ lepReq=false; }   //see if we have loose muons
// to be removed //    if( LooseElectronVeto(evR_,-1,electrons)        ){ lepReq=false; }   //see if we have loose electrons
// to be removed //  }   
// to be removed //  else if( lepReq && evType_ == ETAU_){
// to be removed //    //see if we have loose muons
// to be removed //    if( LooseMuonVeto(-1,muons) )                    { lepReq=false; }   //see if we have loose muons
// to be removed //    if( LooseElectronVeto(evR_,e_init[0],electrons) ){ lepReq=false; }   //see if we have loose electrons
// to be removed //  }
// to be removed //  ////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //  
// to be removed //  // MHT computation ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  mht_=-2; mhtb_=-2; //initial value 
// to be removed //  if     (   lepReq && evType_ == MUTAU_ ){ computeMHT( jets, jetsForMHT_afterLeptonRemoval, muons[0]);      computeMHTb( jets, jetsForMHT_afterLeptonRemoval); }
// to be removed //  else if(   lepReq && evType_ == ETAU_  ){ computeMHT( jets, jetsForMHT_afterLeptonRemoval, electrons[0]);  computeMHTb( jets, jetsForMHT_afterLeptonRemoval); }
// to be removed //  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //  
// to be removed //
// to be removed //  if(newPhys){ newPhysics(vertices, muons,m_init,electrons,e_init,taus,t_afterLeptonRemoval,jets,j_final,numbJets,junc,jerFactors,myKey,ev);}
// to be removed //  
// to be removed //  dileptonEventAnalysis(lepReq, vertices, muons,m_init,electrons,e_init,taus,t_afterLeptonRemoval,jets,j_final,numbJets,junc,jerFactors,myKey,ev);
// to be removed //
// to be removed //
// to be removed //}
// to be removed //
// to be removed //

void CutflowAnalyzer::tauDileptonSelection(
  bool lepReq,
  std::vector<PhysicsObject> & v, 
  std::vector<PhysicsObject> & muons,     vector<int>  & m_init,  
  std::vector<PhysicsObject> & electrons, vector<int>  & e_init,
  std::vector<PhysicsObject> & taus,      vector<int>  & t_afterLeptonRemoval, 
  std::vector<PhysicsObject> & jets,      vector<int>  & j_final, 
  int totalJets, JetCorrectionUncertainty* junc, vector<double> & jerFactors, 
  TString myKey, event::MiniEvent_t *ev,
  PhysicsObjectCollection& mets
) {


  TString add("");


  if(isData_) applybtagweight_ =false;

  // only compute uncertainty due to the trigger when others unc are switched off /////////////////
  bool terr(false); if ( jes_== 0 && unc_== 0 && jer_==0 && btagunc_== 0 && unbtagunc_ && tes_==0) terr=true;
  TString triggErr   = TString("cutflow_triggErr");
  /////////////////////////////////////////////////////////////////////////////////////////////////

  if( jes_       > 0 ){ add = TString("_plus");       }if( jes_       < 0 ){ add = TString("_minus");      }
  if( unc_       > 0 ){ add = TString("_uncplus");    }if( unc_       < 0 ){ add = TString("_uncminus");   } 
  if( jer_       > 0 ){ add = TString("_jerplus");    }if( jer_       < 0 ){ add = TString("_jerminus");   }
  if( btagunc_   > 0 ){ add = TString("_bplus");      }if( btagunc_   < 0 ){ add = TString("_bminus");     }
  if( unbtagunc_ > 0 ){ add = TString("_unbplus");    }if( unbtagunc_ < 0 ){ add = TString("_unbminus");   }
  if( tes_       > 0 ){ add = TString("_tesplus");    }if( tes_       < 0 ){ add = TString("_tesminus");   }
  if( topptunc_  > 0 ){ add = TString("_topptuncplus");  }if( topptunc_  < 0 ){ add = TString("_topptuncminus");   }
  
  TString evYields          = TString("cutflow_yields")    + add;     
  TString evYieldsMC        = TString("cutflow_yields_mc") + add;   
  TString evYieldsMCTrigger = TString("cutflow_mc_triggErr");
  TString opt("optimization");
  
  SelectionMonitor * myMon    = tauDileptonYieldsMons_[myKey];    // monitor for the event yields
  SelectionMonitor * myMCMon(0);                                  // monitor for the event yields for MC
  
  
  // see if we are processing data, if we are processing MC get channel code ////////////////////////////////
  int input_channel = (int)(*(TVectorD *)ev->eventClassif->At(0))[0];
  int channel(-1); int tauDilCh;
  if(  !isData_ ){ 
   channel  = codeChannel(input_channel,urlCode_); 
    if(channel==WQQ_CH || channel==WENU_CH || channel==WTAUNU_CH || channel==WMUNU_CH ){ channel = WJETS_CH; }
    tauDilCh = tdChannel(channel);  
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  TString mcTag(""); 
  if( !isData_ ){ mcTag = myKey + TString(" yields mc");  myMCMon = tauDileptonMCYieldsMons_[myKey]; }
  SelectionMonitor& mon    = *myMon;
  SelectionMonitor& mcmon  = *myMCMon;
  SelectionMonitor& debmon = *(tauDileptonDebugMons_[myKey]);

  
  // Top pt reweighting
  if( url_ == TTBAR_URL){
    double tPt(99999.), tbarPt(99999.);
    event::MiniEvent_t *evmontecarlo = evRMC_->GetNewMiniEvent(i_,"mc");
    if( evmontecarlo == 0 ){ cout<<"\n empty event"<<endl; }
    else{
      std::vector<PhysicsObject> mcquarksColl = evRMC_->GetPhysicsObjectsFrom(evmontecarlo,event::QUARK);
      for( size_t iquark=0; iquark<mcquarksColl.size(); iquark++){
	if( i_ == 45) cout << "pdg id: " << mcquarksColl[iquark][2] << endl;
	if(mcquarksColl[iquark][2] == 6) tPt = mcquarksColl[iquark].Pt();
	if(mcquarksColl[iquark][2] == -6) tbarPt = mcquarksColl[iquark].Pt();
      }
      
    }
    
    // down: no reweighting
    // central: weight -> weight * reweight
    // up: weight -> weight * reweight * reweight
    if(topptunc_ >=0) 
      w_  *= ttbarReweight(tPt,tbarPt);    
    if( topptunc_>0)
      w_  *= ttbarReweight(tPt,tbarPt);
    
    if(i_ == 45) cout << "topptunc: (" << tPt << ", " << tbarPt << ") ---> " <<  ttbarReweight(tPt,tbarPt) <<endl;
  }
  /////////////////////////////////////////////
  
  
  // mini tree stats /////////////////////////////////////////////////////////////////
  
  if( myMCMon ){ mcmon.fill2DHisto( evYieldsMC , mcTag, MINITREE_STEP1, tauDilCh,w_); }  

  //cout<<" EvYields is "<<evYields<<" key is "<<(myKey+TString(" lep_tau yields"))<<" step is "<<MINITREE_STEP2<<" weight is "<<w_<<endl;

  mon.fillHisto(evYields, myKey+TString(" lep_tau yields"),MINITREE_STEP2,w_);
  mon.fillHisto(evYields, myKey+TString(" e_tau yields"),  MINITREE_STEP2,w_); 
  mon.fillHisto(evYields, myKey+TString(" m_tau yields"),  MINITREE_STEP2,w_);
  //  mon.fillHisto(evYields, myKey+TString(" m_m yields"),  MINITREE_STEP2,w_);  // dilepton yields (preparation)
  //  mon.fillHisto(evYields, myKey+TString(" m_e yields"),  MINITREE_STEP2,w_);  // dilepton yields (preparation)
  //  mon.fillHisto(evYields, myKey+TString(" e_e yields"),  MINITREE_STEP2,w_);  // dilepton yields (preparation)
  
  
  ///////////////////////////////////////////////////////////////////////////////////
  
  
  
  
  //debug
  if( ! lepReq ){ return; }
  
  // in case we are processing MC see if we require a specific channel //////////////////////////////
  // TODO INCLUDE TTBAR LIKE EMU_???????
  if( !isData_ ){
    // tau dilepton analysis //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if(  ttbarLike_ == ETAU_  && channel != ETAU_CH  )                                                                             { return; }
    else if(  ttbarLike_ == MUTAU_ && channel != MUTAU_CH )                                                                             { return; }
    else if(  ttbarLike_ == MUMU_  && channel != MUMU_CH  )                                                                             { return; }
    else if(  ttbarLike_ == EMU_   && channel != EMU_CH  )                                                                              { return; }
    else if(  ttbarLike_ == TTBAR_MCBKG_ && ( channel ==ETAU_CH || channel == MUTAU_CH || channel == EJETS_CH || channel== MUJETS_CH ) ){ return; }
    else if(  ttbarLike_ == TTBAR_DDBKG_ && (channel != EJETS_CH && channel!= MUJETS_CH ) )                                             { return; }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    // Top pt reweighting was here


    if( checkleptonsFromTauDecay_ ){
      event::MiniEvent_t *evmc = evRMC_->GetNewMiniEvent(i_,"mc");  if( evmc == 0 ){ cout<<"\n empty event"<<endl; return;}
      std::vector<PhysicsObject> mctausColl = evRMC_->GetPhysicsObjectsFrom(evmc,event::CHLEPTON);
      //get the charged lepton from tau //////////////////////////////////////////////////////////////////////////////////////////////////
     std::vector<PhysicsObject> mcleps;  mcleps.clear();
     for(size_t igtau = 0; igtau < mctausColl.size(); ++igtau){
       if(fabs(mctausColl[igtau][1]) != double(11) && fabs(mctausColl[igtau][1]) != double(13)) continue; // if it is not electron or muon
       if(mctausColl[igtau].Pt() < 20 || fabs(mctausColl[igtau].Eta()) > 2.1) continue;
       if(fabs(mctausColl[igtau][2]) != double(15)) continue; // check if lepton is from a tau decay
       mcleps.push_back(mctausColl[igtau]);
     }
     if( !( mcleps.size() == 1 && mcleps[0].Pt() > 20) )return ;
     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Fill debug info ///////////////////////////////////////////////////
  fillDebugHistograms(myKey,m_init,muons,e_init,electrons,j_final,jets); 
  //////////////////////////////////////////////////////////////////////
  
  PhysicsObject met = mets[0]; 
  
  // finding leading lepton and qualify tag ////////////////////////////////////////////////////////////////////////////////////////////////
  double lepton_pt(0); double lepton_charge(0); uint lepton_ind(0);
  vector<int> * p_i ; vector<PhysicsObject> * p_obj; PhysicsObject * lep_obj;
  
  if     ( evType_ == MUTAU_ ){ p_i = & m_init ; p_obj = &muons;    }
  else if( evType_ == ETAU_  ){ p_i = & e_init ; p_obj = &electrons;}
  else if( evType_ == MUMU_  ){ p_i = & m_init ; p_obj = &muons;    }
  else if( evType_ == EMU_   ){ p_i = & e_init ; p_obj = &electrons; for(unsigned int si=0; si<m_init.size(); ++si){ (*p_i).push_back(m_init[si]); (*p_obj).push_back(muons[si]); } }
  
  for(uint myi=0; myi < (*p_i).size(); ++myi){
    int ind = (*p_i)[myi]; double lPt = TMath::Abs((*p_obj)[ind].Pt()); double charge = (*p_obj)[ind][0]; PhysicsObject * tmp_obj= &( (*p_obj)[ind] );
    if( lPt > lepton_pt ){ lepton_pt = lPt; lepton_charge = charge; lepton_ind = ind; lep_obj = tmp_obj;}                                   
  }
  
  std::vector<TString> evTags; 
  if     (evType_ == ETAU_  ){ evTags.push_back(myKey+TString(" lep_tau"));  evTags.push_back(myKey+TString(" e_tau")); }
  else if(evType_ == MUTAU_ ){ evTags.push_back(myKey+TString(" lep_tau")); evTags.push_back(myKey+TString(" m_tau"));  }
  else if(evType_ == MUMU_  ){ evTags.push_back(myKey+TString(" lep_lep")); evTags.push_back(myKey+TString(" mu_mu")); }
  else if(evType_ == EMU_   ){ evTags.push_back(myKey+TString(" lep_lep")); evTags.push_back(myKey+TString(" e_mu")); }
 
  // WORKING HERE
  double metValue = met.Pt(); 

  double tau_charge(0); // , tau_pt(0); // unused 
  int tau_i;
  PhysicsObject * tau_obj;
  if(t_afterLeptonRemoval.size() == 1){ tau_i = t_afterLeptonRemoval[0]; tau_charge = taus[tau_i][0]; 
    // tau_pt = taus[tau_i].Pt(); // unused
    tau_obj = &(taus[tau_i]); }
  
  // Muon trigger/ID/Iso scale factors for efficiency from https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceEffs 
  if(!eChONmuChOFF_) LeadMuonTriggerEfficiency(muons, lepton_ind, muontriggerefficiency_);

  // lepton requirement includes trigger selection requirement ( the two highest pt jets should fire the trigger ) //////////////////////////////////
  // on MC   : we apply a trigger efficiency
  // on data : we require that the two highest pt jet should fire the trigger (returned eff from the method is either 0/1)
 

  // lepton requirement includes trigger selection requirement ( the two highest pt jets should fire the trigger ) //////////////////////////////////
  // on MC   : we apply a trigger efficiency
  // on data : we require that the two highest pt jet should fire the trigger (returned eff from the method is either 0/1)
  double errorOnEff(0); pair<double,double> eff(0,0);
  

  if( eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETSPLUSMET_ ){
    pair<double,double> efftemp( getEfficiencyAndError( jets,j_final,junc,jerFactors));
    eff.first = efftemp.first; eff.second = efftemp.second;

    if ( eff.first == 0 ) return;
    else{ 
      electrontriggerefficiency_ = eff.first;  
      if( !isData_ ){ leptontriggerefficiency_ = electrontriggerefficiency_; w_= w_*leptontriggerefficiency_; errorOnEff =  w_*(eff.second); } // get trigger efficiency for electrons 

      // take into account weights into efficiency error
    }
  }
  else if (eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_ ) { if( !isData_ )leptontriggerefficiency_ = electrontriggerefficiency_;}
  else{                                                      if( !isData_ ){
      leptontriggerefficiency_ = muontriggerefficiency_;  
      w_=w_*leptontriggerefficiency_;}
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // New way to deal with btag uncertainies //////////////////
  int nbtags_taus(0);
  bool nbtag1(false),  nbtag2(false); // ENABLED BTAGGING CUT
  ////////////////////////////////////////////////////////////



  // new way to compute btag uncertainty //////////////////////////////////////////
  if( isData_ || !  applybtagweight_){
    for(uint j=0; j<j_final.size(); ++j){  
      int j_ind = j_final[j]; 
      double btagvalue = jets[j_ind][BTAGIND_] ;
      if( btagvalue>BTAG_CUT_){nbtags_taus++; }
      cout << "DEBUG: jet number " << j << " has btag discriminator " << btagvalue << ". Currently we have already selected " << nbtags_taus << " btags" << endl; 
    }
  }
  else {
    for(uint j=0; j<j_final.size(); ++j){  
      int    j_ind     = j_final[j]; 
      double btagvalue = jets[j_ind][BTAGIND_];
      bool   isTagged = false;
      double bjpt(jets[j_ind].Pt()), bjeta(jets[j_ind].Eta()); 

      if( btagvalue > BTAG_CUT_) isTagged = true;

      double newBTagSF     = getSFb(bjpt, BTAGIND_);
      double newLightJetSF = getSFlight(bjpt, bjeta, BTAGIND_);
      
      // get flavour of the jet

      int jet_flavor = TMath::Abs(jets[j_ind][jetpgid_]);
      double err_newBTagSF = err_BTagSF_;

      if(jet_flavor == PGID_C ){err_newBTagSF =2*err_BTagSF_;}

      if(btagunc_   > 0){ newBTagSF     = newBTagSF+ err_newBTagSF; }
      else if (btagunc_   < 0){ newBTagSF     = newBTagSF- err_newBTagSF; }

      if(unbtagunc_ > 0){ newLightJetSF = newLightJetSF + err_LightJetSF_;}
      else if(unbtagunc_<0){ newLightJetSF = newLightJetSF - err_LightJetSF_;}

      //       double BTagEff     = newBTagSF*    BTAG_eff_R_;
      //       double LightJeteff = newLightJetSF*BTAG_eff_F_;

      double jet_phi = jets[j_ind].Phi();

      double sin_phi = sin(jet_phi*1000000);
      double seed = abs(static_cast<int>(sin_phi*100000));

      //Initialize class
      BTagSFUtil btsfutil(seed);
    
      //modify tags
      btsfutil.modifyBTagsWithSF(isTagged, jet_flavor, newBTagSF, /*BTagEff*/ BTAG_eff_R_, newLightJetSF, /*LightJeteff*/ BTAG_eff_F_);

      if(isTagged) nbtags_taus++;

    }
  }
  if(nbtags_taus > 0 ) nbtag1=true;
  if(nbtags_taus > 1)  nbtag2=true;
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  

  // events with 1 lepton /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for( size_t itag=0; itag<evTags.size();  ++itag ) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),LEP_STEP2,w_); 
  if (terr){
    for( size_t itag=0; itag<evTags.size(); ++itag ) mon.fillHisto(triggErr , evTags[itag]+TString(" yields"),LEP_STEP2,errorOnEff);
    if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,LEP_STEP2,tauDilCh,errorOnEff); }
  }
  if( myMCMon ){  mcmon.fill2DHisto(evYieldsMC,mcTag,LEP_STEP2,tauDilCh,w_); }
  fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"init",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // events with 1 jets //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(j_final.size()==0) return;
  for(size_t itag=0; itag<evTags.size() ; ++itag) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),JET1_STEP2,w_); 

  if (terr){
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),JET1_STEP2,errorOnEff);
    if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,JET1_STEP2,tauDilCh,errorOnEff); }
  }
  if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,JET1_STEP2,tauDilCh,w_); }
  fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 1 jet",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // events with at least 2 jets///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(j_final.size()<2 ) return;                                       
  for(size_t itag=0; itag<evTags.size();  ++itag) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),JET2_STEP2,w_);  
  if (terr){
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),JET2_STEP2,errorOnEff);
    if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,JET2_STEP2,tauDilCh,errorOnEff); }
  }
  if( myMCMon ){  mcmon.fill2DHisto(evYieldsMC,mcTag,JET2_STEP2,tauDilCh, w_);}
  fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 2 jet",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  //trigger efficiencies studies on data (MET part) /////////////////////////////////////////////////////////////////////////////////////
  // /lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/Ele_MetTrig_X.root
  // 1) Events selected with trigger : HLT_EleY_CaloIdVT_TrkIdT_CentralJet30_CentralJet25_vX 
  // HLT_EleY_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15_vX is placed at the 3rd position of trigger information.
  // These data is for measuring only the MHT part of trigger efficiency.
  if(isData_ && trigEffStudy_ ){

    //trigger /////////////////////////////////////////
    TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
    bool hasTesttrig = ((*trig)[3]>0); 
    ///////////////////////////////////////////////////
 
    //overall trigger efficiency
    for(int mymetcut = 5; mymetcut<150; ++mymetcut){
      if( metValue > mymetcut ){

        //denominator
        debmon.fillHisto("den_eff_trigger_met", myKey+TString(" e_tau debug"), mymetcut);
 
        if(hasTesttrig){
          //numerator
          debmon.fillHisto("num_eff_trigger_met", myKey+TString(" e_tau debug"), mymetcut);
        }


        if( j_final.size() + totalJets >= 3 ){
          //denominator
          debmon.fillHisto("den_eff_trigger_met3j", myKey+TString(" e_tau debug"), mymetcut);
 
          if(hasTesttrig){
            //numerator
            debmon.fillHisto("num_eff_trigger_met3j", myKey+TString(" e_tau debug"), mymetcut);
          }

        }

      }
    }

    // absolute efficiencies
    debmon.fillHisto("abs_den_eff_trigger_met", myKey+TString(" e_tau debug"), metValue);
 
    // numerator
    if(hasTesttrig){ debmon.fillHisto("abs_num_eff_trigger_met", myKey+TString(" e_tau debug"), metValue ); }

  
    if( j_final.size() + totalJets >= 3){
      // absolute efficiencies
      debmon.fillHisto("abs_den_eff_trigger_met3j", myKey+TString(" e_tau debug"), metValue);
 
      // numerator
      if(hasTesttrig){ debmon.fillHisto("abs_num_eff_trigger_met3j", myKey+TString(" e_tau debug"), metValue ); }

    }

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //trigger efficiencies studies on data (JET part) /////////////////////////////////////////////////////////////////////////////////////
  // /lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/Ele_JetMetTrig_X.root
  // 2) Events selected with trigger : HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_vX
  // HLT_EleY_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15_vX is placed at the 3rd position of trigger information.
  // These data is to be used for measuring combined Jet+MHT part of trigger efficiency from which we can extract jet efficiency part.
  // The files  4 and 5 have
  // HLT_Ele17_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15
  // Files 6, 7, 8 and 9 have
  // HLT_Ele22_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT20
  // File 10 have HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT20
  // They correspond to different run numbers and different trigger
  // thresholds. So the efficiency should be measured separately.
  // NOTE jet cut should be at 20 !!!!
  //
  // note : the tau is removed from the trigger list  
  //
  if(isData_ && trigEffStudy_ && metValue > 45 ){
    
    //trigger /////////////////////////////////////////
    // TVectorD *trig = (TVectorD *)ev->triggerColl->At(0); // unused
    // bool hasTesttrig = ((*trig)[3]>0);  // unused
    ///////////////////////////////////////////////////


 
    /*
    for(int jetPtCut = 20;jetPtCut<100; jetPtCut++){

      int numbjets(0);     
      for(uint i=0; i< jetsForTrigger_.size(); i++){
        int ind=jetsForTrigger_[i];
        double jetPt =  getJetPt( jets[ind], junc,0, jes_);
        bool triggeredJet = jets[ind][4];
        if(jetPt>jetPtCut && triggeredJet ) numbjets++;
      }
      if( numbjets>1 ){
   
        //denominator
        debmon.fillHisto("den_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPtCut);

 
        if(hasTesttrig){
          //denominator
          debmon.fillHisto("num_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPtCut);
        }
      }
    }
    */

    // absolute efficiencies get the 2 most energetic jet ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    double jetPt1(0),jetPt2(0); int ind2(-1), ind1(-1);
    for(uint i=0; i<j_final.size(); ++i){
      int ind=j_final[i];
      double jetPt =  getJetPt( jets[ind], junc,0, jes_);
      if( jetPt > jetPt1       ){ jetPt2=jetPt1; ind2=ind1; jetPt1=jetPt; ind1=ind; }
      else if( jetPt > jetPt2  ){ jetPt2=jetPt;  ind2=ind;                          }
    }
    
    if(jetPt2){
      //denominator
      debmon.fillHisto("abs_den_eff_trigger_jetptcut1", myKey+TString(" e_tau debug"), jetPt1);
      debmon.fillHisto("abs_den_eff_trigger_jetptcut2", myKey+TString(" e_tau debug"), jetPt2);

      //if(hasTesttrig){

        //if(jetPt2>20){ cout<<" jetPt is : "<<jetPt2<<" triggeredJet2 is : "<<(jets[ind2][4])<<endl;}

        //numerator
        double triggeredJet1 = jets[ind1][4];  // most energetic jet
        double triggeredJet2 = jets[ind2][4];  // second most energetic jet

        //if( triggeredJet1 && triggeredJet2 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPt2 ); }

        if( triggeredJet1 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut1", myKey+TString(" e_tau debug"), jetPt1 ); }
        if( triggeredJet2 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut2", myKey+TString(" e_tau debug"), jetPt2 ); }

      //}
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  }


  // trigger efficiencies studies ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( evType_==ETAU_ && trigEffStudy_ && t_afterLeptonRemoval.size()<2 && jes_==0){

    int jmult = t_afterLeptonRemoval.size() + j_final.size();

    //trigger ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool trigger(false); TVectorD *trig = (TVectorD *)ev->triggerColl->At(0); bool hasEGtrig = ((*trig)[1]>0); if( hasEGtrig ){ trigger =true;}
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // efficiencies on electron + tau + jets channels //////////////////////////////////////////////////////////////////////////////////////////////
    if( channel == ETAU_CH && t_afterLeptonRemoval.size()== 1 ){

      // get the highest third pt jet /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      double jetPtMax1(0),jetPtMax2(0),jetPtMax3(0),jetEtaPtMax1(0),jetEtaPtMax2(0),jetEtaPtMax3(0);
      for(uint j_i=0; j_i<j_final.size(); ++j_i){
        int ind = j_final[j_i];
        double jetPt =  getJetPt( jets[ind], junc,0, jes_); double jetEta = fabs( jets[ind].Eta());
        
        if     ( jetPt > jetPtMax1 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPtMax1; jetEtaPtMax2=jetEtaPtMax1; jetPtMax1=jetPt; jetEtaPtMax1=jetEta; }
        else if( jetPt > jetPtMax2 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPt;     jetEtaPtMax2=jetEta;                                             }
        else if( jetPt > jetPtMax3 ){jetPtMax3=jetPt;     jetEtaPtMax3 = jetEta;                                                                                             }
        
      }
      if(t_afterLeptonRemoval.size()==1){ // this condition was already verified...
        //jer assumed to be 1
        int ind = t_afterLeptonRemoval[0];
        double jetPt = taus[ind][7]; double jetEta = taus[ind][8]; 
        junc->setJetEta(jetEta); junc->setJetPt(jetPt); double corr=junc->getUncertainty(true); 
        jetPt = (1+corr)*jetPt;
        //if we have only 2 jets in the event....
        if(jetPtMax3==0){ 
          if(jetPt<jetPtMax2){    jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
          else{                   jetPtMax3=jetPtMax2; jetEtaPtMax3=jetEtaPtMax2; }
        }
        else if(jetPt<jetPtMax3){ jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
        
      }
      //////////////////////////////////////////////////////////////////////////////////////////


      debmon.fillHisto("jmult_den_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
      debmon.fillHisto("jmult_den_pt",  myKey+TString(" e_tau debug"), jetPtMax3);
      debmon.fillHisto("jmult_den",     myKey+TString(" e_tau debug"), jmult,1);

      if(trigger){  
        debmon.fillHisto("jmult_num_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
        debmon.fillHisto("jmult_num_pt",  myKey+TString(" e_tau debug"), jetPtMax3); 
        debmon.fillHisto("jmult_num",     myKey+TString(" e_tau debug"), jmult,1);
      }

    }

    // efficiencies on electron + jets
    if(channel == WJETS_CH && jmult >=3 ){

      // get the highest third pt jet /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      double jetPtMax1(0),jetPtMax2(0),jetPtMax3(0),jetEtaPtMax1(0),jetEtaPtMax2(0),jetEtaPtMax3(0);
      for(uint j_i=0; j_i<j_final.size(); ++j_i){
        int ind = j_final[j_i];
        double jetPt   =  getJetPt( jets[ind], junc,0, jes_); double jetEta  = fabs( jets[ind].Eta());
        
        if     ( jetPt > jetPtMax1 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPtMax1; jetEtaPtMax2=jetEtaPtMax1; jetPtMax1=jetPt; jetEtaPtMax1=jetEta;}
        else if( jetPt > jetPtMax2 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPt;     jetEtaPtMax2=jetEta;                                            }
        else if( jetPt > jetPtMax3 ){jetPtMax3=jetPt;     jetEtaPtMax3 = jetEta;                                                                                            }
        
      }
      if(t_afterLeptonRemoval.size()==1){
        //jer assumed to be 1
        int ind = t_afterLeptonRemoval[0];
        double jetPt = taus[ind][7]; double jetEta = taus[ind][8]; 
        junc->setJetEta(jetEta); junc->setJetPt(jetPt); double corr=junc->getUncertainty(true); 
        jetPt = (1+corr)*jetPt;
        //if we have only 2 jets in the event....
        if(jetPt<jetPtMax3){ jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
        
      }
      //////////////////////////////////////////////////////////////////////////////////////////



      debmon.fillHisto("jmult_den_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3 );
      debmon.fillHisto("jmult_den_pt",  myKey+TString(" e_tau debug"), jetPtMax3    );
      debmon.fillHisto("jmult_den",     myKey+TString(" e_tau debug"), jmult,1      );

      if(trigger){  
        debmon.fillHisto("jmult_num_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
        debmon.fillHisto("jmult_num_pt",  myKey+TString(" e_tau debug"), jetPtMax3   ); 
        debmon.fillHisto("jmult_num",     myKey+TString(" e_tau debug"), jmult,1);
      }

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //  double tempw(1.);
  // Here temp
  
  // events with at least 2 jets+1 jet>20GeV ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // number of jets above jet pt cut and extra jet between tau pt and jet pt cut 
  if( j_final.size() + totalJets <3) return;
  for(size_t itag=0; itag<evTags.size();  ++itag) mon.fillHisto(evYields ,evTags[itag]+TString(" yields"),    JET3_STEP2,w_);  
  if (terr){ 
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),JET3_STEP2,errorOnEff);  
    if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,JET3_STEP2,tauDilCh,errorOnEff); }
  }
  if( myMCMon){ mcmon.fill2DHisto(evYieldsMC,mcTag,JET3_STEP2,tauDilCh,w_);}
  fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 3 jet",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // events passing MET cut //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( metValue < MET_CUT_) return;
  for(size_t itag=0; itag<evTags.size();  ++itag) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),    MET_STEP2,w_);  
  if (terr){
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),MET_STEP2,errorOnEff);
    if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,MET_STEP2,tauDilCh,errorOnEff); }
  }
  if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,MET_STEP2,tauDilCh,w_);}
  fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"MET",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Here temp
  //testMe_+=errorOnEff2; testMe_Nev_++;
  //cout<<endl<<" Deb :  eff  = "<<setprecision (5)<<(eff.first)<<" +- "<<(eff.second)<<" errorOnEff2  = "<<errorOnEff2<<" total is "<<testMe_<<" in N events : "<<testMe_Nev_<<endl;







  // MT cut /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  double deltaPhi = (*p_obj)[lepton_ind].DeltaPhi( mets[0] );  
  double mt = sqrt (  2*lepton_pt*metValue*(1 - cos(deltaPhi) ) ) ;
  if( APPLY_MT_CUT_){ 
    if( mt<MT_CUT_ ){ return; } 
    for(size_t itag=0; itag<evTags.size();  ++itag) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"), MT_STEP2,w_);  
    if (terr){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),MT_STEP2,errorOnEff);
      if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,MT_STEP2,tauDilCh,errorOnEff); }
    }
    if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,MT_STEP2,tauDilCh,w_);}
    fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"MT",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // BTAG multiplicity was here.




  //cout<<endl<<"tau dilepton event analysis H"<<endl;
  //taus ////////////////////////////////////////////
  bool taucut(false);
  if( t_afterLeptonRemoval.size()== 1 ) taucut =true;

  int numbProngs(0);
  double tauP(0),tauE,r(0);
  tauR_ = -1;
  

  if( taucut ){ 
    numbProngs = GetNumbProngs(taus[tau_i]);
    tauP = taus[tau_i][16];  // lead charged hadron P
    tauE = taus[tau_i].E();  // tau energy
    if(tauE){ r= tauP/tauE; tauR_ = r;} // tau polarization
    
  }
  ///////////////////////////////////////////////////
   

  //cout<<endl<<"tau dilepton event analysis I"<<endl;
  //Opposite sign //////////////////////////////////
  bool oscut(false);
  if( lepton_charge * tau_charge < 0 ) oscut = true; 
  if(oscut) is_os_ = 1.;
  //////////////////////////////////////////////////


  // 1 btag option ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // hack
  int BTAG1_NOMT;
  if(!APPLY_MT_CUT_) BTAG1_NOMT = BTAG1_STEP2 -1;
  else               BTAG1_NOMT = BTAG1_STEP2;
  // end hack
  if( nbtag1 ) {
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),    BTAG1_NOMT, w_); 
    if (terr){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),BTAG1_NOMT, errorOnEff);  
      if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,BTAG1_NOMT,tauDilCh,errorOnEff); }
    }
    if( myMCMon){ mcmon.fill2DHisto(evYieldsMC,mcTag,BTAG1_NOMT,tauDilCh,w_); } 
    fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"1 btag",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  // TAU ID selection step //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // hack
  int TAU_NOMT;
  if(!APPLY_MT_CUT_) TAU_NOMT = TAU_STEP2 -1;
  else               TAU_NOMT = TAU_STEP2;
  // end hack
  
  
  if(nbtag1 && taucut){
    // l->tau fakes rescaling
    if(!isData_ &&   channel==EMU_CH ){
      if(fabs(taus[tau_i].Eta()) < 1.442)      w_ *= 0.85;
      else if (fabs(taus[tau_i].Eta()) > 1.566) w_ *= 0.65;
    }
    
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),    TAU_NOMT,w_);  
    if (terr){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),TAU_NOMT, errorOnEff); 
      if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,TAU_NOMT,tauDilCh,errorOnEff); }
    }

    if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,TAU_NOMT,tauDilCh,w_); }
    fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"1 Tau",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);

    
    for(size_t itag=0; itag<evTags.size(); ++itag){ 
      //Build Tree quantites based on tau jet 
      PhysicsObject tauJet; 
      double tauJetPt  = (*tau_obj)[7];
      double tauJetEta = (*tau_obj)[8];
      double tauJetPhi = (*tau_obj)[9];
      tauJet.SetPtEtaPhiE(tauJetPt,tauJetEta,tauJetPhi,tauJetPt);
      tauJetPt =  getJetPt( tauJet, junc, 0, jes_); //correct pt
      tauJet.SetPtEtaPhiE(tauJetPt,tauJetEta,tauJetPhi,tauJetPt);
      double tauJetRadius(0); //Warning this needs to be changed (not needed)
      TString treeOption("Selected"); treeOption += add;
      fillTauDileptonObjTree(ev,treeOption,evTags[itag],junc,mets[0],tauDilCh,TString("1 Tau"),lep_obj,tauJetRadius, tau_obj, &(taus[tau_i]), j_final, jets, jerFactors, metValue,nbtags_taus);
    }
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


   // SPY events for the w+jet and ttbar other contribution//////////////////////////////////////////////
   //if(  (!applybtagweight_  && nbtag1 && taucut ) ||  (applybtagweight_ && taucut )  ){
   //    if(jes_== 0 && unc_== 0 && jer_== 0 && btagunc_==0 && pu_ == NOPU ) listOfReaders_[0]->SpyEvent();
   //}
   ///////////////////////////////////////////////////////////////////////////////////////////////////////


  // hack
  int OS_NOMT;
  if(!APPLY_MT_CUT_) OS_NOMT = OS_STEP2 -1;
  else               OS_NOMT = OS_STEP2;
  // end hack

  if(  nbtag1 && taucut && oscut ){

    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),OS_NOMT,w_);
    if (terr){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),OS_NOMT, errorOnEff);
      if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,OS_NOMT,tauDilCh,errorOnEff); }
    }

    if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,OS_NOMT,tauDilCh,w_); }
    fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"OS",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);

    //SPY AFTER TAU ID AND OS /////////////////////////////////////////////////////////////////////////
    //if(jes_== 0 && unc_== 0 && jer_== 0 && btagunc_==0 && pu_ == NOPU ) listOfReaders_[0]->SpyEvent();
    ///////////////////////////////////////////////////////////////////////////////////////////////////

   


    // PDF Uncertainties ////////////////////////////////////////////////////////////////////////////////////////////////////
    // fill weighted events and reset selected events vector weightsMS Emeri
    if( !isData_ && pdfweights_ && !sys_){
      TVectorD * classifMC(0);
      //PDFSelectedEvents_ += (w_); 
      PDFSelectedEvents_ += 1.;
      event::MiniEvent_t *evMC = evRMC_->GetNewMiniEvent(i_,"mc");  if( evMC == 0 ){ cout<<"\n empty event"<<endl; return;}
      classifMC = (TVectorD *)evMC->eventClassif->At(0);
  
      for(int i=14; i<classifMC->GetNoElements();++i ){
        cout.precision(3);
        myWeights_[i-14] += (*classifMC)[i]/(*classifMC)[14];        
        cout<<endl<<" sel. Evnt :"<<i_<<", (i-14) = "<<(i-14)<<" w : "<<(*classifMC)[i]<<" rel w : "<<((*classifMC)[i]/(*classifMC)[i-14])<<" total w : "<<myWeights_[i-14];
        //weightedPDFSelectedEvents_[i-6]  += ( ( (*classifMC)[i] ) * w_ * w1b_ ); weighted2PDFSelectedEvents_[i-6] += ( ( (*classifMC)[i] ) * w_ )*( ((*classifMC)[i])*w_);
        weightedPDFSelectedEvents_[i-14]  += (*classifMC)[i] ;  weighted2PDFSelectedEvents_[i-14] += ( (*classifMC)[i] ) * ( ( *classifMC)[i] );
      }

    } 
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



    // Polarization //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if( numbProngs==1 && r > 0.7 ){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),R_STEP2,w_);
      if (terr){
        for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),R_STEP2, errorOnEff);
        if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,OS_STEP2,tauDilCh,errorOnEff); }
      }
      if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,R_STEP2,tauDilCh,w_); }
      fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"R",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  }

  
  if( taucut && oscut && nbtag2 ){
    for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),BTAG2_STEP2,w_); 
    if (terr){
      for(size_t itag=0; itag<evTags.size(); ++itag) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),BTAG2_STEP2, errorOnEff); 
       if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,BTAG2_STEP2,tauDilCh,errorOnEff); }
    }
    if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,BTAG2_STEP2,tauDilCh,w_); } 
    fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"2 btag",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
  }


  //optimization ////////////////////////////////////////////////////////////////
  /*
  int cutIndex;
  for(uint i=0;i<5;i++){ //index running on tau pt
    for(uint j=0;j<5;j++){
      double metThreshold   = 30 + i*5;
      double tauPtThreshold = 20 + j*5;
      int index = i + 5*j;
      bool passMet(false);
      bool passTauPt(false);

      if( metValue > metThreshold   ){ passMet   = true; }
      if( tau_pt   > tauPtThreshold ){ passTauPt = true; }

      if( passMet  && nbtag1 && passTauPt && oscut ){ 
        for(size_t itag=0; itag<evTags.size(); itag++){
          debmon.fillHisto("optSelection",evTags[itag]+TString(" debug"),index,w_);
        }
      }
    }
  }
   */
  ////////////////////////////////////////////////////////////////////////////////

}


// to be removed //   void CutflowAnalyzer::dileptonEventAnalysis(
// to be removed // 					      bool lepReq,
// to be removed // 					    std::vector<PhysicsObject> & v, 
// to be removed // 					    std::vector<PhysicsObject> & muons,     vector<int>  & m_init,  
// to be removed // 					    std::vector<PhysicsObject> & electrons, vector<int>  & e_init,
// to be removed // 					    std::vector<PhysicsObject> & taus,      vector<int>  & t_afterLeptonRemoval, 
// to be removed // 					    std::vector<PhysicsObject> & jets,      vector<int>  & j_final, 
// to be removed // 					    int totalJets, JetCorrectionUncertainty* junc, vector<double> & jerFactors, 
// to be removed // 					    TString myKey, event::MiniEvent_t *ev
// to be removed // 					    ) {
// to be removed //   
// to be removed //   
// to be removed //   TString add("");
// to be removed //   
// to be removed //   
// to be removed //   if(isData_) applybtagweight_ =false;
// to be removed // 
// to be removed //   // only compute uncertainty due to the trigger when others unc are switched off /////////////////
// to be removed //   bool terr(false); if ( jes_== 0 && unc_== 0 && jer_==0 && btagunc_== 0 && unbtagunc_ ) terr=true;
// to be removed //   TString triggErr   = TString("cutflow_triggErr");
// to be removed //   /////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   if( jes_       > 0 ){ add = TString("_plus");       }if( jes_       < 0 ){ add = TString("_minus");      }
// to be removed //   if( unc_       > 0 ){ add = TString("_uncplus");    }if( unc_       < 0 ){ add = TString("_uncminus");   } 
// to be removed //   if( jer_       > 0 ){ add = TString("_jerplus");    }if( jer_       < 0 ){ add = TString("_jerminus");   }
// to be removed //   if( btagunc_   > 0 ){ add = TString("_bplus");      }if( btagunc_   < 0 ){ add = TString("_bminus");     }
// to be removed //   if( unbtagunc_ > 0 ){ add = TString("_unbplus");    }if( unbtagunc_ < 0 ){ add = TString("_unbminus");   }
// to be removed //   
// to be removed //   
// to be removed //   TString evYields          = TString("cutflow_yields")    + add;     
// to be removed //   TString evYieldsMC        = TString("cutflow_yields_mc") + add;   
// to be removed //   TString evYieldsMCTrigger = TString("cutflow_mc_triggErr");
// to be removed //   TString opt("optimization");
// to be removed //   
// to be removed //   SelectionMonitor * myMon    = tauDileptonYieldsMons_[myKey];    // monitor for the event yields
// to be removed //   SelectionMonitor * myMCMon(0);                                  // monitor for the event yields for MC
// to be removed //   
// to be removed //   
// to be removed //   // see if we are processing data, if we are processing MC get channel code ////////////////////////////////
// to be removed //   int input_channel = (int)(*(TVectorD *)ev->eventClassif->At(0))[0];
// to be removed //   int channel(-1); int tauDilCh;
// to be removed //   //bool isSignal(false), isBkg(false);
// to be removed //   if(  !isData_ ){ 
// to be removed //     channel  = codeChannel(input_channel,urlCode_); 
// to be removed //     //    if(channel==MUMU_CH) cout<<endl<<"Channel: MUMU"<<endl; // Ok, they are all there (ttbar)
// to be removed //     //    if(channel==EMU_CH) cout<<endl<<"Channel: EMU"<<endl;
// to be removed //     //    if(channel==EE_CH) cout<<endl<<"Channel: EE"<<endl;
// to be removed //     //    if(channel==ETAU_CH) cout<<endl<<"Channel: ETAU"<<endl;
// to be removed //     //    if(channel==MUTAU_CH) cout<<endl<<"Channel: MUTAU"<<endl;
// to be removed //     if(channel==WQQ_CH || channel==WENU_CH || channel==WTAUNU_CH || channel==WMUNU_CH ){ channel = WJETS_CH; }
// to be removed //     tauDilCh = tdChannel(channel);  
// to be removed //   }
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   //cout<<endl<<" input channel is "<<input_channel<<" out channel is "<<channel<<endl;
// to be removed // 
// to be removed //   
// to be removed //   TString mcTag(""); 
// to be removed //   if( !isData_ ){ mcTag = myKey + TString(" yields mc");  myMCMon = tauDileptonMCYieldsMons_[myKey]; }
// to be removed //   SelectionMonitor& mon    = *myMon;
// to be removed //   SelectionMonitor& mcmon  = *myMCMon;
// to be removed //   SelectionMonitor& debmon = *(tauDileptonDebugMons_[myKey]);
// to be removed // 
// to be removed //   
// to be removed //   // mini tree stats /////////////////////////////////////////////////////////////////
// to be removed //   
// to be removed //   if( myMCMon ){ mcmon.fill2DHisto( evYieldsMC , mcTag, DIL_MINITREE_STEP1, tauDilCh,w_); }  
// to be removed //   
// to be removed //   //cout<<" EvYields is "<<evYields<<" key is "<<(myKey+TString(" lep_tau yields"))<<" step is "<<MINITREE_STEP2<<" weight is "<<w_<<endl;
// to be removed //   
// to be removed //   mon.fillHisto(evYields, myKey+TString(" lep_tau yields"),DIL_MINITREE_STEP2,w_);
// to be removed //   mon.fillHisto(evYields, myKey+TString(" e_tau yields"),  DIL_MINITREE_STEP2,w_); 
// to be removed //   mon.fillHisto(evYields, myKey+TString(" m_tau yields"),  DIL_MINITREE_STEP2,w_);
// to be removed //   ///////////////////////////////////////////////////////////////////////////////////
// to be removed //   
// to be removed //   
// to be removed //   
// to be removed //   
// to be removed //   //debug
// to be removed //   //if(evType_ == ETAU_) cout<<endl<<" ETAU input at minitree step "<<endl;   
// to be removed //   
// to be removed //   // >1 leptons veto disabled REENABLE CONSISTENT WAY
// to be removed //   //  if( ! lepReq ){ return; }
// to be removed //   
// to be removed //   //debug
// to be removed //   //if(evType_ == ETAU_) cout<<endl<<" pass dilepton and loose veto cuts 2) "<<tauDilCh<<endl;
// to be removed // 
// to be removed // 
// to be removed //   // in case we are processing MC see if we require a specific channel //////////////////////////////
// to be removed //   // TODO INCLUDE TTBAR LIKE EMU_???????
// to be removed //   // FIXME: include in channels the etau and mutau and mumu and emu. LoL.
// to be removed //   if( !isData_ ){
// to be removed //     // tau dilepton analysis //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     if(  ttbarLike_ == ETAU_  && channel != ETAU_CH  )                                                                             { return; }
// to be removed //     else if(  ttbarLike_ == MUTAU_ && channel != MUTAU_CH )                                                                             { return; }
// to be removed //     else if(  ttbarLike_ == MUMU_ && channel != MUMU_CH )                                                                             { return; }
// to be removed //     else if(  ttbarLike_ == EMU_ && channel != EMU_CH )                                                                             { return; }
// to be removed //     //else if(  ttbarLike_ == TTBAR_MCBKG_ && ( channel == MUMU_CH || channel == EMU_CH || channel ==ETAU_CH || channel == MUTAU_CH || channel == EJETS_CH || channel== MUJETS_CH ) ){ return; }
// to be removed //     else if(  ttbarLike_ == TTBAR_MCBKG_ && ( channel ==ETAU_CH || channel == MUTAU_CH || channel == EJETS_CH || channel== MUJETS_CH ) ){ return; }
// to be removed //     else if(  ttbarLike_ == TTBAR_DDBKG_ && (channel != EJETS_CH && channel!= MUJETS_CH ) )                                             { return; }
// to be removed //     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     
// to be removed //     // FIXME: remove personalization
// to be removed //     checkleptonsFromTauDecay_ = false;
// to be removed //     if( checkleptonsFromTauDecay_ ){
// to be removed //       event::MiniEvent_t *evmc = evRMC_->GetNewMiniEvent(i_,"mc");  if( evmc == 0 ){ cout<<"\n empty event"<<endl; return;}
// to be removed //       std::vector<PhysicsObject> mctausColl = evRMC_->GetPhysicsObjectsFrom(evmc,event::CHLEPTON);
// to be removed //       //get the charged lepton from tau //////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //       std::vector<PhysicsObject> mcleps;  mcleps.clear();
// to be removed //       for(size_t igtau = 0; igtau < mctausColl.size(); igtau++){
// to be removed // 	if(fabs(mctausColl[igtau][1]) != double(11) && fabs(mctausColl[igtau][1]) != double(13)) continue; // if it is not electron or muon
// to be removed // 	if(mctausColl[igtau].Pt() < 20 || fabs(mctausColl[igtau].Eta()) > 2.1) continue;
// to be removed //   
// to be removed // 	//cout<<"\n debug we have found a lepton parent is : "<<(fabs(mctausColl[igtau][2]))<<endl;
// to be removed // 	
// to be removed // 	if(fabs(mctausColl[igtau][2]) != double(15)) continue; // check if lepton is from a tau decay
// to be removed // 	mcleps.push_back(mctausColl[igtau]);
// to be removed //       }
// to be removed //       if( !( mcleps.size() == 1 && mcleps[0].Pt() > 20) )return ;
// to be removed //       /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     }
// to be removed //     
// to be removed //   }
// to be removed //   ///////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   
// to be removed //   // Fill debug info ///////////////////////////////////////////////////
// to be removed //   fillDebugHistograms(myKey,m_init,muons,e_init,electrons,j_final,jets); 
// to be removed //   //////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   
// to be removed //   uint metAlgo;
// to be removed //   if( myKey=="PF"               ) { metAlgo=event::PF;        }
// to be removed //   else if( myKey.Contains("TaNC")    ) { metAlgo=event::PF;        }
// to be removed //   else if( myKey.Contains("HPS")     ) { metAlgo=event::PF;        } 
// to be removed //   else if( myKey.Contains("PFlow")   ) { metAlgo=event::PFLOWMET;  } 
// to be removed //   
// to be removed //   PhysicsObjectCollection mets = evR_->GetPhysicsObjectsFrom(ev,event::MET,metAlgo);           
// to be removed //   if(mets.size()==0) { cout << "No met available for " << myKey <<" analyis type ! "<< endl;  return;}
// to be removed //   PhysicsObject met = mets[0]; 
// to be removed // 
// to be removed //   // finding leading and subleading lepton and qualify tag ////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   double lepton_pt(0); double lepton_charge(0); uint lepton_ind(0);
// to be removed //   double lepton2_pt(0); double lepton2_charge(0); uint lepton2_ind(0);
// to be removed //   
// to be removed //   vector<int> * p_i ; vector<PhysicsObject> * p_obj; PhysicsObject * lep_obj;
// to be removed //   PhysicsObject* lep2_obj;
// to be removed //   if     ( evType_ == MUTAU_ ){ p_i = & m_init ; p_obj = &muons;    }
// to be removed //   else if( evType_ == ETAU_  ){ p_i = & e_init ; p_obj = &electrons;}
// to be removed //   else if( evType_ == MUMU_ ){  p_i = & m_init ; p_obj = &muons;    }
// to be removed //   else if( evType_ == EMU_  ){ p_i = & e_init ; p_obj = &electrons; for(unsigned int si=0; si<m_init.size(); ++si){ (*p_i).push_back(m_init[si]); (*p_obj).push_back(muons[si]); }}
// to be removed //   
// to be removed //   
// to be removed //   for(uint myi=0; myi < (*p_i).size(); myi++){
// to be removed //     int ind = (*p_i)[myi]; double lPt = TMath::Abs((*p_obj)[ind].Pt()); double charge = (*p_obj)[ind][0]; PhysicsObject * tmp_obj= &( (*p_obj)[ind] );
// to be removed //     if( (*p_i).size() > 1 && lPt > lepton2_pt && !( lPt>lepton_pt ) ){ lepton2_pt = lPt; lepton2_charge = charge; lepton2_ind = ind; lep2_obj = tmp_obj;}                             
// to be removed //     if( lPt > lepton_pt ){ lepton_pt = lPt; lepton_charge = charge; lepton_ind = ind; lep_obj = tmp_obj;}                                   
// to be removed //   }
// to be removed //   
// to be removed //   std::vector<TString> evTags; 
// to be removed //   if     (evType_ == ETAU_  ){ evTags.push_back(myKey+TString(" lep_tau"));  evTags.push_back(myKey+TString(" e_tau")); }
// to be removed //   else if(evType_ == MUTAU_ ){ evTags.push_back(myKey+TString(" lep_tau")); evTags.push_back(myKey+TString(" m_tau"));  }
// to be removed //   else if(evType_ == MUMU_  ){ evTags.push_back(myKey+TString(" mu_mu"));  evTags.push_back(myKey+TString(" mu_mu")); }
// to be removed //   else if(evType_ == EMU_ )  { evTags.push_back(myKey+TString(" e_mu")); evTags.push_back(myKey+TString(" e_mu"));  }
// to be removed //   
// to be removed //   
// to be removed //   //  double metValue = jetMETScaling( jerFactors, jes_, junc , jets ,met.Px(), met.Py());
// to be removed //   double metValue = jetMETScalingTest( jerFactors, jes_, junc, jets  , met);
// to be removed //   // rescaling of met based on unclustered energy ////////////////////////////////////////////////
// to be removed //   //  if( unc_ ){ metValue = jetMETUnclustered( jerFactors, lep_obj, unc_, jets, met.Px(), met.Py());}
// to be removed //   //  if( jer_ ){ metValue = jetMETResolution( jerFactors, jets, met.Px(), met.Py());}
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   
// to be removed //   double tau_charge(0);// , tau_pt(0); // unused
// to be removed //   int tau_i;
// to be removed //   PhysicsObject * tau_obj;
// to be removed //   if(t_afterLeptonRemoval.size() == 1){ tau_i = t_afterLeptonRemoval[0]; tau_charge = taus[tau_i][0];
// to be removed //     // tau_pt = taus[tau_i].Pt(); // unused
// to be removed //     tau_obj = &(taus[tau_i]); }
// to be removed //   
// to be removed //   
// to be removed //   // lepton requirement includes trigger selection requirement ( the two highest pt jets should fire the trigger ) //////////////////////////////////
// to be removed //   // on MC   : we apply a trigger efficiency
// to be removed //   // on data : we require that the two highest pt jet should fire the trigger (returned eff from the method is either 0/1)
// to be removed //   double errorOnEff(0); pair<double,double> eff(0,0);
// to be removed //   
// to be removed // 
// to be removed //   if( eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETSPLUSMET_ ){
// to be removed //     pair<double,double> efftemp( getEfficiencyAndError( jets,j_final,junc,jerFactors));
// to be removed //     eff.first = efftemp.first; eff.second = efftemp.second;
// to be removed // 
// to be removed //     if ( eff.first == 0 ) return;
// to be removed //     else{ 
// to be removed //       electrontriggerefficiency_ = eff.first;  
// to be removed //       if( !isData_ ){ leptontriggerefficiency_ = electrontriggerefficiency_; w_= w_*leptontriggerefficiency_; errorOnEff =  w_*(eff.second); } // get trigger efficiency for electrons 
// to be removed // 
// to be removed //       // take into account weights into efficiency error
// to be removed //     }
// to be removed //   }
// to be removed //   else if (eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_ ) { if( !isData_ )leptontriggerefficiency_ = electrontriggerefficiency_;}
// to be removed //   else{                                                      if( !isData_ )leptontriggerefficiency_ = muontriggerefficiency_;    }
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   // events with 1 lepton /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   for( size_t itag=0; itag<evTags.size();  itag++ ) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),DIL_LEP_STEP2,w_); 
// to be removed //   if (terr){
// to be removed //     for( size_t itag=0; itag<evTags.size(); itag++ ) mon.fillHisto(triggErr , evTags[itag]+TString(" yields"),DIL_LEP_STEP2,errorOnEff);
// to be removed //     if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_LEP_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon ){  mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_LEP_STEP2,tauDilCh,w_); }
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"init",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);
// to be removed //   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   // events with at least 2 leptons (replace 1 jet) //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if((*p_i).size() <2 ) return;
// to be removed //   //  if(j_final.size()==0) return;
// to be removed //   for(size_t itag=0; itag<evTags.size() ; itag++) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),DIL_JET1_STEP2,w_); 
// to be removed //   
// to be removed //   if (terr){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),DIL_JET1_STEP2,errorOnEff);
// to be removed //     if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_JET1_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_JET1_STEP2,tauDilCh,w_); }
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 2 leptons",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);
// to be removed //   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   int firstLepInd = (*p_i)[0];
// to be removed //   int secondLepInd= (*p_i)[1];
// to be removed //   PhysicsObject dilL1 = (*p_obj)[firstLepInd];
// to be removed //   PhysicsObject dilL2 = (*p_obj)[secondLepInd];
// to be removed //   TLorentzVector dilVec(dilL1+dilL2);
// to be removed //   double dileptonMass = dilVec.M();
// to be removed //   
// to be removed //   // events with mass outside of Z range and higher that 12GeV/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if( dileptonMass < 12 ){ return; } // Low dilepton mass resonances
// to be removed //   if( fabs( dileptonMass - 91) < 15  ){ return; } // Z mass window
// to be removed //   
// to be removed //   for(size_t itag=0; itag<evTags.size();  itag++) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"), DIL_MT_STEP2,w_);  
// to be removed //   if (terr){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),DIL_MT_STEP2,errorOnEff);
// to be removed //     if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_MT_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_MT_STEP2,tauDilCh,w_);}
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"Z and low mass veto",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);
// to be removed //   
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   
// to be removed // 
// to be removed //   // events with at least 2 jets///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if(j_final.size()<2 ) return;                                       
// to be removed //   for(size_t itag=0; itag<evTags.size();  itag++) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),DIL_JET2_STEP2,w_);  
// to be removed //   if (terr){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),DIL_JET2_STEP2,errorOnEff);
// to be removed //     if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_JET2_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon ){  mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_JET2_STEP2,tauDilCh, w_);}
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 2 jet",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);
// to be removed //   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   //trigger efficiencies studies on data (MET part) /////////////////////////////////////////////////////////////////////////////////////
// to be removed //   // /lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/Ele_MetTrig_X.root
// to be removed //   // 1) Events selected with trigger : HLT_EleY_CaloIdVT_TrkIdT_CentralJet30_CentralJet25_vX 
// to be removed //   // HLT_EleY_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15_vX is placed at the 3rd position of trigger information.
// to be removed //   // These data is for measuring only the MHT part of trigger efficiency.
// to be removed //   if(isData_ && trigEffStudy_ ){
// to be removed // 
// to be removed //     //trigger /////////////////////////////////////////
// to be removed //     TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
// to be removed //     bool hasTesttrig = ((*trig)[3]>0); 
// to be removed //     ///////////////////////////////////////////////////
// to be removed //  
// to be removed //     //overall trigger efficiency
// to be removed //     for(int mymetcut = 5; mymetcut<150; mymetcut++){
// to be removed //       if( metValue > mymetcut ){
// to be removed // 
// to be removed //         //denominator
// to be removed //         debmon.fillHisto("den_eff_trigger_met", myKey+TString(" e_tau debug"), mymetcut);
// to be removed //  
// to be removed //         if(hasTesttrig){
// to be removed //           //numerator
// to be removed //           debmon.fillHisto("num_eff_trigger_met", myKey+TString(" e_tau debug"), mymetcut);
// to be removed //         }
// to be removed // 
// to be removed // 
// to be removed //         if( j_final.size() + totalJets >= 3 ){
// to be removed //           //denominator
// to be removed //           debmon.fillHisto("den_eff_trigger_met3j", myKey+TString(" e_tau debug"), mymetcut);
// to be removed //  
// to be removed //           if(hasTesttrig){
// to be removed //             //numerator
// to be removed //             debmon.fillHisto("num_eff_trigger_met3j", myKey+TString(" e_tau debug"), mymetcut);
// to be removed //           }
// to be removed // 
// to be removed //         }
// to be removed // 
// to be removed //       }
// to be removed //     }
// to be removed // 
// to be removed //     // absolute efficiencies
// to be removed //     debmon.fillHisto("abs_den_eff_trigger_met", myKey+TString(" e_tau debug"), metValue);
// to be removed //  
// to be removed //     // numerator
// to be removed //     if(hasTesttrig){ debmon.fillHisto("abs_num_eff_trigger_met", myKey+TString(" e_tau debug"), metValue ); }
// to be removed // 
// to be removed //   
// to be removed //     if( j_final.size() + totalJets >= 3){
// to be removed //       // absolute efficiencies
// to be removed //       debmon.fillHisto("abs_den_eff_trigger_met3j", myKey+TString(" e_tau debug"), metValue);
// to be removed //  
// to be removed //       // numerator
// to be removed //       if(hasTesttrig){ debmon.fillHisto("abs_num_eff_trigger_met3j", myKey+TString(" e_tau debug"), metValue ); }
// to be removed // 
// to be removed //     }
// to be removed // 
// to be removed //   }
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   //trigger efficiencies studies on data (JET part) /////////////////////////////////////////////////////////////////////////////////////
// to be removed //   // /lustre/data3/cmslocal/samples/CMSSW_4_2_X/data/mTrees-v3/Ele_JetMetTrig_X.root
// to be removed //   // 2) Events selected with trigger : HLT_Ele32_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_vX
// to be removed //   // HLT_EleY_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15_vX is placed at the 3rd position of trigger information.
// to be removed //   // These data is to be used for measuring combined Jet+MHT part of trigger efficiency from which we can extract jet efficiency part.
// to be removed //   // The files  4 and 5 have
// to be removed //   // HLT_Ele17_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT15
// to be removed //   // Files 6, 7, 8 and 9 have
// to be removed //   // HLT_Ele22_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT20
// to be removed //   // File 10 have HLT_Ele27_CaloIdVT_CaloIsoT_TrkIdT_TrkIsoT_CentralJet30_CentralJet25_PFMHT20
// to be removed //   // They correspond to different run numbers and different trigger
// to be removed //   // thresholds. So the efficiency should be measured separately.
// to be removed //   // NOTE jet cut should be at 20 !!!!
// to be removed //   //
// to be removed //   // note : the tau is removed from the trigger list  
// to be removed //   //
// to be removed //   if(isData_ && trigEffStudy_ && metValue > 45 ){
// to be removed //     
// to be removed //     //trigger /////////////////////////////////////////
// to be removed //     //    TVectorD *trig = (TVectorD *)ev->triggerColl->At(0); // unused
// to be removed //     //    bool hasTesttrig = ((*trig)[3]>0);  // unused
// to be removed //     ///////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //  
// to be removed //     /*
// to be removed //     for(int jetPtCut = 20;jetPtCut<100; jetPtCut++){
// to be removed // 
// to be removed //       int numbjets(0);     
// to be removed //       for(uint i=0; i< jetsForTrigger_.size(); i++){
// to be removed //         int ind=jetsForTrigger_[i];
// to be removed //         double jetPt =  getJetPt( jets[ind], junc,0, jes_);
// to be removed //         bool triggeredJet = jets[ind][4];
// to be removed //         if(jetPt>jetPtCut && triggeredJet ) numbjets++;
// to be removed //       }
// to be removed //       if( numbjets>1 ){
// to be removed //    
// to be removed //         //denominator
// to be removed //         debmon.fillHisto("den_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPtCut);
// to be removed // 
// to be removed //  
// to be removed //         if(hasTesttrig){
// to be removed //           //denominator
// to be removed //           debmon.fillHisto("num_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPtCut);
// to be removed //         }
// to be removed //       }
// to be removed //     }
// to be removed //     */
// to be removed // 
// to be removed //     // absolute efficiencies get the 2 most energetic jet ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     double jetPt1(0),jetPt2(0); int ind2(-1), ind1(-1);
// to be removed //     for(uint i=0; i<j_final.size(); i++){
// to be removed //       int ind=j_final[i];
// to be removed //       double jetPt =  getJetPt( jets[ind], junc,0, jes_);
// to be removed //       if( jetPt > jetPt1       ){ jetPt2=jetPt1; ind2=ind1; jetPt1=jetPt; ind1=ind; }
// to be removed //       else if( jetPt > jetPt2  ){ jetPt2=jetPt;  ind2=ind;                          }
// to be removed //     }
// to be removed //     
// to be removed //     if(jetPt2){
// to be removed //       //denominator
// to be removed //       debmon.fillHisto("abs_den_eff_trigger_jetptcut1", myKey+TString(" e_tau debug"), jetPt1);
// to be removed //       debmon.fillHisto("abs_den_eff_trigger_jetptcut2", myKey+TString(" e_tau debug"), jetPt2);
// to be removed // 
// to be removed //       //if(hasTesttrig){
// to be removed // 
// to be removed //         //if(jetPt2>20){ cout<<" jetPt is : "<<jetPt2<<" triggeredJet2 is : "<<(jets[ind2][4])<<endl;}
// to be removed // 
// to be removed //         //numerator
// to be removed //         double triggeredJet1 = jets[ind1][4];  // most energetic jet
// to be removed //         double triggeredJet2 = jets[ind2][4];  // second most energetic jet
// to be removed // 
// to be removed //         //if( triggeredJet1 && triggeredJet2 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut", myKey+TString(" e_tau debug"), jetPt2 ); }
// to be removed // 
// to be removed //         if( triggeredJet1 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut1", myKey+TString(" e_tau debug"), jetPt1 ); }
// to be removed //         if( triggeredJet2 ){ debmon.fillHisto("abs_num_eff_trigger_jetptcut2", myKey+TString(" e_tau debug"), jetPt2 ); }
// to be removed // 
// to be removed //       //}
// to be removed //     }
// to be removed //     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   }
// to be removed // 
// to be removed // 
// to be removed //   // trigger efficiencies studies ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if( evType_==ETAU_ && trigEffStudy_ && t_afterLeptonRemoval.size()<2 && jes_==0){
// to be removed // 
// to be removed //     int jmult = t_afterLeptonRemoval.size() + j_final.size();
// to be removed // 
// to be removed //     //trigger ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     bool trigger(false); TVectorD *trig = (TVectorD *)ev->triggerColl->At(0); bool hasEGtrig = ((*trig)[1]>0); if( hasEGtrig ){ trigger =true;}
// to be removed //     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //     // efficiencies on electron + tau + jets channels //////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     if( channel == ETAU_CH && t_afterLeptonRemoval.size()== 1 ){
// to be removed // 
// to be removed //       // get the highest third pt jet /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //       double jetPtMax1(0),jetPtMax2(0),jetPtMax3(0),jetEtaPtMax1(0),jetEtaPtMax2(0),jetEtaPtMax3(0);
// to be removed //       for(uint j_i=0; j_i<j_final.size(); j_i++){
// to be removed //         int ind = j_final[j_i];
// to be removed //         double jetPt =  getJetPt( jets[ind], junc,0, jes_); double jetEta = fabs( jets[ind].Eta());
// to be removed //         
// to be removed //         if     ( jetPt > jetPtMax1 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPtMax1; jetEtaPtMax2=jetEtaPtMax1; jetPtMax1=jetPt; jetEtaPtMax1=jetEta; }
// to be removed //         else if( jetPt > jetPtMax2 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPt;     jetEtaPtMax2=jetEta;                                             }
// to be removed //         else if( jetPt > jetPtMax3 ){jetPtMax3=jetPt;     jetEtaPtMax3 = jetEta;                                                                                             }
// to be removed //         
// to be removed //       }
// to be removed //       if(t_afterLeptonRemoval.size()==1){ // this condition was already verified...
// to be removed //         //jer assumed to be 1
// to be removed //         int ind = t_afterLeptonRemoval[0];
// to be removed //         double jetPt = taus[ind][7]; double jetEta = taus[ind][8]; 
// to be removed //         junc->setJetEta(jetEta); junc->setJetPt(jetPt); double corr=junc->getUncertainty(true); 
// to be removed //         jetPt = (1+corr)*jetPt;
// to be removed //         //if we have only 2 jets in the event....
// to be removed //         if(jetPtMax3==0){ 
// to be removed //           if(jetPt<jetPtMax2){    jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
// to be removed //           else{                   jetPtMax3=jetPtMax2; jetEtaPtMax3=jetEtaPtMax2; }
// to be removed //         }
// to be removed //         else if(jetPt<jetPtMax3){ jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
// to be removed //         
// to be removed //       }
// to be removed //       //////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //       debmon.fillHisto("jmult_den_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
// to be removed //       debmon.fillHisto("jmult_den_pt",  myKey+TString(" e_tau debug"), jetPtMax3);
// to be removed //       debmon.fillHisto("jmult_den",     myKey+TString(" e_tau debug"), jmult,1);
// to be removed // 
// to be removed //       if(trigger){  
// to be removed //         debmon.fillHisto("jmult_num_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
// to be removed //         debmon.fillHisto("jmult_num_pt",  myKey+TString(" e_tau debug"), jetPtMax3); 
// to be removed //         debmon.fillHisto("jmult_num",     myKey+TString(" e_tau debug"), jmult,1);
// to be removed //       }
// to be removed // 
// to be removed //     }
// to be removed // 
// to be removed //     // efficiencies on electron + jets
// to be removed //     if(channel == WJETS_CH && jmult >=3 ){
// to be removed // 
// to be removed //       // get the highest third pt jet /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //       double jetPtMax1(0),jetPtMax2(0),jetPtMax3(0),jetEtaPtMax1(0),jetEtaPtMax2(0),jetEtaPtMax3(0);
// to be removed //       for(uint j_i=0; j_i<j_final.size(); j_i++){
// to be removed //         int ind = j_final[j_i];
// to be removed //         double jetPt   =  getJetPt( jets[ind], junc,0, jes_); double jetEta  = fabs( jets[ind].Eta());
// to be removed //         
// to be removed //         if     ( jetPt > jetPtMax1 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPtMax1; jetEtaPtMax2=jetEtaPtMax1; jetPtMax1=jetPt; jetEtaPtMax1=jetEta;}
// to be removed //         else if( jetPt > jetPtMax2 ){jetPtMax3=jetPtMax2; jetEtaPtMax3 = jetEtaPtMax2; jetPtMax2=jetPt;     jetEtaPtMax2=jetEta;                                            }
// to be removed //         else if( jetPt > jetPtMax3 ){jetPtMax3=jetPt;     jetEtaPtMax3 = jetEta;                                                                                            }
// to be removed //         
// to be removed //       }
// to be removed //       if(t_afterLeptonRemoval.size()==1){
// to be removed //         //jer assumed to be 1
// to be removed //         int ind = t_afterLeptonRemoval[0];
// to be removed //         double jetPt = taus[ind][7]; double jetEta = taus[ind][8]; 
// to be removed //         junc->setJetEta(jetEta); junc->setJetPt(jetPt); double corr=junc->getUncertainty(true); 
// to be removed //         jetPt = (1+corr)*jetPt;
// to be removed //         //if we have only 2 jets in the event....
// to be removed //         if(jetPt<jetPtMax3){ jetPtMax3=jetPt;     jetEtaPtMax3=fabs(jetEta); }
// to be removed //         
// to be removed //       }
// to be removed //       //////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //       debmon.fillHisto("jmult_den_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3 );
// to be removed //       debmon.fillHisto("jmult_den_pt",  myKey+TString(" e_tau debug"), jetPtMax3    );
// to be removed //       debmon.fillHisto("jmult_den",     myKey+TString(" e_tau debug"), jmult,1      );
// to be removed // 
// to be removed //       if(trigger){  
// to be removed //         debmon.fillHisto("jmult_num_eta", myKey+TString(" e_tau debug"), jetEtaPtMax3);
// to be removed //         debmon.fillHisto("jmult_num_pt",  myKey+TString(" e_tau debug"), jetPtMax3   ); 
// to be removed //         debmon.fillHisto("jmult_num",     myKey+TString(" e_tau debug"), jmult,1);
// to be removed //       }
// to be removed // 
// to be removed //     }
// to be removed //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   }
// to be removed //   ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   // events with at least 2 jets+1 jet>20GeV ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   // number of jets above jet pt cut and extra jet between tau pt and jet pt cut 
// to be removed //   if( j_final.size() + totalJets <3) return;
// to be removed //   for(size_t itag=0; itag<evTags.size();  itag++) mon.fillHisto(evYields ,evTags[itag]+TString(" yields"),    DIL_JET3_STEP2,w_);  
// to be removed //   if (terr){ 
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_JET3_STEP2,errorOnEff);  
// to be removed //     if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_JET3_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_JET3_STEP2,tauDilCh,w_);}
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"#geq 3 jet",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   // events passing MET cut //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if( metValue < MET_CUT_) return;
// to be removed //   for(size_t itag=0; itag<evTags.size();  itag++) mon.fillHisto(evYields,  evTags[itag]+TString(" yields"),    DIL_MET_STEP2,w_);  
// to be removed //   if (terr){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr, evTags[itag]+TString(" yields"),DIL_MET_STEP2,errorOnEff);
// to be removed //     if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_MET_STEP2,tauDilCh,errorOnEff); }
// to be removed //   }
// to be removed //   if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_MET_STEP2,tauDilCh,w_);}
// to be removed //   fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"MET",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue);  
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //   //testMe_+=errorOnEff2; testMe_Nev_++;
// to be removed //   //cout<<endl<<" Deb :  eff  = "<<setprecision (5)<<(eff.first)<<" +- "<<(eff.second)<<" errorOnEff2  = "<<errorOnEff2<<" total is "<<testMe_<<" in N events : "<<testMe_Nev_<<endl;
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   // New way to deal with btag uncertainies //////////////////
// to be removed //   int nbtags_taus(0);
// to be removed //   bool nbtag1(false),  nbtag2(false); // ENABLED BTAGGING CUT
// to be removed //   ////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   // new way to compute btag uncertainty //////////////////////////////////////////
// to be removed //   if( isData_ || !  applybtagweight_){
// to be removed //     for(uint j=0; j<j_final.size(); j++){  
// to be removed //       int j_ind = j_final[j]; 
// to be removed //       double btagvalue = jets[j_ind][BTAGIND_] ;
// to be removed //       if( btagvalue>BTAG_CUT_){nbtags_taus++;} 
// to be removed //     }
// to be removed //   }
// to be removed //   else {
// to be removed //     for(uint j=0; j<j_final.size(); j++){  
// to be removed //       int    j_ind     = j_final[j]; 
// to be removed //       double btagvalue = jets[j_ind][BTAGIND_];
// to be removed //       bool   isTagged = false;
// to be removed //       double bjpt(jets[j_ind].Pt()), bjeta(jets[j_ind].Eta()); 
// to be removed // 
// to be removed //       if( btagvalue > BTAG_CUT_) isTagged = true;
// to be removed // 
// to be removed //       double newBTagSF     = getSFb(bjpt, BTAGIND_);
// to be removed //       double newLightJetSF = getSFlight(bjpt, bjeta, BTAGIND_);
// to be removed //       
// to be removed // 
// to be removed //       // get flavour of the jet
// to be removed // 
// to be removed //       int jet_flavor = TMath::Abs(jets[j_ind][jetpgid_]);
// to be removed //       double err_newBTagSF = err_BTagSF_;
// to be removed // 
// to be removed //       if(jet_flavor == PGID_C ){err_newBTagSF =2*err_BTagSF_;}
// to be removed // 
// to be removed //       if(btagunc_   > 0){ newBTagSF     = BTagSF_+ err_newBTagSF; }
// to be removed //       else              { newBTagSF     = BTagSF_- err_newBTagSF; }
// to be removed // 
// to be removed //       if(unbtagunc_ > 0){ newLightJetSF = LightJetSF_ + err_LightJetSF_;}
// to be removed //       else              { newLightJetSF = LightJetSF_ - err_LightJetSF_;}
// to be removed // 
// to be removed //       //      double BTagEff     = newBTagSF*BTAG_eff_R_;
// to be removed //       //      double LightJeteff = newLightJetSF*BTAG_eff_F_;
// to be removed // 
// to be removed //       double jet_phi = jets[j_ind].Phi();
// to be removed // 
// to be removed //       double sin_phi = sin(jet_phi*1000000);
// to be removed //       double seed = abs(static_cast<int>(sin_phi*100000));
// to be removed // 
// to be removed //       //Initialize class
// to be removed //       BTagSFUtil btsfutil(seed);
// to be removed //     
// to be removed //       //modify tags
// to be removed //       btsfutil.modifyBTagsWithSF(isTagged, jet_flavor, newBTagSF, /*BTagEff*/ BTAG_eff_R_, newLightJetSF, /*LightJeteff*/ BTAG_eff_F_);
// to be removed // 
// to be removed //       if(isTagged) nbtags_taus++;
// to be removed // 
// to be removed //     }
// to be removed //   }
// to be removed //   if(nbtags_taus > 0 ) nbtag1=true;
// to be removed //   if(nbtags_taus > 1)  nbtag2=true;
// to be removed //   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   //cout<<endl<<"tau dilepton event analysis H"<<endl;
// to be removed //   //taus ////////////////////////////////////////////
// to be removed //   bool taucut(false);
// to be removed //   if( t_afterLeptonRemoval.size()== 1 ) taucut =true;
// to be removed // 
// to be removed //   int numbProngs(0);
// to be removed //   double tauP(0),tauE,r(0);
// to be removed //   tauR_ = -1;
// to be removed // 
// to be removed //   if( taucut ){ 
// to be removed //     numbProngs = GetNumbProngs(taus[tau_i]);
// to be removed //     tauP = taus[tau_i][16];  // lead charged hadron P
// to be removed //     tauE = taus[tau_i].E();  // tau energy
// to be removed //     if(tauE){ r= tauP/tauE; tauR_ = r;} // tau polarization
// to be removed //     
// to be removed //   }
// to be removed //   ///////////////////////////////////////////////////
// to be removed //    
// to be removed // 
// to be removed //   //cout<<endl<<"tau dilepton event analysis I"<<endl;
// to be removed //   //Opposite sign //////////////////////////////////
// to be removed //   bool oscut(false);
// to be removed //   if( lepton_charge * tau_charge < 0 ) oscut = true; 
// to be removed //   if(oscut) is_os_ = 1.;
// to be removed //   //////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   // 1 btag option ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if( nbtag1 ) {
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),    DIL_BTAG1_STEP2, w_); 
// to be removed //     if (terr){
// to be removed //       for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_BTAG1_STEP2, errorOnEff);  
// to be removed //       if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_BTAG1_STEP2,tauDilCh,errorOnEff); }
// to be removed //     }
// to be removed //     if( myMCMon){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_BTAG1_STEP2,tauDilCh,w_); } 
// to be removed //     fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"1 btag",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
// to be removed //   }
// to be removed //   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   // TAU ID selection step //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //   if(nbtag1 && taucut){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),    DIL_TAU_STEP2,w_);  
// to be removed //     if (terr){
// to be removed //       for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_TAU_STEP2, errorOnEff); 
// to be removed //       if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_TAU_STEP2,tauDilCh,errorOnEff); }
// to be removed //     }
// to be removed // 
// to be removed //     if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_TAU_STEP2,tauDilCh,w_); }
// to be removed //     fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"1 Tau",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
// to be removed // 
// to be removed //     
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++){ 
// to be removed //       //Build Tree quantites based on tau jet 
// to be removed //       PhysicsObject tauJet; 
// to be removed //       double tauJetPt  = (*tau_obj)[7];
// to be removed //       double tauJetEta = (*tau_obj)[8];
// to be removed //       double tauJetPhi = (*tau_obj)[9];
// to be removed //       tauJet.SetPtEtaPhiE(tauJetPt,tauJetEta,tauJetPhi,tauJetPt);
// to be removed //       tauJetPt =  getJetPt( tauJet, junc, 0, jes_); //correct pt
// to be removed //       tauJet.SetPtEtaPhiE(tauJetPt,tauJetEta,tauJetPhi,tauJetPt);
// to be removed //       double tauJetRadius(0); //Warning this needs to be changed (not needed)
// to be removed //       TString treeOption("Selected"); treeOption += add;
// to be removed //       fillTauDileptonObjTree(ev,treeOption,evTags[itag],junc,mets[0],tauDilCh,TString("1 Tau"),lep_obj,tauJetRadius, tau_obj, &(taus[tau_i]), j_final, jets, jerFactors, metValue,nbtags_taus);
// to be removed //     }
// to be removed //   }
// to be removed //   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //    // SPY events for the w+jet and ttbar other contribution//////////////////////////////////////////////
// to be removed //    //if(  (!applybtagweight_  && nbtag1 && taucut ) ||  (applybtagweight_ && taucut )  ){
// to be removed //    //    if(jes_== 0 && unc_== 0 && jer_== 0 && btagunc_==0 && pu_ == NOPU ) listOfReaders_[0]->SpyEvent();
// to be removed //    //}
// to be removed //    ///////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //   if(  nbtag1 && taucut && oscut ){
// to be removed // 
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),DIL_OS_STEP2,w_);
// to be removed //     if (terr){
// to be removed //       for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_OS_STEP2, errorOnEff);
// to be removed //       if( myMCMon ){ mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_OS_STEP2,tauDilCh,errorOnEff); }
// to be removed //     }
// to be removed // 
// to be removed //     if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_OS_STEP2,tauDilCh,w_); }
// to be removed //     fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"OS",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
// to be removed // 
// to be removed //     //SPY AFTER TAU ID AND OS /////////////////////////////////////////////////////////////////////////
// to be removed //     //if(jes_== 0 && unc_== 0 && jer_== 0 && btagunc_==0 && pu_ == NOPU ) listOfReaders_[0]->SpyEvent();
// to be removed //     ///////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed //    
// to be removed //     // PDF Uncertainties ////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     // fill weighted events and reset selected events vector weightsMS Emeri
// to be removed //     if( !isData_ && pdfweights_ && !sys_){
// to be removed //       TVectorD * classifMC(0);
// to be removed //       //PDFSelectedEvents_ += (w_); 
// to be removed //       PDFSelectedEvents_ += 1.;
// to be removed //       event::MiniEvent_t *evMC = evRMC_->GetNewMiniEvent(i_,"mc");  if( evMC == 0 ){ cout<<"\n empty event"<<endl; return;}
// to be removed //       classifMC = (TVectorD *)evMC->eventClassif->At(0);
// to be removed //   
// to be removed //       for(int i=14; i<classifMC->GetNoElements();i++ ){
// to be removed //         cout.precision(3);
// to be removed //         myWeights_[i-14] += (*classifMC)[i]/(*classifMC)[14];        
// to be removed //         cout<<endl<<" sel. Evnt :"<<i_<<", (i-14) = "<<(i-14)<<" w : "<<(*classifMC)[i]<<" rel w : "<<((*classifMC)[i]/(*classifMC)[i-14])<<" total w : "<<myWeights_[i-14];
// to be removed //         //weightedPDFSelectedEvents_[i-6]  += ( ( (*classifMC)[i] ) * w_ * w1b_ ); weighted2PDFSelectedEvents_[i-6] += ( ( (*classifMC)[i] ) * w_ )*( ((*classifMC)[i])*w_);
// to be removed //         weightedPDFSelectedEvents_[i-14]  += (*classifMC)[i] ;  weighted2PDFSelectedEvents_[i-14] += ( (*classifMC)[i] ) * ( ( *classifMC)[i] );
// to be removed //       }
// to be removed // 
// to be removed //     } 
// to be removed //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed // 
// to be removed //     // Polarization //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed //     if( numbProngs==1 && r > 0.7 ){
// to be removed //       for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),DIL_R_STEP2,w_);
// to be removed //       if (terr){
// to be removed //         for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_R_STEP2, errorOnEff);
// to be removed //         if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_OS_STEP2,tauDilCh,errorOnEff); }
// to be removed //       }
// to be removed //       if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_R_STEP2,tauDilCh,w_); }
// to be removed //       fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"R",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
// to be removed //     }
// to be removed //     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // 
// to be removed //   }
// to be removed // 
// to be removed //   
// to be removed //   if( taucut && oscut && nbtag2 ){
// to be removed //     for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(evYields,evTags[itag]+TString(" yields"),DIL_BTAG2_STEP2,w_); 
// to be removed //     if (terr){
// to be removed //       for(size_t itag=0; itag<evTags.size(); itag++) mon.fillHisto(triggErr,evTags[itag]+TString(" yields"),DIL_BTAG2_STEP2, errorOnEff); 
// to be removed //        if( myMCMon ){  mcmon.fill2DHisto(evYieldsMCTrigger,mcTag,DIL_BTAG2_STEP2,tauDilCh,errorOnEff); }
// to be removed //     }
// to be removed //     if( myMCMon ){ mcmon.fill2DHisto(evYieldsMC,mcTag,DIL_BTAG2_STEP2,tauDilCh,w_); } 
// to be removed //     fillTauDileptonObjHistograms(junc,v,mets[0],tauDilCh,myKey,"2 btag",m_init,muons,e_init,electrons,t_afterLeptonRemoval,taus,j_final,jets,jerFactors,metValue,nbtags_taus);
// to be removed //   }
// to be removed // 
// to be removed // 
// to be removed //   //optimization ////////////////////////////////////////////////////////////////
// to be removed //   /*
// to be removed //   int cutIndex;
// to be removed //   for(uint i=0;i<5;i++){ //index running on tau pt
// to be removed //     for(uint j=0;j<5;j++){
// to be removed //       double metThreshold   = 30 + i*5;
// to be removed //       double tauPtThreshold = 20 + j*5;
// to be removed //       int index = i + 5*j;
// to be removed //       bool passMet(false);
// to be removed //       bool passTauPt(false);
// to be removed // 
// to be removed //       if( metValue > metThreshold   ){ passMet   = true; }
// to be removed //       if( tau_pt   > tauPtThreshold ){ passTauPt = true; }
// to be removed // 
// to be removed //       if( passMet  && nbtag1 && passTauPt && oscut ){ 
// to be removed //         for(size_t itag=0; itag<evTags.size(); itag++){
// to be removed //           debmon.fillHisto("optSelection",evTags[itag]+TString(" debug"),index,w_);
// to be removed //         }
// to be removed //       }
// to be removed //     }
// to be removed //   }
// to be removed //    */
// to be removed //   ////////////////////////////////////////////////////////////////////////////////
// to be removed // 
// to be removed // }
// to be removed // 
// to be removed // 

void CutflowAnalyzer::fillTauDileptonObjTree(
  event::MiniEvent_t *ev,
  TString qualifier, // Selected or DataDriven
  TString tag,
  JetCorrectionUncertainty * junc,
  PhysicsObject & metObj ,TString key, TString step , PhysicsObject * lepton, double tauJetRadius, PhysicsObject * tauJet, 
  PhysicsObject* theTau,
  vector<int> & j_v, vector<PhysicsObject> & jets, vector<double> & jerFactors, double met, int nbtags_taus
){


  int eventnumb = ev->iEvent;

  int jmul = j_v.size();

  // for data driven produced in w+jets we should also compute the number of bjets
  if(qualifier.Contains("Data")){
    nbtags_taus = 0;
    for(uint j=0; j<j_v.size(); ++j){  
      int j_ind = j_v[j]; 
      double btagvalue = jets[j_ind][BTAGIND_] ;
      if( btagvalue>BTAG_CUT_){nbtags_taus++;} 
    }
  }
  ////////////////////////////////////////////////////////////////////////////////

  //HTratio
  double htssm_(0.), htosm_(0.), htratiom_(0.);

  for (unsigned int i =0; i<j_v.size(); ++i){

    //float dPhiM = getdphi( metphi, j_v().at(i).phi() );
    int i_ind = j_v[i];

    double dPhiM = jets[i_ind].DeltaPhi(metObj);

    if ( dPhiM  < (TMath::Pi()/2) ) htssm_=htssm_+jets[i_ind].Pt();
    else htosm_=htosm_+jets[i_ind].Pt();
  }

  htratiom_   = htssm_ / (htosm_ + htssm_);
  //  cout << "htosm_ = " << htosm_ << "   -   htssm_ = " << htssm_ << endl;

  ////////////////////////////////////////////////////////////////////////////////



  // get tree ///////////////////////////////////////////////////////////////////////////////////////////////////////////
  // TODO : on multiple tau algos add key to begin of string) ///////////////////////////////////////////////////////////
  TString myStr;

  if( tag.Contains("lep_tau")  ){ myStr=TString("lep_tau_"); }
  if( tag.Contains("e_tau")    ){ myStr=TString("e_tau_");   }
  if( tag.Contains("m_tau")    ){ myStr=TString("m_tau_");   } 

  myStr += qualifier;

  TTree *t; map< TString, TTree * >::iterator it = treeMap_.find(myStr);
  if( it!= treeMap_.end()){ t = (*it).second; }
  else { cout<<endl<<"CutflowAnalyzer::fillTauDileptonObjTree tree with selector ="<<myStr<<" not found.. abort..."<<endl; exit(0);}
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  //tau Info ///////////////////////////////////////////////////////////
  double tauJetPt              = TMath::Abs(tauJet->Pt()); // Taujetpt corrected at source
  double tauJetEta             = TMath::Abs(tauJet->Eta());
  double deltaPhiTauJetMET     = TMath::Abs( tauJet->DeltaPhi( metObj ));
  //////////////////////////////////////////////////////////////////////
  
  // theTau info ///////////////////////////////////////////////////////
  double /* unused: tauR(-1),  mT(-1),*/ mt_max(-1), mt_min(-1); // Default values
  // FIXME: tauR
  if( !(theTau==NULL)){

    // unused:     if(theTau->E())
    // unused:       tauR =  (*theTau)[16] / theTau->E();
    
    // DEBUG:    if(tauR != tauR_) cout << "WARNING: local rTau = " << tauR << ", global rTau = " << tauR_ << endl;
    
    double mTl = sqrt( 2* lepton->Pt() * met * (1- cos( lepton->DeltaPhi(metObj) ) ) );
    double mTt = sqrt( 2* theTau->Pt() * met * (1- cos( theTau->DeltaPhi(metObj) ) ) );
  
    if(mTt>=mTl){ mt_max=mTt;mt_min=mTl; }
    else {        mt_max= mTl; mt_min = mTt;}
  }
  //////////////////////////////////////////////////////////////////////

  // lepton info ////////////////////////////////////////////////////
  double leptonPt  = TMath::Abs( lepton->Pt()); 
  double leptonEta = TMath::Abs( lepton->Eta());
  double deltaPhiLeptonMET = TMath::Abs( lepton->DeltaPhi( metObj ) );
  ///////////////////////////////////////////////////////////////////


  //deal with evt number
  TBranch * evtB    = t->GetBranch("evtnumb");  Double_t * evtnumb  =(Double_t *) evtB->GetAddress();     *evtnumb  =  eventnumb;

  //deal with weights // WARNING : btag weight
  TBranch * weightsB     = t->GetBranch("weight");     Double_t * weight      =(Double_t *)     weightsB->GetAddress();  *weight      = w_;

  // deal with os
  TBranch * osB = t->GetBranch("is_os"); Double_t * is_os = (Double_t *) osB->GetAddress(); *is_os = is_os_;


  //deal with tauPt
  TBranch * tPtB = t->GetBranch("pt_t");        Double_t * tPt = (Double_t *) tPtB->GetAddress();         *tPt   = tauJetPt;

  //deal with tauEta
  TBranch * tEtaB = t->GetBranch("eta_t");      Double_t * tEta = (Double_t *) tEtaB->GetAddress();       *tEta  = tauJetEta;

  //deal with tauPt
  TBranch * radiusB = t->GetBranch("radius_t");  Double_t * radius = (Double_t *) radiusB->GetAddress();  *radius  = tauJetRadius;

  //deal with tauR
  TBranch * tRB = t->GetBranch("rc_t");  Double_t * tR = (Double_t *) tRB->GetAddress();  *tR  = tauR_;
  
  // deal with transverse mass
  TBranch* tMtMinB = t->GetBranch("mt_min"); Double_t* tMtMin = (Double_t*) tMtMinB->GetAddress(); *tMtMin = mt_min;
  TBranch* tMtMaxB = t->GetBranch("mt_max"); Double_t* tMtMax = (Double_t*) tMtMaxB->GetAddress(); *tMtMax = mt_max;


  //deal with leptonPt
  TBranch * lPtB = t->GetBranch("pt_l");        Double_t * lPt = (Double_t *) lPtB->GetAddress();         *lPt   = leptonPt;

  //deal with leptonEta
  TBranch * lEtaB = t->GetBranch("eta_l");      Double_t * lEta = (Double_t *) lEtaB->GetAddress();       *lEta  = leptonEta;

  //deal with met
  TBranch * metB = t->GetBranch("pt_met");     Double_t * mymet = (Double_t *) metB->GetAddress();        *mymet  = met;
  
  //deal with Dphi_lepton_met
  TBranch * phiLepMetB = t->GetBranch("Dphi_lepton_met");   Double_t * phiLepMet = (Double_t *) phiLepMetB->GetAddress();   *phiLepMet  = deltaPhiLeptonMET;  

  //deal with Dphi_lepton_met
  TBranch * phiTauMetB = t->GetBranch("Dphi_tau_met");      Double_t * phiTauMet = (Double_t *) phiTauMetB->GetAddress();   *phiTauMet  = deltaPhiTauJetMET;  


  //jet multiplicity
  TBranch * numb_JetsB = t->GetBranch("multiplicity_j");       Double_t *  numb_Jets = (Double_t *) numb_JetsB->GetAddress();    *numb_Jets  = jmul;  


  //bjet multiplicity
  TBranch * numb_bJetsB = t->GetBranch("btagmultiplicity_j");  Double_t *  numb_bJets = (Double_t *) numb_bJetsB->GetAddress();  *numb_bJets  = nbtags_taus;  

  // HT ratio
  TBranch* htratioB = t->GetBranch("htratio"); Double_t* htratio = (Double_t*) htratioB->GetAddress(); *htratio=htratiom_;

  
  //tree multiplicity
  t->Fill();



}

void CutflowAnalyzer::fillTauDileptonObjHistograms(
  JetCorrectionUncertainty * junc,
                     vector<PhysicsObject> & v,
  PhysicsObject & metObj,int channel, TString key, TString step, 
  vector<int> & m_v, vector<PhysicsObject> & muons, 
  vector<int> & e_v, vector<PhysicsObject> & electrons,
  vector<int> & t_v, vector<PhysicsObject> & taus,
  vector<int> & j_v, vector<PhysicsObject> & jets, vector<double> & jerFactors, 
  double met, 
  int btagscorrected// = 0

){
  //  std::cout<<"DEBUG: start event " << i_ << std::endl;
  if(sys_ ) return;  

  TString extra1(key); extra1 += TString(" lep_tau ");
  
  TString extra2(key);
  if(evType_ == MUTAU_){  extra2 += TString(" m_tau "); } 
  else                 {  extra2 += TString(" e_tau "); } 


  // monitor to fill the histograms
  SelectionMonitor * m_ = tauDileptonStepsMons_[key]; SelectionMonitor & mon = *m_;

  
  // jet multiplicity  after MET (needed in earlier selections steps //////////////////////////////////////////////////////////////////////////
  if( met > MET_CUT_ ){  
    mon.fillHisto(TString("multiplicity_jb"),extra1+step, j_v.size(),w_); mon.fillHisto(TString("multiplicity_jb"),extra2 +step, j_v.size(),w_); 
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // jet multiplicity //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mon.fillHisto(TString("multiplicity_j"),extra1+step, j_v.size(),w_);   mon.fillHisto(TString("multiplicity_j"),extra2+step, j_v.size(),w_);  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // weights ///////////////////////////////////////////////////////////////////////////
  mon.fillHisto("weights",extra1+step, w_);  mon.fillHisto("weights", extra2+step, w_); 
  ///////////////////////////////////////////////////////////////////////////////////////


  // Jet distributions ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // jets(pt distribution, leading jet1, next to leading jet2, next to next leading jet 3 )
  // delta phi between      leading and next to leading jet
  // invariant mass between leading and next to leading jet
  // btag value
  // btag
  double jet1(0), jet2(0), jet3(0); 
  double jeta1(0), jeta2(0), jeta3(0);
  double jphi1(0), jphi2(0), jphi3(0);

  int jet1_ind(-1), jet2_ind(-1), jet3_ind(-1);
  int btagmul(0);
  
  PhysicsObject hptbjet; double ptbjet(0);// highest-pt bjet and its pt
  

  for( size_t j=0; j != j_v.size() ; ++j ){ 

    int ind = j_v[j]; double j_pt = jets[ind].Pt(); double j_eta = jets[ind].Eta(); double j_phi = jets[ind].Phi(); double btag =jets[ind][BTAGIND_]; 

    if( ! isData_) j_pt = getJetPt( jets[ind], junc, jerFactors[ind], jes_);
    else           j_pt = GetJetResidualPt(jets[ind]);

    if     ( j_pt > jet1 ){
      jet3=jet2; jeta3=jeta2; jphi3=jphi2; jet3_ind=jet2_ind; 
      jet2=jet1; jeta2=jeta1; jphi2=jphi1; jet2_ind=jet1_ind; 
      jet1=j_pt; jeta1=j_eta; jphi1=j_phi; jet1_ind = ind; 
    } // leading jet
    else if( j_pt > jet2 ){
      jet3=jet2; jeta3=jeta2; jphi3=jphi2; jet3_ind=jet2_ind;
      jet2=j_pt; jeta2=j_eta; jphi2=j_phi; jet2_ind=ind;
    } // next leading jet
    else if( j_pt > jet3 ){ 
      jet3=j_pt; jeta3=j_eta; jphi3=j_phi; jet3_ind=ind;   
    } // next to next to leading jet
    mon.fillHisto(TString("pt_j"),extra1+step, j_pt,w_);  mon.fillHisto(TString("pt_j"), extra2+step, j_pt,w_);

//    std::cout << "DEBUG: ";
//    if(jet1_ind != -1)
//      std::cout << setprecision(6) << " jet1: (" << jet1_ind << ", " << jet1 << ", " << jeta1 << ", " << jphi1 << ") | ";
//    if(jet2_ind != -1)
//      std::cout << setprecision(6) <<" jet2: (" << jet2_ind << ", " << jet2 << ", " << jeta2 << ", " << jphi2 <<") | ";
//    if(jet3_ind != -1)
//      std::cout << setprecision(6) <<" jet3: (" << jet3_ind << ", " << jet3 << ", " << jeta3 << ", " << jphi3 << ") | ";
//    std::cout << endl;

    double dphij1j2(0);
    if ( jet1 && jet2 ){ 
      dphij1j2 = fabs( jets[jet1_ind].DeltaPhi( jets[jet2_ind] )) ;  mon.fillHisto("dphij1j2",extra1+step, dphij1j2,w_);  mon.fillHisto("dphij1j2", extra2+step, dphij1j2,w_);  
      PhysicsObject vj1 = jets[jet1_ind]; PhysicsObject vj2 = jets[jet2_ind];
      TLorentzVector a(vj1+vj2); double mj1j2 = a.M();
      mon.fillHisto("mj1j2", extra1+step, mj1j2 ,w_); mon.fillHisto("mj1j2", extra2+step, mj1j2 ,w_); 
    }

    mon.fillHisto("btag_j",extra1+step, btag,w_ );  mon.fillHisto("btag_j", extra2+step, btag,w_); 



    int pgid(PGID_UNKNOWN); if(!isData_) pgid = TMath::Abs(jets[ind][jetpgid_]);
    if( btag > BTAG_CUT_){
      btagmul++;
      if(j_pt>ptbjet){ // Fill ptbjet with the pt
	hptbjet = jets[ind];
	ptbjet=j_pt;
      }
    }
    
    if(! isData_ ){
      if(btag > BTAG_CUT_){
        if     ( pgid == PGID_B       ){ mon.fillHisto("btag_eff_realbs_num",  extra1+step,  j_pt ,w_);  mon.fillHisto("btag_eff_realbs_num",  extra2+step, j_pt,w_); }
        else if( pgid == PGID_UNKNOWN ){ mon.fillHisto("btag_eff_nomatch_num", extra1+step,  j_pt ,w_);  mon.fillHisto("btag_eff_nomatch_num", extra2+step, j_pt,w_); }
        else                           { mon.fillHisto("btag_eff_fakebs_num",  extra1+step,  j_pt ,w_ ); mon.fillHisto("btag_eff_fakebs_num",  extra2+step, j_pt,w_); }
        if(pgid != PGID_B             ){ mon.fillHisto("btag_eff_fakebs2_num", extra1+step,  j_pt ,w_ ); mon.fillHisto("btag_eff_fakebs2_num", extra2+step, j_pt,w_); }  
      }

      if (     pgid == PGID_B       ){ mon.fillHisto("btag_eff_realbs_den", extra1+step, j_pt ,w_); mon.fillHisto("btag_eff_realbs_den", extra2+step, j_pt,w_);   }
      else if( pgid == PGID_UNKNOWN ){ mon.fillHisto("btag_eff_nomatch_den",extra1+step, j_pt ,w_); mon.fillHisto("btag_eff_nomatch_den", extra2+step, j_pt,w_);  } 
      else                           { mon.fillHisto("btag_eff_fakebs_den", extra1+step, j_pt ,w_); mon.fillHisto("btag_eff_fakebs_den", extra2+step, j_pt,w_);   }
      if(pgid != PGID_B             ){ mon.fillHisto("btag_eff_fakebs2_den",extra1+step, j_pt ,w_); mon.fillHisto("btag_eff_fakebs2_num",  extra2+step, j_pt,w_); } 

    }

  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
  // Fill highest pt bjet kinematics ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(btagmul){
    double etabjet = hptbjet.Eta();
    //FIXME: this fires almost always: calculator precision issue?    if(hptbjet.Pt() != ptbjet) std::cout << "DEBUG: ERROR ERROR" << std::endl;
    
    mon.fillHisto(TString("pt_bj"),extra1+step, ptbjet ,w_);  mon.fillHisto(TString("pt_bj"),extra2+step,ptbjet ,w_); mon.fillHisto(TString("eta_bj"),extra1+step, etabjet,w_);  mon.fillHisto(TString("eta_bj"),extra2+step, etabjet,w_);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  ///////////////////////////////////////////////////////////////////// Study Mjj/Mjjb /////////////////////////////////////////////////////////////////////////////////////

  // FIXME: this piece of code can be heavily optimized

  // New strategy:
  // - Use tau-skimmed collection
  // - Start from 1 btag
  // - Start from W mass
  // - bjets from top should be more energetic (earlier branch of the diagram)


  if(jets.size()>1){ // >=2 jets before tau removal
    //  pT(jet1)+pT(jet2) (leading pt jets, included taus)
    mon.fillHisto("sumpt_jj",extra1+step,jets[0].Pt()+jets[1].Pt(),w_); mon.fillHisto("sumpt_jj",extra2+step,jets[0].Pt()+jets[1].Pt(),w_);    
    
    
  }
  
  if(j_v.size()>1 && t_v.size() == 1 && m_v.size()==1 ){ // >=2 jets after tau removal, 1 tau, 1 muon
    // Calculate invariant mass of lep-jet and tau-jet pairs
    
    //    se riesci, prova a calcolare i valori di massa invariante delle coppie lep-jet e tau-jet, e trova l'accoppiamento che ti da la differenza minima di massa (se prendi i 2 jet piu` energetici, per ciascun evento hai almeno 2 combinazioni).
    //
    //    Poi, fai la distribuzione delle masse cosi' calcolate. Ovvero, hai 2 entries per evento. Alternativamente, puoi fare la distribuzione solo con il valore minimo (o massimo) delle due masse.
    
    
    // lep-jet, tau-jet pairs (using the two most energetic jets)
    vector<pair<TLorentzVector, TLorentzVector> > withJetPairs;
    TLorentzVector vLepJet(muons[m_v[0]]); vLepJet += jets[j_v[0]]; 
    TLorentzVector vTauJet(taus[t_v[0]]);  vTauJet += jets[j_v[1]];
    withJetPairs.push_back( make_pair(vLepJet, vTauJet) );       
    vLepJet = muons[m_v[0]]; vLepJet += jets[j_v[1]]; 
    vTauJet = taus[t_v[0]];  vTauJet += jets[j_v[0]];
    withJetPairs.push_back( make_pair(vLepJet, vTauJet) );       
    
    // Find the assignment which minimizes the mass difference
    double minDeltaM(100000000000.);
    unsigned int chosenPair(10000);
    for(size_t i=0; i< withJetPairs.size(); ++i){
      double deltaM = fabs(withJetPairs[i].first.M() - withJetPairs[i].second.M());
      if(deltaM < minDeltaM){
	minDeltaM = deltaM;
	chosenPair = i;
      }
    }

    if(chosenPair<10000){
      mon.fillHisto("m_lepj_all"  ,extra1+step,withJetPairs[chosenPair].first.M() ,w_); mon.fillHisto("m_lepj_all"  ,extra2+step,withJetPairs[chosenPair].first.M() ,w_);    
      mon.fillHisto("m_lepj_all"  ,extra1+step,withJetPairs[chosenPair].second.M(),w_); mon.fillHisto("m_lepj_all"  ,extra2+step,withJetPairs[chosenPair].second.M(),w_);    
      
      if(withJetPairs[chosenPair].first.M() > withJetPairs[chosenPair].second.M()){
	mon.fillHisto("m_lepj_min"  ,extra1+step,withJetPairs[chosenPair].second.M(),w_); mon.fillHisto("m_lepj_min"  ,extra2+step,withJetPairs[chosenPair].second.M(),w_);    
	mon.fillHisto("m_lepj_max"  ,extra1+step,withJetPairs[chosenPair].first.M(),w_); mon.fillHisto("m_lepj_max"  ,extra2+step,withJetPairs[chosenPair].first.M(),w_);    
      } else{
	mon.fillHisto("m_lepj_min"  ,extra1+step,withJetPairs[chosenPair].first.M(),w_); mon.fillHisto("m_lepj_min"  ,extra2+step,withJetPairs[chosenPair].first.M(),w_);    
	mon.fillHisto("m_lepj_max"  ,extra1+step,withJetPairs[chosenPair].second.M(),w_); mon.fillHisto("m_lepj_max"  ,extra2+step,withJetPairs[chosenPair].second.M(),w_);    
      }

      mon.fillHisto("m_lepj_delta",extra1+step,minDeltaM,w_); mon.fillHisto("m_lepj_delta",extra2+step,minDeltaM,w_);    
      
      mon.fill2DHisto(TString("m_lepj_mmj_mtj"  ),extra1+step, withJetPairs[chosenPair].second.M() , withJetPairs[chosenPair].first.M(), w_);  mon.fill2DHisto(TString("m_lepj_mmj_mtj"  ),extra2+step, withJetPairs[chosenPair].second.M() , withJetPairs[chosenPair].first.M() , w_);             
      mon.fill2DHisto(TString("m_lepj_delta_mtj"),extra1+step, withJetPairs[chosenPair].second.M() , minDeltaM                         , w_);  mon.fill2DHisto(TString("m_lepj_delta_mtj"),extra2+step, withJetPairs[chosenPair].second.M() , minDeltaM                          , w_);             
    }

    
  }      
  
  
  if(j_v.size() > 3){ // Minimum requirement: 4 jets.
    
    vector<PhysicsObject> jetsForMass; // Final working collection
    
    // Acquire bjets
    vector<size_t> btagInd;
    for( size_t j=0; j != j_v.size() ; ++j ){ 
      size_t ind = j_v[j];
      if( jets[ind][BTAGIND_] > BTAG_CUT_)
	btagInd.push_back(ind);
    }
    //
    
    if(btagInd.size() == 1){ // If I have one btag only 
      
      // Take the btag and the three highest-pt remaining jets
      jetsForMass.push_back( jets[btagInd[0] ]);
      for( size_t j=0; j != j_v.size() ; ++j ){ 
	size_t ind = j_v[j];
	if( ind==btagInd[0] ) continue;
	if(jetsForMass.size() < 4) jetsForMass.push_back( jets[ind] );
      }
      //
      if(jetsForMass.size() != 4) cout << "DEBUG: WARNING. btagmult " << btagInd.size() << ", selected " << jetsForMass.size() << " instead of 4 jets." << endl;      
    } // End if 1btag
    else if (btagInd.size() >1) { // If I have two ore more btags

      // Take the two btags and the two highest-pt non-btags
      jetsForMass.push_back( jets[btagInd[0] ] );
      jetsForMass.push_back( jets[btagInd[1] ] );
      for( size_t j=0; j != j_v.size() ; ++j ){ 
	size_t ind = j_v[j];
	if( ind==btagInd[0] || ind==btagInd[1]) continue;
	if(jetsForMass.size() < 4) jetsForMass.push_back( jets[ind] );
      }
      //
      
      if(jetsForMass.size() != 4) cout << "DEBUG: WARNING. btagmult " << btagInd.size() << ", selected " << jetsForMass.size() << " instead of 4 jets." << endl;
    } // End if >1btag
 

    
    

    // Now make the assignments:
    // unused: size_t chosenbtag(1000);
    vector<size_t> chosenbtags;
    vector<size_t> chosenNonbtags;
    

    // Assign nonbtags
    if(btagInd.size() == 1){ // If I have one btag only, I want to take the two non-btags which make mjj closest to mW, and then the btag which make mjjb closest to mTop

      vector<vector<size_t> > comb;
      vector<double> mjjs;
      
      for(size_t i=0; i<3; ++i){
	vector<size_t> tempComb;
	switch(i){
	case 0:
	  tempComb.push_back(1); tempComb.push_back(2);
	  comb.push_back(tempComb);
	  break;
	case 1:
	  tempComb.push_back(1); tempComb.push_back(3);
	  comb.push_back(tempComb);
	  break;
	case 2:
	  tempComb.push_back(2); tempComb.push_back(3);
	  comb.push_back(tempComb);
	  break;
	default:
	  cout << "DEBUG: dafuck? Shoulnd't ever be here" << endl;
	  break;
	}
	TLorentzVector vC( jetsForMass[tempComb[0]] ); vC+= jetsForMass[tempComb[1] ]; mjjs.push_back( vC.M() );  // Avoid temporary object allocation  
      }

      double chosenmjj = 10000000.;
      size_t chosenComb = 10;
      for(size_t i=0; i< mjjs.size(); ++i){ // Select the combination with mass closest to mW  
	if( fabs(mjjs[i] - 80.385) < fabs(chosenmjj - 80.385) ){ // PDG2012
	  chosenmjj = mjjs[i];
	  chosenComb = i;
	}
      }
      // Store the two jets from W
      chosenNonbtags.push_back(comb[chosenComb][0]);
      chosenNonbtags.push_back(comb[chosenComb][1]);
      //
     
      // Now we have to choose the btag. Note that we don't use necessarily the tagged one because it could be from the other leg
      // unused:       double chosenbtag = 10000000.;
      double chosenmjjb = 10000000;
      for(size_t i=0; i<jetsForMass.size(); ++i){
	if(i==chosenNonbtags[0] || i==chosenNonbtags[1]) continue;
	chosenbtags.push_back(i);
	TLorentzVector vC( jetsForMass[chosenNonbtags[0] ] ); vC+= jetsForMass[chosenNonbtags[1] ]; vC+= jetsForMass[i]; // Avoid temporary object allocation  
	if( fabs(vC.M() - 172.5) < fabs(chosenmjjb - 172.5) ) // Nominal reference value
	  chosenmjjb = vC.M(); 
      }
      
      // Plot the masses
      // chosenmjj
      // chosenmjjb
      mon.fillHisto("mjj",extra1+step, chosenmjj,w_ );  mon.fillHisto("mjj", extra2+step, chosenmjj,w_);                                                                                    
      mon.fillHisto("mjjb",extra1+step, chosenmjjb,w_ );  mon.fillHisto("mjjb", extra2+step, chosenmjjb,w_);                                                                                
      mon.fill2DHisto(TString("mjjb_mjj"),extra1+step, chosenmjjb , chosenmjj, w_);  mon.fill2DHisto(TString("mjjb_mjj"),extra2+step, chosenmjjb, chosenmjj, w_);             
                                                 
      

    } // End if 1btag
    else if (btagInd.size() >1) { // If I have two btags, I want to take the two non-btags and the b which make mjjb closest to mTop
      chosenNonbtags.push_back(2);
      chosenNonbtags.push_back(3);
      
      TLorentzVector vW( jetsForMass[chosenNonbtags[0]] ); vW+= jetsForMass[chosenNonbtags[1] ]; 
      double chosenmjj = vW.M();

      // Now we have to choose the btag. Note that we don't use the tagged one because it could be from the other leg
      double chosenbtag = 10000000.;
      double chosenmjjb = 10000000;
      for(size_t i=0; i<jetsForMass.size(); ++i){
	if(i==chosenNonbtags[0] || i==chosenNonbtags[1]) continue;
	chosenbtags.push_back(i);
	TLorentzVector vC( jetsForMass[chosenNonbtags[0] ] ); vC+= jetsForMass[chosenNonbtags[1] ]; vC+= jetsForMass[i]; // Avoid temporary object allocation  
	if( fabs(vC.M() - 172.5) < fabs(chosenmjjb - 172.5) ) // Nominal reference value
	  chosenmjjb = vC.M(); 
      }
      
      // Plot the masses
      // chosenmjj
      // chosenmjjb
      mon.fillHisto("mjj",extra1+step, chosenmjj,w_ );  mon.fillHisto("mjj", extra2+step, chosenmjj,w_);                                                                                    
      mon.fillHisto("mjjb",extra1+step, chosenmjjb,w_ );  mon.fillHisto("mjjb", extra2+step, chosenmjjb,w_);                                                                                
      mon.fill2DHisto(TString("mjjb_mjj"),extra1+step, chosenmjjb , chosenmjj, w_);  mon.fill2DHisto(TString("mjjb_mjj"),extra2+step, chosenmjjb, chosenmjj, w_);             

      
    } // End if >1btag


    // Now that we have the two btags (chosenbtags[]) and the two nonbtags (chosenNonbtags[]) we can play
    /////////
    

    // If we have a tau

    // pT(b)+pT(tau)  /////////////////////////////////////////////////////////////////////////////                                                                                                                
    // This can be done with the main chosen b or with the b which results in a highest sumPt
    if(t_v.size() == 1){
      int t_i = t_v[0]; double tauPt = TMath::Abs(taus[t_i].Pt());   /* unused: double deltaPhiWithTau = taus[t_i].DeltaPhi( metObj ); */
      double chosenSumpt(0.);
      double chosenmass(0.);
      for(size_t i=0; i< chosenbtags.size(); ++i){
	if(chosenSumpt < tauPt+jetsForMass[chosenbtags[i] ].Pt() )
	  chosenSumpt = tauPt+jetsForMass[chosenbtags[i] ].Pt();
	TLorentzVector taujv(taus[t_i]); taujv+=jetsForMass[chosenbtags[i] ]; // Avoid temporary object allocation 
	if(chosenmass < taujv.M())
	  chosenmass = taujv.M();
      }
      
      mon.fillHisto("sumpt_taub",extra1+step,chosenSumpt,w_); mon.fillHisto("sumpt_taub",extra2+step,chosenSumpt,w_);
      mon.fillHisto("m_taub",extra1+step,chosenmass,w_); mon.fillHisto("m_taub",extra2+step,chosenmass,w_);
     

    }
    //
    

    
  } // End of minimum requirement (4 jets)




  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  // Fill btag multiplicity  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mon.fillHisto("btagmultiplicity_j",extra1+step, btagmul,w_);                   mon.fillHisto("btagmultiplicity_j", extra2+step, btagmul,w_);
  mon.fillHisto("corrected_btagmultiplicity_j",extra1+step, btagscorrected,w_);  mon.fillHisto("corrected_btagmultiplicity_j", extra2+step, btagscorrected,w_);   


  // Categorized cutflow

  if(btagmul==0) {mon.fillHisto("btagmultiplicity0_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity0_j", extra2+step, 0,w_);}
  if(btagmul==1) {mon.fillHisto("btagmultiplicity1_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity1_j", extra2+step, 0,w_);}
  if(btagmul==2) {mon.fillHisto("btagmultiplicity2_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity2_j", extra2+step, 0,w_);}
  if(btagmul==3) {mon.fillHisto("btagmultiplicity3_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity3_j", extra2+step, 0,w_);}
  if(btagmul==4) {mon.fillHisto("btagmultiplicity4_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity4_j", extra2+step, 0,w_);}
  if(btagmul>=5) {mon.fillHisto("btagmultiplicity5_j",extra1+step, 0,w_); mon.fillHisto("btagmultiplicity5_j", extra2+step, 0,w_);}


  if(btagscorrected==0){ mon.fillHisto("corrected_btagmultiplicity0_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity0_j", extra2+step, 0,w_);}
  if(btagscorrected==1){ mon.fillHisto("corrected_btagmultiplicity1_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity1_j", extra2+step, 0,w_);}
  if(btagscorrected==2){ mon.fillHisto("corrected_btagmultiplicity2_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity2_j", extra2+step, 0,w_);}
  if(btagscorrected==3){ mon.fillHisto("corrected_btagmultiplicity3_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity3_j", extra2+step, 0,w_);}
  if(btagscorrected==4){ mon.fillHisto("corrected_btagmultiplicity4_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity4_j", extra2+step, 0,w_);}
  if(btagscorrected>=5){ mon.fillHisto("corrected_btagmultiplicity5_j",extra1+step, 0,w_); mon.fillHisto("corrected_btagmultiplicity5_j", extra2+step, 0,w_);}

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // Fill first leading jet, second leading jet, third leading jet //////////////////////////////////////////////////////
  if(jet1){ mon.fillHisto(TString("pt_j1"),extra1+step, jet1,w_);  mon.fillHisto(TString("pt_j1"),extra2+step, jet1,w_); mon.fillHisto(TString("eta_j1"),extra1+step, jeta1,w_);  mon.fillHisto(TString("eta_j1"),extra2+step, jeta1,w_);}
  if(jet2){ mon.fillHisto(TString("pt_j2"),extra1+step, jet2,w_);  mon.fillHisto(TString("pt_j2"),extra2+step, jet2,w_); mon.fillHisto(TString("eta_j2"),extra1+step, jeta2,w_);  mon.fillHisto(TString("eta_j2"),extra2+step, jeta2,w_);}
  if(jet3){ mon.fillHisto(TString("pt_j3"),extra1+step, jet3,w_);  mon.fillHisto(TString("pt_j3"),extra2+step, jet3,w_); mon.fillHisto(TString("eta_j3"),extra1+step, jeta3,w_);  mon.fillHisto(TString("eta_j3"),extra2+step, jeta3,w_);}
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // leptons pt  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(uint e=0;e!=e_v.size();++e){ 
    int e_i = e_v[e]; double e_pt = TMath::Abs(electrons[e_i].Pt()); mon.fillHisto(TString("pt_e"),extra1+step, e_pt,w_); mon.fillHisto(TString("pt_e"),extra2+step, e_pt,w_);
    double eEta = electrons[e_i].Eta(); double ePhi = electrons[e_i].Phi();
    // Eta - Phi map of reconstructed electrons 
    mon.fill2DHisto(TString("eta_phi_lepton"),extra1+step, eEta ,ePhi, w_);  mon.fill2DHisto(TString("eta_phi_lepton"),extra2+step, eEta ,ePhi, w_);
    
  }
  for(uint m=0;m!=m_v.size();++m){
    int m_i = m_v[m]; double m_pt = TMath::Abs(muons[m_i].Pt()); mon.fillHisto(TString("pt_m"),extra1+step, m_pt,w_); mon.fillHisto(TString("pt_m"),extra2+step, m_pt,w_);
    double mEta = muons[m_i].Eta(); double mPhi = muons[m_i].Phi();
    // Eta - Phi map of reconstructed muons 
    mon.fill2DHisto(TString("eta_phi_lepton"),extra1+step, mEta ,mPhi, w_);  mon.fill2DHisto(TString("eta_phi_lepton"),extra2+step, mEta ,mPhi, w_);
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  // Lepton-bjet invariant mass ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(btagmul && m_v.size()>0){
    TLorentzVector v_lbj(muons[m_v[0] ]+hptbjet); double m_lbj = v_lbj.M();
    mon.fillHisto(TString("m_mubj"),extra1+step, m_lbj,w_); mon.fillHisto(TString("m_mubj"),extra2+step, m_lbj,w_); 
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  

  // lepton-jet mass (lead, sublead, subsublead jet) //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if(e_v.size()>0){ 
    if(jet1){
      TLorentzVector v_lj1(electrons[e_v[0] ]+jets[jet1_ind]); double m_lj1 = v_lj1.M();
      mon.fillHisto(TString("m_ej1"),extra1+step, m_lj1,w_); mon.fillHisto(TString("m_ej1"),extra2+step, m_lj1,w_); 
    }
    if(jet2){
      TLorentzVector v_lj2(electrons[e_v[0] ]+jets[jet2_ind]); double m_lj2 = v_lj2.M();
      mon.fillHisto(TString("m_ej2"),extra1+step, m_lj2,w_); mon.fillHisto(TString("m_ej2"),extra2+step, m_lj2,w_); 
    }
    if(jet3){
    TLorentzVector v_lj3(electrons[e_v[0] ]+jets[jet3_ind]); double m_lj3 = v_lj3.M();
    mon.fillHisto(TString("m_ej3"),extra1+step, m_lj3,w_); mon.fillHisto(TString("m_ej3"),extra2+step, m_lj3,w_); 
    }
  }    
  if(m_v.size()>0){ 
    double hm_lj(0.); // highest mass
    double lm_lj(100000000.); // lowest mass 
    if(jet1){
      TLorentzVector v_lj1(muons[m_v[0] ]+jets[jet1_ind]); double m_lj1 = v_lj1.M();
      mon.fillHisto(TString("m_muj1"),extra1+step, m_lj1,w_); mon.fillHisto(TString("m_muj1"),extra2+step, m_lj1,w_); 
      if(m_lj1>hm_lj)
	hm_lj=m_lj1;
      if(m_lj1<lm_lj)
	lm_lj=m_lj1;
      
    }
    if(jet2){
      TLorentzVector v_lj2(muons[m_v[0] ]+jets[jet2_ind]); double m_lj2 = v_lj2.M();
      mon.fillHisto(TString("m_muj2"),extra1+step, m_lj2,w_); mon.fillHisto(TString("m_muj2"),extra2+step, m_lj2,w_); 
      if(m_lj2>hm_lj)
	hm_lj=m_lj2;
      if(m_lj2<lm_lj)
	lm_lj=m_lj2;

    }
    if(jet3){
      TLorentzVector v_lj3(muons[m_v[0] ]+jets[jet3_ind]); double m_lj3 = v_lj3.M();
      mon.fillHisto(TString("m_muj3"),extra1+step, m_lj3,w_); mon.fillHisto(TString("m_muj3"),extra2+step, m_lj3,w_); 
      if(m_lj3>hm_lj)
	hm_lj=m_lj3;
      if(m_lj3<lm_lj)
	lm_lj=m_lj3;
      
    }
 
    if(jet1 || jet2 || jet3)
      mon.fillHisto(TString("max_m_muj"),extra1+step, hm_lj,w_); mon.fillHisto(TString("max_m_muj"),extra2+step, hm_lj,w_); 


 }    
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // W transverse mass ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  double dPhiLeptonMET(0);
  if( (m_v.size()==1 || e_v.size()==1 ) ){
    //mT = sqrt( 2pt(lepton)pt(neutrino)*(1-cosDphi))
    double leptonPt(0), deltaPhi(0);
    if( m_v.size() == 1 ){ int m_i = m_v[0]; leptonPt = TMath::Abs(muons[m_i].Pt());     deltaPhi = muons[m_i].DeltaPhi( metObj );    dPhiLeptonMET = deltaPhi; }
    if( e_v.size() == 1 ){ int e_i = e_v[0]; leptonPt = TMath::Abs(electrons[e_i].Pt()); deltaPhi = electrons[e_i].DeltaPhi( metObj); dPhiLeptonMET = deltaPhi; }
    double mt = sqrt (  2*leptonPt*met*(1 - cos(deltaPhi) ) ) ;

    mon.fillHisto("Dphi_lepton_met", extra1+step, fabs(deltaPhi), w_); mon.fillHisto("Dphi_lepton_met",extra2+step, fabs(deltaPhi), w_);
    mon.fillHisto("w_mass",extra1+step,mt,w_); mon.fillHisto("w_mass",extra2+step,mt,w_);
    mon.fillHisto("w_mass_deltaphi",extra1+step,deltaPhi,w_); mon.fillHisto("w_mass_deltaphi",extra2+step,deltaPhi,w_);
   

    //transverse mass with taus and max min wt /////////////////////////////////////////////////////////////////////////////
    if(t_v.size() == 1){
      int t_i = t_v[0]; double tauPt = TMath::Abs(taus[t_i].Pt());     double deltaPhiWithTau = taus[t_i].DeltaPhi( metObj );  
      double mt_tau = sqrt (  2*tauPt*met*(1 - cos(deltaPhiWithTau) ) ) ;
      mon.fillHisto("w_mass_tau",extra1+step,mt_tau,w_); mon.fillHisto("w_mass_tau",extra2+step,mt_tau,w_);
      double mt_max,mt_min;
      if(mt_tau>=mt){ mt_max=mt_tau; mt_min= mt;  }
      else          { mt_min=mt_tau; mt_max= mt;  }  
      mon.fillHisto("w_mass_max",extra1+step,mt_max,w_); mon.fillHisto("w_mass_max",extra2+step,mt_max,w_);
      mon.fillHisto("w_mass_min",extra1+step,mt_min,w_); mon.fillHisto("w_mass_min",extra2+step,mt_min,w_); 
    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // taus pt and polarization ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    for(uint t=0;t!=t_v.size();++t){ 
      int t_i = t_v[t]; double t_pt = TMath::Abs(taus[t_i].Pt()); mon.fillHisto(TString("pt_t"),extra1+step, t_pt,w_); mon.fillHisto(TString("pt_t"),extra2+step, t_pt,w_);
      double tauLtk   = taus[t_i][4];   // lead track /charged hadron pt 
      double tauJetPt = t_pt;
      double tauP     = taus[t_i][16];  // lead charged hadron P
      double tauE     = taus[t_i].E();  // tau energy
      double tauPhi   = taus[t_i].Phi();
      double tauEta   = taus[t_i].Eta();

      //Delta phi between MET and tau ////////////////////////////////////////////////////////////////////////////////////////////////////////////
      double dPhiTauMET = fabs( taus[ t_i ].DeltaPhi( metObj ) );
      mon.fillHisto(TString("Dphi_tau_met"),extra1+step, dPhiTauMET,w_); mon.fillHisto(TString("Dphi_tau_met"),extra2+step, dPhiTauMET,w_);
      ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      // Eta - Phi map of reconstructed taus//////////////////////////////////////////////////////////////////////////////////////////////////////
      mon.fill2DHisto(TString("eta_phi_t"),extra1+step, tauEta ,tauPhi, w_);  mon.fill2DHisto(TString("eta_phi_t"),extra2+step, tauEta ,tauPhi, w_);
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      //deltaphi tau-met and lepton-met ///////////////////////////////////////////////////////////////
      mon.fill2DHisto(TString("dphiTauMET_dphiLeptonMET"),extra1+step, dPhiTauMET , dPhiLeptonMET, w_);
      mon.fill2DHisto(TString("dphiTauMET_dphiLeptonMET"),extra2+step, dPhiTauMET , dPhiLeptonMET, w_);
      /////////////////////////////////////////////////////////////////////////////////////////////////

      if (! key.Contains("TC")){ tauLtk = taus[t_i][18]; } //lead charged hadron pt

      int numbProngs(0);
      if(key.Contains("HPS")){ numbProngs = GetNumbProngsBasedOnDecayMode(taus[t_i]); }
      else{                    numbProngs = GetNumbProngs(taus[t_i]); }

      double r; 
      r=-1; if(tauJetPt){ r = tauLtk/tauJetPt; }
      mon.fillHisto(TString("r_t"),extra1+step, r,w_); mon.fillHisto(TString("r_t"),extra2+step, r,w_); 
      if     ( numbProngs == 1){  mon.fillHisto(TString("r_t1p"),extra1+step, r,w_); mon.fillHisto(TString("r_t1p"),extra2+step, r,w_); }
      else if( numbProngs == 3){  mon.fillHisto(TString("r_t3p"),extra1+step, r,w_); mon.fillHisto(TString("r_t3p"),extra2+step, r,w_); }
   
      r=-1; tauJetPt = taus[t_i][7];  if(tauJetPt){ r = tauLtk/tauJetPt; }
      mon.fillHisto(TString("rb_t"),extra1+step, r,w_); mon.fillHisto(TString("rb_t"),extra2+step, r,w_); 
      if     ( numbProngs == 1){  mon.fillHisto(TString("rb_t1p"),extra1+step, r,w_); mon.fillHisto(TString("rb_t1p"),extra2+step, r,w_); }
      else if( numbProngs == 3){  mon.fillHisto(TString("rb_t3p"),extra1+step, r,w_); mon.fillHisto(TString("rb_t3p"),extra2+step, r,w_); }

      r=-1; if(tauE){ r = tauP/tauE; }          
      mon.fillHisto(TString("rc_t"),extra1+step, r,w_); mon.fillHisto(TString("rc_t"),extra2+step, r,w_); 
      if     ( numbProngs == 1){  mon.fillHisto(TString("rc_t1p"),extra1+step, r,w_); mon.fillHisto(TString("rc_t1p"),extra2+step, r,w_); }
      else if( numbProngs == 3){  mon.fillHisto(TString("rc_t3p"),extra1+step, r,w_); mon.fillHisto(TString("rc_t3p"),extra2+step, r,w_); }
 
      mon.fillHisto(TString("numbprongs_a"),extra1+step, GetNumbProngs(taus[t_i]),w_ ); 
      mon.fillHisto(TString("numbprongs_a"),extra2+step, GetNumbProngs(taus[t_i]),w_); 
 
      mon.fillHisto(TString("numbprongs_b"),extra1+step, GetNumbProngsBasedOnDecayMode(taus[t_i]) ,w_); 
      mon.fillHisto(TString("numbprongs_b"),extra2+step, GetNumbProngsBasedOnDecayMode(taus[t_i]) ,w_); 

    }   
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // minimal separation between lepton and leading jet/////////////////////////////////////////////////////////////////////////////////////////
  // dr between lepton and leading jet(1), next to leading jet(2), next to next to leading jets (3), all
  double dr1(1000), deta1(1000), dr2(1000), deta2(1000), dr3(1000), deta3(1000), drall(1000), deta(1000); 

  // minimal dr between lepton and  : leading jet and next to leading jet, leading jet + next to leading + next to next to leading//////////////////
  double mindr12(1000), mindr123(1000); double mindeta12(1000), mindeta123(1000);
  if( e_v.size() == 1 ){ 
    if( jet1_ind != -1 ){ dr1 = electrons[ e_v[0] ].DeltaR(jets[jet1_ind]); deta1 = fabs( electrons[ e_v[0] ].Eta() - jets[jet1_ind].Eta() );  }
    if( jet2_ind != -1 ){ dr2 = electrons[ e_v[0] ].DeltaR(jets[jet2_ind]); deta2 = fabs( electrons[ e_v[0] ].Eta() - jets[jet2_ind].Eta() );  }
    if( jet3_ind != -1 ){ dr3 = electrons[ e_v[0] ].DeltaR(jets[jet3_ind]); deta3 = fabs( electrons[ e_v[0] ].Eta() - jets[jet3_ind].Eta() );  }
    for(uint j=0;j!=j_v.size();++j){ 
      int ind = j_v[j];
      double tempDr = electrons[ e_v[0] ].DeltaR(jets[ind]); double tempDeta = fabs( electrons[ e_v[0] ].Eta()-jets[ind].Eta() );
      if(tempDr< drall){ drall = tempDr; } if( tempDeta < deta ){ deta = tempDeta; }
    }

  }
      
  if( m_v.size() == 1){
    if( jet1_ind != -1 ){ dr1 = muons[ m_v[0] ].DeltaR(jets[jet1_ind]); deta1 = fabs( muons[ m_v[0] ].Eta()-jets[jet1_ind].Eta() );  }
    if( jet2_ind != -1 ){ dr2 = muons[ m_v[0] ].DeltaR(jets[jet2_ind]); deta2 = fabs( muons[ m_v[0] ].Eta()-jets[jet2_ind].Eta() );  }
    if( jet3_ind != -1 ){ dr3 = muons[ m_v[0] ].DeltaR(jets[jet3_ind]); deta3 = fabs( muons[ m_v[0] ].Eta()-jets[jet3_ind].Eta() );  }
    for(uint j=0;j!=j_v.size();++j){ 
      int ind = j_v[j];
      double tempDr = muons[ m_v[0] ].DeltaR(jets[ind]); double tempDeta = fabs( muons[ m_v[0] ].Eta()- jets[ind].Eta() );
      if(tempDr< drall){ drall = tempDr; } if( tempDeta < deta ){ deta = tempDeta; }
    }
  }

  mindr12 = dr1; mindr123 = dr1; mindeta12 = deta1; mindeta123 = deta1;

  if( dr2 < dr1    ){ mindr12  = dr2; mindr123=dr2; }  if( deta2 < deta1       ){ mindeta12  = deta2; mindeta123 = deta2; }
  if( dr3<mindr123 ){ mindr123 = dr3; }                if( deta3 < mindeta123  ){ mindeta123 = deta3;                     }       
 
  if(dr1 !=1000){
     mon.fillHisto("DR1_lepton_jet",   extra1+step,dr1,w_);       mon.fillHisto("DR1_lepton_jet",   extra2+step,dr1,w_);
     mon.fillHisto("DR2_lepton_jet",   extra1+step,mindr12,w_);   mon.fillHisto("DR2_lepton_jet",   extra2+step,mindr12,w_);
     mon.fillHisto("DR3_lepton_jet",   extra1+step,mindr123,w_);  mon.fillHisto("DR3_lepton_jet",   extra2+step,mindr123,w_);
     mon.fillHisto("DRALL_lepton_jet", extra1+step,drall,w_);     mon.fillHisto("DRALL_lepton_jet", extra2+step,drall,w_);
  }
  if(deta1 !=1000){
     mon.fillHisto("DEta1_lepton_jet",    extra1+step,deta1,w_);      mon.fillHisto("DEta1_lepton_jet",   extra2+step,deta1,w_);
     mon.fillHisto("DEta2_lepton_jet",    extra1+step,mindeta12,w_);  mon.fillHisto("DEta2_lepton_jet",   extra2+step,mindeta12,w_);
     mon.fillHisto("DEta3_lepton_jet",    extra1+step,mindeta123,w_); mon.fillHisto("DEta3_lepton_jet",   extra2+step,mindeta123,w_);
     mon.fillHisto("DEtaALL_lepton_jet",  extra1+step,deta,w_);       mon.fillHisto("DEtaALL_lepton_jet", extra2+step,deta,w_);
  }
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // distance between tau and leptons //////////////////////////////////////////////////////////////////
  double dr(-1);
  if( e_v.size() == 1 && t_v.size()==1 ){ dr =taus[t_v[0]].DeltaR ( electrons[ e_v[0] ] ); }
  if( m_v.size() == 1 && t_v.size()==1 ){ dr =taus[t_v[0]].DeltaR ( muons[ m_v[0] ] );     }
  if(dr !=-1){ mon.fillHisto("dr_t",extra1+step,dr,w_); mon.fillHisto("dr_t",extra2+step,dr,w_);}
  //////////////////////////////////////////////////////////////////////////////////////////////////////
 


  // ST plot ////////////////////////////////////////////////////////////////////////////////////////
  if( t_v.size()==1 && ( m_v.size()==1 || e_v.size()==1) ){
    double leptonPt(0);
    if     ( m_v.size()==1 ){ leptonPt = TMath::Abs(muons[ m_v[0] ].Pt());     }
    else if( e_v.size()==1 ){ leptonPt = TMath::Abs(electrons[ e_v[0] ].Pt()); }
    double tauPt = TMath::Abs(taus[t_v[0]].Pt());     
    double st = jet1 + jet2 + tauPt + leptonPt; // the 2 most energetic jets    
    mon.fillHisto(TString("st"),extra1+step, st,w_); mon.fillHisto(TString("st"),extra2+step, st,w_);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////


 

  // HT and MHT plot //////////////////////////////////////////////////////////////////////////////////////////////////////////
  // hadronic activity
  double ht(0);
  for(uint j=0;j!=j_v.size();++j){  int ind = j_v[j]; ht += jets[ind].Pt();} // NOTE : this should be corrected for data and MC
  if( t_v.size()==1 ){              int t_i = t_v[0]; ht += taus[t_i][7];  } // extra jet matched with the tau 
  mon.fillHisto(TString("ht"),extra1+step, ht,w_);   mon.fillHisto(TString("ht"),extra2+step, ht,w_);
  mon.fillHisto(TString("mht"),extra1+step,  mht_,w_); mon.fillHisto(TString("mht"),extra2+step, mht_,w_);
  mon.fillHisto(TString("mhtb"),extra1+step, mhtb_,w_); mon.fillHisto(TString("mhtb"),extra2+step, mhtb_,w_);
  mon.fill2DHisto(TString("mhtVersusMET"),extra1+step, mht_,met,w_);    mon.fill2DHisto(TString("mhtVersusMET"),extra2+step, mht_,met,w_);
  mon.fill2DHisto(TString("mhtbVersusMET"),extra1+step, mhtb_,met,w_);  mon.fill2DHisto(TString("mhtbVersusMET"),extra2+step, mhtb_,met,w_);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //object eta /////////////////////////////////////////////////////////////////////////////////////////////////
  for(uint j=0;j!=j_v.size();++j){ int j_i = j_v[j]; double j_eta = jets[j_i].Eta();      
  mon.fillHisto(TString("eta_j"),extra1+step, j_eta,w_); mon.fillHisto(TString("eta_j"),extra2+step, j_eta,w_);} 
  for(uint e=0;e!=e_v.size();++e){ int e_i = e_v[e]; double e_eta = electrons[e_i].Eta(); 
  mon.fillHisto(TString("eta_e"),extra1+step, e_eta,w_); mon.fillHisto(TString("eta_e"),extra2+step, e_eta,w_);}
  for(uint m=0;m!=m_v.size();++m){ int m_i = m_v[m]; double m_eta = muons[m_i].Eta();     
  mon.fillHisto(TString("eta_m"),extra1+step, m_eta,w_); mon.fillHisto(TString("eta_m"),extra2+step, m_eta,w_);}
  for(uint t=0;t!=t_v.size();++t){ int t_i = t_v[t]; double t_eta = taus[t_i].Eta();      
  mon.fillHisto(TString("eta_t"),extra1+step, t_eta,w_); mon.fillHisto(TString("eta_t"),extra2+step, t_eta,w_);}
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // MET ////////////////////////////////////////////////////////////////////////////////////////////////////
  mon.fillHisto(TString("pt_met"),extra1+step,met,w_); mon.fillHisto(TString("pt_met"),extra2+step,met,w_); 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // Number of vertices /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  int nVertex = v.size();
  mon.fillHisto( "vertex",extra1+step,nVertex,w_); mon.fillHisto( "vertex",extra2+step,nVertex,w_);
  mon.fillHisto( "vertexUnweighted",extra1+step,nVertex,leptontriggerefficiency_*scale_); mon.fillHisto( "vertexUnweighted",extra2+step,nVertex,leptontriggerefficiency_*scale_);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  //Weights
  mon.fillHisto( "weights",extra1+step,w_); mon.fillHisto( "weights",extra2+step,w_);

  //  std::cout<<"DEBUG: end event "<< i_ << std::endl;
}


void CutflowAnalyzer::fillDebugHistograms(
  TString key, 
  vector<int> & m_v, vector<PhysicsObject> & muons, 
  vector<int> & e_v, vector<PhysicsObject> & electrons,
  vector<int> & j_v, vector<PhysicsObject> & jets
){

 
  if( sys_ ) return;  // debug plots are only filled without systematics

  TString add(""), extra1(key + TString(" lep_tau debug")); TString extra2(key);
  if(evType_ == MUTAU_){ extra2 += TString(" m_tau debug"); } else{ extra2 += TString(" e_tau debug"); } 

  SelectionMonitor * m_ = tauDileptonDebugMons_[key]; SelectionMonitor & mon = *m_;
 

  // fill debug info in jets //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(uint j=0;j!=j_v.size();++j){ 
    int ind = j_v[j];
 
    mon.fillHisto("jetChargedHadronEnergyFraction", extra1, jets[ind][17] ,w_);             mon.fillHisto("jetChargedHadronEnergyFraction", extra2, jets[ind][17],w_);
    mon.fillHisto("jetNeutralHadronEnergyFraction", extra1, jets[ind][18] ,w_);             mon.fillHisto("jetNeutralHadronEnergyFraction", extra2, jets[ind][18],w_);
    mon.fillHisto("jetNeutralHadronEnergyFractionIncludingHF", extra1, jets[ind][19],w_ );  mon.fillHisto("jetNeutralHadronEnergyFractionIncludingHF", extra2, jets[ind][19],w_);
    mon.fillHisto("jetChargedEmEnergyFraction", extra1, jets[ind][20] ,w_);                 mon.fillHisto("jetChargedEmEnergyFraction", extra2, jets[ind][20],w_);
    mon.fillHisto("jetNeutralEmEnergyFraction", extra1, jets[ind][21] ,w_);                 mon.fillHisto("jetNeutralEmEnergyFraction", extra2, jets[ind][21],w_);
    mon.fillHisto("jetChargedMultiplicity",     extra1, jets[ind][22] ,w_);                 mon.fillHisto("jetChargedMultiplicity",     extra2, jets[ind][22],w_);

    //see if this is mc and do the mathcing
    bool matched(false);
   
    int jetpgid = TMath::Abs(jets[ind][jetpgid_]);

    if( !isData_ && (jetpgid == PGID_D || jetpgid == PGID_U ||  jetpgid == PGID_S || jetpgid == PGID_C || jetpgid == PGID_B || jetpgid == PGID_G) ){ matched = true;}

    if(isData_|| matched){
      mon.fillHisto("jetChargedHadronEnergyFractionMatched", extra1, jets[ind][17],w_ );             mon.fillHisto("jetChargedHadronEnergyFractionMatched", extra2, jets[ind][17],w_);
      mon.fillHisto("jetNeutralHadronEnergyFractionMatched", extra1, jets[ind][18],w_ );             mon.fillHisto("jetNeutralHadronEnergyFractionMatched", extra2, jets[ind][18],w_);
      mon.fillHisto("jetNeutralHadronEnergyFractionIncludingHFMatched", extra1, jets[ind][19],w_ );  mon.fillHisto("jetNeutralHadronEnergyFractionIncludingHFMatched", extra2, jets[ind][19],w_);
      mon.fillHisto("jetChargedEmEnergyFractionMatched", extra1, jets[ind][20],w_ );                 mon.fillHisto("jetChargedEmEnergyFractionMatched", extra2, jets[ind][20],w_);
      mon.fillHisto("jetNeutralEmEnergyFractionMatched", extra1, jets[ind][21],w_ );                 mon.fillHisto("jetNeutralEmEnergyFractionMatched", extra2, jets[ind][21],w_);
      mon.fillHisto("jetChargedMultiplicityMatched",     extra1, jets[ind][22],w_ );                 mon.fillHisto("jetChargedMultiplicityMatched",     extra2, jets[ind][22],w_);
    }

  } 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //cout<<endl<<"fill histo 3"<<endl;
  // leptons pt  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  for(uint e=0;e!=e_v.size();++e){ 
    int ind = e_v[e]; 

    mon.fillHisto("electronTrackIso", extra1, electrons[ind][15],w_ );  mon.fillHisto("electronTrackIso", extra2, electrons[ind][15],w_);
    mon.fillHisto("electronEcalIso",  extra1, electrons[ind][16],w_ );  mon.fillHisto("electronEcalIso",  extra2, electrons[ind][16],w_);
    mon.fillHisto("electronCaloIso",  extra1, electrons[ind][17],w_ );  mon.fillHisto("electronCaloIso",  extra2, electrons[ind][17],w_);
    mon.fillHisto("electronRelIso",   extra1, electrons[ind][18],w_ );  mon.fillHisto("electronRelIso",   extra2, electrons[ind][18],w_);
    mon.fillHisto("electronEOverP",   extra1, electrons[ind][5],w_ );   mon.fillHisto("electronEOverP",   extra2, electrons[ind][5],w_);
    mon.fillHisto("electronHOverE",   extra1, electrons[ind][6],w_ );   mon.fillHisto("electronHOverE",   extra2, electrons[ind][6],w_);


    bool matched(false);    int pgid = TMath::Abs(electrons[ind][1]);

   if(!isData_ && pgid == PGID_ELECTRON ) matched = true;
      
    if(isData_|| matched){

      mon.fillHisto("electronTrackIsoMatched", extra1, electrons[ind][15],w_ );  mon.fillHisto("electronTrackIsoMatched", extra2, electrons[ind][15],w_);
      mon.fillHisto("electronEcalIsoMatched",  extra1, electrons[ind][16],w_ );  mon.fillHisto("electronEcalIsoMatched",  extra2, electrons[ind][16],w_);
      mon.fillHisto("electronCaloIsoMatched",  extra1, electrons[ind][17],w_ );  mon.fillHisto("electronCaloIsoMatched",  extra2, electrons[ind][17],w_);
      mon.fillHisto("electronRelIsoMatched",   extra1, electrons[ind][18],w_ );  mon.fillHisto("electronRelIsoMatched",   extra2, electrons[ind][18],w_);
      mon.fillHisto("electronEOverPMatched",   extra1, electrons[ind][5],w_ );   mon.fillHisto("electronEOverPMatched",   extra2, electrons[ind][5],w_);
      mon.fillHisto("electronHOverEMatched",   extra1, electrons[ind][6],w_ );   mon.fillHisto("electronHOverEMatched",   extra2, electrons[ind][6],w_);
    }

  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}

void CutflowAnalyzer::wPlusJetAnalysis(TString myKey, event::MiniEvent_t *ev,double jes, double unc, double jer , double btagunc, double unbtagunc, double tes){
  

   // see if we are processing electron channel or muon channel /////////////////////////////////
  if (  ( !eChONmuChOFF_ && evType_ == ETAU_) || (eChONmuChOFF_ && evType_ == MUTAU_  ) ) return;
  //////////////////////////////////////////////////////////////////////////////////////////////


  if(isData_) applybtagweight_ =false;

  jes_=jes; unc_=unc; jer_=jer; btagunc_=btagunc; unbtagunc_=unbtagunc; tes_ = tes;

  // see if we run with systematics...///////////////////////////// 
  if( jes_|| unc_|| jer_|| btagunc_|| unbtagunc_ || tes_) sys_ = true; else sys_= false;
  /////////////////////////////////////////////////////////////////

  if(sys_ ) return;
  

  SelectionMonitor * myMon = tauDileptonBkgMons_[myKey];  

  TString jmultiplicity("wjets_jmult"), wmultiplicity("wjets_wjmult");

  // inclusive contribution 
  TString wplusjets_pt("wplusjets_pt");                                
  TString wplusjets_eta("wplusjets_eta");                               
  TString wplusjets_pt_gluon("wplusjets_pt_gluon");                        
  TString wplusjets_pt_quark("wplusjets_pt_quark");                        
  TString wplusjets_pt_bquark("wplusjets_pt_bquark");                      
  TString wplusjets_pt_unknown("wplusjets_pt_unknown");                  
  TString wplusjets_pt_allquark("wplusjets_pt_allquark");               
  TString wplusjets_ptR("wplusjets_ptR");                                 
  TString wplusjets_etaR("wplusjets_etaR");                               



  // see if we are processing data, if we are processing MC get channel code ////////////////////////////////////////////////////////////////////////
  int input_channel = (int)(*(TVectorD *)ev->eventClassif->At(0))[0];
  int channel(-1); int tauDilCh(-1);


  if( !isData_ ){ 
    channel = codeChannel(input_channel, urlCode_); 
    if(channel==WQQ_CH || channel==WENU_CH || channel==WTAUNU_CH || channel==WMUNU_CH ){ channel = WJETS_CH; }
    tauDilCh = tdChannel(channel);   
    
    if( url_ == TTBAR_URL){
      if(  ttbarLike_ == ETAU_  && channel != ETAU_CH  )                                                                             { return; }
      else if(  ttbarLike_ == MUTAU_ && channel != MUTAU_CH )                                                                             { return; }
      else if(  ttbarLike_ == TTBAR_MCBKG_ && ( channel ==ETAU_CH || channel == MUTAU_CH || channel == EJETS_CH || channel== MUJETS_CH ) ){ return; }
      else if(  ttbarLike_ == TTBAR_DDBKG_ && (channel != EJETS_CH && channel!= MUJETS_CH ) )                                             { return; }
    }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  
  unsigned int jetAlgo, metAlgo, leptonAlgo, tauType;
  JetCorrectionUncertainty * junc(0); JetResolution * jerc(0);

  if(myKey.Contains("PFlow")) { 
    jetAlgo=event::AK5PFLOW; leptonAlgo=event::PFLOWLEPTON; metAlgo=event::PFLOWMET; tauType = PFLOWTAU; 
    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_;  } else{ junc=jecUnc_data_ak5_pf_ ;   }     
  }
  else if(myKey=="TC")         { 
    jetAlgo=event::AK5JPT;   leptonAlgo=event::STDLEPTON;   metAlgo=event::TC; tauType = CALOTAU; 
    if(!isData_){junc=jecUnc_ak5_jpt_; jerc = jerUnc_ak5_jpt_pt_; } else{ junc = jecUnc_data_ak5_jpt_ ;} 
  }
  else                    { 
    jetAlgo=event::AK5PF; leptonAlgo=event::STDLEPTON;  metAlgo=event::PF;    

    if(myKey=="PF" || myKey.Contains("TaNC")){ tauType = PFTAU;  }
    else if(myKey.Contains("HPS"))           { tauType = HPSTAU; }

    if(!isData_){junc=jecUnc_ak5_pf_;  jerc = jerUnc_ak5_pf_pt_ ; } else{ junc = jecUnc_data_ak5_pf_  ;} 
  }


  SelectionMonitor & mon_ = *myMon;

  //trigger ///////////////////////////////////////////////////////
  TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
  bool hasMutrig = ((*trig)[0]>0); bool hasEGtrig = ((*trig)[1]>0);
  /////////////////////////////////////////////////////////////////  

  uint jetCorr;
  if(isData_){ jetCorr = event::Reader::VTXCONSTRAINED | event::Reader::RESJECCORR; }
  else       { jetCorr = event::Reader::VTXCONSTRAINED;                             }


  // get physics objects ///////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<PhysicsObject> jets_without_arbitration = evR_->GetPhysicsObjectsFrom(ev,event::JET, jetAlgo  ); 
  std::vector<PhysicsObject> vertices                 = evR_->GetVertexCollection(ev);
  std::vector<PhysicsObject> jets                     = evR_->GetJetsWithArbitration( jets_without_arbitration, jetCorr ); 
  std::vector<PhysicsObject> muons                    = evR_->GetPhysicsObjectsFrom(ev,event::MUON, leptonAlgo);
  std::vector<PhysicsObject> electrons                = evR_->GetPhysicsObjectsFrom(ev,event::ELECTRON, leptonAlgo);
  PhysicsObjectCollection mets                        = evR_->GetPhysicsObjectsFrom(ev,event::MET,metAlgo);

  

  // tau collection are needed for extracting the taus from the list of jets for trigger efficiencies considerations
  std::vector<PhysicsObject> tausColl                 = evR_->GetPhysicsObjectsFrom(ev,event::TAU);
  std::vector<PhysicsObject> taus; 
  for(size_t iorigtau=0; iorigtau<tausColl.size(); ++iorigtau){ 
    if(tausColl[iorigtau][17] == tauType ){ taus.push_back(tausColl[iorigtau]); }
  } 
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //jet resolution corrections ////////////////////////////////////////////////////////////////////////////////

  // Add here allinone met propagation and do stuff here for that yaya.
  if(mets.size()==0) { cout << "No met available for " << myKey <<" analyis type ! "<< endl;  return;}
  PhysicsObject met = mets[0]; 

  vector<double> jerFactors;
  doPropagations( jerFactors, jes, jer, junc, jets , met, isData_);
  if(i_ == 45){
    cout<<endl<< "RESCALED JETS IN EVENT " << i_ << endl;
    for(size_t ijet=0; ijet<jets.size(); ++ijet){
      cout << setprecision(6) << "\t\t jet " << ijet << ", pt " << jets[ijet].Pt() <<", eta " << jets[ijet].Eta() << ", phi " << jets[ijet].Phi() << endl;
      
    }
    cout<<endl<< "PROPAGATED MET: pt " << met.Pt() << ", eta " << met.Eta() << ", phi " << met.Phi() << endl;
  }

  //double metValue = met.Pt();
  mets[0] = met;
  ///////////////////////////////////////////////////

  if(vertices.size()==0){ cout<<endl<<" Vertex was zero ???????"<<endl; return; }
  PhysicsObject & primaryVertex = vertices[0];
 
  TVectorD *classif = (TVectorD *)ev->eventClassif->At(0);
  if(! isData_ ) if(classif==0) return;

  // pileup reweighting /////////////////////////////////////////////////////////
  if(! isData_ ){ 
    outtimepuWeight_= 1;     /*outtimepuWeight_ = eventPUWeight( (*classif)[3] );*/ 
    int npv = (*classif)[2]; //intime pu vertices
    intimepuWeight_ = LumiWeights_.ITweight(npv);
  } 
  else  { intimepuWeight_ = 1;  outtimepuWeight_= 1; }
  ////////////////////////////////////////////////////////////////////////////////


  //cout<<"\n deb1 "<<"intime pu = "<<std::setprecision(10)<<intimepuWeight_<<endl; 


  ///////////////////////////////////////////////////////////////////////////////////////////////////////

  //  double metWithJES = jetMETScaling(jerFactors, jes_,junc, jets , met.Px(), met.Py()); 
  double metWithJES = met.Pt();  // FIXME: check the jes
  //cout<<"deb 2 met : "<<std::setprecision(10)<<metWithJES<<endl;
  if( metWithJES < MET_CUT_ ) return;


  // preselect jets with pt minimum equal to tau pt min //////////////////////////////////////////////////////////////
  DisableLtkCutOnJets(); 
  Pt_Jet(TAU_PT_MIN_);   
  vector<int> e_init, m_init, j_init,t_init;
  PreSelectMuons(      evR_,&m_init,muons, primaryVertex);
  PreSelectElectrons(  evR_,&e_init,electrons, primaryVertex);
  PreSelectTaus( &t_init,taus,TAUPRONGS_, myKey, primaryVertex, tes ); // this is needed to clean taus from triggering jets
  PreSelectJets( isData_, jerFactors, jes, junc,jetAlgo,&j_init,jets);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // only accept jets if dr > drmin in respect to electrons and muons /////////////////
  vector<int> emptyColl, j_toRemove; 
  vector<int> j_afterLeptonRemoval;
  ProcessCleaning(&j_init, &j_toRemove, &e_init, &emptyColl, jets, electrons, DRMIN_JET_E_ );
  ProcessCleaning(&j_init, &j_toRemove, &m_init, &emptyColl, jets, muons,     DRMIN_JET_M_ );
  ApplyCleaning(&j_init, &j_toRemove, &j_afterLeptonRemoval);
  /////////////////////////////////////////////////////////////////////////////////////


  //get soft and hard jets////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  vector<int> softJets; 
  if( TAU_PT_MIN_ < JET_PT_CUT_ ) GetObjectsBasedOnPt( isData_, jerFactors,jes,junc, ObjectSelector::JETS_TYPE, softJets, j_afterLeptonRemoval, jets, TAU_PT_MIN_, JET_PT_CUT_);
  vector<int> hardJets;     GetObjectsBasedOnPt( isData_, jerFactors,jes,junc,ObjectSelector::JETS_TYPE, hardJets, j_afterLeptonRemoval, jets, JET_PT_CUT_, 0);
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // This processing is applied to extract the taus from the list of hard jets ///////////////////////////////////
  // only accept taus if dr > drmin in respect to electrons and muons 
  vector<int> t_toRemove; vector<int> t_afterLeptonRemoval; 
  ProcessCleaning(&t_init, &t_toRemove, &e_init, &emptyColl, taus, electrons, DRMIN_T_E_ );
  ProcessCleaning(&t_init, &t_toRemove, &m_init, &emptyColl, taus, muons,     DRMIN_T_M_ );
  ApplyCleaning(&t_init, &t_toRemove, &t_afterLeptonRemoval);
  // remove jets if dr > drmin in respect to taus 
  j_toRemove.clear(); t_toRemove.clear();
  vector<int> j_final;
  ProcessCleaning(&hardJets, &j_toRemove, &t_afterLeptonRemoval, &t_toRemove, jets, taus, DRMIN_T_J_ );
  ApplyCleaning(&hardJets, &j_toRemove, &j_final);
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////

  int tau_i;
  PhysicsObject* theTau = NULL;
  if(t_afterLeptonRemoval.size() == 1){ tau_i = t_afterLeptonRemoval[0];
    // tau_pt = taus[tau_i].Pt(); // unused
    theTau = &(taus[tau_i]); }



  //cout<<"\n number of soft jets : "<<softJets.size()<<endl;
  //cout<<"\n number of hard jets : "<<hardJets.size()<<endl;


 
  // new way to compute btag uncertainty //////////////////////////////////////////
  int index_btag(-1); int numb_btags(0); 

  if( isData_ || !  applybtagweight_){
    for(uint j=0; j<j_final.size(); ++j){  
      int j_ind = j_final[j]; 
      double btagvalue = jets[j_ind][BTAGIND_] ;
      if( btagvalue>BTAG_CUT_){numb_btags++; index_btag = j_ind;} 
    }
  }
  else {
      
    for(uint j=0; j<j_final.size(); ++j){  
      int    j_ind     = j_final[j]; 
      double btagvalue = jets[j_ind][BTAGIND_];
      bool   isTagged = false;
      double bjpt(jets[j_ind].Pt()), bjeta(jets[j_ind].Eta()); 

      if( btagvalue > BTAG_CUT_) isTagged = true;
      
      //      double newBTagSF     = BTagSF_;
      //      double newLightJetSF = LightJetSF_;
      
      double newBTagSF     = getSFb(bjpt, BTAGIND_);
      double newLightJetSF = getSFlight(bjpt, bjeta, BTAGIND_);
      

      // get flavour of the jet

      int jet_flavor = TMath::Abs(jets[j_ind][jetpgid_]);
      double err_newBTagSF = err_BTagSF_;

      if(jet_flavor == PGID_C ){err_newBTagSF =2*err_BTagSF_;}

      if(btagunc_   > 0){ newBTagSF     = BTagSF_+ err_newBTagSF; }
      else if(btagunc_ < 0){ newBTagSF     = BTagSF_- err_newBTagSF; }

      if(unbtagunc_ > 0){ newLightJetSF = LightJetSF_ + err_LightJetSF_;}
      else if(unbtagunc_ < 0){ newLightJetSF = LightJetSF_ - err_LightJetSF_;}

//      double BTagEff     = newBTagSF*BTAG_eff_R_;
//      double LightJeteff = newLightJetSF*BTAG_eff_F_;

      double jet_phi = jets[j_ind].Phi();

      double sin_phi = sin(jet_phi*1000000);
      double seed = abs(static_cast<int>(sin_phi*100000));

      //Initialize class
      BTagSFUtil btsfutil(seed); // Seed must be the same per-event
      
      //modify tags
      btsfutil.modifyBTagsWithSF(isTagged, jet_flavor, newBTagSF, /*BTagEff*/ BTAG_eff_R_, newLightJetSF, /*LightJeteff*/ BTAG_eff_F_);

      if(isTagged) { numb_btags++; index_btag = j_ind;} 

    }
  }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////





  // apply lepton+jetlike selection ///////////////////////////////////////////////////////////////////////////// 
  int numb_e(0), numb_m(0);

  if( hasEGtrig ){ numb_e = e_init.size(); if(numb_e){ evType_ = ETAU_ ; w_ = intimepuWeight_*scale_; }}
  if( hasMutrig ){ numb_m = m_init.size(); if(numb_m){ evType_ = MUTAU_; w_ = intimepuWeight_*scale_; }}
  if( numb_e + numb_m != 1){ return; }

  //  double muAbsEta = fabs( muons[ m_init[0] ].Eta() );
  //  if( muAbsEta >0. && muAbsEta < 0.9)


  if(isData_){w_=1; leptontriggerefficiency_=1;}


  // lepton requirement includes trigger selection requirement ( the two highest pt jets should fire the trigger ) //////////////////////////////////
  // on MC   : we apply a trigger efficiency
  // on data : we require that the two highest pt jet should fire the trigger (returned eff from the method is either 0/1)
  // double errorOnEff(0); // unused
  if( eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETSPLUSMET_ ){
    pair<double,double> eff( getEfficiencyAndError( jets,j_final,junc,jerFactors));
    if ( eff.first == 0 ) return;
    else{ 
      electrontriggerefficiency_ = eff.first;  
      if( !isData_ ){ /* errorOnEff =  w_*eff.second; */ /* unused */ leptontriggerefficiency_ = electrontriggerefficiency_; w_= w_*leptontriggerefficiency_; } // get trigger efficiency for electrons 
    }
  }
  else if (eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_ ) { if( !isData_ )leptontriggerefficiency_ = electrontriggerefficiency_;}
  else{                                                      if( !isData_ )leptontriggerefficiency_ = muontriggerefficiency_;    }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  if( evType_ == MUTAU_){
    if( LooseMuonVeto( m_init[0], muons )           ){ return;}   //see if we have loose muons
    if( LooseElectronVeto(evR_,-1,electrons)        ){ return;}   // see if we have loose electrons
  }   
  else if( evType_ == ETAU_){
    //see if we have loose muons
    if( LooseMuonVeto(-1,muons) )                    { return;}   // see if we have loose muons
    if( LooseElectronVeto(evR_,e_init[0],electrons) ){ return;}   // see if we have loose electrons  
  }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // finding leading lepton and qualify tag ////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<TString> evTags;

  double lepton_pt(0); double lepton_charge(0); uint lepton_ind(0);

  vector<int> * p_i ; vector<PhysicsObject> * p_obj; PhysicsObject * lep_obj;

  if     (evType_ == MUTAU_ ){ p_i = & m_init ; p_obj = &muons;    }
  else if(evType_ == ETAU_  ){ p_i = & e_init ; p_obj = &electrons;}
 
  for(uint myi=0; myi < (*p_i).size(); ++myi){
    int ind = (*p_i)[myi]; double lPt = TMath::Abs((*p_obj)[ind].Pt()); double charge = (*p_obj)[ind][0];
    if( lPt > lepton_pt ){ lepton_pt = lPt; lepton_charge = charge; lepton_ind = ind; lep_obj= &( (*p_obj)[ind]); }                                   
  }
  if(lepton_pt==0) return;
  if     (evType_ == ETAU_ ){  evTags.push_back(myKey+TString(" lep_tau bkg"));   evTags.push_back(myKey+TString(" e_tau bkg")); }
  else if(evType_ == MUTAU_){  evTags.push_back(myKey+TString(" lep_tau bkg"));   evTags.push_back(myKey+TString(" m_tau bkg")); }
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




  // MT cut ////////////////////////////////////////////////////////////
  double deltaPhi = (*p_obj)[lepton_ind].DeltaPhi( mets[0] );  
  double mt = sqrt (  2*lepton_pt*metWithJES *(1 - cos(deltaPhi) ) ) ;
  if( APPLY_MT_CUT_){  if( mt<MT_CUT_ ){ return; } } 
  //////////////////////////////////////////////////////////////////////


  //jetmultiplicity (jet multiplicity in w+jets) /////////////////////////////////
  for(size_t itag=0; itag<evTags.size(); ++itag){
    if( hardJets.size() > 2 || ( hardJets.size()==2 && softJets.size()>0 ) ){
      int numbj = hardJets.size()+softJets.size();
      if(numb_btags){  
        mon_.fillHisto(jmultiplicity, evTags[itag], numbj,w_); 
        mon_.fillHisto(wmultiplicity, evTags[itag], numbj,w_);
      }
    }
  }
  ////////////////////////////////////////////////////////////////////////////////


  // Acquire W pt
  // W is lepton+met  
  double thewpt(met.Pt() + lep_obj->Pt());
  for(size_t itag=0; itag<evTags.size(); ++itag){
    mon_.fillHisto("recow_pt", evTags[itag], thewpt, w_);
  }
  
// manipulate for w //  if( url_ == TTBAR_URL){
// manipulate for w //    double tPt(99999.), tbarPt(99999.);
// manipulate for w //    event::MiniEvent_t *evmontecarlo = evRMC_->GetNewMiniEvent(i_,"mc");
// manipulate for w //    if( evmontecarlo == 0 ){ cout<<"\n empty event"<<endl; }
// manipulate for w //    else{
// manipulate for w //      std::vector<PhysicsObject> mcquarksColl = evRMC_->GetPhysicsObjectsFrom(evmontecarlo,event::TOP);
// manipulate for w //      for( size_t iquark=0; iquark<mcquarksColl.size(); iquark++){
// manipulate for w //	if( i_ == 45) cout << "pdg id: " << mcquarksColl[iquark][1] << endl;
// manipulate for w //	if(mcquarksColl[iquark][1] == 6) tPt = mcquarksColl[iquark].Pt();
// manipulate for w //	if(mcquarksColl[iquark][1] == -6) tbarPt = mcquarksColl[iquark].Pt();
// manipulate for w //      }
// manipulate for w //      
// manipulate for w //    }
// manipulate for w //    
// manipulate for w //    // down: no reweighting
// manipulate for w //    // central: weight -> weight * reweight
// manipulate for w //    // up: weight -> weight * reweight * reweight
// manipulate for w //    if(topptunc_ >=0) 
// manipulate for w //      w_  *= ttbarReweight(tPt,tbarPt);    
// manipulate for w //    if( topptunc_>0)
// manipulate for w //      w_  *= ttbarReweight(tPt,tbarPt);
// manipulate for w //    
// manipulate for w //    if(i_ == 45) cout << "topptunc: (" << tPt << ", " << tbarPt << ") ---> " <<  ttbarReweight(tPt,tbarPt) <<endl;
// manipulate for w //  }
// manipulate for w //  /////////////////////////////////////////////



  // CONTRIBUTIONS ==========================================================================================================
  if( hardJets.size() >=  2){ //-> if we have at least 2 hard jets

    
    for(uint k=0; k!=softJets.size(); ++k){ //------------------------------> soft jet contribution

      int ind = softJets[k]; double eta = jets[ind].Eta(); double phi = jets[ind].Phi();

      //bool jetbtagged(false); if( jets[ind][BTAGIND_]>BTAG_CUT_ ) continue;

      double pt = jets[ind].Pt(); 

      if(jerFactors.size() !=0 ){ pt = getJetPt( jets[ind], junc, jerFactors[ind], jes_); }
      else                      { pt = getJetPt( jets[ind], junc, 0, jes_);               }

      if( isData_              ){ pt = GetJetResidualPt(jets[ind]);                       }
    
      // get flavour of the jet
      int pgid(PGID_UNKNOWN); if(!isData_) pgid = TMath::Abs(jets[ind][jetpgid_]);

      //compute Radious of jet = sqrt(etaeta+phiphi);
      double R(0);  ( jets[ind][29] + jets[ind][30])  > 0  ? R=sqrt( jets[ind][29]+jets[ind][30]) : R=0;  

      for(size_t itag=0; itag<evTags.size(); ++itag){
  
        // if we have at least one btag add also soft jet contribution to the collection of fakable taus /////////////////////////////////////////        
        if( numb_btags ){   
          mon_.fillHisto(wplusjets_pt,evTags[itag],pt,w_);    mon_.fillHisto(wplusjets_eta,evTags[itag],eta,w_);  
          mon_.fillHisto(wplusjets_ptR,evTags[itag],pt,R,w_); mon_.fillHisto(wplusjets_etaR,evTags[itag],eta,R,w_);

          PhysicsObject tau_obj;  tau_obj.SetPtEtaPhiE(pt,eta,phi,pt); 
          fillTauDileptonObjTree(ev,"DataDriven",evTags[itag],junc,mets[0],myKey,"",lep_obj, R, &tau_obj, theTau, hardJets, jets, jerFactors, metWithJES);
        }       
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
     
        if( ! isData_  ){       
          // if we have at least one btag add also soft jet contribution to the collection of fakable taus   /////////////////////////////////////////////////////////
          if( numb_btags ){
            if     ( pgid==PGID_UNKNOWN )                                                               { mon_.fillHisto(wplusjets_pt_unknown,  evTags[itag], pt,w_);}
            else if( pgid == PGID_D || pgid == PGID_U || pgid == PGID_S || pgid == PGID_C )             { mon_.fillHisto(wplusjets_pt_quark,    evTags[itag], pt,w_);}
            else if( pgid == PGID_B     )                                                               { mon_.fillHisto(wplusjets_pt_bquark,   evTags[itag], pt,w_);}
            else if( pgid == PGID_G     )                                                               { mon_.fillHisto(wplusjets_pt_gluon,    evTags[itag], pt,w_);}
            if( pgid == PGID_D || pgid == PGID_U || pgid == PGID_S || pgid == PGID_C || pgid == PGID_B ){ mon_.fillHisto(wplusjets_pt_allquark, evTags[itag], pt,w_);}
          }
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }

      }

    }
    
    if( hardJets.size()>2){ //---------------------------------------------> hard jet contribution 
 
      for(uint k=0; k!=hardJets.size(); ++k){ 
    
        int ind = hardJets[k];  double eta = jets[ind].Eta();  double phi = jets[ind].Phi();

        double pt = jets[ind].Pt(); 
        if(jerFactors.size() !=0 ){ pt = getJetPt( jets[ind], junc, jerFactors[ind], jes_); }
        else                      { pt = getJetPt( jets[ind], junc, 0, jes_);               }

        if( isData_              )  pt = GetJetResidualPt(jets[ind]);

   
        // get flavour of the jet
        int pgid(PGID_UNKNOWN); if(!isData_)pgid = TMath::Abs(jets[ind][jetpgid_]);

        //compute Radious of jet = sqrt(etaeta+phiphi);
        double R(0); ( jets[ind][29] + jets[ind][30])  > 0  ? R=sqrt( jets[ind][29]+jets[ind][30]) : R=0; 

        bool addbs(false);
        if(numb_btags >1 || (numb_btags == 1 && ind !=index_btag) ){ addbs=true; }
        

        for(size_t itag=0; itag<evTags.size(); ++itag){
            
          // adding method 2 inclusive contribution /////////////////////////////////////////////////////////////////////////////////
          if( addbs  ){
            
            mon_.fillHisto(wplusjets_pt,evTags[itag],pt,w_); mon_.fillHisto(wplusjets_eta,evTags[itag],eta,w_); 
            mon_.fillHisto(wplusjets_ptR,evTags[itag],pt,R,w_*w1b_); mon_.fillHisto(wplusjets_etaR,evTags[itag],eta,R,w_);                     

            //Build Tree quantites based on tau jet
            PhysicsObject tau_obj;  tau_obj.SetPtEtaPhiE(pt,eta,phi,pt); 
            vector<int> hardJets_without_taus;
            for( size_t hj=0; hj<hardJets.size(); ++hj ){  if( hardJets[hj] != ind ) hardJets_without_taus.push_back( hardJets[hj] );   }
            fillTauDileptonObjTree(ev,"DataDriven",evTags[itag],junc,mets[0],myKey,"",lep_obj, R, &tau_obj, theTau, hardJets_without_taus, jets, jerFactors, metWithJES);
 
            
          }       
          /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


          if(!isData_ ){
            // adding method 2 inclusive contribution //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////        
            if( addbs ){
              if     ( pgid==PGID_UNKNOWN )                                                               { mon_.fillHisto(wplusjets_pt_unknown,  evTags[itag],pt,w_);}
              else if( pgid == PGID_D || pgid == PGID_U || pgid == PGID_S || pgid == PGID_C )             { mon_.fillHisto(wplusjets_pt_quark,    evTags[itag],pt,w_);}
              else if( pgid == PGID_B     )                                                               { mon_.fillHisto(wplusjets_pt_bquark,   evTags[itag],pt,w_);}
              else if( pgid == PGID_G     )                                                               { mon_.fillHisto(wplusjets_pt_gluon,    evTags[itag],pt,w_);}
              if( pgid == PGID_D || pgid == PGID_U || pgid == PGID_S || pgid == PGID_C || pgid == PGID_B ){ mon_.fillHisto(wplusjets_pt_allquark, evTags[itag],pt,w_);}
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          }
        }
      }
    }
  }

  //=====================================================================================================================




}































/* 
  newPhysics(vertices, muons,m_init,electrons,e_init,taus,t_afterLeptonRemoval,jets,j_final,numbJets,junc,jerFactors,myKey,ev);
*/


void CutflowAnalyzer::newPhysics(
    vector<PhysicsObject> & vertices,
    std::vector<PhysicsObject> & muons,     vector<int>  & m_init,  
    std::vector<PhysicsObject> & electrons, vector<int>  & e_init,
    std::vector<PhysicsObject> & taus,      vector<int>  & t_init, 
    std::vector<PhysicsObject> & jets,      vector<int>  & j_final, int totalJets, JetCorrectionUncertainty * junc, vector<double> & jerFactors, 
    TString myKey,
    event::MiniEvent_t* ev
    ){


  SelectionMonitor * myMon = newPhysicsMons_[myKey];    // monitor for the selection plots

 
  unsigned int metAlgo;  


  if     ( myKey=="TC"               ) { metAlgo=event::TC;       }
  else if( myKey=="PF"               ) { metAlgo=event::PF;       }
  else if( myKey.Contains("TaNC")    ) { metAlgo=event::PF;       }
  else if( myKey.Contains("HPS")     ) { metAlgo=event::PF;       } 
  else if( myKey.Contains("PFlow")   ) { metAlgo=event::PFLOWMET; }
  

  TString mcTag(""); TString mcTag2("");
  
  PhysicsObjectCollection mets = evR_->GetPhysicsObjectsFrom(ev,event::MET,metAlgo);           
  if(mets.size()==0) { cout << "No met available for " << myKey <<" analyis type ! "<< endl;  return;}
  PhysicsObject met = mets[0]; 


  vector<PhysicsObject> leptonsV;
  vector<double> leptonChargeV;
  for(uint i=0;i<t_init.size(); ++i){ PhysicsObject & obj = taus[ t_init[i] ];      leptonsV.push_back(obj); leptonChargeV.push_back(taus[      t_init[i] ][0] );}
  for(uint i=0;i<e_init.size(); ++i){ PhysicsObject & obj = electrons[ e_init[i] ]; leptonsV.push_back(obj); leptonChargeV.push_back(electrons[ e_init[i] ][0] );}
  for(uint i=0;i<m_init.size(); ++i){ PhysicsObject & obj = muons[ m_init[i] ];     leptonsV.push_back(obj); leptonChargeV.push_back(muons[     m_init[i] ][0] );}
  if(leptonsV.size()!=2) return;

 


  //Get the leptons ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  std::vector<TString> evTags;
  evTags.push_back(myKey+TString(" lep_lep"));
  if(  e_init.size() == 2                                                                       ){ evTags.push_back( myKey+TString(" e_e"));     }
  if(  m_init.size() == 2                                                                       ){ evTags.push_back( myKey+TString(" m_m"));     }
  if(  t_init.size() == 2                                                                       ){ evTags.push_back( myKey+TString(" tau_tau")); }
  if(  e_init.size() == 1 &&  t_init.size() == 1                                                ){ evTags.push_back( myKey+TString(" e_tau"));   }
  if(  m_init.size() == 1 &&  t_init.size() == 1                                                ){ evTags.push_back( myKey+TString(" m_tau"));   }
  if( (m_init.size() == 1 &&  t_init.size() == 1 )|| (e_init.size() == 1 &&  t_init.size() == 1)){ evTags.push_back( myKey+TString(" lep_tau")); }
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // compute invariant mass method 1  (visible component) ///////////////////////
  PhysicsObject v1 = leptonsV[0]; PhysicsObject v2 = leptonsV[1];
  TLorentzVector a(v1+v2); double mass_1 = a.M();
  
  // compute invariant mass method 2 (colinear apro)  ///////////////////////////////
  double metX = met.Px(); double metY = met.Py();

  double ptNeu1_num = metX*sin( v2.Phi() ) - metY*cos( v2.Phi());
  double ptNeu1_den = cos( v1.Phi() )*sin( v2.Phi() ) - cos( v2.Phi() )*sin( v1.Phi() );

  double ptNeu2_num = metX*sin( v1.Phi() ) - metY*cos( v1.Phi());
  double ptNeu2_den = cos( v2.Phi())*sin( v2.Phi())-cos( v1.Phi())*sin( v2.Phi() );

  double px1(0),py1(0),pz1(0),px2(0),py2(0),pz2(0);
  bool nonNull(false);
  if( ptNeu1_den && ptNeu2_den && tan( v1.Theta()) && tan( v2.Theta()) ){
    px1 = (ptNeu1_num /ptNeu1_den)*cos(v1.Phi());
    py1 = (ptNeu1_num /ptNeu1_den)*sin(v1.Phi());
    pz1 = (ptNeu1_num /ptNeu1_den)/(tan(v1.Theta()));
   
    px2 = (ptNeu2_num /ptNeu2_den)*cos(v2.Phi());
    py2 = (ptNeu2_num /ptNeu2_den)*sin(v2.Phi());
    pz2 = (ptNeu2_num /ptNeu2_den)/(tan(v2.Theta()));
    nonNull = true;
  }
  double px =px1+px2+v1.Px()+v2.Px();
  double py =py1+py2+v1.Py()+v2.Py();
  double pz =pz1+pz2+v1.Pz()+v2.Pz();

  TLorentzVector total(px,py,pz, pow( px*px + py*py + pz*pz, 0.5) );
  double mass_2(0);
  if(nonNull)  mass_2=total.M();
  //////////////////////////////////////////////////////////////////////////////////////

  // compute inv mass method 3 /////////////////////////////////////////////////////////////
  double x1Num = ( px1*py2-px2*py1 );
  double x1Den = (py2*px1+py2*metX-px2*py1-px2*metY);
  double x2Den = (py2*px1+px1*metY-px2*py1-py1*metX);
  double mass_3 =0;
  double deltaPhi = cos( v1.DeltaPhi(v2));
  if( deltaPhi>-0.99 && x1Den && x2Den ){ mass_3 = mass_1/sqrt(x1Num*x1Num/(x1Den*x2Den)); }
  //////////////////////////////////////////////////////////////////////////////////////////
  


  //  double metValue = jetMETScaling( jerFactors, jes_, junc , jets ,met.Px(), met.Py());
  double metValue = jetMETScalingTest( jerFactors, jes_, junc, jets  , met);


  // INIT OPTION //////// ////////////////////////////////////////////////////////////////////////////////////////////////////
  for(size_t itag=0; itag<evTags.size(); ++itag) myMon->fillHisto("inv_mass", evTags[itag]+TString(" invmass init"),mass_1,w_); 
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass2", evTags[itag]+TString(" invmass init"),mass_2,w_);
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass3", evTags[itag]+TString(" invmass init"),mass_3,w_);  
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  // OS selection //////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( leptonChargeV[0]*leptonChargeV[1] >=0 ) return;
  for(size_t itag=0; itag<evTags.size(); ++itag) myMon->fillHisto("inv_mass", evTags[itag]+TString(" invmass OS"),mass_1,w_); 
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass2", evTags[itag]+TString(" invmass OS"),mass_2,w_); 
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass3", evTags[itag]+TString(" invmass OS"),mass_3,w_);  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
The reconstructed mass (mcol) is calculated using the collinear approximation as
mcol = mvis �� / �� x�1 · �2 where mvis �� is the invariant mass of the two identified tau visible decay
products and x�1 and x�2 are the fractions of the tau momenta carried by the visible decay daughters.
*/



  // debug info after OS ////////////////////////////////////////////////////////////////////////////////////////////////////
  if(mass_1 >450){
    infoFile_<<endl<<" ============================================================================";
    infoFile_<<endl<<" Run : "<<ev->iRun <<" , Lum : "<<(ev->iLumi)<<" , evt :"<< ev->iEvent <<endl;
    // infoFile_<<endl<<" HIGH INV MASS for : "<<evTags[1]<<" inv mass = "<<mass_1;
    // infoFile_<<endl<<" HIGH INV MASS for : " inv mass = "<<mass_1;
    // infoFile_<<endl<<" leptons are filled with this order taus, electrons, muons ";
    infoFile_<<endl<<" leptons are filled with this order taus, electrons, muons ";
    infoFile_<<endl<<" first lepton ***** ";
    infoFile_<<endl<<" Pt : "<<v1.Pt()<<", phi : "<<v1.Phi()<<" , eta : "<<v1.Eta()<<" , theta : "<<v1.Theta();
    infoFile_<<endl<<" Px : "<<v1.Px()<<", Py : "<<v1.Py()<<" , Pz : "<<v1.Pz();
    infoFile_<<endl<<" second lepton ***** ";
    infoFile_<<endl<<" Pt : "<<v2.Pt()<<", phi : "<<v2.Phi()<<" , eta : "<<v2.Eta()<<" , theta : "<<v2.Theta();
    infoFile_<<endl<<" Px : "<<v2.Px()<<", Py : "<<v2.Py()<<" , Pz : "<<v2.Pz();
    infoFile_<<endl<<" Delta Phi : "<<v1.DeltaPhi(v2);
    infoFile_<<endl<<" MET ***** ";
    infoFile_<<endl<<" MET Pt (corrected) : "<<metValue;
    infoFile_<<endl<<" metX : "<<(met.Px())<<" , metY "<<(met.Py())<<" metZ"<<(met.Pz());
    infoFile_<<endl<<" ============================================================================"<<endl;
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  // MET cut //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( metValue< MET_CUT_ ) return;
  for(size_t itag=0; itag<evTags.size(); ++itag) myMon->fillHisto("inv_mass", evTags[itag]+TString(" invmass met"),mass_1,w_); 
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass2", evTags[itag]+TString(" invmass met"),mass_2,w_); 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



  // delta phi //////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if( deltaPhi>=2.9 ) return;
  for(size_t itag=0; itag<evTags.size(); ++itag) myMon->fillHisto("inv_mass", evTags[itag]+TString(" invmass deltaPhi"),mass_1,w_); 
  for(size_t itag=0; itag<evTags.size(); ++itag)  myMon->fillHisto("inv_mass2", evTags[itag]+TString(" invmass deltaPhi"),mass_2,w_); 
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}



void CutflowAnalyzer::getBtagEfficiencies(){

 

 TFile * file  = TFile::Open("results-mc/out-mc.root"); 


 TH1 * btagEff_realbs_num =  (TH1*) file->Get("PFlow/RecoSteps/btag1/lep_tau/MT/btag_eff_realbs_num_6");
 TH1 * btagEff_realbs_den =  (TH1*) file->Get("PFlow/RecoSteps/btag1/lep_tau/MT/btag_eff_realbs_den_6");
 TH1 * btagEff_fakebs2_num = (TH1*) file->Get("PFlow/RecoSteps/btag1/lep_tau/MT/btag_eff_fakebs2_num_6");
 TH1 * btagEff_fakebs2_den = (TH1*) file->Get("PFlow/RecoSteps/btag1/lep_tau/MT/btag_eff_fakebs2_den_6");


 // get Numerator and denominator /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 double realbs_num_err(0); double realbs_num = btagEff_realbs_num->IntegralAndError(1,btagEff_realbs_num->GetNbinsX(), realbs_num_err );
 double realbs_den_err(0); double realbs_den = btagEff_realbs_den->IntegralAndError(1,btagEff_realbs_den->GetNbinsX(), realbs_den_err ); 

 double fakebs2_num_err(0); double fakebs2_num = btagEff_fakebs2_num->IntegralAndError(1,btagEff_fakebs2_num->GetNbinsX(), fakebs2_num_err );
 double fakebs2_den_err(0); double fakebs2_den = btagEff_fakebs2_den->IntegralAndError(1,btagEff_fakebs2_den->GetNbinsX(), fakebs2_den_err ); 

 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
  

 if(realbs_den){
   cout<<endl<<" Real Bs : numerator is : "<<realbs_num<<" denominator is : "<<realbs_den
       <<" Computed efficiency is : "<<(realbs_num/realbs_den)<<" +/- "<<getErrorFraction(realbs_num ,realbs_den,realbs_num_err,realbs_den_err)<<endl;
 }


 if(fakebs2_den){
   cout<<endl<<" Fake 2 Bs : numerator is : "<<fakebs2_num<<" denominator is : "<<fakebs2_den
       <<" Computed efficiency is : "<<(fakebs2_num/fakebs2_den)<<" +/- "<<getErrorFraction(fakebs2_num ,fakebs2_den,fakebs2_num_err,fakebs2_den_err)<<endl;
 }




}




pair<double, pair<double,double> >CutflowAnalyzer::getNbtags( std::vector<PhysicsObject> & jets, vector<int>  & j_v){

  int nb(0);
  int nq(0);

  // count pre-tag bs or udscg jets ////////////////////////////////////////////////////////////
  for(uint j=0;j!=j_v.size();++j){ 
    int ind = j_v[j]; int pgid(PGID_UNKNOWN); if(!isData_)pgid = TMath::Abs(jets[ind][jetpgid_]);
    if( pgid == PGID_B ){ nb++; }
    else nq++;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////

  //double sfb     = 0.9; double sfl     = 1.08;
  //double sfb_err = 0.1; double sfl_err = 0.11;

  double sfb     = 0.95; double sfq     = 1.11;
  double sfb_err = 0.11; double sfq_err = 0.11;  
  //double eff_b   = 0.803264; double eff_q  = 0.143096; // need to compute values
  double eff_b   = BTAG_eff_R_; double eff_q  = BTAG_eff_F_; // need to compute values
  

  double prob_btags0 =  pow( (1-sfb*eff_b) , nb ) * pow( (1-sfq*eff_q) , nq );
  double prob_btags1 = 1- prob_btags0;

  double unc_tmp_a =  ( nb >0 ? ( nb*sfb_err*eff_b * pow( 1-sfb*eff_b , nb -1)* pow( 1-sfq*eff_q , nq  )) : 0 );   
  double unc_tmp_b =  ( nq >0 ? ( nq*sfq_err*eff_q * pow( 1-sfb*eff_b , nb   )* pow( 1-sfq*eff_q , nq-1)) : 0 ); 

  double unc = sqrt( unc_tmp_a*unc_tmp_a + unc_tmp_b*unc_tmp_b );

  pair< double, double > probs(prob_btags0,prob_btags1);

  pair< double, pair< double, double> > ret(unc, probs);

  return ret;


}





// uncertainty and weight for the 2b jet case
pair< double,double > CutflowAnalyzer::get2Nbtags( std::vector<PhysicsObject> & jets, vector<int>  & j_v){


//  double eb = 0.803264; 
//  double el = 0.143096;

  double eb = BTAG_eff_R_;
  double el = BTAG_eff_F_;

  //double sfb     = 0.9; double sfl     = 1.08;
  //double sfb_err = 0.1; double sfl_err = 0.11;
  double p0,p1,p2,p0_err,p1_err,p2_err;

  double sfb     = 0.95; double sfl     = 1.11;
  double sfb_err = 0.06; double sfl_err = 0.11;

  if(run2012_){
    sfb=0.97; sfb_err=0.04;
    sfl=1.10; sfb_err=0.11; 
  }


  int nb(0);
  int nl(0);

  // count pre-tag bs or udscg jets ////////////////////////////////////////////////////////////
  for(uint j=0;j!=j_v.size();++j){ 
    int ind = j_v[j]; int pgid(PGID_UNKNOWN); if(!isData_)pgid = TMath::Abs(jets[ind][jetpgid_]);
    if( pgid == PGID_B ){ nb++; }
    else nl++;
  }
  //////////////////////////////////////////////////////////////////////////////////////////////
  


  //central values (based on the MC b-tag/mistag rates + data/MC scale factors)
  p0 = pow(1-sfb*eb,nb)*pow(1-sfl*el,nl);
     
  p1 = (nb>0 ? nb*sfb*eb*pow(1-sfb*eb,nb-1)*pow(1-sfl*el,nl) : 0);
  p1 += (nl>0 ? nl*sfl*el*pow(1-sfb*eb,nb)*pow(1-sfl*el,nl-1) : 0);
      
  p2 = 1-p0-p1;
      
  //errors (quadratic propagation of the uncertainty of the scale factor)
  p0_err = (nb>0 ? pow(nb*sfb_err*eb*pow(1-sfb*eb,nb-1)*pow(1-sfl*el,nl),2)  : 0. );
  p0_err += (nl>0 ? pow(nl*sfl_err*el*pow(1-sfb*eb,nb)*pow(1-sfl*el,nl-1),2) : 0. );
  p0_err = sqrt(p0_err);

  p1_err = (nb>0 ? pow(nb*sfb_err*eb*pow(1-sfb*eb,nb-1)*pow(1-sfl*el,nl),2) : 0.);
  p1_err += (nb>1 ? pow(nb*(nb-1)*sfb_err*sfb*pow(eb,2)*pow(1-sfb*eb,nb-2)*pow(1-sfl*el,nl),2) : 0.);
  p1_err += (nl>0 ? pow(nl*sfl_err*el*pow(1-sfb*eb,nb)*pow(1-sfl*el,nl-1),2) : 0.);
  p1_err += (nl>1 ? pow(nl*(nl-1)*sfl_err*sfl*pow(el,2)*pow(1-sfb*eb,nb)*pow(1-sfl*el,nl-2),2) : 0. );
  p1_err = sqrt(p1_err);
      
  p2_err = sqrt( pow(p1_err,2) + pow(p0_err,2) );

  pair<double,double> ret(p2_err,p2);

  return ret;

}

  

void CutflowAnalyzer::computeMHT( std::vector<PhysicsObject> & jets,  vector<int>  & jetsForMHT, PhysicsObject & lepton ){


  mht_= -1; // value before MHT computation

  // MHT defined as the vectorial sum of jets and leptons
  // note on selected events we only consider one lepton
  TLorentzVector mH(lepton); 
  for( uint i=0; i< jetsForMHT.size(); ++i){ 
    int ind = jetsForMHT[i]; mH += jets[ind];
  }
  mht_ = mH.Pt();

}



void CutflowAnalyzer::computeMHTb(  std::vector<PhysicsObject> & jets,  vector<int>  & jetsForMHT ){

  mhtb_= -1; // value before MHT computation

  // MHT defined as the vectorial sum of jets and leptons
  // note on selected events we only consider one lepton
  TLorentzVector mH; 
  for( uint i=0; i< jetsForMHT.size(); ++i){    
    int ind = jetsForMHT[i]; mH += jets[ind];
  }
  mhtb_ = mH.Pt();

}


//checks if in the list of jets the two highest pt jets without taking into consideration the excluded jet are triggered matched
// note this should not be applyied befre trigger efficiency studies

/*
bool CutflowAnalyzer::jetMatchCondition( needs junc , std::vector<PhysicsObject> & jets, vector<int>  & selectedjets, int excluded ){

  bool jetMatch(false);

  double jetPt1(0),jetPt2(0); int ind2, ind1;
  for(int i=0; i< selectedjets.size(); i++){
    int ind=selectedjets[i];
    if (i == excluded )continue;
    double jetPt =  getJetPt( jets[ind], junc,0, jes_);
    if( jetPt > jetPt1       ){ jetPt2=jetPt1; ind2=ind1; jetPt1=jetPt; ind1=ind; }
    else if( jetPt > jetPt2  ){ jetPt2=jetPt;  ind2=ind;                          }
  }

  if(jetPt2){
    //numerator
    double triggeredJet1 = jets[ind1][4];
    double triggeredJet2 = jets[ind2][4];
    //if(triggeredJet1 && triggeredJet2){ jetMatch = true; }
    if( (triggeredJet1 >= 25 && triggeredJet2 >= 30) || (triggeredJet1 >= 30 && triggeredJet2 >= 25) ){ jetMatch = true;}
  }
  
  return jetMatch;


}

//gives the efficiency of the lowest pt jet of the two leading jets without the excluded jets 
double CutflowAnalyzer::effOnMatchCondition( needs junc, std::vector<PhysicsObject> & jets, vector<int>  & selectedjets, int excluded ){

  double eff(1.);

  double jetPt1(0),jetPt2(0); int ind2, ind1;
  for(int i=0; i< selectedjets.size(); i++){
    int ind=selectedjets[i];
    if (i == excluded )continue;
    double jetPt =  getJetPt( jets[ind], junc,0, jes_);
    if( jetPt > jetPt1       ){ jetPt2=jetPt1; ind2=ind1; jetPt1=jetPt; ind1=ind; }
    else if( jetPt > jetPt2  ){ jetPt2=jetPt;  ind2=ind;                          }
  }

  if(jetPt2){
    //set match efficiency to value that was get from the efficiency studies
  }
  
  return eff;


}
*/


pair<double,double> CutflowAnalyzer::getEfficiencyAndError(double pt,double A0,double errA0, double A1, double errA1,double A2, double errA2){

  double err;

  double sqrt2pi = sqrt(2*3.14159265);

  double temp1 = exp( (-1)*pow( (pt-A0)/(sqrt(2)*A1),2 ));
  
  double term1 = pow( temp1*(   A2/(sqrt2pi*A1)   ) ,2)*pow(errA0,2);

  double term2 = pow( temp1*(  A2*(pt-A0)/(sqrt2pi*A1*A1) ) , 2)* pow(errA1,2);

  double term3 = pow( 0.5*( 1 + TMath::Erf( (pt - A0)/( sqrt(2)*A1) ) ) ,2)*pow(errA2,2);

  err = sqrt (term1+term2+term3);

  double value = 0.5*A2*( 1 + TMath::Erf( (pt - A0)/( sqrt(2)*A1) ));

  //cout<<endl<<"\n DEBUG : jet pt is : "<<setprecision(4)<<pt<<" eff is "<<value<<" error is "<<err<<endl;

  pair<double,double> res(value,err);

  return res;


}




// get trigger selection efficiency and associated error
pair<double,double> CutflowAnalyzer::getEfficiencyAndError( std::vector<PhysicsObject> & jets,  vector<int>  & j_final, JetCorrectionUncertainty * junc, vector<double> & jerFactors ){

  pair<double,double> result(0,0);

  // ind1-> leading jet pt, ind2-> next to leading jet pt
  double jetPt1(0),jetPt2(0); int ind2(-1), ind1(-1);
  for(uint i=0; i<j_final.size(); ++i){
    int ind=j_final[i]; double jetPt =  getJetPt( jets[ind], junc,0, jes_);
    if( jetPt > jetPt1       ){ jetPt2=jetPt1; ind2=ind1; jetPt1=jetPt; ind1=ind; }
    else if( jetPt > jetPt2  ){ jetPt2=jetPt;  ind2=ind;                          }
  }

 
  //if(jetPt1 && jetPt1<40) cout<<" Debug jetP1 : "<<jetPt1<<" pass : "<<(jets[ind1][4])<<endl;
  //if(jetPt2 && jetPt2<40) cout<<" Debug jetP2 : "<<jetPt2<<" pass : "<<(jets[ind2][4])<<endl;
 

  // if we are processing data, event is selected only if we have the first two jets firing the trigger
  if( isData_ && jetPt2 && (jets[ind1][4]) && (jets[ind2][4]) ) result.first = 1; 

  else if(! isData_ && jetPt2){

    pair<double,double> set1 = getEfficiencyAndError( jetPt1, A0_1_ ,errA0_1_,A1_1_,errA1_1_, A2_1_, errA2_1_);
    pair<double,double> set2 = getEfficiencyAndError( jetPt2, A0_2_ ,errA0_2_,A1_2_,errA1_2_, A2_2_, errA2_2_);

    double eff1 = (set1.first); double eff1_err = (set1.second);
    double eff2 = (set2.first); double eff2_err = (set2.second);

    result.first  = eff1*eff2; // efficiency
    result.second = sqrt( pow(eff2,2)*pow(eff1_err,2) + pow(eff1,2)*pow(eff2_err,2) );


    //cout<<endl<<" eff1      = "<<setprecision (5)<<eff1<<" +- "<<eff1_err<<" eff2 = "<<eff2<<" +- "<<eff2_err<<endl;
    //cout<<endl<<" eff1*eff2 = "<<(result.first)<<" +- "<<result.second<<endl;


  }

  return result;

}





void CutflowAnalyzer::setSelectionParameters(){

  // parameters to be used in the electron+2jets+mht scenario //////////////////////////////////
  //highest jet pt
  A0_1_= 18.33; errA0_1_=2.555;  A1_1_=16.63;  errA1_1_=1.952;  A2_1_=0.9708;  errA2_1_=0.002003;
  //second jet pt
  A0_2_= 20.77; errA0_2_=0.7723; A1_2_=10.83;  errA1_2_=0.6287; A2_2_=0.9821;  errA2_2_=0.001287;
  ///////////////////////////////////////////////////////////////////////////////////////////////


  if(       eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETSPLUSMET_ ){ MET_CUT_= 45; JET_PT_MIN_=35; JET_PT_CUT_=35; E_ET_MIN_=35;  } 
  else if(  eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_        ){ MET_CUT_= 40; JET_PT_MIN_=30; JET_PT_CUT_=30; E_ET_MIN_=70;  }
  //else if(  eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_      ){ MET_CUT_= 45; JET_PT_MIN_=35; JET_PT_CUT_=35; E_ET_MIN_=35;  }
  if(      !eChONmuChOFF_                                        ){ MET_CUT_= 40; JET_PT_MIN_=30; JET_PT_CUT_=30; E_ET_MIN_=30;  }  

  //WARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNINGWARNING
  //if(      !eChONmuChOFF_                                        ){ MET_CUT_= 45; JET_PT_MIN_=35; JET_PT_CUT_=35; E_ET_MIN_=35;  }  // delete this using same thresholds in etau mutau


/*
  // Setting MET to zero in order to study OS efficiency
  if(       eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETSPLUSMET_ ){ MET_CUT_= 0; JET_PT_MIN_=35; JET_PT_CUT_=35; E_ET_MIN_=35;  } 
  else if(  eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_        ){ MET_CUT_= 0; JET_PT_MIN_=30; JET_PT_CUT_=30; E_ET_MIN_=70;  }
  //else if(  eChONmuChOFF_ &&  MODE_ == STARTING_AT_LJETS_      ){ MET_CUT_= 45; JET_PT_MIN_=35; JET_PT_CUT_=35; E_ET_MIN_=35;  }
  if(      !eChONmuChOFF_                                        ){ MET_CUT_= 0; JET_PT_MIN_=30; JET_PT_CUT_=30; E_ET_MIN_=30;  }                                          
*/

}


void CutflowAnalyzer::evaluatePDFUncertainty(){

  cout.precision(4);

  cout<<endl<<"\n ========================================================= ";
  if (originalPDFEvents_  == 0 ) {  cout<<"\n PDFAnalysis :: NO EVENTS => NO RESULTS "<<endl;          return; }      
  if ( PDFSelectedEvents_ == 0 ) {  cout<<"\n PDFAnalysis :: NO SELECTED EVENTS => NO RESULTS "<<endl; return; }

  cout<< "\n PDFAnalysis :: Begin of PDF weight systematics summary >>>>"
      << "\n PDFAnalysis :: Total number of analyzed data: " << originalPDFEvents_ << " [events]";

  double originalAcceptance = PDFSelectedEvents_/originalPDFEvents_;
  cout<< "\n PDFAnalysis :: Total number of selected data: " << PDFSelectedEvents_ << " [events], corresponding to acceptance: [" << std::setprecision(4)<<originalAcceptance*100. << " +- "  << std::setprecision(4)<<100.*sqrt( originalAcceptance*(1.-originalAcceptance)/originalPDFEvents_) << "] %";

  cout<<"\n PDFAnalysis :: >>>>> PDF UNCERTAINTIES ON ACCEPTANCE >>>>>>";

// for (unsigned int i=0; i<pdfWeightTags_.size(); ++i) {
// bool nnpdfFlag = (pdfWeightTags_[i].instance().substr(0,5)=="NNPDF");

  bool nnpdfFlag(false);

  unsigned int nmembers = weightedPDFEvents_.size();

  unsigned int npairs = (nmembers-1)/2;

  cout<<"\n PDFAnalysis :: ACCEPTANCE Results for PDF ---->";

  double acc_central(0); 
  double acc2_central(0); 
  double waverage(0);    
 
  if( weightedPDFEvents_[0]>0){
    acc_central   = weightedPDFSelectedEvents_[0]/weightedPDFEvents_[0]; 
    acc2_central  = weighted2PDFSelectedEvents_[0]/weightedPDFEvents_[0]; 
  }
  
  waverage      = weightedPDFEvents_[0]/originalPDFEvents_;

  cout<< "\n PDFAnalysis :: \tEstimate for central PDF member acceptance: [" << std::setprecision(4)<<acc_central*100. << " +- " <<std::setprecision(4)<<100.*sqrt((acc2_central/waverage-acc_central*acc_central)/originalPDFEvents_)<< "] %";
  double xi = acc_central-originalAcceptance;
  double deltaxi = (acc2_central-(originalAcceptance+2*xi+xi*xi))/originalPDFEvents_;
  if (deltaxi>0) deltaxi = sqrt(deltaxi); //else deltaxi = 0.;
  cout<< "\n PDFAnalysis ::  \ti.e. [" << std::setprecision(4) << 100.*xi/originalAcceptance << " +- " << std::setprecision(4) << 100*deltaxi/originalAcceptance << "] % relative variation with respect to the original PDF";

  if (npairs>0) {
    cout<< "\n PDFAnalysis :: \tNumber of eigenvectors for uncertainty estimation: " << npairs;
    double wplus = 0.;
    double wminus = 0.;
    unsigned int nplus = 0;
    unsigned int nminus = 0;
    for (unsigned int j=0; j<npairs; ++j) {
      double wa = 0.;
      if ( weightedPDFEvents_[2*j+1]>0 ) wa = ( weightedPDFSelectedEvents_[2*j+1]/weightedPDFEvents_[2*j+1])/acc_central-1.;
      double wb = 0.;
      if ( weightedPDFEvents_[2*j+2]>0 ) wb = ( weightedPDFSelectedEvents_[2*j+2]/weightedPDFEvents_[2*j+2])/acc_central-1.;
      if ( nnpdfFlag ) {
        if (wa>0.) { wplus += wa*wa; nplus++; } else { wminus += wa*wa; nminus++; }
        if (wb>0.) { wplus += wb*wb; nplus++; } else { wminus += wb*wb; nminus++; }
      } else {
        if (wa>wb) { 
          if (wa<0.) wa = 0.; if (wb>0.) wb = 0.;
          wplus += wa*wa; wminus += wb*wb;
        } else {
          if (wb<0.) wb = 0.; if (wa>0.) wa = 0.;
          wplus += wb*wb; wminus += wa*wa;
        }
      }
    }

    if (wplus>0) wplus = sqrt(wplus);
    if (wminus>0) wminus = sqrt(wminus);
    if (nnpdfFlag) {
      if (nplus>0)  wplus /= sqrt(nplus);
      if (nminus>0) wminus /= sqrt(nminus);
    }
    
    cout<<"\n PDFAnalysis :: \tRelative uncertainty with respect to central member: +" << std::setprecision(4) << 100.*wplus << " / -" << std::setprecision(4) << 100.*wminus << " [%]";

  } else { cout<< "\n PDFAnalysis :: \tNO eigenvectors for uncertainty estimation"; }
   
  cout<< "\n PDFAnalysis :: >>>> End of PDF weight systematics summary >>>>";
  


  cout<<endl<<"=======================================";
  
 double deltaPlus(0),deltaMinus(0);

 cout<<"\n Final weights :<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"<<endl;
 
 cout<<endl<<" weight zero is : "<<myWeights_[0]<<endl;

 for(uint i=1;i<myWeights_.size();++i){
   
   cout<<"\n myWeights i = "<<i<<myWeights_[i]<<endl;

   if(i%2==1 && myWeights_[i] !=0 )          deltaPlus += pow(myWeights_[i]-myWeights_[0],2);
   else if( i%2==0 && myWeights_[i] !=0 )    deltaMinus += pow(myWeights_[i]-myWeights_[0],2);

   cout<<endl<<" delta plus  is : "<<deltaPlus<<endl;
   cout<<endl<<" delta minus is : "<<deltaMinus<<endl;


 }


 cout << "\n PDF unc. +" << 100.*sqrt(deltaPlus)/myWeights_[0] << "% -" <<  100.*sqrt(deltaMinus)/myWeights_[0] << " %" << endl;


}


//value, uncertainty of btagSF
pair<double,double> CutflowAnalyzer::btagSF(double jetPt){


  double err_sf(0);

  // 2012 prompt reco payload
       if( jetPt>30  && jetPt<=40  ){ err_sf = SFb_error[0];  }
  else if( jetPt>40  && jetPt<=50  ){ err_sf = SFb_error[1];  }
  else if( jetPt>50  && jetPt<=60  ){ err_sf = SFb_error[2];  }
  else if( jetPt>60  && jetPt<=70  ){ err_sf = SFb_error[3];  }
  else if( jetPt>70  && jetPt<=80  ){ err_sf = SFb_error[4];  }
  else if( jetPt>80  && jetPt<=100 ){ err_sf = SFb_error[5];  }
  else if( jetPt>100 && jetPt<=120 ){ err_sf = SFb_error[6];  }
  else if( jetPt>120 && jetPt<=160 ){ err_sf = SFb_error[7];  }
  else if( jetPt>160 && jetPt<=210 ){ err_sf = SFb_error[8];  }
  else if( jetPt>210 && jetPt<=260 ){ err_sf = SFb_error[9];  }
  else if( jetPt>260 && jetPt<=320 ){ err_sf = SFb_error[10]; }
  else if( jetPt>320 && jetPt<=400 ){ err_sf = SFb_error[11]; }
  else if( jetPt>400 && jetPt<=500 ){ err_sf = SFb_error[12]; }
  else if( jetPt>500 && jetPt<=670 ){ err_sf = SFb_error[13]; }
  else if( jetPt> 670 ){ 
   //use the SFb value at 670 GeV with twice the quoted uncertainty 
   jetPt=670; err_sf = 2*SFb_error[13];
  }
       
//  // 2012 22Jan2013ReReco payload https://twiki.cern.ch/twiki/pub/CMS/BtagPOG/SFb-pt_NOttbar_payload_EPS13.txt
//       float ptmin[] = {20, 30, 40, 50, 60, 70, 80, 100, 120, 160, 210, 260, 320, 400, 500, 600};
//       float ptmax[] = {30, 40, 50, 60, 70, 80,100, 120, 160, 210, 260, 320, 400, 500, 600, 800};
//       
//       if( jetPt>20  && jetPt<=30  ){ err_sf = SFb_error[0];  }
//  else if( jetPt>30  && jetPt<=40  ){ err_sf = SFb_error[1];  }
//  else if( jetPt>40  && jetPt<=50  ){ err_sf = SFb_error[2];  }
//  else if( jetPt>50  && jetPt<=60  ){ err_sf = SFb_error[3];  }
//  else if( jetPt>60  && jetPt<=70  ){ err_sf = SFb_error[4];  }
//  else if( jetPt>70  && jetPt<=80 ){ err_sf = SFb_error[5];  }
//  else if( jetPt>80 && jetPt<=100 ){ err_sf = SFb_error[6];  }
//  else if( jetPt>100 && jetPt<=120 ){ err_sf = SFb_error[7];  }
//  else if( jetPt>120 && jetPt<=160 ){ err_sf = SFb_error[8];  }
//  else if( jetPt>160 && jetPt<=210 ){ err_sf = SFb_error[9];  }
//  else if( jetPt>210 && jetPt<=260 ){ err_sf = SFb_error[10]; }
//  else if( jetPt>260 && jetPt<=320 ){ err_sf = SFb_error[11]; }
//  else if( jetPt>320 && jetPt<=400 ){ err_sf = SFb_error[12]; }
//  else if( jetPt>400 && jetPt<=500 ){ err_sf = SFb_error[13]; }
//  else if( jetPt>500 && jetPt<=600 ){ err_sf = SFb_error[14]; }
//  else if( jetPt>600 && jetPt<=800 ){ err_sf = SFb_error[15]; }
//  else if( jetPt> 800 ){ 
//   //use the SFb value at 670 GeV with twice the quoted uncertainty 
//   jetPt=670; err_sf = 2*SFb_error[15];
//  }
//



  double sf = 0.603913*((1.+(0.286361*jetPt))/(1.+(0.170474*jetPt)));

  if(jetPt<=30){  err_sf =  0.12/sf; }

  pair<double,double> val(sf,err_sf);

  return val;


}



