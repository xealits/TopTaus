#include "LIP/TopTaus/interface/SampleProcessor.hh"

// System headers
#include <iostream>
#include <iomanip>
#include <fstream>



void SampleProcessor::init(vector<TString> listOfurls, vector<double> listOfXSections, bool fullStats, double lum, uint n_data){

  
  /*
    
  // QCD MVA
  qcdMVA_   = new MVAComputer("/u/home/cms/vischia/public/tau_dilepton/mvaWeights_428TrainedDiJetDataPFlow_pt20.mva");
  
  // W plus jets MVA
  wjetsMVA_ =  new MVACompute("/u/home/cms/vischia/public/tau_dilepton/mvaWeights_428TrainedWMuDataPFlow_pt20.mva");
  
  */
  
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
  ///////////////////////////////////////////////////////////////////

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
    listOfScales_[i] = (lum_*listOfXSections_[i])/listOfEvents_[i];   

    if( listOfXSections_[i] == 0){ listOfScales_[i]=1;}

    listOfReaders_[i]     = new event::Reader(); 
    listOfReadersMC_[i]   = new event::Reader();

    listOfEventsToProcess_[i]   = (listOfReaders_[i])->AssignMiniEventTreeFrom(listOfFiles_[i],tag,id);
    listOfEventsToProcessMC_[i] = (listOfReadersMC_[i])->AssignMiniEventTreeFrom(listOfFiles_[i],"mc",id);


    //SPY ///////////////////////////////////////////////////////////
    TString extra;
    if(eChONmuChOFF_) extra=TString("_spy_eltau.root");
    else extra=TString("_spy_mutau.root");
    TString spyName = outFileName_ + extra;
    //TTree * spyTree = listOfReaders_[i]->PrepareToSpyEvents(spyName);
    //cout<<endl<<"Spy Events ... on file ... "<<spyName<<endl;
    //////////////////////////////////////////////////////////////////

    if(fullStats_){
      cout<<endl<<"\n processing file : "<<(listOfurls_[i])
          <<"\n Tau pt cut was      : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<sampleTauPtCut_
          <<"\n sample -> scale     : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<(listOfScales_[i])
          <<"\n evt processed       : "<<setw(8)<<setprecision(1)<<(listOfEvents_[i])
          <<"\n evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i])
          <<"\n MC evt in minitree  : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcessMC_[i])
          <<"\n computed X sections : "<<(listOfXSections_[i])<<endl;

     infoFile_<<endl<<"\n processing file : "<<(listOfurls_[i])
          <<"\n Tau pt cut was      : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<sampleTauPtCut_
          <<"\n sample -> scale     : "<<setw(12)<<setiosflags(ios::fixed) << setprecision(8)<<(listOfScales_[i])
          <<"\n evt processed       : "<<setw(8)<<setprecision(1)<<(listOfEvents_[i])
          <<"\n evt in minitree     : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcess_[i])
          <<"\n MC evt in minitree  : "<<setw(8)<<setprecision(1)<<(listOfEventsToProcessMC_[i])
          <<"\n computed X sections : "<<(listOfXSections_[i])<<endl;
 
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
      if(applybtagweight_){  infoFile_<<" BTAG weights are ON  ";      cout<<" BTAG weights are ON ";       }
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
  else if( urlCode == TBH250_URL               ){  ret = TBH_CH;       }                   
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
  else if( i == EE_CH || i == EMU_CH || i == MUMU_CH ){  ret = LEPLEP_TDCH;    }
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





void SampleProcessor::process_ttbar(){

  //APLGEN TTBAR
  //process(false, TT0J_URL, iFolder_ + TString("TT0Jets_Alpgen.root"), oFolder_ + TString("out-TT0_Alpgen.root",keys_); 
  //process(false, TT1J_URL, iFolder_ + TString("TT1Jets_Alpgen.root"), oFolder_ + TString("out-TT1_Alpgen.root",keys_); 
  //process(false, TT2J_URL, iFolder_ + TString("TT2Jets_Alpgen.root"), oFolder_ + TString("out-TT2_Alpgen.root",keys_); 
  //process(false, TT3J_URL, iFolder_ + TString("TT3Jets_Alpgen.root"), oFolder_ + TString("out-TT3_Alpgen.root",keys_); 
  //process(false, TT4J_URL, iFolder_ + TString("TT4Jets_Alpgen.root"), oFolder_ + TString("out-TT4_Alpgen.root",keys_); 
  //MADGRAPH
  //process(false, TTBAR_URL, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar-madgraph.root"),keys_);                          
  //process(false, TTBAR_URL, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar-signal-madgraph.root"),keys_, TTBAR_SIGNAL_ );   
  //process(false, TTBAR_URL, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar-other-madgraph.root"),keys_, TTBAR_OTHER_ );     

  url_= TTBAR_URL; 


  //NORMAL SAMPLE
  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar.root"),keys_);
  //process(false, url_, iFolder_ + TString("AllTTJets_TuneZ2star_8TeV-madgraph-tauola.root"), oFolder_+TString("out-ttbar.root"),keys_);
  
  
  

  //SAMPLE WITH WEIGHTS
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  //pdfweights_=true; process(false, url_, iFolder_ + TString("ttbar-pdf-nofilter-B.root"), oFolder_+TString("out-ttbar-pdf.root"),keys_); pdfweights_=false;
  ///lustre/data3/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3-B/ttbar-pdf-nofilter-B.root


  // Trigger efficiencies study ////////////////////////////////////////////////////////////////////////
  //iFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3/");
  //process(false, url_, iFolder_ + TString("trig-ttbar.root"), oFolder_+TString("out-ttbar.root"),keys_);
  //////////////////////////////////////////////////////////////////////////////////////////////////////    





}




void SampleProcessor::process_ttbar_unc(){

  url_= TTBAR_URL;
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



void SampleProcessor::process_ttbar_bychannel(){

  url_= TTBAR_URL;


  //  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar_etau.root"),keys_,  ETAU_  );    
  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar_mutau.root"),keys_, MUTAU_ ); 
  
   
  // dd bkg
  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar_ddbkg.root"),keys_, TTBAR_DDBKG_);   

  // mc bkg
  process(false, url_, iFolder_ + TString("ttbar.root"), oFolder_+TString("out-ttbar_mcbkg.root"),keys_, TTBAR_MCBKG_ );


}


void SampleProcessor::process_singletop(){

  
  //SINGLE TOP
  url_= S_URL; process(false, url_, iFolder_ + TString("stop_s.root"),     oFolder_ + TString("out-stop_s.root"),keys_);   
  url_= T_URL; process(false, url_, iFolder_ + TString("stop_t.root"),     oFolder_ + TString("out-stop_t.root"),keys_);  
  url_= W_URL; process(false, url_, iFolder_ + TString("stop_tW-DR.root"), oFolder_ + TString("out-stop-DR_tW.root"),keys_); 

  //ANTI SINGLE TOP
  url_= A_S_URL; process(false, url_, iFolder_ + TString("stopbar_s.root"),     oFolder_ + TString("out-stopbar_s.root"),keys_);   
  url_= A_T_URL; process(false, url_, iFolder_ + TString("stopbar_t.root"),     oFolder_ + TString("out-stopbar_t.root"),keys_);  
  url_= A_W_URL; process(false, url_, iFolder_ + TString("stopbar_tW-DR.root"), oFolder_ + TString("out-stopbar-DR_tW.root"),keys_);   


}


void SampleProcessor::process_wjets(){

 

  // WARNING W+jets with new pgid info
  url_= WJMADGRAPH_URL;
  process(false, url_, iFolder_ + TString("WJetsToLNu.root"), oFolder_+TString("out-wjets.root"),keys_);


  // Trigger efficiencies study //////////////////////////////////////////////////////////////////////////
  // iFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3/");
  // process(false, url_, iFolder_ + TString("trig-WJetsToLNu.root"), oFolder_+TString("out-wjets.root"),keys_);
  //////////////////////////////////////////////////////////////////////////////////////////////////////////



}

void SampleProcessor::process_zjets(){

  // MADGRAPH Z+JETS
  // Not available ??????????????????
  //url_ = DY_10TO50_URL;
  //process(false, url_, iFolder_ + TString("dy_10to50.root"), oFolder_+TString("out-dy_10to50.root"),keys_);    

  url_ = DY_FROM50_URL; 
  process(false, url_, iFolder_ + TString("dy_from50.root"), oFolder_+TString("out-dy_from50.root"),keys_); 

}

void SampleProcessor::process_hh_higgs(){

  url_ = HH80_URL;  process(false, url_, iFolder_ + TString("hh-pythia-m80.root"),  oFolder_+TString("out-hh-pythia-m80.root"),keys_);  
  url_ = HH100_URL; process(false, url_, iFolder_ + TString("hh-pythia-m100.root"), oFolder_+TString("out-hh-pythia-m100.root"),keys_); 
  url_ = HH120_URL; process(false, url_, iFolder_ + TString("hh-pythia-m120.root"), oFolder_+TString("out-hh-pythia-m120.root"),keys_); 
  url_ = HH140_URL; process(false, url_, iFolder_ + TString("hh-pythia-m140.root"), oFolder_+TString("out-hh-pythia-m140.root"),keys_); 
  url_ = HH150_URL; process(false, url_, iFolder_ + TString("hh-pythia-m150.root"), oFolder_+TString("out-hh-pythia-m150.root"),keys_); 
  url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m155.root"), oFolder_+TString("out-hh-pythia-m155.root"),keys_); 
  url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m160.root"), oFolder_+TString("out-hh-pythia-m160.root"),keys_); 


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
  url_ = HH155_URL; process(false, url_, iFolder_ + TString("hh-pythia-m155.root"), oFolder_+TString("out-hh-pythia-m155_mutau.root"),keys_, MUTAU_); 
  url_ = HH160_URL; process(false, url_, iFolder_ + TString("hh-pythia-m160.root"), oFolder_+TString("out-hh-pythia-m160_mutau.root"),keys_, MUTAU_); 

}


void SampleProcessor::process_wh_higgs(){

  url_ = WH80_URL;  process(false, url_, iFolder_ + TString("wh-pythia-m80.root"), oFolder_+TString("out-wh-pythia-m80.root"),keys_  ); 
  url_ = WH100_URL; process(false, url_, iFolder_ + TString("wh-pythia-m100.root"), oFolder_+TString("out-wh-pythia-m100.root"),keys_); 
  url_ = WH120_URL; process(false, url_, iFolder_ + TString("wh-pythia-m120.root"), oFolder_+TString("out-wh-pythia-m120.root"),keys_); 
  url_ = WH140_URL; process(false, url_, iFolder_ + TString("wh-pythia-m140.root"), oFolder_+TString("out-wh-pythia-m140.root"),keys_); 
  url_ = WH150_URL; process(false, url_, iFolder_ + TString("wh-pythia-m150.root"), oFolder_+TString("out-wh-pythia-m150.root"),keys_); 
  url_ = WH155_URL; process(false, url_, iFolder_ + TString("wh-pythia-m155.root"), oFolder_+TString("out-wh-pythia-m155.root"),keys_); 
  url_ = WH160_URL; process(false, url_, iFolder_ + TString("wh-pythia-m160.root"), oFolder_+TString("out-wh-pythia-m160.root"),keys_); 


//    -rw-r--r-- 1 vischia cms  1.9M Aug 28 15:22 wh-pythia-m100.root
//    -rw-r--r-- 1 vischia cms  144M Aug 28 15:22 wh-pythia-m140.root
//    -rw-r--r-- 1 vischia cms  136M Aug 28 15:22 wh-pythia-m155.root
//    -rw-r--r-- 1 vischia cms  135M Aug 28 15:23 wh-pythia-m160.root
//    -rw-r--r-- 1 vischia cms  105M Aug 28 15:23 hh-pythia-m140.root



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
  url_ = TBH250_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m250.root"), oFolder_+TString("out-tbh-pythia-m250_mutau.root"),keys_, MUTAU_); 
  //tbh-pythia-m190.root  tbh-pythia-m200.root  tbh-pythia-m220.root  tbh-pythia-m250.root
}


void SampleProcessor::process_tbh_higgs(){
  //tbh-pythia-m190.root  tbh-pythia-m200.root  tbh-pythia-m220.root  tbh-pythia-m250.root
  url_ = TBH250_URL; process(false, url_, iFolder_ + TString("tbh-pythia-m250.root"), oFolder_+TString("out-tbh-pythia-m250.root"),keys_); 

}


void SampleProcessor::process_qcd(){

     
  url_= QCD_EM30TO80_URL;     process(false,url_, iFolder_ + TString("qcd_EM_Pt30to80.root"),     oFolder_ + TString("out-qcd_EM_Pt30to80.root"),keys_); 
  url_= QCD_EM80TO170_URL;    process(false,url_, iFolder_ + TString("qcd_EM_Pt80to170.root"),    oFolder_ + TString("out-qcd_EM_Pt80to170.root"),keys_);    
  url_= QCD_BCTOE30TO80_URL;  process(false,url_, iFolder_ + TString("qcd_30to80_BCtoE.root"),    oFolder_ + TString("out-qcd_30to80_BCtoE.root"),keys_);  
  url_= QCD_BCTOE80TO170_URL; process(false,url_, iFolder_ + TString("qcd_80to170_BCtoE.root"),   oFolder_ + TString("out-qcd_80to170_BCtoE.root"),keys_);  
 
  url_= QCD_PHOTON30TO50_URL;   process(false,url_, iFolder_ + TString("PhotonJets_30to50.root"),   oFolder_ + TString("out-PhotonJets_30to50.root"),keys_);  
  url_= QCD_PHOTON50TO80_URL;   process(false,url_, iFolder_ + TString("PhotonJets_50to80.root"),   oFolder_ + TString("out-PhotonJets_50to80.root"),keys_);  
  url_= QCD_PHOTON80TO120_URL;  process(false,url_, iFolder_ + TString("PhotonJets_80to120.root"),  oFolder_ + TString("out-PhotonJets_80to120.root"),keys_); 
  url_= QCD_PHOTON120TO170_URL; process(false,url_, iFolder_ + TString("PhotonJets_120to170.root"), oFolder_ + TString("out-PhotonJets_120to170.root"),keys_); 

  // mu + jets specific samples
  url_= QCD_MU_URL; process(false, url_, iFolder_ + TString("qcdmu15_20toinf.root"), oFolder_ + TString("out-qcdmu15_20toinf.root"),keys_);   

 
 
}

void SampleProcessor::process_dibosons(){

  //iFolder_ = TString("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/mTrees-v3/");  

  url_= WW_URL; process(false, url_, iFolder_ + TString("WW.root"), oFolder_+TString("out-ww.root"),keys_); 
  url_= WZ_URL; process(false, url_, iFolder_ + TString("WZ.root"), oFolder_+TString("out-wz.root"),keys_);
  url_= ZZ_URL; process(false, url_, iFolder_ + TString("ZZ.root"), oFolder_+TString("out-zz.root"),keys_);

}



void SampleProcessor::process_data_RunA(){

  url_ = DATA_URL;


  // DON't FORGET TO SET MUTAU ON
  if( ! eChONmuChOFF_ && ! run2012_){ 
    // Muon inclusive samples

    process(true,url_,iDFolder_+TString("Muon_RunA_ReReco.root"),oDFolder_+TString("out-Muon_RunA_ReReco.root"),keys_);
    //  process(true,url_,iDFolder_+TString("Muon_RunB_ReReco.root"),oDFolder_+TString("out-Muon_RunB_ReReco.root"),keys_);
//    //RUN 2011A
//    process(true,url_,iDFolder_+TString("Muon_1.root"),oDFolder_+TString("out-Muon_1.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_2.root"),oDFolder_+TString("out-Muon_2.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_3.root"),oDFolder_+TString("out-Muon_3.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_4.root"),oDFolder_+TString("out-Muon_4.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_5.root"),oDFolder_+TString("out-Muon_5.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_6.root"),oDFolder_+TString("out-Muon_6.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_7.root"),oDFolder_+TString("out-Muon_7.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_8.root"),oDFolder_+TString("out-Muon_8.root"),keys_);
//
//    process(true,url_,iDFolder_+TString("Muon_9.root"),oDFolder_+TString("out-Muon_9a.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_9_05Aug.root"),oDFolder_+TString("out-Muon_9b.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_9_v5.root"),oDFolder_+TString("out-Muon_9c.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_10.root"),oDFolder_+TString("out-Muon_10.root"),keys_);
//
//
//    //RUN 2011B
//    process(true,url_,iDFolder_+TString("Muon_RunB_1.root"),oDFolder_+TString("out-Muon_RunB_1.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_2.root"),oDFolder_+TString("out-Muon_RunB_2.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_3.root"),oDFolder_+TString("out-Muon_RunB_3.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_4.root"),oDFolder_+TString("out-Muon_RunB_4.root"),keys_);





  }

  if( ! eChONmuChOFF_ && run2012_){ 

    //test sample
    //process(true,url_,iDFolder_+TString("Monitor_data_tau.root"),oDFolder_+TString("out-test.root"),keys_);


    process(true,url_,iDFolder_+TString("dataTryall204.root"),oDFolder_+TString("out-test.root"),keys_);

/*
    // Muon inclusive samples
    process(true,url_,iDFolder_+TString("Muon_1.root"),oDFolder_+TString("out-Muon_1.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_2.root"),oDFolder_+TString("out-Muon_2.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_3.root"),oDFolder_+TString("out-Muon_3.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_4.root"),oDFolder_+TString("out-Muon_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_5.root"),oDFolder_+TString("out-Muon_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_6.root"),oDFolder_+TString("out-Muon_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_7.root"),oDFolder_+TString("out-Muon_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_8.root"),oDFolder_+TString("out-Muon_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9.root"),oDFolder_+TString("out-Muon_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9_05Aug.root"),oDFolder_+TString("out-Muon_9_05Aug.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9_v5.root"),oDFolder_+TString("out-Muon_9_v5.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_10.root"),oDFolder_+TString("out-Muon_10.root"),keys_);
*/


  }


/*
  else{ 
    // inclusive electron trigger
    process(true,url_,iDFolder_+TString("Electron_1.root"), oDFolder_+TString("out-Electron_1.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_2.root"), oDFolder_+TString("out-Electron_2.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_3.root"), oDFolder_+TString("out-Electron_3.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_4.root"), oDFolder_+TString("out-Electron_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_5.root"), oDFolder_+TString("out-Electron_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_6.root"), oDFolder_+TString("out-Electron_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_7.root"), oDFolder_+TString("out-Electron_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_8.root"), oDFolder_+TString("out-Electron_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_9.root"), oDFolder_+TString("out-Electron_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_10.root"), oDFolder_+TString("out-Electron_10.root"),keys_);
  }
*/

  else if(! eChONmuChOFF_ && ! run2012_ ) {
    // electron+2jets+mht samples
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_4.root"), oDFolder_+TString("out-eJTrigMatch_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_5.root"), oDFolder_+TString("out-eJTrigMatch_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_6.root"), oDFolder_+TString("out-eJTrigMatch_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_7.root"), oDFolder_+TString("out-eJTrigMatch_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_8.root"), oDFolder_+TString("out-eJTrigMatch_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_9_aug05Reco.root"), oDFolder_+TString("out-eJTrigMatch_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_10.root"), oDFolder_+TString("out-eJTrigMatch_10.root"),keys_);
  }
  

}


void SampleProcessor::process_data_RunB(){

  url_ = DATA_URL;


  // DON't FORGET TO SET MUTAU ON
  if( ! eChONmuChOFF_ && ! run2012_){ 
    // Muon inclusive samples

    //    process(true,url_,iDFolder_+TString("Muon_RunA_ReReco.root"),oDFolder_+TString("out-Muon_RunA_ReReco.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_RunB_ReReco.root"),oDFolder_+TString("out-Muon_RunB_ReReco.root"),keys_);
//    //RUN 2011A
//    process(true,url_,iDFolder_+TString("Muon_1.root"),oDFolder_+TString("out-Muon_1.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_2.root"),oDFolder_+TString("out-Muon_2.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_3.root"),oDFolder_+TString("out-Muon_3.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_4.root"),oDFolder_+TString("out-Muon_4.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_5.root"),oDFolder_+TString("out-Muon_5.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_6.root"),oDFolder_+TString("out-Muon_6.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_7.root"),oDFolder_+TString("out-Muon_7.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_8.root"),oDFolder_+TString("out-Muon_8.root"),keys_);
//
//    process(true,url_,iDFolder_+TString("Muon_9.root"),oDFolder_+TString("out-Muon_9a.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_9_05Aug.root"),oDFolder_+TString("out-Muon_9b.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_9_v5.root"),oDFolder_+TString("out-Muon_9c.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_10.root"),oDFolder_+TString("out-Muon_10.root"),keys_);
//
//
//    //RUN 2011B
//    process(true,url_,iDFolder_+TString("Muon_RunB_1.root"),oDFolder_+TString("out-Muon_RunB_1.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_2.root"),oDFolder_+TString("out-Muon_RunB_2.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_3.root"),oDFolder_+TString("out-Muon_RunB_3.root"),keys_);
//    process(true,url_,iDFolder_+TString("Muon_RunB_4.root"),oDFolder_+TString("out-Muon_RunB_4.root"),keys_);





  }

  if( ! eChONmuChOFF_ && run2012_){ 

    //test sample
    //process(true,url_,iDFolder_+TString("Monitor_data_tau.root"),oDFolder_+TString("out-test.root"),keys_);


    process(true,url_,iDFolder_+TString("dataTryall204.root"),oDFolder_+TString("out-test.root"),keys_);

/*
    // Muon inclusive samples
    process(true,url_,iDFolder_+TString("Muon_1.root"),oDFolder_+TString("out-Muon_1.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_2.root"),oDFolder_+TString("out-Muon_2.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_3.root"),oDFolder_+TString("out-Muon_3.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_4.root"),oDFolder_+TString("out-Muon_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_5.root"),oDFolder_+TString("out-Muon_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_6.root"),oDFolder_+TString("out-Muon_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_7.root"),oDFolder_+TString("out-Muon_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_8.root"),oDFolder_+TString("out-Muon_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9.root"),oDFolder_+TString("out-Muon_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9_05Aug.root"),oDFolder_+TString("out-Muon_9_05Aug.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_9_v5.root"),oDFolder_+TString("out-Muon_9_v5.root"),keys_);
    process(true,url_,iDFolder_+TString("Muon_10.root"),oDFolder_+TString("out-Muon_10.root"),keys_);
*/


  }


/*
  else{ 
    // inclusive electron trigger
    process(true,url_,iDFolder_+TString("Electron_1.root"), oDFolder_+TString("out-Electron_1.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_2.root"), oDFolder_+TString("out-Electron_2.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_3.root"), oDFolder_+TString("out-Electron_3.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_4.root"), oDFolder_+TString("out-Electron_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_5.root"), oDFolder_+TString("out-Electron_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_6.root"), oDFolder_+TString("out-Electron_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_7.root"), oDFolder_+TString("out-Electron_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_8.root"), oDFolder_+TString("out-Electron_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_9.root"), oDFolder_+TString("out-Electron_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Electron_10.root"), oDFolder_+TString("out-Electron_10.root"),keys_);
  }
*/

  else if(! eChONmuChOFF_ && ! run2012_ ) {
    // electron+2jets+mht samples
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_4.root"), oDFolder_+TString("out-eJTrigMatch_4.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_5.root"), oDFolder_+TString("out-eJTrigMatch_5.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_6.root"), oDFolder_+TString("out-eJTrigMatch_6.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_7.root"), oDFolder_+TString("out-eJTrigMatch_7.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_8.root"), oDFolder_+TString("out-eJTrigMatch_8.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_9_aug05Reco.root"), oDFolder_+TString("out-eJTrigMatch_9.root"),keys_);
    process(true,url_,iDFolder_+TString("Ele_Met_eJTrigMatch_10.root"), oDFolder_+TString("out-eJTrigMatch_10.root"),keys_);
  }
  

}


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


