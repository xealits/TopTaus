#ifndef  _SampleProcessor_hh
#define  _SampleProcessor_hh

#if !defined(__CINT__) || defined(__MAKECINT__)

#include "LIP/TopTaus/interface/CommonDefinitions.hh"
#include "LIP/TopTaus/interface/MiniEvent.hh"

// System headers
#include <iostream>
#include <vector>
#include <fstream>

#endif

// ROOT headers
#include "TString.h"
#include "TFile.h"
#include "TH1D.h"

using namespace std;
using namespace commondefinitions;

// FIXME: clean code


//using namespace PhysicsTools;

typedef unsigned int uint;

class SampleProcessor {
  
public :
  /*
    Fills default X sections 
  */
  SampleProcessor( double);


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
  void process_data_RunAr();
  void process_data_RunB(int);
  void process_data_RunC1();
  void process_data_RunC2(int);
  void process_data_RunD(int);

  
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

