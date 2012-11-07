#include "LIP/TopTaus/interface/LandSShapesProducer.hh"

// System includes
#include <sstream>

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// ROOT includes
#include <THStack.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TText.h>
#include <TPaveText.h>

using namespace std;
using namespace RooFit;

LandSShapesProducer::LandSShapesProducer(string parSet):
  parSet_(parSet)
{
  
  using namespace std; 
  using namespace RooFit ;
  
  Init();

  SetOptions();

}


void LandSShapesProducer::Init(){

  // Clear vectors
  nVars_ = 0;
  //  uncSources_.clear();
  systComponents_.clear();
  systFancyComponents_.clear();
  nMcSamples_ = 0;
  nMassPoints_ = 0;
  currentMassPoint_ = 999;
  fitVars_.clear();
  vars_.clear();
  mins_.clear(); 
  maxs_.clear();
  bins_.clear();
  hmin_.clear();
  hmax_.clear();
  unbinned_.clear(); 
  smoothOrder_.clear();

  signalShapesToCompare_.clear();
  signalShapesToCompareHH_.clear();
  signalShapesToCompareWH_.clear();
  
  // Init RooFit variables
  sigVar_             = 0;              
  sigMeanVar_         = 0;              
  sigSigmaVar_        = 0;              
  ttbarmcbkgVar_      = 0;
  ttbarmcbkgMeanVar_  = 0;
  ttbarmcbkgSigmaVar_ = 0;
  mcbkgVar_           = 0;               
  mcbkgMeanVar_       = 0;         
  mcbkgSigmaVar_      = 0;         
  ddbkgVar_           = 0;              
  ddbkgMeanVar_       = 0;         
  ddbkgSigmaVar_      = 0;        

  identifier_ = "";

  cout << "Getting Parameter sets" << endl;

  // Get ParameterSet from cfg file
  const edm::ParameterSet &mFitPars = edm::readPSetsFrom(parSet_)->getParameter<edm::ParameterSet>("LandSShapesProducerParSet");

  outFolder_        = mFitPars.getParameter<std::string>("outFolder");
  outputFileName_  = mFitPars.getParameter<std::string>("outputFileName");
  outputFileNameSuffix_  = mFitPars.getParameter<vector<std::string> >("outputFileNameSuffix");
  nMassPoints_ = outputFileNameSuffix_.size();
  baseMCDir_        = mFitPars.getParameter<std::string>("baseMCDir");
  baseDataDir_      = mFitPars.getParameter<std::string>("baseDataDir");
  
  signalFileNameWH_   = mFitPars.getParameter<vector<std::string> >("signalFileNameWH");
  signalFileNameHH_   = mFitPars.getParameter<vector<std::string> >("signalFileNameHH");

  //  uncSources_         = mFitPars.getParameter<vector<std::string> >("uncSources");

  ddBkgFileName_      = mFitPars.getParameter<std::string>("ddBkgFileName");
  vector<string>mcBkgFileNameTemp = mFitPars.getParameter<vector<std::string> >("mcBkgFileName");
  for(size_t f=0; f<mcBkgFileNameTemp.size(); f++)
    mcBkgFileName_.push_back( mcBkgFileNameTemp[f] );
  dataFileName_       = mFitPars.getParameter<std::string>("dataFileName");
  
  signalSampleNameWH_   = mFitPars.getParameter<std::string>("signalSampleNameWH");
  signalSampleNameHH_   = mFitPars.getParameter<std::string>("signalSampleNameHH");
  ddBkgSampleName_      = mFitPars.getParameter<std::string>("ddBkgSampleName");
  mcBkgSampleName_      = mFitPars.getParameter<vector<std::string> >("mcBkgSampleName");
  //  vector<string>mcBkgSampleNameTemp = mFitPars.getParameter<vector<std::string> >("mcBkgSampleName");
  //  for(size_t f=0; f<mcBkgSampleNameTemp.size(); f++)
  //    mcBkgSampleName_.push_back( mcBkgSampleNameTemp[f] );
  dataSampleName_       = mFitPars.getParameter<std::string>("dataSampleName");

  signalFancySampleNameWH_   = mFitPars.getParameter<std::string>("signalFancySampleNameWH");
  ddBkgFancySampleName_      = mFitPars.getParameter<std::string>("ddBkgFancySampleName");
  mcBkgFancySampleName_      = mFitPars.getParameter<vector<std::string> >("mcBkgFancySampleName");
  //  vector<string>mcBkgSampleNameTemp = mFitPars.getParameter<vector<std::string> >("mcBkgSampleName");
  //  for(size_t f=0; f<mcBkgSampleNameTemp.size(); f++)
  //    mcBkgSampleName_.push_back( mcBkgSampleNameTemp[f] );
  dataFancySampleName_       = mFitPars.getParameter<std::string>("dataFancySampleName");


  signalSampleColor_   = mFitPars.getParameter<Int_t>("signalSampleColor");
  ddBkgSampleColor_    = mFitPars.getParameter<Int_t>("ddBkgSampleColor");
  mcBkgSampleColor_     = mFitPars.getParameter<vector<Int_t> >("mcBkgSampleColor");

  produceOnly_ = mFitPars.getParameter<bool>("produceOnly");
  
  minitreeSelected_   = mFitPars.getParameter<std::string>("minitreeSelected");
  minitreeDataDriven_ = mFitPars.getParameter<std::string>("minitreeDataDriven");
  
  vector<string>systComponentsTemp = mFitPars.getParameter<vector<std::string> >("systComponents");
  for(size_t f=0; f<systComponentsTemp.size(); f++)
    systComponents_.push_back( systComponentsTemp[f] );

  vector<string>systFancyComponentsTemp = mFitPars.getParameter<vector<std::string> >("systFancyComponents");
  for(size_t f=0; f<systFancyComponentsTemp.size(); f++)
    systFancyComponents_.push_back( systFancyComponentsTemp[f] );

  
  osCutEff_    = mFitPars.getParameter<double>("osCutEff");
  
  vars_        = mFitPars.getParameter<vector<string> >("vars");
  mins_        = mFitPars.getParameter<vector<double> >("mins");
  maxs_        = mFitPars.getParameter<vector<double> >("maxs");
  bins_        = mFitPars.getParameter<vector<int> >("bins");
  hmin_        = mFitPars.getParameter<vector<double> >("hmin");
  hmax_        = mFitPars.getParameter<vector<double> >("hmax");
  unbinned_    = mFitPars.getParameter<vector<Int_t> >("unbinned");
  smoothOrder_ = mFitPars.getParameter<vector<Int_t> >("smoothOrder");
  
  cout << "Got all parameter sets" << endl;
  
  // Open files and get trees
  // ddBkg is the only to be taken from data driven estimation (tree)
  nMcSamples_ = mcBkgFileName_.size();
  nSysts_     = systComponents_.size();
  
  cout << "Files opened" << endl;
  // Set variables
  //
  nVars_ = vars_.size();
  
  
  for(size_t i=0; i<nVars_; i++)
    fitVars_.push_back( new FitVar(vars_[i], mins_[i], maxs_[i], bins_[i], hmin_[i], hmax_[i], unbinned_[i], smoothOrder_[i]));
    
  
  // Set canvas
  SetTDRStyle();
  canvas_ = new TCanvas("canvas","My plots ",0,0,1000,1000);
  canvas_->cd();
  
  resultsFile_.open ((outFolder_+resultsFileName_).c_str());
  
  //  Uncomment following line in order to redirect stdout to file
  //  streamToFile_ = std::cout.rdbuf(resultsFile_.rdbuf());
  
  cout << "Init process complete" << endl;
}

void LandSShapesProducer::SetOptions(){
  //  myStyle_->SetOptStat(0);
}

void LandSShapesProducer::InitMassPoint(size_t s){

  cout << "Initializing mass point " << endl;

  currentMassPoint_ = s;
  mcBkgFile_.clear();
  mcBkgTree_.clear();
  
  signalSystTreeWH_.clear();
  signalSystTreeHH_.clear();
  ddBkgSystTree_.clear();
  mcBkgSystTree_.clear();

  perMassPointSignalShapesToCompareHH_.clear();
  perMassPointSignalShapesToCompareWH_.clear();
  perMassPointSignalShapesToCompare_  .clear();
  
  // Get files
  signalFileWH_   = TFile::Open(baseMCDir_   + signalFileNameWH_[currentMassPoint_]  );
  signalFileHH_   = TFile::Open(baseMCDir_   + signalFileNameHH_[currentMassPoint_]  );
  ddBkgFile_      = TFile::Open(baseDataDir_   + ddBkgFileName_     );
  for(size_t f=0; f<nMcSamples_; f++)
    mcBkgFile_.push_back( TFile::Open(baseMCDir_   + mcBkgFileName_[f]     ) );
  dataFile_       = TFile::Open(baseDataDir_ + dataFileName_      );

  cout << "Got files" << endl;

  // Get base trees
  signalTreeWH_   = (TTree*) signalFileWH_  ->Get(minitreeSelected_);
  signalTreeHH_   = (TTree*) signalFileHH_  ->Get(minitreeSelected_);
  ddBkgTree_      = (TTree*) ddBkgFile_     ->Get(minitreeDataDriven_);
  for(size_t f=0; f<nMcSamples_; f++)
    mcBkgTree_.push_back( (TTree*) mcBkgFile_[f]     ->Get(minitreeSelected_) );
  dataTree_       = (TTree*) dataFile_      ->Get(minitreeSelected_);

  cout << "Got base trees " << endl;
  // Get syst trees
  for(size_t a=0; a<nSysts_; a++){
    cout << "Getting tree " << systComponents_[a] << endl;
    signalSystTreeWH_.push_back(  (TTree*) signalFileWH_  ->Get(minitreeSelected_  + systComponents_[a])   );
    signalSystTreeHH_.push_back(  (TTree*) signalFileHH_  ->Get(minitreeSelected_  + systComponents_[a])   );
    cout << "Gotten dd tree " << minitreeSelected_ + systComponents_[a] << endl;
    //    ddBkgSystTree_   .push_back(  (TTree*)((TTree*) ddBkgFile_     ->Get(minitreeDataDriven_))->Clone(systComponents_[a]) );//+ systComponents_[a]) );
    ddBkgSystTree_   .push_back((TTree*) ddBkgFile_     ->Get(minitreeDataDriven_) );//+ systComponents_[a]) );

    vector<TTree*> temp;
    temp.clear();
    for(size_t f=0; f<nMcSamples_; f++){
      temp.push_back( (TTree*) mcBkgFile_[f]     ->Get(minitreeSelected_ + systComponents_[a]) );
    }
    mcBkgSystTree_.push_back(temp);
  }


  
  
  cout << "Mass points set" << endl;
}

void LandSShapesProducer::InitPerVariableAmbient(size_t i){
  
  // Base trees
  signalTreeWH_    ->ResetBranchAddresses();
  signalTreeHH_    ->ResetBranchAddresses();
  ddBkgTree_       ->ResetBranchAddresses();
  for(size_t f=0; f<nMcSamples_; f++) mcBkgTree_[f] ->ResetBranchAddresses()  ;
  dataTree_      ->ResetBranchAddresses()  ;

  // Syst trees
  for(size_t a=0; a<nSysts_; a++){
    signalSystTreeWH_[a]    ->ResetBranchAddresses();
    signalSystTreeHH_[a]    ->ResetBranchAddresses();
    cout << "Name: " << ddBkgSystTree_[a]->GetName() << ", entries " << ddBkgSystTree_[a]->GetEntries() << endl;
    ddBkgSystTree_[a]       ->ResetBranchAddresses();
    for(size_t f=0; f<nMcSamples_; f++){
      cout << "variable " << i << ", syst " << a << ", mcSample " << f << ", entries " << mcBkgSystTree_[a][f]->GetEntries() << endl;
      mcBkgSystTree_[a][f]->ResetBranchAddresses()  ;
    }
  }


  mcbkgHisto_.clear();

  myMCBkgDSName_.clear();
  myMCBkgDS_.clear();

  for(size_t a=0; a<nSysts_; a++){
    mySignalSystDSNameWH_.clear();
    mySignalSystDSNameHH_.clear();
    myDDBkgSystDSName_.clear();
    myMCBkgSystDSName_.clear();
  }


  mySignalDSWH_        = 0; //= new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
  mySignalDSHH_        = 0; //= new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
  myDDBkgDS_      = 0; 
  myMCBkgDS_.clear();
  myDataDS_       = 0; 

  mySignalSystDSWH_ .clear();
  mySignalSystDSHH_ .clear();
  myDDBkgSystDS_    .clear();
  myMCBkgSystDS_    .clear();
  

  signalHistWH_  = 0;
  signalHistHH_  = 0;
  ddbkgHist_     = 0;
  ddbkgHistUp_   = 0;
  ddbkgHistDown_ = 0;
  mcbkgHist_.clear();
  dataHist_ = 0;

  signalHistWHStatUp_ = 0;
  signalHistHHStatUp_ = 0;
  ddbkgHistStatUp_    = 0;
  mcbkgHistStatUp_.clear();
  dataHistStatUp_ = 0;

  signalHistWHStatDown_ = 0;
  signalHistHHStatDown_ = 0;
  ddbkgHistStatDown_    = 0;
  mcbkgHistStatDown_.clear();
  dataHistStatDown_ = 0;



  signalSystHistWH_ .clear();
  signalSystHistHH_ .clear();
  ddbkgSystHist_    .clear();
  mcbkgSystHist_.clear();


  signalHistoWH_     = 0;
  signalHistoHH_     = 0;
  ddbkgHisto_      = 0;
  mcbkgHisto_.clear();
  dataHisto_ = 0;

  signalSystHistoWH_  .clear();
  signalSystHistoHH_  .clear();
  ddbkgSystHisto_     .clear();
  mcbkgSystHisto_.clear();

  leg_            = 0;
  

  cout << "------------------------------------------------------" << endl;
  cout << "----------------- index " << i << "..................." << endl;
  cout << "Variable: " << fitVars_[i]->getVarName().c_str() << endl;
  cout << "Minimum: " << fitVars_[i]->getMin() << endl;
  cout << "Maximum: " << fitVars_[i]->getMax() << endl;
  cout << "Bins: " << fitVars_[i]->getBins() << endl;
  cout << "SmoothOrder: " << fitVars_[i]->getSmoothOrder() << endl;
  cout << "------------------------------------------------------" << endl;

  // Binned fit variable
  myvar_           = new RooRealVar(fitVars_[i]->getVarName().c_str(), fitVars_[i]->getVarName().c_str(), fitVars_[i]->getMin(), fitVars_[i]->getMax());  myvar_->setBins(fitVars_[i]->getBins()); 
  myvar_weights_   = new RooRealVar("weight","weight",0,1000);
  isOSvar_         = new RooRealVar("is_os","is_os",0,2);
  
  
  //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mySignalDSNameWH_= fitVars_[i]->getVarName().c_str() + signalSampleNameWH_ + outputFileNameSuffix_[currentMassPoint_];
  mySignalDSNameHH_= fitVars_[i]->getVarName().c_str() + signalSampleNameHH_ + outputFileNameSuffix_[currentMassPoint_];
  myDDBkgDSName_ = fitVars_[i]->getVarName().c_str() + ddBkgSampleName_ + outputFileNameSuffix_[currentMassPoint_];
  for(size_t f=0; f<nMcSamples_; f++){
    myMCBkgDSName_.push_back( fitVars_[i]->getVarName().c_str() + mcBkgSampleName_[f] + outputFileNameSuffix_[currentMassPoint_] );
  }
  myDataDSName_   = fitVars_[i]->getVarName().c_str() + dataSampleName_ + outputFileNameSuffix_[currentMassPoint_];
  


  // Define syst data sets
  for(size_t a=0; a<nSysts_; a++){
    mySignalSystDSNameWH_.push_back(mySignalDSNameWH_+string(systComponents_[a]));
    mySignalSystDSNameHH_.push_back(mySignalDSNameHH_+string(systComponents_[a]));
    myDDBkgSystDSName_.push_back(myDDBkgDSName_+string(systComponents_[a]));
    vector<string> temp;
    temp.clear();
    for(size_t f=0; f<nMcSamples_; f++){
      temp.push_back( myMCBkgDSName_[f] + string(systComponents_[a]) );
     }
     myMCBkgSystDSName_.push_back(temp);
  }
  
  sumWeights_ = 0;
  cout << "per-variable ambient initialized" << endl;
}

void LandSShapesProducer::BuildDatasets(size_t i){

  
  // Temp variables for setting branch addresses
  double myVarAllocator, myVarWeightAllocator;
  double isOSsig;
  
  mySignalDSWH_         = new RooDataSet(mySignalDSNameWH_.c_str(),mySignalDSNameWH_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter  

  // Get WH events
  signalTreeWH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
  signalTreeWH_->SetBranchAddress("weight", &myVarWeightAllocator);
  signalTreeWH_->SetBranchAddress("is_os", &isOSsig);
  for(unsigned int ev=0; ev<signalTreeWH_->GetEntries(); ev++){
    signalTreeWH_->GetEntry(ev);
    if(isOSsig<0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
    myvar_->setVal(myVarAllocator);
    sumWeights_ += myVarWeightAllocator;
    myvar_weights_->setVal(myVarWeightAllocator);
    mySignalDSWH_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
  }
  mySignalDSHH_         = new RooDataSet(mySignalDSNameHH_.c_str(),mySignalDSNameHH_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
  // Get HH events
  signalTreeHH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
  signalTreeHH_->SetBranchAddress("weight", &myVarWeightAllocator);
  signalTreeHH_->SetBranchAddress("is_os", &isOSsig);
  //    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<signalTreeHH_->GetEntries(); ev++){
    signalTreeHH_->GetEntry(ev);
    if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
    myvar_->setVal(myVarAllocator);
    if(myVarAllocator>1 && fitVars_[i]->getVarName() == "rc_t" ) cout << "var: " << myVarAllocator << endl;
    sumWeights_ += myVarWeightAllocator;
    myvar_weights_->setVal(myVarWeightAllocator);
    mySignalDSHH_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
  }
  
  myDDBkgDS_         = new RooDataSet(myDDBkgDSName_.c_str(),myDDBkgDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
  // Get DD events
  ddBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
  ddBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  ddBkgTree_->SetBranchAddress("is_os", &isOSsig);
  //    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<ddBkgTree_->GetEntries(); ev++){
    ddBkgTree_->GetEntry(ev);
    if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
    myvar_->setVal(myVarAllocator);
    sumWeights_ += myVarWeightAllocator;
    myvar_weights_->setVal(osCutEff_*myVarWeightAllocator);
    //if(myVarAllocator < 0.1) continue;
    myDDBkgDS_->add(RooArgSet(*myvar_,*myvar_weights_),osCutEff_*myVarWeightAllocator);
  }

  for(size_t f=0; f<nMcSamples_; f++){
    myMCBkgDS_.push_back( new RooDataSet(myMCBkgDSName_[f].c_str(),myMCBkgDSName_[f].c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    // Get MCBkg events
    mcBkgTree_[f]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    mcBkgTree_[f]->SetBranchAddress("weight", &myVarWeightAllocator);
    mcBkgTree_[f]->SetBranchAddress("is_os", &isOSsig);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<mcBkgTree_[f]->GetEntries(); ev++){
      mcBkgTree_[f]->GetEntry(ev);
      if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
      myvar_->setVal(myVarAllocator);
      if(f==1) cout << "DEBUG: ttll var is " << myVarAllocator << endl;
      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator);
      myMCBkgDS_[f]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
    }
  }

  cout << "Got base datasets" << endl;

  ///// Syst get
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    cout << "getting datasets for syst " << a << endl;
    mySignalSystDSWH_.push_back( new RooDataSet(mySignalSystDSNameWH_[a].c_str(),mySignalSystDSNameWH_[a].c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter  
    
    // Get WH events
    signalSystTreeWH_[a]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    signalSystTreeWH_[a]->SetBranchAddress("weight", &myVarWeightAllocator);
    signalSystTreeWH_[a]->SetBranchAddress("is_os", &isOSsig);
    for(unsigned int ev=0; ev<signalSystTreeWH_[a]->GetEntries(); ev++){
      signalSystTreeWH_[a]->GetEntry(ev);
      if(isOSsig<0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
      myvar_->setVal(myVarAllocator);
      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator);
      mySignalSystDSWH_[a]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
    }
    cout << "Got WH events " << endl; 

    mySignalSystDSHH_.push_back(new RooDataSet(mySignalSystDSNameHH_[a].c_str(),mySignalSystDSNameHH_[a].c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    // Get HH events
    signalSystTreeHH_[a]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    signalSystTreeHH_[a]->SetBranchAddress("weight", &myVarWeightAllocator);
    signalSystTreeHH_[a]->SetBranchAddress("is_os", &isOSsig);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<signalSystTreeHH_[a]->GetEntries(); ev++){
      signalSystTreeHH_[a]->GetEntry(ev);
      if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
      myvar_->setVal(myVarAllocator);
      if(myVarAllocator>1 && fitVars_[i]->getVarName() == "rc_t" ) cout << "var: " << myVarAllocator << endl;
      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator);
      mySignalSystDSHH_[a]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
    }
    
    cout << "Got HH events " << endl; 
    myDDBkgSystDS_.push_back(new RooDataSet(myDDBkgSystDSName_[a].c_str(),myDDBkgSystDSName_[a].c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    // Get DD events
    ddBkgSystTree_[a]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    ddBkgSystTree_[a]->SetBranchAddress("weight", &myVarWeightAllocator);
    ddBkgSystTree_[a]->SetBranchAddress("is_os", &isOSsig);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<ddBkgSystTree_[a]->GetEntries(); ev++){
      ddBkgSystTree_[a]->GetEntry(ev);
      if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
      myvar_->setVal(myVarAllocator);
      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(osCutEff_*myVarWeightAllocator);
      //if(myVarAllocator < 0.1) continue;
      myDDBkgSystDS_[a]->add(RooArgSet(*myvar_,*myvar_weights_),osCutEff_*myVarWeightAllocator);
    }

    cout << "Got DD events " << endl; 
    vector<RooDataSet*> temp;
    temp.clear();
    for(size_t f=0; f<nMcSamples_; f++){
      temp.push_back( new RooDataSet(myMCBkgSystDSName_[a][f].c_str(),myMCBkgSystDSName_[a][f].c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    }
    myMCBkgSystDS_.push_back(temp);
    for(size_t f=0; f<nMcSamples_; f++){
      // Get MCBkg events
      mcBkgSystTree_[a][f]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
      mcBkgSystTree_[a][f]->SetBranchAddress("weight", &myVarWeightAllocator);
      mcBkgSystTree_[a][f]->SetBranchAddress("is_os", &isOSsig);
      //    cout << "getIsoS      ";
      for(unsigned int ev=0; ev<mcBkgSystTree_[a][f]->GetEntries(); ev++){
	mcBkgSystTree_[a][f]->GetEntry(ev);
	if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
	myvar_->setVal(myVarAllocator);
	sumWeights_ += myVarWeightAllocator;
	myvar_weights_->setVal(myVarWeightAllocator);
	myMCBkgSystDS_[a][f]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
      }
    }
    
  } // End loop on syst components
  ///// End syst get

  cout << "Got syst datasets" << endl;





  
  myDataDS_         = new RooDataSet(myDataDSName_.c_str(),myDataDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
  // Get Data events
  dataTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
  dataTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  dataTree_->SetBranchAddress("is_os", &isOSsig);
  //    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<dataTree_->GetEntries(); ev++){
    dataTree_->GetEntry(ev);
    if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
    myvar_->setVal(myVarAllocator);
    sumWeights_ += myVarWeightAllocator;
    myvar_weights_->setVal(myVarWeightAllocator);
    myDataDS_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
  }
  
  cout << "Datasets built" << endl;
  
  // // Legacy: seems not to give correct inputs
  //  string myOsCut = "is_os>0.5";
  //  unrMyDDBkgDS_      = new RooDataSet(myDDBkgDSName_.c_str(), myDDBkgDSName_.c_str(),  ddBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
  //  myDDBkgDS_ = (RooDataSet*) unrMyDDBkgDS_->reduce(RooArgSet(*myvar_,*myvar_weights_));
  //  if(standaloneTTbar_){
  //    unrMyTTBARMCBkgDS_ = new RooDataSet(myTTBARMCBkgDSName_.c_str(), myTTBARMCBkgDSName_.c_str(),  ttbarmcBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
  //    myTTBARMCBkgDS_ = (RooDataSet*) unrMyTTBARMCBkgDS_->reduce(RooArgSet(*myvar_,*myvar_weights_));
  //  }
  //  unrMyMCBkgDS_      = new RooDataSet(myMCBkgDSName_.c_str(), myMCBkgDSName_.c_str(),  mcBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
  //  myMCBkgDS_ = (RooDataSet*) unrMyMCBkgDS_->reduce(RooArgSet(*myvar_,*myvar_weights_));
  //  unrMyDataDS_       = new RooDataSet(myDataDSName_.c_str(),  myDataDSName_.c_str(),   dataTree_,   RooArgSet(*myvar_,*isOSvar_), myOsCut.c_str() );
  //  myDataDS_ = (RooDataSet*) unrMyDataDS_->reduce(RooArgSet(*myvar_,*myvar_weights_));
  //  
  


  // Build binned clones
  signalHistoWH_ = mySignalDSWH_->binnedClone();
  signalHistoHH_ = mySignalDSHH_->binnedClone();
  ddbkgHisto_    = myDDBkgDS_->binnedClone();
  for(size_t f=0; f<nMcSamples_; f++) mcbkgHisto_.push_back( myMCBkgDS_[f]->binnedClone());
  dataHisto_  = myDataDS_ ->binnedClone();

  cout << "Binned clones built" << endl;

  cout << mySignalDSNameWH_ << " unbinned entries: " << mySignalDSWH_->numEntries() << ". weighted entries: " << signalHistoWH_->sum(kFALSE) << endl;
  cout << mySignalDSNameHH_ << " unbinned entries: " << mySignalDSHH_->numEntries() << ". weighted entries: " << signalHistoHH_->sum(kFALSE) << endl;
  cout << myDDBkgDSName_ << " unbinned entries: " << myDDBkgDS_->numEntries() << ". weighted entries: " << ddbkgHisto_->sum(kFALSE) << endl;
  for(size_t f=0; f<nMcSamples_; f++)  cout << myMCBkgDSName_[f] << " unbinned entries: " << myMCBkgDS_[f]->numEntries() << ". weighted entries: " << mcbkgHisto_[f]->sum(kFALSE) << endl;
  cout << myDataDSName_ << " unbinned entries: " << myDataDS_->numEntries() << ". weighted entries: " << dataHisto_->sum(kFALSE) << endl;
  
  cout << "BuildDatasets successful" << endl;



  /// Build binned clones - syst case
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    
    signalSystHistoWH_.push_back( mySignalSystDSWH_[a]->binnedClone() );
    signalSystHistoHH_.push_back( mySignalSystDSHH_[a]->binnedClone() );
    ddbkgSystHisto_.push_back(  myDDBkgSystDS_[a]->binnedClone() );
    vector<RooDataHist*> temp;
    temp.clear();
    for(size_t f=0; f<nMcSamples_; f++){
      temp.push_back( myMCBkgSystDS_[a][f]->binnedClone() );
    }
    mcbkgSystHisto_.push_back(temp);
    cout << "Syst binned clones built" << endl;
    
    cout << mySignalSystDSNameWH_[a] << " unbinned entries: " << mySignalSystDSWH_[a]->numEntries() << ". weighted entries: " << signalSystHistoWH_[a]->sum(kFALSE) << endl;
    cout << mySignalSystDSNameHH_[a] << " unbinned entries: " << mySignalSystDSHH_[a]->numEntries() << ". weighted entries: " << signalSystHistoHH_[a]->sum(kFALSE) << endl;
    cout << myDDBkgSystDSName_[a] << " unbinned entries: " << myDDBkgSystDS_[a]->numEntries() << ". weighted entries: " << ddbkgSystHisto_[a]->sum(kFALSE) << endl;
    for(size_t f=0; f<nMcSamples_; f++)  cout << myMCBkgSystDSName_[a][f] << " unbinned entries: " << myMCBkgSystDS_[a][f]->numEntries() << ". weighted entries: " << mcbkgSystHisto_[a][f]->sum(kFALSE) << endl;
    cout << "BuildDatasets successful" << endl;
  } // End syst loop
    /// End build binned clones - syst case
    


//void LandSShapesProducer::BuildPDFs(size_t i){
//  
//  switch(fitVars_[i]->getUnbinned()){
//  case 1 : // Unbinned
//    if(includeSignal_) u_signalModel_ = new RooKeysPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *mySignalDS_, RooKeysPdf::MirrorBoth,2);
//    u_ddbkgModel_  = new RooKeysPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(),  *myvar_, *myDDBkgDS_ , RooKeysPdf::MirrorBoth,2);
//    if(standaloneTTbar_) u_ttbarmcbkgModel_  = new RooKeysPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *myTTBARMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
//    u_mcbkgModel_  = new RooKeysPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *myMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
//    break;
//  case 0:  // Binned (w/ or w/out smoothing)
//    if(includeSignal_) b_signalModel_ = new RooHistPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *signalHisto_, fitVars_[i]->getSmoothOrder());
//    b_ddbkgModel_  = new RooHistPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(), *myvar_, *ddbkgHisto_ , fitVars_[i]->getSmoothOrder());
//    if(standaloneTTbar_) b_ttbarmcbkgModel_ = new RooHistPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *ttbarmcbkgHisto_  , fitVars_[i]->getSmoothOrder()); 
//    b_mcbkgModel_  = new RooHistPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *mcbkgHisto_  , fitVars_[i]->getSmoothOrder()); 
//    break;
//  default : // Dummy - should never arrive here
//    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
//  }
//}


}

void LandSShapesProducer::DrawTemplates(size_t i){


  
  string outputFileName = outputFileName_ + string("_") + outputFileNameSuffix_[currentMassPoint_] + string("_") + fitVars_[i]->getVarName();
  TFile* outputFile;


  if(produceOnly_)
    outputFile = new TFile((outFolder_+outputFileName+string(".root")).c_str(), "RECREATE");
  
  canvas_->cd();
  canvas_->Clear();

  //signal histogram ////////////////////////////////////////////////////////////
  signalHistWH_ = signalHistoWH_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  signalHistHH_ = signalHistoHH_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  signalHistWH_->Sumw2();
  signalHistHH_->Sumw2();
  
  signalHistWHStatUp_   = (TH1*) signalHistWH_->Clone(signalHistWH_->GetName() + TString("StatUp"));
  signalHistWHStatDown_ = (TH1*) signalHistWH_->Clone(signalHistWH_->GetName() + TString("StatDown"));
  for(int ibin=0; ibin<signalHistWH_->GetNbinsX(); ibin++){
    signalHistWHStatUp_  ->SetBinContent(ibin, signalHistWH_->GetBinContent(ibin) + signalHistWH_->GetBinError(ibin) );
    signalHistWHStatDown_->SetBinContent(ibin, signalHistWH_->GetBinContent(ibin) - signalHistWH_->GetBinError(ibin) );
  }

  signalHistHHStatUp_   = (TH1*) signalHistHH_->Clone(signalHistHH_->GetName() + TString("StatUp"));
  signalHistHHStatDown_ = (TH1*) signalHistHH_->Clone(signalHistHH_->GetName() + TString("StatDown"));
  for(int ibin=0; ibin<signalHistHH_->GetNbinsX(); ibin++){
    signalHistHHStatUp_  ->SetBinContent(ibin, signalHistHH_->GetBinContent(ibin) + signalHistHH_->GetBinError(ibin) );
    signalHistHHStatDown_->SetBinContent(ibin, signalHistHH_->GetBinContent(ibin) - signalHistHH_->GetBinError(ibin) );
  }

  

  signalHistWH_->SetOption("0000");
  signalHistWH_->SetLineWidth(3);
  signalHistWH_->SetTitle("");
  signalHistWH_->GetYaxis()->SetTitle("a.u.");
  signalHistWH_->GetYaxis()->SetTitleOffset(1.5);
  //  signalHistWH_->SetLineColor(kGreen);
  signalHistWH_->SetLineColor(signalSampleColor_);
  signalHistWH_->SetFillStyle(0); //3017);

  signalHistHH_->SetOption("0000");
  signalHistHH_->SetLineWidth(3);
  signalHistHH_->SetTitle("");
  signalHistHH_->GetYaxis()->SetTitle("a.u.");
  signalHistHH_->GetYaxis()->SetTitleOffset(1.5);
  //signalHistHH_->SetLineColor(kGreen);
  signalHistHH_->SetLineColor(signalSampleColor_);
  signalHistHH_->SetFillStyle(0);//3017);


  //  ///////////////////////////////////////////////////////////////////////////////
  
  
  // dd bkg histogram /////////////////////////////////////////////////
  ddbkgHist_ = ddbkgHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  ddbkgHist_->Sumw2();

  ddbkgHistStatUp_   = (TH1*)ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("StatUp"));
  ddbkgHistStatDown_ = (TH1*)ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("StatDown"));
  for(int ibin=0; ibin<ddbkgHist_->GetNbinsX(); ibin++){
    ddbkgHistStatUp_  ->SetBinContent(ibin, ddbkgHist_->GetBinContent(ibin) + ddbkgHist_->GetBinError(ibin) );
    ddbkgHistStatDown_->SetBinContent(ibin, ddbkgHist_->GetBinContent(ibin) - ddbkgHist_->GetBinError(ibin) );
  }

//  ddbkgHistUp_ =   ddbkgHist_;
//  ddbkgHistDown_ = ddbkgHist_;
//
//  ddbkgHistUp_   ->Scale((222.+11.4)/222.);
//  ddbkgHistDown_ ->Scale((222.-11.4)/222.);
  

  ddbkgHistUp_ =   (TH1*) ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("Up") );
  ddbkgHistDown_ = (TH1*) ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("Down") );


  // ddbkgHist_->SetLineColor(kRed);
  ddbkgHist_->SetFillColor(ddBkgSampleColor_);
  ddbkgHist_->SetLineWidth(3);
  ddbkgHist_->SetFillStyle(1001);//3017);

  ddbkgHistUp_->SetFillColor(ddBkgSampleColor_);
  ddbkgHistUp_->SetLineWidth(3);
  ddbkgHistUp_->SetFillStyle(1001);//3017);

  ddbkgHistDown_->SetFillColor(ddBkgSampleColor_);
  ddbkgHistDown_->SetLineWidth(3);
  ddbkgHistDown_->SetFillStyle(1001);//3017);

//  /////////////////////////////////////////////////////////////////////
  cout << "sample " << ddBkgSampleName_ << ", integral " << ddbkgHist_->Integral() << endl;
  ddbkgHist_->Scale(222/ddbkgHist_->Integral());

  ddbkgHistUp_->Scale((222+11.4)/ddbkgHistUp_->Integral());
  ddbkgHistDown_->Scale((222-11.4)/ddbkgHistDown_->Integral());
  cout << "sample " << ddBkgSampleName_ << ", integral " << ddbkgHist_->Integral() << " after rescaling " << endl;

  // mc bkg histogram ////////////////////////////////////////////////
  for(size_t f=0; f<nMcSamples_; f++){
    
    mcbkgHist_.push_back( mcbkgHisto_[f]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
    mcbkgHist_[f]->Sumw2();
    mcbkgHistStatUp_  .push_back( (TH1*)mcbkgHist_[f]->Clone(mcbkgHist_[f]->GetName() + TString("StatUp")));
    mcbkgHistStatDown_.push_back( (TH1*)mcbkgHist_[f]->Clone(mcbkgHist_[f]->GetName() + TString("StatDown")));
    for(int ibin=0; ibin<mcbkgHist_[f]->GetNbinsX(); ibin++){
      mcbkgHistStatUp_[f]  ->SetBinContent(ibin, mcbkgHist_[f]->GetBinContent(ibin) + mcbkgHist_[f]->GetBinError(ibin) );
      mcbkgHistStatDown_[f]->SetBinContent(ibin, mcbkgHist_[f]->GetBinContent(ibin) - mcbkgHist_[f]->GetBinError(ibin) );
    }

    // mcbkgHist_[f]->SetLineColor(kBlack+9+f);
    mcbkgHist_[f]->SetFillColor(mcBkgSampleColor_[f]);
    mcbkgHist_[f]->SetLineWidth(3);
    mcbkgHist_[f]->SetFillStyle(1001);//3017);
    cout << "sample " << mcBkgSampleName_[f] << ", integral " << mcbkgHist_[f]->Integral() << endl;
  }
  //    ///////////////////////////////////////////////////////////////////
    
  mcbkgHist_.push_back( (TH1*)mcbkgHist_[3]->Clone(  mcBkgSampleName_[nMcSamples_].c_str()) );
  mcbkgHist_.push_back( (TH1*)mcbkgHist_[3]->Clone(  mcBkgSampleName_[nMcSamples_+1].c_str()) );
  mcbkgHist_[nMcSamples_]->Scale(0.4/mcbkgHist_[nMcSamples_]->Integral());
  mcbkgHist_[nMcSamples_+1]->Scale(50.6/mcbkgHist_[nMcSamples_+1]->Integral());

  mcbkgHistStatUp_  .push_back( (TH1*)mcbkgHist_[nMcSamples_]->Clone(mcbkgHist_[nMcSamples_]->GetName() + TString("StatUp")));
  mcbkgHistStatDown_.push_back( (TH1*)mcbkgHist_[nMcSamples_]->Clone(mcbkgHist_[nMcSamples_]->GetName() + TString("StatDown")));
  for(int ibin=0; ibin<mcbkgHist_[nMcSamples_]->GetNbinsX(); ibin++){
    mcbkgHistStatUp_[nMcSamples_]  ->SetBinContent(ibin, mcbkgHist_[nMcSamples_]->GetBinContent(ibin) + mcbkgHist_[nMcSamples_]->GetBinError(ibin) );
    mcbkgHistStatDown_[nMcSamples_]->SetBinContent(ibin, mcbkgHist_[nMcSamples_]->GetBinContent(ibin) - mcbkgHist_[nMcSamples_]->GetBinError(ibin) );
  }

  mcbkgHistStatUp_  .push_back( (TH1*)mcbkgHist_[nMcSamples_+1]->Clone(mcbkgHist_[nMcSamples_+1]->GetName() + TString("StatUp")));
  mcbkgHistStatDown_.push_back( (TH1*)mcbkgHist_[nMcSamples_+1]->Clone(mcbkgHist_[nMcSamples_+1]->GetName() + TString("StatDown")));
  for(int ibin=0; ibin<mcbkgHist_[nMcSamples_+1]->GetNbinsX(); ibin++){
    mcbkgHistStatUp_[nMcSamples_  +1]  ->SetBinContent(ibin, mcbkgHist_[nMcSamples_+1]->GetBinContent(ibin) + mcbkgHist_[nMcSamples_+1]->GetBinError(ibin) );
    mcbkgHistStatDown_[nMcSamples_+1]->SetBinContent(ibin, mcbkgHist_[nMcSamples_+1]->GetBinContent(ibin) - mcbkgHist_[nMcSamples_+1]->GetBinError(ibin) );
  }

  
  //  mcbkgHist_[1]->Scale(22.9/mcbkgHist_[1]->Integral());

  //  mcbkgHist_[nMcSamples_]->SetLineColor(kBlack+9+nMcSamples_);
  mcbkgHist_[nMcSamples_]->SetFillColor(mcBkgSampleColor_[nMcSamples_]);
  mcbkgHist_[nMcSamples_]->SetFillStyle(1001);//3017);

  //  mcbkgHist_[nMcSamples_+1]->SetLineColor(kBlack+9+nMcSamples_+1);
  mcbkgHist_[nMcSamples_+1]->SetFillColor(mcBkgSampleColor_[nMcSamples_+1]);
  mcbkgHist_[nMcSamples_+1]->SetFillStyle(1001);//3017);



  dataHist_ = dataHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  dataHist_->Sumw2();
  dataHistStatUp_  = (TH1*)dataHist_->Clone(dataHist_->GetName() + TString("StatUp"));
  dataHistStatDown_= (TH1*)dataHist_->Clone(dataHist_->GetName() + TString("StatDown"));
  for(int ibin=0; ibin<dataHist_->GetNbinsX(); ibin++){
    dataHistStatUp_  ->SetBinContent(ibin, dataHist_->GetBinContent(ibin) + dataHist_->GetBinError(ibin) );
    dataHistStatDown_->SetBinContent(ibin, dataHist_->GetBinContent(ibin) - dataHist_->GetBinError(ibin) );
  }



  /// Produce plots - syst case
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
  //signal histogram ////////////////////////////////////////////////////////////
  signalSystHistWH_.push_back( signalSystHistoWH_[a]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
  signalSystHistHH_.push_back( signalSystHistoHH_[a]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
  signalSystHistWH_[a]->Sumw2();
  signalSystHistHH_[a]->Sumw2();

  signalSystHistWH_[a]->SetOption("0000");
  signalSystHistWH_[a]->SetLineWidth(3);
  signalSystHistWH_[a]->SetTitle("");
  signalSystHistWH_[a]->GetYaxis()->SetTitle("a.u.");
  signalSystHistWH_[a]->GetYaxis()->SetTitleOffset(1.5);
  signalSystHistWH_[a]->SetLineColor(signalSampleColor_);
  signalSystHistWH_[a]->SetFillStyle(0); //3017);

  signalSystHistHH_[a]->SetOption("0000");
  signalSystHistHH_[a]->SetLineWidth(3);
  signalSystHistHH_[a]->SetTitle("");
  signalSystHistHH_[a]->GetYaxis()->SetTitle("a.u.");
  signalSystHistHH_[a]->GetYaxis()->SetTitleOffset(1.5);
  signalSystHistHH_[a]->SetLineColor(kGreen);
  signalSystHistHH_[a]->SetLineColor(signalSampleColor_);
  signalSystHistHH_[a]->SetFillStyle(0);//3017);
  
  // dd bkg histogram /////////////////////////////////////////////////
  ddbkgSystHist_.push_back( ddbkgSystHisto_[a]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
  ddbkgSystHist_[a]->Sumw2();
  ddbkgSystHist_[a]->SetFillColor(ddBkgSampleColor_);
  ddbkgSystHist_[a]->SetFillStyle(1001);//3017);

  cout << "sample " << ddBkgSampleName_ << ", integral " << ddbkgHist_->Integral() << endl;
  if(a == 0 || a == 2 || a == 4)
    ddbkgSystHist_[a]->Scale((222+11.4/5)/ddbkgSystHist_[a]->Integral());
  else
    ddbkgSystHist_[a]->Scale((222-11.4/5)/ddbkgSystHist_[a]->Integral());
  cout << "sample " << ddBkgSampleName_ << ", integral " << ddbkgHist_->Integral() << " after rescaling " << endl;

  // mc bkg histogram ////////////////////////////////////////////////
  vector<TH1*> temp;
  temp.clear();
  for(size_t f=0; f<nMcSamples_; f++){
    temp.push_back( mcbkgSystHisto_[a][f]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
  }
  mcbkgSystHist_.push_back(temp);
  // mcbkgHist_[f]->SetLineColor(kBlack+9+f);
  for(size_t f=0; f<nMcSamples_; f++){
    mcbkgSystHist_[a][f]->Sumw2();
    mcbkgSystHist_[a][f]->SetFillColor(mcBkgSampleColor_[f]);
    //mcbkgHist_[f]->SetLineWidth(3);
    mcbkgSystHist_[a][f]->SetFillStyle(1001);//3017);
    cout << "sample " << mcBkgSampleName_[f] << ", integral " << mcbkgHist_[f]->Integral() << endl;
  }
  //    ///////////////////////////////////////////////////////////////////
  
  mcbkgSystHist_[a].push_back( (TH1*)mcbkgSystHist_[a][3]->Clone(  mcBkgSampleName_[nMcSamples_].c_str()) );
  mcbkgSystHist_[a].push_back( (TH1*)mcbkgSystHist_[a][3]->Clone(  mcBkgSampleName_[nMcSamples_+1].c_str()) );
  mcbkgSystHist_[a][nMcSamples_]->Scale(0.4/mcbkgSystHist_[a][nMcSamples_]->Integral());
  mcbkgSystHist_[a][nMcSamples_+1]->Scale(50.6/mcbkgSystHist_[a][nMcSamples_+1]->Integral());

  //  mcbkgHist_[nMcSamples_]->SetLineColor(kBlack+9+nMcSamples_);
  mcbkgSystHist_[a][nMcSamples_]->SetFillColor(mcBkgSampleColor_[nMcSamples_]);
  mcbkgSystHist_[a][nMcSamples_]->SetFillStyle(1001);//3017);

  //  mcbkgHist_[nMcSamples_+1]->SetLineColor(kBlack+9+nMcSamples_+1);
  mcbkgSystHist_[a][nMcSamples_+1]->SetFillColor(mcBkgSampleColor_[nMcSamples_+1]);
  mcbkgSystHist_[a][nMcSamples_+1]->SetFillStyle(1001);//3017);



  } // End loop on systs

  /// End produce plots - syst case



  if(!produceOnly_){ 
    TGraphAsymmErrors ddbkgBands;
    getErrorBands(*ddbkgHist_, *ddbkgHistUp_, *ddbkgHistDown_, ddbkgBands);

    ddbkgBands.SetFillColor(1);
    ddbkgBands.SetFillStyle(3004);
    ddbkgBands.GetYaxis()->SetTitle("a.u.");
    ddbkgBands.GetYaxis()->SetTitleOffset(0.85);
    ddbkgBands.GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    ddbkgBands.GetXaxis()->SetTitleOffset(0.85);
    ddbkgBands.GetXaxis()->SetTitleSize(5);
    ddbkgBands.GetXaxis()->SetRange(0.001,1.001);    
    ddbkgBands.GetXaxis()->SetRangeUser(0.001,1.001);    


    TLegend* leg2_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg2_->SetTextFont(62);
    leg2_->SetBorderSize(0);
    leg2_->SetLineColor(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColor(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(signalHistWH_,"Base","l");
    leg2_->AddEntry(signalSystHistWH_[0],"JESup","l");
    leg2_->AddEntry(signalSystHistWH_[1],"JESdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");

    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColor(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColor(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(signalHistWH_,"Base","l");
    leg_->AddEntry(signalSystHistWH_[0],"JESup","l");
    leg_->AddEntry(signalSystHistWH_[1],"JESdown","l");

    signalHistWH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistWH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[0]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[0]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[1]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[1]->GetXaxis()->SetRangeUser(0.001,1.001);    
    
    signalSystHistWH_[0]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistWH_->SetLineColor(1);
    signalSystHistWH_[0]->SetLineColor(2);
    signalSystHistWH_[1]->SetLineColor(3);
    signalSystHistWH_[0]->Draw("hist");
    signalSystHistWH_[1]->Draw("histsame");
    signalHistWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColor(signalSampleColor_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[0]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[0]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[1]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[1]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[0]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColor(1);
    signalSystHistHH_[0]->SetLineColor(2);
    signalSystHistHH_[1]->SetLineColor(3);
    signalSystHistHH_[0]->Draw("hist");
    signalSystHistHH_[1]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColor(signalSampleColor_);
    
    ddbkgSystHist_[0]->SetFillStyle(0);
    ddbkgSystHist_[1]->SetFillStyle(0);
    ddbkgHist_->SetFillStyle(0);

    ddbkgHist_->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgHist_->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[0]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[0]->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[1]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[1]->GetXaxis()->SetRangeUser(0.001,1.001);    

    ddbkgSystHist_[0]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    ddbkgHist_->SetMarkerColor(1);
    ddbkgSystHist_[0]->SetLineColor(2);
    ddbkgSystHist_[1]->SetLineColor(3);
    ddbkgSystHist_[0]->Draw("hist");
    ddbkgSystHist_[1]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColor(ddBkgSampleColor_);

    ddbkgSystHist_[0]->SetFillStyle(1001);
    ddbkgSystHist_[1]->SetFillStyle(1001);
    ddbkgHist_->SetFillStyle(1001);


    for(size_t f=0; f<nMcSamples_+2; f++){

      mcbkgSystHist_[0][f]->SetFillStyle(0);
      mcbkgSystHist_[1][f]->SetFillStyle(0);
      mcbkgHist_[f]->SetFillStyle(0);

      mcbkgHist_[f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgHist_[f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[0][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[0][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[1][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[1][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
 
      mcbkgSystHist_[0][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
      mcbkgHist_[f]->SetLineColor(1);
      mcbkgSystHist_[0][f]->SetLineColor(2);
      mcbkgSystHist_[1][f]->SetLineColor(3);
      mcbkgSystHist_[0][f]->Draw("hist");
      mcbkgSystHist_[1][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColor(mcBkgSampleColor_[f]);
      mcbkgSystHist_[0][f]->SetFillStyle(1001);
      mcbkgSystHist_[1][f]->SetFillStyle(1001);
      mcbkgHist_[f]->SetFillStyle(1001);

    }


    ////////////////////////////
    leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg2_->SetTextFont(62);
    leg2_->SetBorderSize(0);
    leg2_->SetLineColor(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColor(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(signalHistWH_,"Base","l");
    leg2_->AddEntry(signalSystHistWH_[0],"METup","l");
    leg2_->AddEntry(signalSystHistWH_[1],"METdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");


    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColor(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColor(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(signalHistWH_,"Base","l");
    leg_->AddEntry(signalSystHistWH_[2],"METup","l");
    leg_->AddEntry(signalSystHistWH_[3],"METdown","l");

    signalHistWH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistWH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[2]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[2]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[3]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[3]->GetXaxis()->SetRangeUser(0.001,1.001);    

    for(int mm=0; mm<signalHistWH_->GetNbinsX(); mm++)
      cout << "bin: " << mm << ", diff up: " << signalHistWH_->GetBinContent(mm) - signalSystHistWH_[2]->GetBinContent(mm) << ", diff down: " << signalHistWH_->GetBinContent(mm) - signalSystHistWH_[3]->GetBinContent(mm) << endl;

    signalSystHistWH_[2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistWH_->SetLineColor(1);
    signalSystHistWH_[2]->SetLineColor(2);
    signalSystHistWH_[3]->SetLineColor(3);
    signalSystHistWH_[2]->Draw("hist");
    signalSystHistWH_[3]->Draw("histsame");
    signalHistWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColor(signalSampleColor_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[2]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[2]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[3]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[3]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColor(1);
    signalSystHistHH_[2]->SetLineColor(2);
    signalSystHistHH_[3]->SetLineColor(3);
    signalSystHistHH_[2]->Draw("hist");
    signalSystHistHH_[3]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColor(signalSampleColor_);
    
    ddbkgSystHist_[2]->SetFillStyle(0);
    ddbkgSystHist_[3]->SetFillStyle(0);
    ddbkgHist_->SetFillStyle(0);

    ddbkgHist_->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgHist_->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[2]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[2]->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[3]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[3]->GetXaxis()->SetRangeUser(0.001,1.001);    

    ddbkgSystHist_[2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    ddbkgHist_->SetMarkerColor(1);
    ddbkgSystHist_[2]->SetLineColor(2);
    ddbkgSystHist_[3]->SetLineColor(3);
    ddbkgSystHist_[2]->Draw("hist");
    ddbkgSystHist_[3]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColor(ddBkgSampleColor_);
    
    ddbkgSystHist_[2]->SetFillStyle(1001);
    ddbkgSystHist_[3]->SetFillStyle(1001);
    ddbkgHist_->SetFillStyle(1001);


    for(size_t f=0; f<nMcSamples_+2; f++){

      mcbkgSystHist_[2][f]->SetFillStyle(0);
      mcbkgSystHist_[3][f]->SetFillStyle(0);
      mcbkgHist_[f]->SetFillStyle(0);

      mcbkgHist_[f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgHist_[f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[2][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[2][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[3][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[3][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
            
      mcbkgSystHist_[2][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
      mcbkgHist_[f]->SetLineColor(1);
      mcbkgSystHist_[2][f]->SetLineColor(2);
      mcbkgSystHist_[3][f]->SetLineColor(3);
      mcbkgSystHist_[2][f]->Draw("hist");
      mcbkgSystHist_[3][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColor(mcBkgSampleColor_[f]);
      mcbkgSystHist_[2][f]->SetFillStyle(1001);
      mcbkgSystHist_[3][f]->SetFillStyle(1001);
      mcbkgHist_[f]->SetFillStyle(1001);

    }

    /////////////////////////

    leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg2_->SetTextFont(62);
    leg2_->SetBorderSize(0);
    leg2_->SetLineColor(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColor(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(signalHistWH_,"Base","l");
    leg2_->AddEntry(signalSystHistWH_[0],"JERup","l");
    leg2_->AddEntry(signalSystHistWH_[1],"JERdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");

    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColor(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColor(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(signalHistWH_,"Base","l");
    leg_->AddEntry(signalSystHistWH_[4],"JERup","l");
    leg_->AddEntry(signalSystHistWH_[5],"JERdown","l");

    signalHistWH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistWH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[4]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[4]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistWH_[5]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistWH_[5]->GetXaxis()->SetRangeUser(0.001,1.001);    
    
    signalSystHistWH_[4]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistWH_->SetLineColor(1);
    signalSystHistWH_[4]->SetLineColor(2);
    signalSystHistWH_[5]->SetLineColor(3);
    signalSystHistWH_[4]->Draw("hist");
    signalSystHistWH_[5]->Draw("histsame");
    signalHistWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColor(signalSampleColor_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[4]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[4]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[5]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[5]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[4]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColor(1);
    signalSystHistHH_[4]->SetLineColor(2);
    signalSystHistHH_[5]->SetLineColor(3);
    signalSystHistHH_[4]->Draw("hist");
    signalSystHistHH_[5]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColor(signalSampleColor_);
    
    ddbkgSystHist_[4]->SetFillStyle(0);
    ddbkgSystHist_[5]->SetFillStyle(0);
    ddbkgHist_->SetFillStyle(0);

    ddbkgHist_->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgHist_->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[4]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[4]->GetXaxis()->SetRangeUser(0.001,1.001);    
    ddbkgSystHist_[5]->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgSystHist_[5]->GetXaxis()->SetRangeUser(0.001,1.001);    
    
    ddbkgSystHist_[4]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    ddbkgHist_->SetMarkerColor(1);
    ddbkgSystHist_[4]->SetLineColor(2);
    ddbkgSystHist_[5]->SetLineColor(3);
    ddbkgSystHist_[4]->Draw("hist");
    ddbkgSystHist_[5]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColor(ddBkgSampleColor_);

    
    ddbkgSystHist_[4]->SetFillStyle(1001);
    ddbkgSystHist_[5]->SetFillStyle(1001);
    ddbkgHist_->SetFillStyle(1001);


    for(size_t f=0; f<nMcSamples_+2; f++){

      mcbkgSystHist_[4][f]->SetFillStyle(0);
      mcbkgSystHist_[5][f]->SetFillStyle(0);
      mcbkgHist_[f]->SetFillStyle(0);

      mcbkgHist_[f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgHist_[f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[4][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[4][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      mcbkgSystHist_[5][f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgSystHist_[5][f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      
      mcbkgSystHist_[4][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
      mcbkgHist_[f]->SetLineColor(1);
      mcbkgSystHist_[4][f]->SetLineColor(2);
      mcbkgSystHist_[5][f]->SetLineColor(3);
      mcbkgSystHist_[4][f]->Draw("hist");
      mcbkgSystHist_[5][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColor(mcBkgSampleColor_[f]);
      mcbkgSystHist_[4][f]->SetFillStyle(1001);
      mcbkgSystHist_[5][f]->SetFillStyle(1001);
      mcbkgHist_[f]->SetFillStyle(1001);

    }













    //// END SYST

   //  leg_ = new TLegend(0.3,0.635,0.63,0.93,NULL,"brNDC");
    leg_ = new TLegend(0.23,0.535,0.62,0.93,NULL,"brNDC");
    //  leg_ = new TLegend(0.7147651,0.6346154,0.9446309,0.9353147,NULL,"brNDC");
    //leg_ = new TLegend(0.75,0.6346154,1.,1.,NULL,"brNDC");
    
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColor(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColor(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(dataHist_,(dataFancySampleName_).c_str(),"lep");
    leg_->AddEntry(signalHistWH_,signalFancySampleNameWH_.c_str(),"l");
    //  leg_->AddEntry(signalHistHH_,signalSampleNameHH_.c_str(),"f");
    leg_->AddEntry(ddbkgHist_,ddBkgFancySampleName_.c_str(),"f");
    for(size_t f=0; f<nMcSamples_+2; f++) leg_->AddEntry(mcbkgHist_[f],mcBkgFancySampleName_[f].c_str(),"f");
    // for fig7 // for(size_t f=0; f<nMcSamples_; f++) leg_->AddEntry(mcbkgHist_[f],mcBkgFancySampleName_[f].c_str(),"f");
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries
    
    

    perMassPointSignalShapesToCompareHH_.push_back((TH1*)signalHistHH_->Clone(signalHistHH_->GetName() +TString("comparison") + outputFileNameSuffix_[currentMassPoint_].c_str()) );
    perMassPointSignalShapesToCompareWH_.push_back((TH1*)signalHistWH_->Clone(signalHistWH_->GetName() +TString("comparison") + outputFileNameSuffix_[currentMassPoint_].c_str()) );
    
    TH1* higgsH_ = 0;
    double cHiggsBR_ = 0.05; // Perhaps move to cfg file.
    double fhh(cHiggsBR_*cHiggsBR_) , fhw( 2*(1-cHiggsBR_)*cHiggsBR_), ftt(1-fhh-fhw);
    //  signalHistWH_->Scale(fhw/signalHistWH_->Integral());
    //  signalHistHH_->Scale(fhh/signalHistHH_->Integral());
    
    //  signalHistWH_->Add(signalHistHH_, fhh);
    higgsH_ = signalHistWH_;
    higgsH_->Scale(fhw/higgsH_->Integral());
    higgsH_->Add(signalHistHH_,fhh);
    
    perMassPointSignalShapesToCompare_.push_back((TH1*)higgsH_->Clone(higgsH_->GetName() + TString("totComparison") + outputFileNameSuffix_[currentMassPoint_].c_str() ) );    
    // Stacked drawing -------------------------------------------
    
    
    
    dataHist_->GetYaxis()->SetTitle("a.u.");
    dataHist_->GetYaxis()->SetTitleOffset(0.85);
    dataHist_->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
    dataHist_->GetXaxis()->SetTitleOffset(0.85);
    
    cout << " DATA HIST BIN 0: " << dataHist_->GetBinContent(0) << " +/- " << dataHist_->GetBinError(0) << endl;
    cout << " DATA HIST BIN 1: " << dataHist_->GetBinContent(1) << " +/- " << dataHist_->GetBinError(1) << endl;
    
    signalHistWH_->GetYaxis()->SetTitle("a.u.");
    signalHistWH_->GetYaxis()->SetTitleOffset(0.85);
    signalHistWH_->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
    signalHistWH_->GetXaxis()->SetTitleOffset(0.85);
    
    

    THStack hs("hs","stacked");
    
    signalHistHH_->DrawNormalized("histsame");
    for(size_t f=0; f<nMcSamples_+2; f++){
      // for fig7 // for(size_t f=0; f<nMcSamples_; f++){
      mcbkgHist_[f]->GetYaxis()->SetTitle("a.u.");
      mcbkgHist_[f]->GetYaxis()->SetTitleOffset(0.85);
      mcbkgHist_[f]->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
      mcbkgHist_[f]->GetXaxis()->SetTitleOffset(0.85);
      mcbkgHist_[f]->GetXaxis()->SetTitleSize(5);
      mcbkgHist_[f]->GetXaxis()->SetRange(0.001,1.001);    
      mcbkgHist_[f]->GetXaxis()->SetRangeUser(0.001,1.001);    
      hs.Add(mcbkgHist_[f],"hist");
    }
    ddbkgHist_->GetYaxis()->SetTitle("a.u.");
    ddbkgHist_->GetYaxis()->SetTitleOffset(0.85);
    ddbkgHist_->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
    ddbkgHist_->GetXaxis()->SetTitleOffset(0.85);
    ddbkgHist_->GetXaxis()->SetTitleSize(5);
    ddbkgHist_->GetXaxis()->SetRange(0.001,1.001);    
    ddbkgHist_->GetXaxis()->SetRangeUser(0.001,1.001);    
    hs.Add(ddbkgHist_,"hist");
    //  dataHist_->SetMarkerStyle(1);
    //  dataHist_->SetMarkerSize(0.8);
    
 
    cout << "dd integral: " << ddbkgHist_->Integral() << endl;
    normalize(hs, 1.);
    hs.SetMaximum(0.4);
    hs.Draw("hist");
    hs.GetXaxis()->SetRange(0.001,1.001);    
    hs.GetXaxis()->SetRangeUser(0.001,1.001);    
    hs.GetYaxis()->SetTitle("a.u.");
    //  hs.GetYaxis()->SetTitleOffset(1.5);
    hs.GetXaxis()->SetTitle("p^{lead.track}/E^{#tau}");
    //  hs.GetXaxis()->SetTitleOffset(1.5);
    
    dataHist_->Scale(1./dataHist_->Integral());
    dataHist_->GetXaxis()->SetRange(0.001,1.001);    
    dataHist_->GetXaxis()->SetRangeUser(0.001,1.001);    
  
    dataHist_->Draw("same");
    higgsH_->Scale(1./signalHistWH_->Integral());
    higgsH_->GetXaxis()->SetRange(0.001,1.001);    
    higgsH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    higgsH_->Draw("histsame");
    
    TGraphErrors myBkgError;
    getErrorBands(hs, myBkgError);
    myBkgError.SetFillColor(1);
    myBkgError.SetFillStyle(3004);
    myBkgError.GetYaxis()->SetTitle("a.u.");
    myBkgError.GetYaxis()->SetTitleOffset(0.85);
    myBkgError.GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
    myBkgError.GetXaxis()->SetTitleOffset(0.85);
    myBkgError.GetXaxis()->SetTitleSize(5);
    myBkgError.GetXaxis()->SetRange(0.001,1.001);    
    myBkgError.GetXaxis()->SetRangeUser(0.001,1.001);    
    myBkgError.Draw("2");
    
    TGraphErrors mySignalError;
    getErrorBands(*higgsH_, mySignalError);
    mySignalError.SetName("blahSignalError");
    mySignalError.SetFillColor(616);
    mySignalError.SetFillStyle(3005);
    mySignalError.GetYaxis()->SetTitle("a.u.");
    mySignalError.GetYaxis()->SetTitleOffset(0.85);
    mySignalError.GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
    mySignalError.GetXaxis()->SetTitleOffset(0.85);
    mySignalError.GetXaxis()->SetTitleSize(5);
    mySignalError.GetXaxis()->SetRange(0.001,1.001);    
    mySignalError.GetXaxis()->SetRangeUser(0.001,1.001);    
    mySignalError.Draw("2");
    
    leg_->AddEntry(myBkgError.GetName(),"bkg total unc.","f");
    leg_->AddEntry(mySignalError.GetName(),"signal total unc.","f");
    
    TPaveText *pt1 = new TPaveText(0.17,0.45,0.65,0.5, "brNDC");
    pt1->SetBorderSize(1);
    pt1->SetFillColor(19);
    pt1->SetFillStyle(0);
    pt1->SetLineColor(0);
    pt1->SetTextFont(132);
    pt1->SetTextSize(0.033);
    //  TText *text = pt1->AddText("#splitline{m_{H^{#pm}} = 120 GeV/c^{2},}{BR(t #rightarrow H^{+}b) = 0.05}");
    TText *text = pt1->AddText("BR(t #rightarrow H^{+}b) = 0.05");
    text->SetTextAlign(11);
    pt1->Draw("same");
    
    TPaveText *pt = new TPaveText(0.15,0.93,0.9,1.0, "brNDC");
    pt->SetBorderSize(1);
    pt->SetFillColor(19);
    pt->SetFillStyle(0);
    pt->SetLineColor(0);
    pt->SetTextFont(132);
    pt->SetTextSize(0.045);
    //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.1 fb^{-1}  CMS Preliminary");
    //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.1 fb^{-1} CMS ");
    //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.2 fb^{-1} CMS Preliminary");
    //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  1.9 fb^{-1}  CMS Preliminary");
    //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  1.9 fb^{-1}  CMS ");
    //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  4.0 fb^{-1} CMS Preliminary");
    //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  4.7 fb^{-1} CMS Preliminary");
    TText *textPrel = pt->AddText("#sqrt{s} = 7 TeV,  4.9 fb^{-1} CMS Preliminary");
    textPrel->SetTextAlign(11);
    pt->Draw("same");
    
    
    // End stacked drawing ---------------------------------------------
    
    // Normal drawing ----------------------------------------
    //  signalHistWH_->Draw("hist");
    //  //  signalHistHH_->DrawNormalized("histsame");
    //  
    //  ddbkgHist_->Draw("histsame");
    //  for(size_t f=0; f<nMcSamples_; f++)
    //    mcbkgHist_[f]->Draw("histsame");
    //  dataHist_->Draw("histsame");    
    // End normal drawing -----------------------------------
    
    leg_->Draw();
    
    canvas_->SaveAs((outFolder_+outputFileName+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string(".png")).c_str());
    canvas_->cd();
    canvas_->Clear();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    

    
  } // End if !produceOnly_


  // Set names
  signalHistWH_->SetName(signalSampleNameWH_.c_str());
  signalHistHH_->SetName(signalSampleNameHH_.c_str());
  ddbkgHist_->SetName(ddBkgSampleName_.c_str());
  for(size_t f=0; f<nMcSamples_+2; f++)
    mcbkgHist_[f]->SetName(mcBkgSampleName_[f].c_str());
//  mcbkgHist_[nMcSamples_]->SetName(mcBkgSampleName_[nMcSamples_].c_str());
//  mcbkgHist_[nMcSamples_+1]->SetName(mcBkgSampleName_[nMcSamples_+1].c_str());

  signalHistWHStatUp_->SetName(signalHistWH_->GetName()+TString("_")+signalHistWH_->GetName()+TString("_StatUp"));
  signalHistHHStatUp_->SetName(signalHistHH_->GetName()+TString("_")+signalHistHH_->GetName()+TString("_StatUp"));
  ddbkgHistStatUp_->SetName(ddbkgHist_->GetName()+TString("_")+ddbkgHist_->GetName()+TString("_StatUp"));
  for(size_t f=0; f<nMcSamples_+2; f++)
    mcbkgHistStatUp_[f]->SetName(mcBkgSampleName_[f].c_str()+TString("_")+mcBkgSampleName_[f].c_str()+TString("_StatUp"));
//  mcbkgHist_[nMcSamples_]->SetName(mcBkgSampleName_[nMcSamples_].c_str());
//  mcbkgHist_[nMcSamples_+1]->SetName(mcBkgSampleName_[nMcSamples_+1].c_str());
  dataHistStatUp_->SetName(dataHist_->GetName()+TString("_")+dataHist_->GetName()+TString("_StatUp"));


  signalHistWHStatDown_->SetName(signalSampleNameWH_.c_str()+TString("_")+signalSampleNameWH_.c_str()+TString("_StatDown"));
  signalHistHHStatDown_->SetName(signalSampleNameHH_.c_str()+TString("_")+signalSampleNameHH_.c_str()+TString("_StatDown"));
  ddbkgHistStatDown_->SetName(ddbkgHist_->GetName()+TString("_")+ddbkgHist_->GetName()+TString("_StatDown"));
  for(size_t f=0; f<nMcSamples_+2; f++)
    mcbkgHistStatDown_[f]->SetName(mcBkgSampleName_[f].c_str()+TString("_")+mcBkgSampleName_[f].c_str()+TString("_StatDown"));
//  mcbkgHist_[nMcSamples_]->SetName(mcBkgSampleName_[nMcSamples_].c_str());
//  mcbkgHist_[nMcSamples_+1]->SetName(mcBkgSampleName_[nMcSamples_+1].c_str());
  dataHistStatDown_->SetName(dataHist_->GetName()+TString("_")+dataHist_->GetName()+TString("_StatDown"));
  
  // Syst case
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    signalSystHistWH_[a]->SetName(signalSampleNameWH_.c_str()+systFancyComponents_[a]);
    signalSystHistHH_[a]->SetName(signalSampleNameHH_.c_str()+systFancyComponents_[a]);
    ddbkgSystHist_[a]->SetName(ddBkgSampleName_.c_str()+systFancyComponents_[a]);
    for(size_t f=0; f<nMcSamples_; f++){
      mcbkgSystHist_[a][f]->SetName(mcBkgSampleName_[f].c_str()+systFancyComponents_[a]);
    }
    mcbkgSystHist_[a][nMcSamples_]->SetName(mcBkgSampleName_[nMcSamples_].c_str()+systFancyComponents_[a]);
    mcbkgSystHist_[a][nMcSamples_+1]->SetName(mcBkgSampleName_[nMcSamples_+1].c_str()+systFancyComponents_[a]);
  } // End syst loop
    
  // End syst case
  
  
  dataHist_->SetName(dataSampleName_.c_str());
  
  if(produceOnly_){
    outputFile->Write();
    outputFile->Close();
  }


 
  cout << "File closed" << endl;

}

void LandSShapesProducer::StorePerMassSignalShapes(){
  for(size_t i=0; i<perMassPointSignalShapesToCompare_.size(); i++){
    perMassPointSignalShapesToCompare_[i]  ->SetLineColor(currentMassPoint_);
    perMassPointSignalShapesToCompareHH_[i]->SetLineColor(currentMassPoint_);
    perMassPointSignalShapesToCompareWH_[i]->SetLineColor(currentMassPoint_);
  }
  signalShapesToCompare_  .push_back(perMassPointSignalShapesToCompare_  );
  signalShapesToCompareHH_.push_back(perMassPointSignalShapesToCompareHH_);
  signalShapesToCompareWH_.push_back(perMassPointSignalShapesToCompareWH_);

  cout << "Stored per mass signal shapes" << endl;
}

void LandSShapesProducer::DrawSignalShapesComparison(){
  cout << "Drawing signal shapes comparisons" << endl;
  canvas_->cd();
  
  leg_ = new TLegend(0.23,0.535,0.62,0.93,NULL,"brNDC");
  //  leg_ = new TLegend(0.7147651,0.6346154,0.9446309,0.9353147,NULL,"brNDC");
  //leg_ = new TLegend(0.75,0.6346154,1.,1.,NULL,"brNDC");
  leg_->SetTextFont(62);
  leg_->SetBorderSize(0);
  leg_->SetLineColor(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColor(0);
  leg_->SetFillStyle(0);
  
  for(size_t s=0; s<signalShapesToCompare_.size(); s++){
    //    for(size_t i=0; i<signalShapesToCompare_[s].size(); i++)
    leg_->AddEntry(signalShapesToCompare_[s][0],(outputFileNameSuffix_[s]).c_str(),"l");
  }  
  
  // Draw them
  for(size_t i=0; i<nVars_; i++){
    canvas_->cd();
    for(size_t s=0; s<signalShapesToCompare_.size(); s++){     
      cout << "DEBUG: signalShapesToCompare: " << signalShapesToCompare_[s].size() << endl;
      signalShapesToCompare_[s][i]->SetLineColor(s);
      cout << "DEBUG: line color has been set" << endl;
      if(s=0) signalShapesToCompare_[s][i]->Draw("hist");
      else signalShapesToCompare_[s][i]->Draw("histsame");
      cout << "DEBUG: histograms have been drawn" << endl;
    }
    leg_->Draw();
    string outputFileName = outputFileName_ + string("_") + fitVars_[i]->getVarName();
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalShapesComparison")+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalShapesComparison")+string(".png")).c_str());
    canvas_->Clear();
    cout << "DEBUG: canvas saved and cleared" << endl;
  }

  for(size_t i=0; i<nVars_; i++){
    canvas_->cd();
    for(size_t s=0; s<signalShapesToCompareHH_.size(); s++){     
      signalShapesToCompareHH_[s][i]->SetLineColor(s);
      if(s=0) signalShapesToCompareHH_[s][i]->Draw("hist");
      else signalShapesToCompareHH_[s][i]->Draw("histsame");
    }
    leg_->Draw();
    string outputFileName = outputFileName_ + string("_") + fitVars_[i]->getVarName();
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalHHShapesComparison")+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalHHShapesComparison")+string(".png")).c_str());
    canvas_->Clear();
  }

  for(size_t i=0; i<nVars_; i++){
    canvas_->cd();
    for(size_t s=0; s<signalShapesToCompareWH_.size(); s++){     
      signalShapesToCompareWH_[s][i]->SetLineColor(s);
      if(s=0) signalShapesToCompareWH_[s][i]->Draw("hist");
      else signalShapesToCompareWH_[s][i]->Draw("histsame");
    }
    leg_->Draw();
    string outputFileName = outputFileName_ + string("_") + fitVars_[i]->getVarName();
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalWHShapesComparison")+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_signalWHShapesComparison")+string(".png")).c_str());
    canvas_->Clear();
  }

    
}

void LandSShapesProducer::Produce(){
  
  //cout << "INIT: signal tree entries:"<<signalTree_->GetEntries()<<endl;

  for(size_t s=0; s< nMassPoints_; s++){
    
    InitMassPoint(s);
    
    for(size_t i = 0; i< nVars_; i++){
      
      InitPerVariableAmbient(i);
      
      BuildDatasets(i);
      
      //    BuildPDFs(i);
      
      DrawTemplates(i);
      
      //    BuildConstrainedModels(i);
      //    
      //    DoPerVariableFit(i);
      //    
      //    DrawPerVariableFit(i);
      //    
      //    DoPerVariableLikelihoodFit(i);
      
    }
    
    //    StorePerMassSignalShapes();
  }
  //  DoCombinedLikelihoodFit();
  //  DrawSignalShapesComparison();
  //}
  //  cout.rdbuf(old); // restore   
}
