#include "LIP/TopTaus/interface/TauDileptonPDFBuilderFitter.hh"

// System includes
#include <sstream>

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// ROOT includes
#include "RooMinuit.h"
#include "RooConstVar.h"
#include <THStack.h>

using namespace std;
using namespace RooFit;

TauDileptonPDFBuilderFitter::TauDileptonPDFBuilderFitter(string parSet):
  parSet_(parSet)
{
  
  using namespace std; 
  using namespace RooFit ;
  
  Init();

  SetOptions();

}

TauDileptonPDFBuilderFitter::~TauDileptonPDFBuilderFitter(){
  
  delete myStyle_;
  delete signalFileWH_;
  delete signalFileHH_;
  delete ddBkgFile_;
  delete ttbarmcBkgFile_;
  delete mcBkgFile_;
  delete dataFile_;
  delete signalTreeWH_;
  delete signalTreeHH_; 
  delete ddBkgTree_ ;
  delete ttbarmcBkgTree_;
  delete mcBkgTree_;
  delete dataTree_;
  delete canvas_;
  delete sigVar_             ;              
  delete sigMeanVar_         ;              
  delete sigSigmaVar_        ;              
  delete ttbarmcbkgVar_      ;
  delete ttbarmcbkgMeanVar_  ;
  delete ttbarmcbkgSigmaVar_ ;
  delete mcbkgVar_           ;               
  delete mcbkgMeanVar_       ;         
  delete mcbkgSigmaVar_      ;         
  delete ddbkgVar_           ;              
  delete ddbkgMeanVar_       ;         
  delete ddbkgSigmaVar_      ;        
  delete myvar_         ;
  delete myvar_weights_ ;
  delete isOSvar_       ;
  delete mySignalDS_      ;
  delete unrMyDDBkgDS_      ; // Unreduced datasets for OS cut
  delete unrMyTTBARMCBkgDS_ ;
  delete unrMyMCBkgDS_      ; 
  delete unrMyDataDS_       ; 
  delete myDDBkgDS_      ; // Reduced datasets
  delete myTTBARMCBkgDS_ ;
  delete myMCBkgDS_      ; 
  delete myDataDS_       ; 
  delete signalHisto_    ;
  delete ttbarmcbkgHisto_;
  delete mcbkgHisto_     ;
  delete ddbkgHisto_     ;
  delete dataHisto_      ;
  delete b_signalModel_    ;
  delete b_ddbkgModel_     ;
  delete b_ttbarmcbkgModel_;  
  delete b_mcbkgModel_     ;  
  delete u_signalModel_    ; 
  delete u_ddbkgModel_     ;  
  delete u_ttbarmcbkgModel_;  
  delete u_mcbkgModel_     ;  
  delete signalHist_;
  delete ddbkgHist_;
  delete ttbarmcbkgHist_;
  delete mcbkgHist_;
  delete dataHist_;
  delete leg_;
  delete signalConstraint_;
  delete ttbarmcbkgConstraint_;
  delete ddbkgConstraint_;
  delete mcbkgConstraint_;
  delete sumModel_;
  delete model_;
  delete constrainedModelFit_;
  delete myFrame_;
  delete nll_;
  for(size_t i=0; i<nVars_;i++)
    delete likelihoodVector_[i];
  delete myNllFitResult_;
  delete contourPlot_;
  delete combll_;

}

  
void TauDileptonPDFBuilderFitter::Init(){

  // Clear vectors
  nVars_ = 0;
  fitVars_.clear();
  vars_.clear();
  mins_.clear(); 
  maxs_.clear();
  bins_.clear();
  hmin_.clear();
  hmax_.clear();
  unbinned_.clear(); 
  smoothOrder_.clear();


  likelihoodVector_.clear();

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
  
  // Get ParameterSet from cfg file
  const edm::ParameterSet &mFitPars = edm::readPSetsFrom(parSet_)->getParameter<edm::ParameterSet>("TauDileptonPDFBuilderFitterParSet");

  outFolder_        = mFitPars.getParameter<std::string>("outFolder");
  resultsFileName_  = mFitPars.getParameter<std::string>("resultsFileName");
  baseMCDir_        = mFitPars.getParameter<std::string>("baseMCDir");
  baseDataDir_      = mFitPars.getParameter<std::string>("baseDataDir");

  useOS_            = mFitPars.getParameter<bool>("useOS");  
  cHiggsBR_         = mFitPars.getParameter<double>("cHiggsBR");
  catNBtags_        = mFitPars.getParameter<int>("NBtags");

  signalFileNameWH_   = mFitPars.getParameter<std::string>("signalFileNameWH");
  signalFileNameHH_   = mFitPars.getParameter<std::string>("signalFileNameHH");
  dataFileName_       = mFitPars.getParameter<std::string>("dataFileName");
  ddBkgFileName_      = mFitPars.getParameter<std::string>("ddBkgFileName");
  ttbarmcBkgFileName_ = mFitPars.getParameter<std::string>("ttbarmcBkgFileName");
  mcBkgFileName_      = mFitPars.getParameter<std::string>("mcBkgFileName");
  
  minitreeSelected_   = mFitPars.getParameter<std::string>("minitreeSelected");
  minitreeDataDriven_ = mFitPars.getParameter<std::string>("minitreeDataDriven");
  minitreeUnc_   = mFitPars.getParameter<std::string>("minitreeUnc");
  
  vector<int> tempFitType;
  tempFitType.clear();
  tempFitType     = mFitPars.getParameter<vector<int> >("fitType");
  for(size_t k=0; k<tempFitType.size(); k++){
    cout << "fitType chosen: " << tempFitType[k] << ". SM3BKG = " << SM3BKG << endl;
    if(tempFitType[k]==SM2BKG) fitType_.push_back(SM2BKG);
    if(tempFitType[k]==SM3BKG) fitType_.push_back(SM3BKG);
    if(tempFitType[k]==HIGGS2BKG) fitType_.push_back(HIGGS2BKG);
    if(tempFitType[k]==HIGGS3BKG) fitType_.push_back(HIGGS3BKG);
  }

  osCutEff_    = mFitPars.getParameter<double>("osCutEff");

  vars_        = mFitPars.getParameter<vector<string> >("vars");
  mins_        = mFitPars.getParameter<vector<double> >("mins");
  maxs_        = mFitPars.getParameter<vector<double> >("maxs");
  bins_        = mFitPars.getParameter<vector<int> >("bins");
  hmin_        = mFitPars.getParameter<vector<double> >("hmin");
  hmax_        = mFitPars.getParameter<vector<double> >("hmax");
  unbinned_    = mFitPars.getParameter<vector<Int_t> >("unbinned");
  smoothOrder_ = mFitPars.getParameter<vector<Int_t> >("smoothOrder");
  
  

  // Set variables
  //
  nVars_ = vars_.size();
  
  for(size_t i=0; i<nVars_; i++)
    fitVars_.push_back(new FitVar(vars_[i], mins_[i], maxs_[i], bins_[i], hmin_[i], hmax_[i], unbinned_[i], smoothOrder_[i]));
      
  // Set canvas
  SetTDRStyle();
  canvas_ = new TCanvas("canvas","My plots ",0,0,1000,500);
  //  canvas_->SetCanvasColor(kWhite);
  canvas_->cd();
  
 
  //  Uncomment following line in order to redirect stdout to file
  //  streamToFile_ = std::cout.rdbuf(resultsFile_.rdbuf());

  // Open files and get trees
  // ddBkg is the only to be taken from data driven estimation (tree)
  //signalFileWH_   = TFile::Open(baseMCDir_   + signalFileNameWH_  ); signalTreeWH_   = (TTree*) signalFileWH_  ->Get(minitreeSelected_);
  //signalFileHH_   = TFile::Open(baseMCDir_   + signalFileNameHH_  ); signalTreeHH_   = (TTree*) signalFileHH_  ->Get(minitreeSelected_);
  ddBkgFile_      = TFile::Open(baseDataDir_   + ddBkgFileName_     ); ddBkgTree_      = (TTree*) ddBkgFile_     ->Get(minitreeDataDriven_);
  ttbarmcBkgFile_ = TFile::Open(baseMCDir_   + ttbarmcBkgFileName_); ttbarmcBkgTree_ = (TTree*) ttbarmcBkgFile_->Get(minitreeSelected_);
  mcBkgFile_      = TFile::Open(baseMCDir_   + mcBkgFileName_     ); mcBkgTree_      = (TTree*) mcBkgFile_     ->Get(minitreeSelected_);
  dataFile_       = TFile::Open(baseDataDir_ + dataFileName_      ); dataTree_       = (TTree*) dataFile_      ->Get(minitreeSelected_);

  
  cout << "Init process complete" << endl;
}

void TauDileptonPDFBuilderFitter::SetOptions(){
  //  myStyle_->SetOptStat(0);
}

void TauDileptonPDFBuilderFitter::InitFitSettings(size_t f){
  
  // Clear corr vars array (for BuildDatasetsWithCorrelations)
  myvars_.clear();
  
  baseIdentifier_="";
  
  // FIXME: hardcoded
  signalStatError_= 6.5;  //  antiMuTight: 6.5 (wh) and 0.1 (hh) propagation                  8.7; //                   6.7; // 8.7 (wh) and 0.2 (hh) propagation 
  ddbkgStatError_ =35.71;// total error.stat +syst  
  
  switch(fitType_[f]){
  case SM2BKG :
    includeSignal_=false;
    standaloneTTbar_=false;
    baseIdentifier_.append("SM2BKG");
    mcbkgStatError_ = 17.89; // 2.8*2.8  +0.8*0.8+0.4*0.4+17.6*17.6+1.2*1.2+0.5*0.5  // MUST UPDATE
    break;
  case SM3BKG:
    includeSignal_=false;
    standaloneTTbar_=true;
    baseIdentifier_.append("SM3BKG");
    ttbarmcbkgStatError_ = 21.2; //only stat  39.9=stat+syst
    mcbkgStatError_ = 5.2;//only stat 6=stat+syst 
    break;
  case HIGGS2BKG:
    includeSignal_=true;
    standaloneTTbar_=false;
    baseIdentifier_.append("HIGGS2BKG");
    mcbkgStatError_ = 17.89; // 2.8*2.8  +0.8*0.8+0.4*0.4+17.6*17.6+1.2*1.2+0.5*0.5  // MUST UPDATE
    break;
  case HIGGS3BKG:
    includeSignal_=true;
    standaloneTTbar_=true;
    baseIdentifier_.append("HIGGS3BKG");
    ttbarmcbkgStatError_ = 2.8; // 2.8
    mcbkgStatError_ = 17.67;// +0.8*0.8+0.4*0.4+17.6*17.6+1.2*1.2+0.5*0.5 
    break;
  default : // Dummy - should never arrive here
    cout<<"Type of fit not available. Check your options motherfucker"<<endl;
  }
  resultsFile_.open ((outFolder_+resultsFileName_+string("_")+baseIdentifier_+string(".txt")).c_str());
  likelihoodVector_.clear();
}

void TauDileptonPDFBuilderFitter::BuildDatasetWithCorrelations(size_t f){
  // Build single dataset with all variables in order to be able to plot correlations
  cout<<"BuildDatasetWithCorrelations start"<<endl;
//   signalTreeWH_   ->ResetBranchAddresses();
//   signalTreeHH_   ->ResetBranchAddresses();
  ddBkgTree_      ->ResetBranchAddresses();
  ttbarmcBkgTree_ ->ResetBranchAddresses();
  mcBkgTree_      ->ResetBranchAddresses();
  dataTree_       ->ResetBranchAddresses();
  myvars_.clear();
  cout<<"BuildDatasetWithCorrelations: reset stuff"<<endl;

  for(size_t i=0; i<nVars_; i++){
    myvars_.push_back( new RooRealVar(fitVars_[i]->getVarName().c_str(), fitVars_[i]->getVarName().c_str(), fitVars_[i]->getMin(), fitVars_[i]->getMax()));
    myvars_[i]->setBins(fitVars_[i]->getBins()); 
  }
    myvar_weightsGlob_   = new RooRealVar("weight","weight",0,1000);
  
  RooArgSet varList(*myvar_weightsGlob_); // blah
  
  for(size_t i=0; i<nVars_; i++)
    varList.add(*(myvars_[i]));
  cout<<"BuildDatasetWithCorrelations: argset is set, lol"<<endl;

  //  RooArgSet(*myvar_,*myvar_weightsGlob_),  


  // Temp variables for setting branch addresses
  std::vector<double> myVarAllocatorVec;
  myVarAllocatorVec.clear();
  for(size_t i=0; i<nVars_; i++)
    myVarAllocatorVec.push_back(0);
  
  double myVarWeightAllocator;
  double isOSsig, btagmult;
  
  double ftt(1);
//   // Signal dataset from WH and HH
//   if(includeSignal_){
//     mySignalDSGlob_         = new RooDataSet(mySignalDSName_.c_str(),mySignalDSName_.c_str(), varList, "weight"); // This constructor does not accept the cut parameter
    
    
//     // Cross section
//     // FIXME: hardcoded. Must bring it to normal values
//     double fhh(cHiggsBR_*cHiggsBR_) , fhw( 2*(1-cHiggsBR_)*cHiggsBR_) ;      
//     ftt=1;//-fhh-fhw;
//     //    ftt=1-fhh-fhw;
    
//     //double fhh(0.1*0.1) , fhw( 2*(1-0.1)*0.1) ;      
//     // Get WH events
//     for(size_t i=0; i<nVars_; i++)
//       signalTreeWH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
//     signalTreeWH_->SetBranchAddress("weight", &myVarWeightAllocator);
//     signalTreeWH_->SetBranchAddress("is_os", &isOSsig);
//     for(unsigned int ev=0; ev<signalTreeWH_->GetEntries(); ev++){
//       signalTreeWH_->GetEntry(ev);
//       if(useOS_ && isOSsig<0.5) continue;
//       for(size_t i=0; i<nVars_; i++)
// 	myvars_[i]->setVal(myVarAllocatorVec[i]);
//       //	myvar_->setVal(myVarAllocator);
//       sumWeights_ += myVarWeightAllocator;
//       myvar_weightsGlob_->setVal(fhw*myVarWeightAllocator);
//       mySignalDSGlob_->add(varList,fhw*myVarWeightAllocator);
//     }
//     // Get HH events
//     for(size_t i=0; i<nVars_; i++)
//       signalTreeHH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
//     signalTreeHH_->SetBranchAddress("weight", &myVarWeightAllocator);
//     signalTreeHH_->SetBranchAddress("is_os", &isOSsig);
//     //    cout << "getIsoS      ";
//     for(unsigned int ev=0; ev<signalTreeHH_->GetEntries(); ev++){
//       signalTreeHH_->GetEntry(ev);
//       if(useOS_ && isOSsig < 0.5) continue;
//       for(size_t i=0; i<nVars_; i++)
// 	myvars_[i]->setVal(myVarAllocatorVec[i]);
//       sumWeights_ += myVarWeightAllocator;
//       myvar_weightsGlob_->setVal(fhh*myVarWeightAllocator);
//       mySignalDSGlob_->add(varList,fhh*myVarWeightAllocator);
//     }
//   }
  
  myDDBkgDSGlob_         = new RooDataSet(myDDBkgDSName_.c_str(),myDDBkgDSName_.c_str(), varList, "weight"); // This constructor does not accept the cut parameter
  // Get DD events
  for(size_t i=0; i<nVars_; i++)
    ddBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
  ddBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  ddBkgTree_->SetBranchAddress("is_os", &isOSsig);
  ddBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
  ////    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<ddBkgTree_->GetEntries(); ev++){
    ddBkgTree_->GetEntry(ev);

    if(useOS_ && isOSsig < 0.5) continue;
    if(catNBtags_ == 1 && btagmult != 1) continue;
    if(catNBtags_ > 1 && btagmult < 2) continue;
    //    cout << "init -- catNBtags = " << catNBtags_ << " btagmult =  " << btagmult << endl ;

    for(size_t i=0; i<nVars_; i++)
      myvars_[i]->setVal(myVarAllocatorVec[i]);
    //      sumWeights_ += myVarWeightAllocator;
    if(useOS_) myvar_weightsGlob_->setVal(osCutEff_*myVarWeightAllocator);
    else myvar_weightsGlob_->setVal(myVarWeightAllocator);
    if(useOS_) myDDBkgDSGlob_->add(varList,osCutEff_*myVarWeightAllocator);
    else myDDBkgDSGlob_->add(varList,myVarWeightAllocator);
  }
  
  if(standaloneTTbar_){
    myTTBARMCBkgDSGlob_         = new RooDataSet(myTTBARMCBkgDSName_.c_str(),myTTBARMCBkgDSName_.c_str(), varList, "weight"); // This constructor does not accept the cut parameter
    // Get TTMCBkg events
    for(size_t i=0; i<nVars_; i++)
      ttbarmcBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
    ttbarmcBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
    ttbarmcBkgTree_->SetBranchAddress("is_os", &isOSsig);
    ttbarmcBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);

    ////    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<ttbarmcBkgTree_->GetEntries(); ev++){
      ttbarmcBkgTree_->GetEntry(ev);

      if(useOS_ && isOSsig < 0.5) continue;
      if(catNBtags_ == 1 && btagmult != 1) continue;
      if(catNBtags_ != 1 && btagmult < 2) continue;

      for(size_t i=0; i<nVars_; i++)
	myvars_[i]->setVal(myVarAllocatorVec[i]);
      //      sumWeights_ += myVarWeightAllocator;
      myvar_weightsGlob_->setVal(myVarWeightAllocator*ftt);
      myTTBARMCBkgDSGlob_->add(varList,myVarWeightAllocator*ftt);
    }
    ftt=1; // reset it to 1 for the mcBkg not to be scaled in case it does not contain ttbar
  }

  
  myMCBkgDSGlob_         = new RooDataSet(myMCBkgDSName_.c_str(),myMCBkgDSName_.c_str(), varList, "weight"); // This constructor does not accept the cut parameter
  // Get MCBkg events
  for(size_t i=0; i<nVars_; i++)
    mcBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
  mcBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  mcBkgTree_->SetBranchAddress("is_os", &isOSsig);
  mcBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
  ////    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<mcBkgTree_->GetEntries(); ev++){
    mcBkgTree_->GetEntry(ev);

    if(useOS_ && isOSsig < 0.5) continue;
    if(catNBtags_ == 1 && btagmult != 1) continue;
    if(catNBtags_ != 1 && btagmult < 2) continue;

    for(size_t i=0; i<nVars_; i++)
      myvars_[i]->setVal(myVarAllocatorVec[i]);
      //      sumWeights_ += myVarWeightAllocator;
    myvar_weightsGlob_->setVal(myVarWeightAllocator*ftt);
    myMCBkgDSGlob_->add(varList,myVarWeightAllocator*ftt);
  }
  
  myDataDSGlob_         = new RooDataSet(myDataDSName_.c_str(),myDataDSName_.c_str(), varList, "weight"); // This constructor does not accept the cut parameter
  // Get Data events
  for(size_t i=0; i<nVars_; i++)
    dataTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocatorVec[i]);
  dataTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  dataTree_->SetBranchAddress("is_os", &isOSsig);
  dataTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
  ////    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<dataTree_->GetEntries(); ev++){
    dataTree_->GetEntry(ev);

    if(useOS_ && isOSsig < 0.5) continue;
    if(catNBtags_ == 1 && btagmult != 1) continue;
    if(catNBtags_ != 1 && btagmult < 2) continue;

    for(size_t i=0; i<nVars_; i++)
      myvars_[i]->setVal(myVarAllocatorVec[i]);
    //      sumWeights_ += myVarWeightAllocator;
    myvar_weightsGlob_->setVal(myVarWeightAllocator);
    myDataDSGlob_->add(varList,myVarWeightAllocator);
  }
  
  
  cout<<"BuildDatasetWithCorrelations has been run correctly"<<endl;  
}

void TauDileptonPDFBuilderFitter::InitPerVariableAmbient(size_t i){

  // Totally necessary (otherwise branches remain tied to the variables and datasets get messed up for nVars_>1)
  //signalTreeWH_   ->ResetBranchAddresses();
  //signalTreeHH_   ->ResetBranchAddresses();
  ddBkgTree_      ->ResetBranchAddresses();
  ttbarmcBkgTree_ ->ResetBranchAddresses();
  mcBkgTree_      ->ResetBranchAddresses();
  dataTree_       ->ResetBranchAddresses();
  

  
  // String identifiers
  switch(fitVars_[i]->getUnbinned()){
  case 1 : // Unbinned
    identifier_=baseIdentifier_+string("_unbinned_")+fitVars_[i]->getVarName();
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    identifier_=baseIdentifier_+string("_binned_")+fitVars_[i]->getVarName();
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
  }
  
  // Binned fit variable
  myvar_           = new RooRealVar(fitVars_[i]->getVarName().c_str(), fitVars_[i]->getVarName().c_str(), fitVars_[i]->getMin(), fitVars_[i]->getMax());  myvar_->setBins(fitVars_[i]->getBins()); 
  myvar_weights_   = new RooRealVar("weight","weight",0,1000);
  isOSvar_         = new RooRealVar("is_os","is_os",0,2);
  
  //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mySignalDSName_= fitVars_[i]->getVarName().c_str() + string("_mySignalDS");
  myDDBkgDSName_ = fitVars_[i]->getVarName().c_str() + string("_myDDBkgDS");
  myTTBARMCBkgDSName_ = fitVars_[i]->getVarName().c_str() + string("_myTTBARMCBkgDS");
  myMCBkgDSName_  = fitVars_[i]->getVarName().c_str() + string("_myMCBkgDS");
  myDataDSName_   = fitVars_[i]->getVarName().c_str() + string("_myDataDS");

  mySignalDS_        = 0; //= new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyDDBkgDS_      = 0; //= new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyTTBARMCBkgDS_ = 0; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyMCBkgDS_      = 0; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyDataDS_       = 0; //= new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   *myvar );
  myDDBkgDS_      = 0; 
  myTTBARMCBkgDS_ = 0; 
  myMCBkgDS_      = 0; 
  myDataDS_       = 0; 

  // Define model names
  signalModelName_           = fitVars_[i]->getVarName() + string("_SignalModel");
  signalConstrainedName_       = fitVars_[i]->getVarName() + string("_signalConstrained");    signalConstraintName_ =fitVars_[i]->getVarName()+string("_signalConstraint");
  ddbkgModelName_            = fitVars_[i]->getVarName() + string("_ddbkgModel");  
  ddbkgConstrainedName_        = fitVars_[i]->getVarName() + string("_ddbkgConstrained");    ddbkgConstraintName_ =fitVars_[i]->getVarName()+string("_ddbkgConstraint");
  ttbarmcbkgModelName_       = fitVars_[i]->getVarName() + string("_ttbarmcbkgModel");  
  ttbarmcbkgConstrainedName_   = fitVars_[i]->getVarName() + string("_ttbarmcbkgConstrained");   ttbarmcbkgConstraintName_ =fitVars_[i]->getVarName()+string("_ttbarmcbkgConstraint");
  mcbkgModelName_            = fitVars_[i]->getVarName() + string("_mcbkgModel");  
  mcbkgConstrainedName_        = fitVars_[i]->getVarName() + string("_mcbkgConstrained");   mcbkgConstraintName_ =fitVars_[i]->getVarName()+string("_mcbkgConstraint");
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Define var names
  signalVarName_     =fitVars_[i]->getVarName()+string("_signalVar");string signalMeanVarName  =fitVars_[i]->getVarName()+string("_signalMeanVar");              string signalSigmaVarName     =fitVars_[i]->getVarName()+string("_signalSigmaVar");
  ddbkgVarName_      =fitVars_[i]->getVarName()+string("_ddbkgVar"); string ddbkgMeanVarName  =fitVars_[i]->getVarName()+string("_ddbkgMeanVar");                string ddbkgSigmaVarName      =fitVars_[i]->getVarName()+string("_ddbkgSigmaVar");
  ttbarmcbkgVarName_ =fitVars_[i]->getVarName()+string("_ttbarmcbkgVar"); string ttbarmcbkgMeanVarName  =fitVars_[i]->getVarName()+string("_ttbarmcbkgMeanVar"); string ttbarmcbkgSigmaVarName =fitVars_[i]->getVarName()+string("_ttbarmcbkgSigmaVar");
  mcbkgVarName_      =fitVars_[i]->getVarName()+string("_mcbkgVar"); string mcbkgMeanVarName  =fitVars_[i]->getVarName()+string("_mcbkgMeanVar");                string mcbkgSigmaVarName      =fitVars_[i]->getVarName()+string("_mcbkgSigmaVar");

  // PDF models
  b_signalModel_     = 0;
  b_ddbkgModel_      = 0;
  b_ttbarmcbkgModel_ = 0;  
  b_mcbkgModel_      = 0;  
  u_signalModel_     = 0; 
  u_ddbkgModel_      = 0;  
  u_ttbarmcbkgModel_ = 0;  
  u_mcbkgModel_      = 0;  

  signalHist_     = 0;
  dataHist_       = 0;
  ddbkgHist_      = 0;
  ttbarmcbkgHist_ = 0;
  mcbkgHist_      = 0;
  leg_            = 0;
  
  signalConstraint_     = 0;
  ttbarmcbkgConstraint_ = 0;
  ddbkgConstraint_      = 0;
  mcbkgConstraint_      = 0;

  sumWeights_ = 0;
  
  sumModel_            = 0;
  model_               = 0;
  constrainedModelFit_ = 0;
  
  myFrame_ = 0;

  nll_ = 0;
  myNllFitResult_ = 0;
  contourPlot_ = 0;

  combll_ = 0;
}

void TauDileptonPDFBuilderFitter::BuildDatasets(size_t i){
  
    // Temp variables for setting branch addresses
    double myVarAllocator, myVarWeightAllocator;
    double isOSsig, btagmult;
    
    double ftt(1);
//     // Signal dataset from WH and HH
//     if(includeSignal_){
//     //  mySignalDS     = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
//     mySignalDS_         = new RooDataSet(mySignalDSName_.c_str(),mySignalDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
    
    
//     // Cross section
//     // FIXME: hardcoded. Must bring it to normal values
//     double fhh(cHiggsBR_*cHiggsBR_) , fhw( 2*(1-cHiggsBR_)*cHiggsBR_) ;      
//     ftt=1;//-fhh-fhw;
//     //    ftt=1-fhh-fhw;

//     //double fhh(0.1*0.1) , fhw( 2*(1-0.1)*0.1) ;      
//     // Get WH events
//     signalTreeWH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
//     signalTreeWH_->SetBranchAddress("weight", &myVarWeightAllocator);
//     signalTreeWH_->SetBranchAddress("is_os", &isOSsig);
//     for(unsigned int ev=0; ev<signalTreeWH_->GetEntries(); ev++){
//       signalTreeWH_->GetEntry(ev);
//       if(useOS_ && isOSsig<0.5) continue;
//       myvar_->setVal(myVarAllocator);
//       sumWeights_ += myVarWeightAllocator;
//       myvar_weights_->setVal(fhw*myVarWeightAllocator);
//       mySignalDS_->add(RooArgSet(*myvar_,*myvar_weights_),fhw*myVarWeightAllocator);
//     }
//     // Get HH events
//     signalTreeHH_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
//     signalTreeHH_->SetBranchAddress("weight", &myVarWeightAllocator);
//     signalTreeHH_->SetBranchAddress("is_os", &isOSsig);
//     //    cout << "getIsoS      ";
//     for(unsigned int ev=0; ev<signalTreeHH_->GetEntries(); ev++){
//       signalTreeHH_->GetEntry(ev);
//       if(useOS_ && isOSsig < 0.5) continue;
//       myvar_->setVal(myVarAllocator);
//       sumWeights_ += myVarWeightAllocator;
//       myvar_weights_->setVal(fhh*myVarWeightAllocator);
//       mySignalDS_->add(RooArgSet(*myvar_,*myvar_weights_),fhh*myVarWeightAllocator);
//     }
//     }

  myDDBkgDS_         = new RooDataSet(myDDBkgDSName_.c_str(),myDDBkgDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
  // Get DD events
  ddBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
  ddBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
  ddBkgTree_->SetBranchAddress("is_os", &isOSsig);
  ddBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
  //    cout << "getIsoS      ";
  for(unsigned int ev=0; ev<ddBkgTree_->GetEntries(); ev++){
    ddBkgTree_->GetEntry(ev);

    if(useOS_ && isOSsig < 0.5) continue;
    if(catNBtags_ == 1 && btagmult != 1) continue;
    if(catNBtags_ != 1 && btagmult < 2) continue;

    myvar_->setVal(myVarAllocator);
    //      sumWeights_ += myVarWeightAllocator;
    if(useOS_) myvar_weights_->setVal(osCutEff_*myVarWeightAllocator);
    else myvar_weights_->setVal(myVarWeightAllocator);
    if(useOS_) myDDBkgDS_->add(RooArgSet(*myvar_,*myvar_weights_),osCutEff_*myVarWeightAllocator);
    else myDDBkgDS_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
  }
  
  if(standaloneTTbar_){
    myTTBARMCBkgDS_         = new RooDataSet(myTTBARMCBkgDSName_.c_str(),myTTBARMCBkgDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
    // Get TTMCBkg events
    ttbarmcBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    ttbarmcBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
    ttbarmcBkgTree_->SetBranchAddress("is_os", &isOSsig);
    ttbarmcBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<ttbarmcBkgTree_->GetEntries(); ev++){
      ttbarmcBkgTree_->GetEntry(ev);

      if(useOS_ && isOSsig < 0.5) continue;
      if(catNBtags_ == 1 && btagmult != 1) continue;
      if(catNBtags_ != 1 && btagmult < 2) continue;
      
      myvar_->setVal(myVarAllocator);
      //      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator*ftt);
      myTTBARMCBkgDS_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator*ftt);
    }
    ftt=1; // reset it to 1 for the mcBkg not to be scaled in case it does not contain ttbar
  }

  
  myMCBkgDS_         = new RooDataSet(myMCBkgDSName_.c_str(),myMCBkgDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
    // Get MCBkg events
    mcBkgTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    mcBkgTree_->SetBranchAddress("weight", &myVarWeightAllocator);
    mcBkgTree_->SetBranchAddress("is_os", &isOSsig);
    mcBkgTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<mcBkgTree_->GetEntries(); ev++){
      mcBkgTree_->GetEntry(ev);

      if(useOS_ && isOSsig < 0.5) continue;
      if(catNBtags_ == 1 && btagmult != 1) continue;
      if(catNBtags_ != 1 && btagmult < 2) continue;

      myvar_->setVal(myVarAllocator);
      //      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator*ftt);
      myMCBkgDS_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator*ftt);
    }

  myDataDS_         = new RooDataSet(myDataDSName_.c_str(),myDataDSName_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter
    // Get Data events
    dataTree_->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    dataTree_->SetBranchAddress("weight", &myVarWeightAllocator);
    dataTree_->SetBranchAddress("is_os", &isOSsig);
    dataTree_->SetBranchAddress("btagmultiplicity_j", &btagmult);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<dataTree_->GetEntries(); ev++){
      dataTree_->GetEntry(ev);

      if(useOS_ && isOSsig < 0.5) continue;
      if(catNBtags_ == 1 && btagmult != 1) continue;
      if(catNBtags_ != 1 && btagmult < 2) continue;
    
      myvar_->setVal(myVarAllocator);
      //      sumWeights_ += myVarWeightAllocator;
      myvar_weights_->setVal(myVarWeightAllocator);
      myDataDS_->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
    }

    // Old.
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
//     if(includeSignal_)
//       signalHisto_ = mySignalDS_->binnedClone();
    ddbkgHisto_ = myDDBkgDS_->binnedClone();
    if(standaloneTTbar_)
      ttbarmcbkgHisto_  = myTTBARMCBkgDS_->binnedClone(); 
    mcbkgHisto_ = myMCBkgDS_->binnedClone();
    dataHisto_  = myDataDS_ ->binnedClone();
    
}

void TauDileptonPDFBuilderFitter::BuildPDFs(size_t i){
  
  switch(fitVars_[i]->getUnbinned()){
  case 1 : // Unbinned
    if(includeSignal_) u_signalModel_ = new RooKeysPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *mySignalDS_, RooKeysPdf::MirrorBoth,2);
    u_ddbkgModel_  = new RooKeysPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(),  *myvar_, *myDDBkgDS_ , RooKeysPdf::MirrorBoth,2);
    if(standaloneTTbar_) u_ttbarmcbkgModel_  = new RooKeysPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *myTTBARMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
    u_mcbkgModel_  = new RooKeysPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *myMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    if(includeSignal_) b_signalModel_ = new RooHistPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *signalHisto_, fitVars_[i]->getSmoothOrder());
    b_ddbkgModel_  = new RooHistPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(), *myvar_, *ddbkgHisto_ , fitVars_[i]->getSmoothOrder());
    if(standaloneTTbar_) b_ttbarmcbkgModel_ = new RooHistPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *ttbarmcbkgHisto_  , fitVars_[i]->getSmoothOrder()); 
    b_mcbkgModel_  = new RooHistPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *mcbkgHisto_  , fitVars_[i]->getSmoothOrder()); 
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
  }
}

void TauDileptonPDFBuilderFitter::DrawTemplates(size_t i){

  SetTDRStyle();

  canvas_->cd();
  canvas_->Clear();

  //signal histogram ////////////////////////////////////////////////////////////
  if(includeSignal_){
    signalHist_ = signalHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
    signalHist_->SetOption("0000");
    signalHist_->SetTitle("");
    signalHist_->GetYaxis()->SetTitle("a.u.");
    signalHist_->GetYaxis()->SetTitleOffset(0.9);
//    signalHist_->SetLineWidth(3);
//    signalHist_->SetLineColor(kGreen);
    signalHist_->SetFillColor(kGreen);
    signalHist_->SetMarkerColor(kGreen);
    signalHist_->SetMarkerStyle(21);
  }
  ///////////////////////////////////////////////////////////////////////////////
  
  
  // data histogram /////////////////////////////////////////////////
  dataHist_ = dataHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  dataHist_->SetLineWidth(3);
  dataHist_->SetLineColor(kBlack);
  //  ddbkgHist_->SetFillColor(kRed);
  //  ddbkgHist_->SetFillStyle(1001);
  dataHist_->SetMarkerColor(kBlack);
  dataHist_->SetMarkerStyle(21);
  /////////////////////////////////////////////////////////////////////
  
  // dd bkg histogram /////////////////////////////////////////////////
  ddbkgHist_ = ddbkgHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );
  ddbkgHist_->SetLineWidth(3);
  ddbkgHist_->SetLineColor(kRed);
  //  ddbkgHist_->SetFillColor(kRed);
  //  ddbkgHist_->SetFillStyle(1001);
  ddbkgHist_->SetMarkerColor(kRed);
  ddbkgHist_->SetMarkerStyle(21);
  /////////////////////////////////////////////////////////////////////
  
  // ttbar mc bkg histogram ////////////////////////////////////////////////
  if(standaloneTTbar_){
    ttbarmcbkgHist_ = ttbarmcbkgHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );   
    //ttbarmcbkgHist_->SetLineWidth(3);
    //ttbarmcbkgHist_->SetLineColor(kGreen);
    ttbarmcbkgHist_->SetFillColor(kGreen);
    ttbarmcbkgHist_->SetFillStyle(1001);
    ttbarmcbkgHist_->SetMarkerColor(kGreen);
    ttbarmcbkgHist_->SetMarkerStyle(21);
  }
  ///////////////////////////////////////////////////////////////////
  
  // mc bkg histogram ////////////////////////////////////////////////
  mcbkgHist_ = mcbkgHisto_->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() );   
  mcbkgHist_->SetLineWidth(3);
  mcbkgHist_->SetLineColor(kBlue);
  //mcbkgHist_->SetFillColor(kBlack);
  //mcbkgHist_->SetFillStyle(1001);
  mcbkgHist_->SetMarkerColor(kBlue);
  mcbkgHist_->SetMarkerStyle(21);
  ///////////////////////////////////////////////////////////////////
  
  leg_ = new TLegend(0.3,0.665,0.55,0.86,NULL,"brNDC");
  leg_->SetTextFont(62);
  leg_->SetBorderSize(0);
  leg_->SetLineColor(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColor(0);
  leg_->SetFillStyle(1001);
  //if(includeSignal_) leg_->AddEntry(signalHist_,"signal template","f");
  leg_->AddEntry(ddbkgHist_,"dd tau fake bkg","f");
  if(standaloneTTbar_) leg_->AddEntry(ttbarmcbkgHist_,"mc signal","f");
  leg_->AddEntry(mcbkgHist_,"mc other bkg","f");
  
  canvas_->cd(); 




//   // Normal drawing -----------------------------------------
//  // Order chosen to have good Y axis boundaries
//   canvas_->Divide(1,3);
//   if(standaloneTTbar_){
//     if(fitVars_[i]->getHmax()){ 
//       ttbarmcbkgHist_->SetMaximum(fitVars_[i]->getHmax()); ttbarmcbkgHist_->SetMinimum(fitVars_[i]->getHmin());
//     }
//     canvas_->cd(1); ttbarmcbkgHist_->Draw("hist");  //fn ttbarmcbkgHist_->DrawNormalized("hist");
//     canvas_->cd(2); mcbkgHist_->Draw("hist");  //fn mcbkgHist_->DrawNormalized("histsame"); 
//   }
//   else {
//     if(fitVars_[i]->getHmax()){ mcbkgHist_->SetMaximum(fitVars_[i]->getHmax()); mcbkgHist_->SetMinimum(fitVars_[i]->getHmin());
//     } 
//     mcbkgHist_->DrawNormalized("hist");
//   }
  
//  // if(includeSignal_)
//  //   signalHist_->DrawNormalized("histsame");
 
//   //fn  if(standaloneTTbar_) ttbarmcbkgHist_->DrawNormalized("histsame"); // in order for it to be on top and thus viewable for discriminating it from higgs in rc_t plots
//   // End normal drawing --------------------------------------  

//   canvas_->cd(3); ddbkgHist_->Draw("histe"); //fn ddbkgHist_->DrawNormalized("histesame");

  // Normal drawing -----------------------------------------
 // Order chosen to have good Y axis boundaries
  dataHist_->Draw("e");
  if(standaloneTTbar_){
    //    if(fitVars_[i]->getHmax()){ 
    //      ttbarmcbkgHist_->SetMaximum(fitVars_[i]->getHmax()); ttbarmcbkgHist_->SetMinimum(fitVars_[i]->getHmin());
    //   }
    ttbarmcbkgHist_->Scale(1.06*ttbarmcbkgHist_->Integral()/ttbarmcbkgHist_->Integral());
    ttbarmcbkgHist_->Draw("histsame");
    mcbkgHist_->Draw("histsame"); 
  }
  else {
    if(fitVars_[i]->getHmax()){ mcbkgHist_->SetMaximum(fitVars_[i]->getHmax()); mcbkgHist_->SetMinimum(fitVars_[i]->getHmin());
    } 
    mcbkgHist_->Draw("hist");
  }
  
 // if(includeSignal_)
 //   signalHist_->DrawNormalized("histsame");
 
  //fn  if(standaloneTTbar_) ttbarmcbkgHist_->DrawNormalized("histsame"); // in order for it to be on top and thus viewable for discriminating it from higgs in rc_t plots
  // End normal drawing --------------------------------------  

  ddbkgHist_->Scale(osCutEff_*2431.*0.7/ddbkgHist_->Integral());
  ddbkgHist_->Draw("histsame");

//    // Stacked drawing --------------------------------------
//    THStack hs("hs","stacked");

// // //  if(standaloneTTbar_)
// // //    ttbarmcbkgHist_->Scale(1);
// // //  mcbkgHist_->Scale(1);
// // //  if(includeSignal_)
// // //    signalHist_->Scale(1);
// // //  ddbkgHist_->Scale(1);


//    if(standaloneTTbar_) hs.Add(ttbarmcbkgHist_);
//    hs.Add(mcbkgHist_);
// //   if(includeSignal_)
// //     hs.Add(signalHist_);
//    hs.Add(ddbkgHist_);
//    hs.Draw("histsame");

//   // End stacked drawing -----------------------------------------

  leg_->Draw();
  canvas_->SaveAs((outFolder_+string("shapes_")+identifier_+string(".pdf")).c_str());
  canvas_->SaveAs((outFolder_+string("shapes_")+identifier_+string(".png")).c_str());
  canvas_->SaveAs((outFolder_+string("shapes_")+identifier_+string(".C")).c_str());
  canvas_->cd();
  canvas_->Clear();
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void TauDileptonPDFBuilderFitter::BuildConstrainedModels(size_t i){

  // Get total entries from Histograms ////////////////////////////////////////////////
  cout<<endl<<endl<<"******************************************************************************************"<<endl;
  
  double sig_N   ;  double nsig   ;
  double ddbkg_N ;  double nddbkg ;
  double ttbarmcbkg_N ;  double nttbarmcbkg ; 
  double mcbkg_N ;  double nmcbkg ; 
  double data_N  ;  double ndata;
  
  if(includeSignal_){ sig_N   = mySignalDS_->numEntries(); nsig   = signalHisto_->sum(kFALSE); }
  ddbkg_N = myDDBkgDS_->numEntries();  nddbkg = ddbkgHisto_->sum(kFALSE);
  if(standaloneTTbar_){ ttbarmcbkg_N = myTTBARMCBkgDS_->numEntries();  nttbarmcbkg = ttbarmcbkgHisto_->sum(kFALSE); }
  mcbkg_N = myMCBkgDS_->numEntries();  nmcbkg = mcbkgHisto_->sum(kFALSE); 
  data_N  = myDataDS_->numEntries(); ndata = dataHisto_->sum(kFALSE);   
  
  /////////////////////////////////////////////////////////////////////////////////////
  cout<<endl<<"Identifier: "<<identifier_<<", meaning that includeSignal="<<includeSignal_<<", standaloneTTbar="<<standaloneTTbar_<<endl;
  cout<<endl<<" PDF data set Entries for distribution : "<<fitVars_[i]->getVarName()   << endl;
  if(includeSignal_) cout << mySignalDSName_ << " unbinned entries: " << sig_N << ". weighted entries: " << nsig << endl;
  cout << myDDBkgDSName_ << " unbinned entries: " << ddbkg_N << ". weighted entries: " << nddbkg << endl;
  if(standaloneTTbar_) cout<< myTTBARMCBkgDSName_ <<" unbinned entries: "<< ttbarmcbkg_N << " , weighted entries : "<< nttbarmcbkg << endl;
  cout << myMCBkgDSName_ << " unbinned entries: " << mcbkg_N << ". weighted entries: " << nmcbkg << endl;
  cout << myDataDSName_ << " unbinned entries: " << data_N << ". weighted entries: " << ndata << endl;

  cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
  /////////////////////////////////////////////////////////////////////////////////////
  
  
  // Building the constrained models for signal mc bkg ///////////////////////////////////////////////////////////////////////////////////////////////
  double nsignalMean, nsignalSigma, nttbarmcbkgMean, nttbarmcbkgSigma;
  if(includeSignal_) nsignalMean=nsig; nsignalSigma=signalStatError_;
  double nddbkgMean(nddbkg); double nddbkgSigma(ddbkgStatError_);
  if(standaloneTTbar_) nttbarmcbkgMean=nttbarmcbkg; nttbarmcbkgSigma=ttbarmcbkgStatError_;
  double nmcbkgMean(nmcbkg); double nmcbkgSigma(mcbkgStatError_);
  
  if(includeSignal_){
    if( ! sigVar_         ) sigVar_         = new RooRealVar( "globalSignalVarName",       "globalSignalVarName",         nsig,   0, ndata);
    else{ sigVar_->setMin("",0);   sigVar_->setMax("",ndata);     sigVar_->setVal(nsig); }
    
    if( ! sigMeanVar_   ) sigMeanVar_   = new RooRealVar( "globalSignalMeanVarName",    "globalSignalMeanVarName",      nsignalMean); 
    else{ sigMeanVar_->setVal(nsignalMean);   }
    
    if( ! sigSigmaVar_  ) sigSigmaVar_  = new RooRealVar( "globalSignalSigmaVarName",   "globalSignalSigmaVarName",     nsignalSigma ); 
    else{ sigSigmaVar_->setVal(nsignalSigma); }
  }
    
  if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkg, 0, ndata); 
  else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",ndata);     ddbkgVar_->setVal(nddbkg);}
  
  if(! ddbkgMeanVar_)     ddbkgMeanVar_   = new RooRealVar( "globalDDBkgMeanVarName", "globalDDBkgMeanVarName",         nddbkgMean); 
  else{ ddbkgMeanVar_->setVal(nddbkgMean);   }
  
  if(! ddbkgSigmaVar_)    ddbkgSigmaVar_  = new RooRealVar( "globalDDBkgSigmaVarName","globalDDBkgSigmaVarName",        nddbkgSigma);  
  else{ ddbkgSigmaVar_->setVal(nddbkgSigma);   }
  
  if(standaloneTTbar_){
    if( ! ttbarmcbkgVar_       ) ttbarmcbkgVar_       = new RooRealVar( "globalTTbarMcBkgVarName",        "globalTTbarMcBkgVarName",          nttbarmcbkg, 0, ndata); 
    else{ ttbarmcbkgVar_->setMin("",0); ttbarmcbkgVar_->setMax("",ndata); ttbarmcbkgVar_->setVal(nttbarmcbkg); }
    
    if( ! ttbarmcbkgMeanVar_   ) ttbarmcbkgMeanVar_   = new RooRealVar( "globalTTbarMcBkgMeanVarName",    "globalTTbarMcBkgMeanVarName",      nttbarmcbkgMean); 
    else{ ttbarmcbkgMeanVar_->setVal(nttbarmcbkgMean);   }
    
    if( ! ttbarmcbkgSigmaVar_  ) ttbarmcbkgSigmaVar_  = new RooRealVar( "globalTTbarMcBkgSigmaVarName",   "globalTTbarMcBkgSigmaVarName",     nttbarmcbkgSigma ); 
    else{ ttbarmcbkgSigmaVar_->setVal(nttbarmcbkgSigma); }
  }
  
  if( ! mcbkgVar_       ) mcbkgVar_       = new RooRealVar( "globalMcBkgVarName",        "globalMcBkgVarName",          nmcbkg, 0, ndata); 
  else{ mcbkgVar_->setMin("",0); mcbkgVar_->setMax("",ndata); mcbkgVar_->setVal(nmcbkg); }
  
  if( ! mcbkgMeanVar_   ) mcbkgMeanVar_   = new RooRealVar( "globalMcBkgMeanVarName",    "globalMcBkgMeanVarName",      nmcbkgMean); 
  else{ mcbkgMeanVar_->setVal(nmcbkgMean);   }
  
  if( ! mcbkgSigmaVar_  ) mcbkgSigmaVar_  = new RooRealVar( "globalMcBkgSigmaVarName",   "globalMcBkgSigmaVarName",     nmcbkgSigma ); 
  else{ mcbkgSigmaVar_->setVal(nmcbkgSigma); }
  



  resultsFile_ << endl
	       << "FIT REPORT FOR " << fitVars_[i]->getVarName() << endl
	       << "---------------------------------------------------" << endl
	       << "[Observed] " << ndata << endl 
	       << "---------------------------------------------------" << endl
	       << "[H+ expected]" << nsig << endl
	       << "[DD expected] " <<  nddbkg << endl 
	       << "[ttbar expected]:" << nttbarmcbkg << " +/- " << nttbarmcbkgSigma << endl
	       << "[other mc expected]:" << nmcbkg << " +/- " << nmcbkgSigma << endl
	       << "---------------------------------------------------" << endl;


/// test ///  if(includeSignal_){
/// test ///    if( ! sigVar_         ) sigVar_         = new RooRealVar( "globalSignalVarName",       "globalSignalVarName",         nsig,   0, nsig*2.5);
/// test ///    else{ sigVar_->setMin("",0);   sigVar_->setMax("",nsig*3.5);     sigVar_->setVal(nsig); }
/// test ///    
/// test ///    if( ! sigMeanVar_   ) sigMeanVar_   = new RooRealVar( "globalSignalMeanVarName",    "globalSignalMeanVarName",      nsignalMean); 
/// test ///    else{ sigMeanVar_->setVal(nsignalMean);   }
/// test ///    
/// test ///    if( ! sigSigmaVar_  ) sigSigmaVar_  = new RooRealVar( "globalSignalSigmaVarName",   "globalSignalSigmaVarName",     nsignalSigma ); 
/// test ///    else{ sigSigmaVar_->setVal(nsignalSigma); }
/// test ///  }
/// test ///  
/// test ///  // FIXME?    
/// test ///  //    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkgMean, 0, nddbkgMean*1.5); 
/// test ///  //    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkgMean*3.5);     ddbkgVar_->setVal(nddbkgMean);}
/// test ///  
/// test ///  if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkg, 0, nddbkg*1.5); 
/// test ///  else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkg*1.5);     ddbkgVar_->setVal(nddbkg);}
/// test ///  
/// test ///  if(! ddbkgMeanVar_)     ddbkgMeanVar_   = new RooRealVar( "globalDDBkgMeanVarName", "globalDDBkgMeanVarName",         nddbkgMean); 
/// test ///  else{ ddbkgMeanVar_->setVal(nddbkgMean);   }
/// test ///  
/// test ///  if(! ddbkgSigmaVar_)    ddbkgSigmaVar_  = new RooRealVar( "globalDDBkgSigmaVarName","globalDDBkgSigmaVarName",        nddbkgSigma);  
/// test ///  else{ ddbkgSigmaVar_->setVal(nddbkgSigma);   }
/// test ///  
/// test ///  if(standaloneTTbar_){
/// test ///    if( ! ttbarmcbkgVar_       ) ttbarmcbkgVar_       = new RooRealVar( "globalTTbarMcBkgVarName",        "globalTTbarMcBkgVarName",          nttbarmcbkg, 0, nttbarmcbkg*1.5); 
/// test ///    else{ ttbarmcbkgVar_->setMin("",0); ttbarmcbkgVar_->setMax("",nttbarmcbkg*3.5); ttbarmcbkgVar_->setVal(nttbarmcbkg); }
/// test ///    
/// test ///    if( ! ttbarmcbkgMeanVar_   ) ttbarmcbkgMeanVar_   = new RooRealVar( "globalTTbarMcBkgMeanVarName",    "globalTTbarMcBkgMeanVarName",      nttbarmcbkgMean); 
/// test ///    else{ ttbarmcbkgMeanVar_->setVal(nttbarmcbkgMean);   }
/// test ///    
/// test ///    if( ! ttbarmcbkgSigmaVar_  ) ttbarmcbkgSigmaVar_  = new RooRealVar( "globalTTbarMcBkgSigmaVarName",   "globalTTbarMcBkgSigmaVarName",     nttbarmcbkgSigma ); 
/// test ///    else{ ttbarmcbkgSigmaVar_->setVal(nttbarmcbkgSigma); }
/// test ///  }
/// test ///  
/// test ///  if( ! mcbkgVar_       ) mcbkgVar_       = new RooRealVar( "globalMcBkgVarName",        "globalMcBkgVarName",          nmcbkg, 0, nmcbkg*1.5); 
/// test ///  else{ mcbkgVar_->setMin("",0); mcbkgVar_->setMax("",nmcbkg*3.5); mcbkgVar_->setVal(nmcbkg); }
/// test ///  
/// test ///  if( ! mcbkgMeanVar_   ) mcbkgMeanVar_   = new RooRealVar( "globalMcBkgMeanVarName",    "globalMcBkgMeanVarName",      nmcbkgMean); 
/// test ///  else{ mcbkgMeanVar_->setVal(nmcbkgMean);   }
/// test ///  
/// test ///  if( ! mcbkgSigmaVar_  ) mcbkgSigmaVar_  = new RooRealVar( "globalMcBkgSigmaVarName",   "globalMcBkgSigmaVarName",     nmcbkgSigma ); 
/// test ///  else{ mcbkgSigmaVar_->setVal(nmcbkgSigma); }
  
  
//  if(includeSignal_) signalConstraint_ = new RooGaussian( signalConstraintName_.c_str(), signalConstraintName_.c_str(), *sigVar_,*sigMeanVar_, *sigSigmaVar_);
//  ddbkgConstraint_ = new RooGaussian( ddbkgConstraintName_.c_str(), ddbkgConstraintName_.c_str(), *ddbkgVar_, *ddbkgMeanVar_, *ddbkgSigmaVar_);   
//  if(standaloneTTbar_) ttbarmcbkgConstraint_ = new RooGaussian( ttbarmcbkgConstraintName_.c_str(), ttbarmcbkgConstraintName_.c_str(), *ttbarmcbkgVar_,*ttbarmcbkgMeanVar_, *ttbarmcbkgSigmaVar_);
//  mcbkgConstraint_ = new RooGaussian( mcbkgConstraintName_.c_str(), mcbkgConstraintName_.c_str(), *mcbkgVar_,*mcbkgMeanVar_, *mcbkgSigmaVar_);

//  if(includeSignal_) signalConstraint_ = new RooGaussian( signalConstraintName_.c_str(), signalConstraintName_.c_str(), *sigVar_,RooConst(nsignalMean), RooConst(nsignalSigma));
  if(includeSignal_) signalConstraint_ = new RooGaussian( signalConstraintName_.c_str(), signalConstraintName_.c_str(), *sigVar_,RooConst(0), RooConst(nsignalMean));
  ddbkgConstraint_ = new RooGaussian( ddbkgConstraintName_.c_str(), ddbkgConstraintName_.c_str(), *ddbkgVar_, RooConst(nddbkgMean), RooConst(nddbkgSigma));   
  if(standaloneTTbar_) ttbarmcbkgConstraint_ = new RooGaussian( ttbarmcbkgConstraintName_.c_str(), ttbarmcbkgConstraintName_.c_str(), *ttbarmcbkgVar_,RooConst(nttbarmcbkgMean), RooConst(nttbarmcbkgSigma));
  mcbkgConstraint_ = new RooGaussian( mcbkgConstraintName_.c_str(), mcbkgConstraintName_.c_str(), *mcbkgVar_,RooConst(nmcbkgMean), RooConst(nmcbkgSigma));
    
  
  // build the sum model and model with constrains ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  string sumModelName = fitVars_[i]->getVarName()+string("_sumModel");
  
  string sumModelExp = "";
  if(includeSignal_) sumModelExp.append(signalModelName_+string("+"));
  sumModelExp.append(ddbkgModelName_+string("+"));
  if(standaloneTTbar_) sumModelExp.append(ttbarmcbkgModelName_+string("+"));
  sumModelExp.append(mcbkgModelName_);
  
  string sumModelConstrainedName = fitVars_[i]->getVarName()+string("_sumConstrainedModel");
  
  string sumModelConstrainedExp = sumModelName+string("*");
  if(includeSignal_) sumModelConstrainedExp.append(signalConstraintName_+string("*"));
  sumModelConstrainedExp.append(ddbkgConstraintName_+string("*"));
  if(standaloneTTbar_) sumModelConstrainedExp.append(ttbarmcbkgConstraintName_+string("*"));
  sumModelConstrainedExp.append(mcbkgConstraintName_);
  
  switch(fitVars_[i]->getUnbinned()){
  case 1 : // Unbinned
    if(includeSignal_){
      if(standaloneTTbar_){
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel_, *u_ddbkgModel_, *u_ttbarmcbkgModel_, *u_mcbkgModel_), RooArgList( *sigVar_, *ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
	//	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ddbkgConstraint_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
      }
      else{
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel_, *u_ddbkgModel_, *u_mcbkgModel_), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *mcbkgConstraint_));
	//	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *ddbkgConstraint_, *mcbkgConstraint_));
      }
    } else{
      if(standaloneTTbar_){
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel_, *u_ttbarmcbkgModel_, *u_mcbkgModel_ ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	//model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ddbkgConstraint_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
      }
      else{
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel_, *u_mcbkgModel_), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	//model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ddbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *mcbkgConstraint_));
      }
    }
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    if(includeSignal_){
      if(standaloneTTbar_){
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel_, *b_ddbkgModel_, *b_ttbarmcbkgModel_, *b_mcbkgModel_), RooArgList( *sigVar_, *ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	//model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *ddbkgConstraint_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
      }
      else{
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel_, *b_ddbkgModel_, *b_mcbkgModel_ ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	//model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *ddbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *signalConstraint_, *mcbkgConstraint_));
      }
    } else{
      if(standaloneTTbar_){
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel_, *b_ttbarmcbkgModel_, *b_mcbkgModel_ ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	//	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ddbkgConstraint_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ttbarmcbkgConstraint_, *mcbkgConstraint_));
      }
      else{
	sumModel_ = new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel_, *b_mcbkgModel_), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	//model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *ddbkgConstraint_, *mcbkgConstraint_));
	model_ = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel_, *mcbkgConstraint_));
      }
    }
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
  }
}

void TauDileptonPDFBuilderFitter::DoPerVariableFit(size_t i){
  switch(fitVars_[i]->getUnbinned()){
  case 1 : // Unbinned
    if(includeSignal_){
      if(standaloneTTbar_){
	constrainedModelFit_ = model_->fitTo( *myDataDS_, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
      else{
	constrainedModelFit_ = model_->fitTo( *myDataDS_, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
    } else{
      if(standaloneTTbar_){
	constrainedModelFit_ = model_->fitTo( *myDataDS_, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
      else{
	constrainedModelFit_ = model_->fitTo( *myDataDS_, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
    }
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    if(includeSignal_){
      if(standaloneTTbar_){
	constrainedModelFit_ = model_->fitTo( *dataHisto_, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
      else{
	constrainedModelFit_ = model_->fitTo( *dataHisto_, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
    } else{
      if(standaloneTTbar_){
	constrainedModelFit_ = model_->fitTo( *dataHisto_, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
      else{
	constrainedModelFit_ = model_->fitTo( *dataHisto_, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      }
    }
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
  }
  
  if(includeSignal_) resultsFile_ << "[H+] "      << sigVar_->getVal() << " +/- " << sigVar_->getError()  << endl;
  resultsFile_       << "[DD] "      << ddbkgVar_->getVal() << " +/- " << ddbkgVar_->getError()  << endl;
  if(standaloneTTbar_) resultsFile_       << "[ttbar] "      << ttbarmcbkgVar_->getVal() << " +/- " << ttbarmcbkgVar_->getError()  << endl;
  resultsFile_       << "[other mc] "      << mcbkgVar_->getVal() << " +/- " << mcbkgVar_->getError()  << endl;
  resultsFile_       << "---------------------------------------------------" << endl;;
}

void TauDileptonPDFBuilderFitter::DrawPerVariableFit(size_t i){
  
  SetTDRStyle();

  canvas_->cd();  
  canvas_->Clear();  
  myFrame_ = myvar_->frame();
  myFrame_->SetTitle("");
  myFrame_->GetXaxis()->SetTitle(fitVars_[i]->getVarName().c_str());
  myFrame_->GetYaxis()->SetTitle("Events");
  myFrame_->GetYaxis()->SetTitleOffset(0.7);
  dataHisto_->plotOn(myFrame_,Name("data"));
  model_->plotOn(myFrame_,Name("total"));
  //    model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*signalModel), RooFit::LineColor(kGreen));   

  if(includeSignal_){
    if(standaloneTTbar_){
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(b_signalModel_->GetName()), RooFit::LineColor(kGreen+2), RooFit::FillColor(kGreen+2), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("signal"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components((TString(b_signalModel_->GetName())+TString(",")+TString(b_ttbarmcbkgModel_->GetName()))), RooFit::LineColor(kAzure+10), RooFit::FillColor(kAzure+10), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ttbarbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_signalModel_->GetName())+TString(",")+TString(b_ttbarmcbkgModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())), RooFit::LineColor(kRed), RooFit::FillColor(kRed), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ddbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_signalModel_->GetName())+TString(",")+TString(b_ttbarmcbkgModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())+TString(",")+TString(b_mcbkgModel_->GetName())), RooFit::LineColor(49), RooFit::FillColor(49), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("mcbkg"));   
    } else{
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(b_signalModel_->GetName()), RooFit::LineColor(kGreen+2), RooFit::FillColor(kGreen+2), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("signal"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_signalModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())), RooFit::LineColor(kRed), RooFit::FillColor(kRed), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ddbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_signalModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())+TString(",")+TString(b_mcbkgModel_->GetName())), RooFit::LineColor(49), RooFit::FillColor(49), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("mcbkg"));   
    }
  }
  else{
    if(standaloneTTbar_){
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(b_ttbarmcbkgModel_->GetName()), RooFit::LineColor(kAzure+10), RooFit::FillColor(kAzure+10), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ttbarbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_ttbarmcbkgModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())), RooFit::LineColor(kRed), RooFit::FillColor(kRed), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ddbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_ttbarmcbkgModel_->GetName())+TString(",")+TString(b_ddbkgModel_->GetName())+TString(",")+TString(b_mcbkgModel_->GetName())), RooFit::LineColor(49), RooFit::FillColor(49), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("mcbkg"));   
    } else{
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(b_ddbkgModel_->GetName()), RooFit::LineColor(kRed), RooFit::FillColor(kRed), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("ddbkg"));   
      model_->plotOn(myFrame_, RooFit::LineStyle(kDashed), RooFit::Components(TString(b_ddbkgModel_->GetName())+TString(",")+TString(b_mcbkgModel_->GetName())), RooFit::LineColor(49), RooFit::FillColor(49), RooFit::FillStyle(1020), RooFit::DrawOption("l"), RooFit::NormRange("MB"), Name("mcbkg"));   
    } 
    }


  myFrame_->Draw();

  //  TLegend *leg=new TLegend(0.3,0.665,0.85,0.86,NULL,"brNDC");//new TLegend(0.2,0.82,0.55,0.85);
  //  TLegend *leg = new TLegend(0.7147651,0.6346154,0.9446309,0.9353147,NULL,"brNDC");
  TLegend *leg = new TLegend(0.75,0.6346154,1.,1.,NULL,"brNDC");

  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetTextFont(42);
  //  leg->SetNColumns(3);
  leg->AddEntry("data","Data","p");
  leg->AddEntry("total","Total","l");
  if(includeSignal_)
    leg->AddEntry("signal","Signal","l");
  leg->AddEntry("ddbkg","DD bkg","l");
  if(standaloneTTbar_)
    leg->AddEntry("ttbarbkg","TTbar MC","l");
  leg->AddEntry("mcbkg","Other MC bkg","l");

  
  leg->Draw("same");

  canvas_->SaveAs((outFolder_+string("modelFit_")+identifier_+string(".pdf")).c_str());
  canvas_->SaveAs((outFolder_+string("modelFit_")+identifier_+string(".png")).c_str());
  canvas_->SaveAs((outFolder_+string("modelFit_")+identifier_+string(".C")).c_str());
  canvas_->cd();
  canvas_->Clear();
}

void TauDileptonPDFBuilderFitter::DoPerVariableLikelihoodFit(size_t i){
  // Test on the fit ////////////////////////////////////////////////////////////
  //    RooNLLVar * nll = (RooNLLVar *) model->createNLL(
  //						     *dataHisto_,
  //						     //*myDataDS_,
  //      RooFit::CloneData(kTRUE),
  //      Extended(kTRUE),
  //      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
  //      Range(min,max)
  //    );
  
  switch(fitVars_[i]->getUnbinned()){
  case 1: // Unbinned
    if(includeSignal_){
      if(standaloneTTbar_) nll_ = (RooNLLVar*) model_->createNLL( *myDataDS_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      else nll_ = (RooNLLVar*) model_->createNLL( *myDataDS_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
    } else{
      if(standaloneTTbar_) nll_ = (RooNLLVar *) model_->createNLL( *myDataDS_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      else nll_ = (RooNLLVar*) model_->createNLL( *myDataDS_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
    }
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    if(includeSignal_){
      if(standaloneTTbar_) nll_ = (RooNLLVar*) model_->createNLL( *dataHisto_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      else nll_ = (RooNLLVar*) model_->createNLL( *dataHisto_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
    } else{
      if(standaloneTTbar_) nll_ = (RooNLLVar*) model_->createNLL( *dataHisto_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
      else nll_ = (RooNLLVar*) model_->createNLL( *dataHisto_, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i]->getMin(),fitVars_[i]->getMax()));
    }
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
  }
  
  likelihoodVector_.push_back(nll_); //.push_back(nll); 
  
  RooMinuit minuit(*nll_);
  minuit.migrad();
  minuit.setErrorLevel(0.5);
  minuit.hesse();
  
  myNllFitResult_ = minuit.save();
  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;
  cout<<      "************* IDENTIFIER: "<< identifier_ << "****************************"<<endl;
  cout<<endl<<"FIT RESULTS for variable"<<fitVars_[i]->getVarName()<<endl<<endl;
  myNllFitResult_->Print("v");
  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;

  SetTDRStyle();
  
  canvas_->cd();
  canvas_->Clear();

  if(includeSignal_){
    contourPlot_ = minuit.contour( *ddbkgVar_, *sigVar_,1,2,3);
    contourPlot_->GetYaxis()->SetTitle("N(H+), m_{H} = 120 GeV/c2");
    contourPlot_->GetYaxis()->SetRangeUser(0,200);
  }
  else 
    if(standaloneTTbar_){
      contourPlot_ = minuit.contour( *ddbkgVar_, *ttbarmcbkgVar_,1,2,3);
      contourPlot_->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
      //      contourPlot_->GetYaxis()->SetRangeUser(0,400);
      contourPlot_->GetYaxis()->SetRangeUser(250,400);
    }
    else{
      contourPlot_ = minuit.contour( *ddbkgVar_, *mcbkgVar_,1,2,3);
      contourPlot_->GetYaxis()->SetTitle("N^{MCdriven}_{Bkg}");
      contourPlot_->GetYaxis()->SetRangeUser(0,200);
    }
  contourPlot_->SetTitle("");
  contourPlot_->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
  //  contourPlot_->GetXaxis()->SetRangeUser(0,400);
  contourPlot_->GetXaxis()->SetRangeUser(100,250);
  contourPlot_->GetYaxis()->SetTitleOffset(0.9);
  contourPlot_->Draw();
  canvas_->SaveAs((outFolder_+string("contour_")+identifier_+string(".pdf")).c_str());
  canvas_->SaveAs((outFolder_+string("contour_")+identifier_+string(".png")).c_str());
  canvas_->SaveAs((outFolder_+string("contour_")+identifier_+string(".C")).c_str());
  canvas_->cd();
  canvas_->Clear();
  ///////////////////////////////////////////////////////////
}

void TauDileptonPDFBuilderFitter::DoCombinedLikelihoodFit(){
  //  RooAddition* combll;
  //  RooFitResult* myRes;
  //  RooPlot* contourPlot;
  //  for(int dim=0; dim<NVARS; dim++){
  RooArgSet llSet;
  cout<<"FINAL FIT NUMBER " << nVars_ << ", likelihoodVector with dim: " << likelihoodVector_.size() << endl;    
  // fill arg set with a
  for(size_t i=0; i<nVars_;i++){ llSet.add( *(likelihoodVector_[i]) ); }
  //  for(int i=0; i<likelihoodVector.size();i++){ llSet.add( *(likelihoodVector[i]) ); }
  
  //create a rooAddition
  combll_ = new RooAddition("combll","combll",llSet);
  
  //minimize
  RooMinuit minuit(*combll_);
  minuit.setVerbose(false);
  minuit.setErrorLevel(0.5); //otherwise RooFit seems to assume the chi^2 default
  minuit.hesse();
  minuit.migrad();
  minuit.setErrorLevel(0.5);
  minuit.hesse();
  minuit.minos();
  minuit.save();
  myNllFitResult_ = minuit.save();
  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;
  cout<<      "********************** IDENTIFIER: " << identifier_ << "**************************" << endl;
  cout<<endl<<"FINAL FIT RESULTS"<<endl<<endl;
  myNllFitResult_->Print("v");//myRes->printToStream(resultsFile);
  resultsFile_<<endl<<"*******************************************************************************"<<endl;
  resultsFile_<<      "*******************************************************************************"<<endl;
  resultsFile_<<      "********************** IDENTIFIER: " << identifier_ << "**************************" << endl;
  resultsFile_<<endl<<"FINAL FIT RESULTS"<<endl<<endl;

  if(includeSignal_) resultsFile_ << "[H+] "      << sigVar_->getVal() << " +/- " << sigVar_->getError()  << endl;
  resultsFile_       << "[DD] "      << ddbkgVar_->getVal() << " +/- " << ddbkgVar_->getError()  << endl;
  if(standaloneTTbar_) resultsFile_       << "[ttbar] "      << ttbarmcbkgVar_->getVal() << " +/- " << ttbarmcbkgVar_->getError()  << endl;
  resultsFile_       << "[other mc] "      << mcbkgVar_->getVal() << " +/- " << mcbkgVar_->getError()  << endl;
  resultsFile_       << "---------------------------------------------------" << endl;;

  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;
  
  canvas_->Clear();
  
  if(includeSignal_){
    contourPlot_ = minuit.contour( *ddbkgVar_ , *sigVar_,1,2,3);
    contourPlot_->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
    contourPlot_->GetYaxis()->SetRangeUser(0,200);
  }
  else{
    if(standaloneTTbar_){
      cout << "Here I am" << endl;
      contourPlot_ = minuit.contour( *ddbkgVar_ , *ttbarmcbkgVar_,1,2,3);
      cout << "Here I am. contour generation set to some other stuff" << endl;
      myNllFitResult_->Print("v");
      contourPlot_->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
      cout << "Here I am not. contour access crashed because of lack of component" << endl;
      //      contourPlot_->GetYaxis()->SetRangeUser(0,400);
      contourPlot_->GetYaxis()->SetRangeUser(250,400);
    }
    else{
      contourPlot_ = minuit.contour( *ddbkgVar_ , *mcbkgVar_,1,2,3);
      contourPlot_->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
      contourPlot_->GetYaxis()->SetRangeUser(0,200);
    }
  }
  contourPlot_->SetTitle("");
  contourPlot_->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
  //  contourPlot_->GetXaxis()->SetRangeUser(0,400);
  contourPlot_->GetXaxis()->SetRangeUser(100,250);
  contourPlot_->Draw();
  
  
  std::ostringstream oss;
  oss << nVars_;//dim;
  
  canvas_->SaveAs((outFolder_+string("contour_final_")+baseIdentifier_+oss.str()+string("vars.pdf")).c_str());
  canvas_->SaveAs((outFolder_+string("contour_final_")+baseIdentifier_+oss.str()+string("vars.png")).c_str());
  canvas_->SaveAs((outFolder_+string("contour_final_")+baseIdentifier_+oss.str()+string("vars.C")).c_str());
  ///////////////////////////////////////////////////////////

  minuit.cleanup();
}

//fn void TauDileptonPDFBuilderFitter::DrawSystematics(size_t i){

  //fn  string ResultsFileName = resultsFileName_ + string("_") + fitVars_[i]->getVarName();
  //fn TFile* outputFile; 
  //fn outputFile = new TFile((outFolder_+ResultsFileName+string(".root")).c_str(), "RECREATE");

  //fn SetTDRStyle();

  //fn // Reset canvas
  //fn canvas_->cd();
  //fn canvas_->Clear();

//???????????????????????????????  
//   /// Produce plots - syst case
//   for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    
//     // mc bkg histogram ////////////////////////////////////////////////
//     vector<TH1*> temp;
//     temp.clear();
//     for(size_t f=0; f<nSamples_; f++){
//       temp.push_back( systHisto_[a][f]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
//     }
//     systHist_.push_back(temp);
    
//     for(size_t f=0; f<nSamples_; f++){
//       systHist_[a][f]->Sumw2();
//       systHist_[a][f]->SetFillColor(sampleColour_[f]);
//       systHist_[a][f]->SetFillStyle(sampleFillStyle_[f]);
//       if(isDDbkg_[f]){    
// 	if(a == 0 || a == 2 || a == 4)
// 	  systHist_[a][f]->Scale((222+11.4/5)/systHist_[a][f]->Integral());
// 	else
// 	  systHist_[a][f]->Scale((222-11.4/5)/systHist_[a][f]->Integral());
//       }
//     }
//     //    ///////////////////////////////////////////////////////////////////
    
//     systHist_[a].push_back( (TH1*)systHist_[a][nSamples_-1]->Clone(  sampleName_[nSamples_].c_str()) );
//     systHist_[a].push_back( (TH1*)systHist_[a][nSamples_-1]->Clone(  sampleName_[nSamples_+1].c_str()) );
//     systHist_[a][nSamples_]  ->Sumw2();
//     systHist_[a][nSamples_+1]->Sumw2();
    
//     systHist_[a][nSamples_]  ->Scale( (50.5/systHist_[a][nSamples_]->Integral()  )*( systHist_[a][nSamples_-1]->Integral() / hist_[nSamples_-1]->Integral()  )    );
//     systHist_[a][nSamples_+1]->Scale( (0.4/systHist_[a][nSamples_+1]->Integral() )*( systHist_[a][nSamples_-1]->Integral() / hist_[nSamples_-1]->Integral()  )  );
    
//     for(size_t f=0; f<nSamples_+2; f++){
//       if(f>nSamples_-1){ // Repeat colours for newly cloned histos
// 	systHist_[a][f]->Sumw2();
// 	if(isSignal_[f]){
// 	  systHist_[a][f]->SetOption("0000"); // What did that do, again? LoL
// 	  systHist_[a][f]->SetLineColor(sampleColour_[f]);
// 	}
// 	else
// 	  systHist_[a][f]->SetFillColor(sampleColour_[f]);
// 	systHist_[a][f]->SetLineWidth(3);
// 	systHist_[a][f]->SetFillStyle(sampleFillStyle_[f]); //1001 for background and data, 0 for signal // 3017);
//       }
//       cout << "sample " << sampleName_[f] << "" << systComponents_[a] << ", integral " << systHist_[a][f]->Integral() << endl;
//     }
//   } // End loop on systs
  
//   /// End produce plots - syst case
  

//     // 0: data
//     // 1: WH
//     // 2: HH
//     // 3: DD
//     // 4: other MCs
//   TGraphAsymmErrors ddbkgBands;
//   getErrorBands(*(hist_[3]), *ddbkgHistUp_, *ddbkgHistDown_, ddbkgBands);
  
//   ddbkgBands.SetFillColor(1);
//   ddbkgBands.SetFillStyle(3004);
//   ddbkgBands.GetYaxis()->SetTitle("a.u.");
//   ddbkgBands.GetYaxis()->SetTitleOffset(0.85);
//   ddbkgBands.GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   ddbkgBands.GetXaxis()->SetTitleOffset(0.85);
//   ddbkgBands.GetXaxis()->SetTitleSize(5);
//   ddbkgBands.GetXaxis()->SetRange(displayMin_,displayMax_);    
//   ddbkgBands.GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
  
//   TLegend* leg2_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg2_->SetTextFont(62);
//   leg2_->SetBorderSize(0);
//   leg2_->SetLineColor(1);
//   leg2_->SetLineStyle(1);
//   leg2_->SetLineWidth(1);
//   leg2_->SetFillColor(0);
//   leg2_->SetFillStyle(0);
//   leg2_->AddEntry(hist_[1],"Base","l");
//   leg2_->AddEntry(systHist_[0][1],"JESup","l");
//   leg2_->AddEntry(systHist_[1][1],"JESdown","l");
//   leg2_->AddEntry(&ddbkgBands, "stat+method","f");
  
//   leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg_->SetTextFont(62);
//   leg_->SetBorderSize(0);
//   leg_->SetLineColor(1);
//   leg_->SetLineStyle(1);
//   leg_->SetLineWidth(1);
//   leg_->SetFillColor(0);
//   leg_->SetFillStyle(0);
//   leg_->AddEntry(hist_[1],"Base","l");
//   leg_->AddEntry(systHist_[0][1],"JESup","l");
//   leg_->AddEntry(systHist_[1][1],"JESdown","l");
  
//   hist_[1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[0][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[0][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[1][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[1][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[0][1]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[1]->SetLineColor(1);
//   systHist_[0][1]->SetLineColor(2);
//   systHist_[1][1]->SetLineColor(3);
//   systHist_[0][1]->Draw("hist");
//   systHist_[1][1]->Draw("histsame");
//   hist_[1]->Draw("histsame");
//   leg_->Draw();
//   canvas_->cd(); 
//   // Order chosen to have good Y axis boundaries
  
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.png")).c_str());
//   canvas_->Clear();
//   hist_[1]->SetLineColor(sampleColour_[1]); // Reset line color from black to final one
  
//   hist_[2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[0][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[0][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[1][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[1][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[0][2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[2]->SetLineColor(1);
//   systHist_[0][2]->SetLineColor(2);
//   systHist_[1][2]->SetLineColor(3);
//   systHist_[0][2]->Draw("hist");
//   systHist_[1][2]->Draw("histsame");
//   hist_[2]->Draw("histsame");
//   leg_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.png")).c_str());
//   canvas_->Clear();
//   hist_[2]->SetLineColor(sampleColour_[2]); // Reset line color from black to final one
  
//   systHist_[0][3]->SetFillStyle(0);
//   systHist_[1][3]->SetFillStyle(0);
//   hist_[3]->SetFillStyle(0);
  
//   hist_[3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[0][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[0][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[1][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[1][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[0][3]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[3]->SetMarkerColor(1);
//   systHist_[0][3]->SetLineColor(2);
//   systHist_[1][3]->SetLineColor(3);
//   systHist_[0][3]->Draw("hist");
//   systHist_[1][3]->Draw("histsame");
//   hist_[3]->Draw("same");
//   ddbkgBands.Draw("2same");
//   leg2_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.png")).c_str());
//   canvas_->Clear();
//   hist_[3]->SetLineColor(sampleColour_[3]); // Reset line color from black to final one
  
//   systHist_[0][3]->SetFillStyle(1001);
//   systHist_[1][3]->SetFillStyle(1001);
//   hist_[3]->SetFillStyle(1001);
  
//   for(size_t f=4; f<nSamples_+2; f++){
    
//     systHist_[0][f]->SetFillStyle(0);
//     systHist_[1][f]->SetFillStyle(0);
//     hist_[f]->SetFillStyle(0);
    
//     hist_[f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     hist_[f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[0][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[0][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[1][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[1][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
    
//     systHist_[0][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//     hist_[f]->SetLineColor(1);
//     systHist_[0][f]->SetLineColor(2);
//     systHist_[1][f]->SetLineColor(3);
//     systHist_[0][f]->Draw("hist");
//     systHist_[1][f]->Draw("histsame");
//     hist_[f]->Draw("histsame");
//     leg_->Draw();
//     canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+sampleName_[f].c_str()+string(".pdf")).c_str());
//     canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+sampleName_[f].c_str()+string(".png")).c_str());
//     canvas_->Clear();
    
//     hist_[f]->SetLineColor(sampleColour_[f]);
//     systHist_[0][f]->SetFillStyle(1001);
//     systHist_[1][f]->SetFillStyle(1001);
//     hist_[f]->SetFillStyle(1001);
    
//   }

    
//     ////////////////////////////
//   leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg2_->SetTextFont(62);
//   leg2_->SetBorderSize(0);
//   leg2_->SetLineColor(1);
//   leg2_->SetLineStyle(1);
//   leg2_->SetLineWidth(1);
//   leg2_->SetFillColor(0);
//   leg2_->SetFillStyle(0);
//   leg2_->AddEntry(hist_[1],"Base","l");
//   leg2_->AddEntry(systHist_[0][1],"METup","l");
//   leg2_->AddEntry(systHist_[1][1],"METdown","l");
//   leg2_->AddEntry(&ddbkgBands, "stat+method","f"); // Dummy style show


//   leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg_->SetTextFont(62);
//   leg_->SetBorderSize(0);
//   leg_->SetLineColor(1);
//   leg_->SetLineStyle(1);
//   leg_->SetLineWidth(1);
//   leg_->SetFillColor(0);
//   leg_->SetFillStyle(0);
//   leg_->AddEntry(hist_[1],"Base","l");
//   leg_->AddEntry(systHist_[2][1],"METup","l");
//   leg_->AddEntry(systHist_[3][1],"METdown","l");
  
//   hist_[1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[2][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[2][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[3][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[3][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    

// //    for(int mm=0; mm<signalHistWH_->GetNbinsX(); mm++)
// //      cout << "bin: " << mm << ", diff up: " << signalHistWH_->GetBinContent(mm) - signalSystHistWH_[2]->GetBinContent(mm) << ", diff down: " << signalHistWH_->GetBinContent(mm) - signalSystHistWH_[3]->GetBinContent(mm) << endl;
// //
//   systHist_[2][1]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[1]->SetLineColor(1);
//   systHist_[2][1]->SetLineColor(2);
//   systHist_[3][1]->SetLineColor(3);
//   systHist_[2][1]->Draw("hist");
//   systHist_[3][1]->Draw("histsame");
//   hist_[1]->Draw("histsame");
//   leg_->Draw();
//   canvas_->cd(); 
//     // Order chosen to have good Y axis boundaries

//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.png")).c_str());
//   canvas_->Clear();
//   hist_[1]->SetLineColor(sampleColour_[1]);
  
//   hist_[2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[2][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[2][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[3][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[3][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[2][2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[2]->SetLineColor(1);
//   systHist_[2][2]->SetLineColor(2);
//   systHist_[3][2]->SetLineColor(3);
//   systHist_[2][2]->Draw("hist");
//   systHist_[3][2]->Draw("histsame");
//   hist_[2]->Draw("histsame");
//   leg_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.png")).c_str());
//   canvas_->Clear();
//   hist_[2]->SetLineColor(sampleColour_[2]);
  
//   systHist_[2][3]->SetFillStyle(0);
//   systHist_[3][3]->SetFillStyle(0);
//   hist_[3]->SetFillStyle(0);
  
//   hist_[3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[2][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[2][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[3][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[3][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[2][3]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[3]->SetMarkerColor(1);
//   systHist_[2][3]->SetLineColor(2);
//   systHist_[3][3]->SetLineColor(3);
//   systHist_[2][3]->Draw("hist");
//   systHist_[3][3]->Draw("histsame");
//   hist_[3]->Draw("same");
//   ddbkgBands.Draw("2same");
//   leg2_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.png")).c_str());
//   canvas_->Clear();
//   hist_[3]->SetLineColor(sampleColour_[3]);
  
//   systHist_[2][3]->SetFillStyle(1001);
//   systHist_[3][3]->SetFillStyle(1001);
//   hist_[3]->SetFillStyle(1001);
  
  
//   for(size_t f=4; f<nSamples_+2; f++){
      
//     systHist_[2][f]->SetFillStyle(0);
//     systHist_[3][f]->SetFillStyle(0);
//     hist_[f]->SetFillStyle(0);
    
//     hist_[f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     hist_[f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[2][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[2][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[3][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[3][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
    
//     systHist_[2][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//     hist_[f]->SetLineColor(1);
//     systHist_[2][f]->SetLineColor(2);
//     systHist_[3][f]->SetLineColor(3);
//     systHist_[2][f]->Draw("hist");
//     systHist_[3][f]->Draw("histsame");
//     hist_[f]->Draw("histsame");
//     leg_->Draw();
//     canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+sampleName_[f].c_str()+string(".pdf")).c_str());
//     canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+sampleName_[f].c_str()+string(".png")).c_str());
//     canvas_->Clear();
    
//     hist_[f]->SetLineColor(sampleColour_[f]);
//     systHist_[2][f]->SetFillStyle(1001);
//     systHist_[3][f]->SetFillStyle(1001);
//     hist_[f]->SetFillStyle(1001);
    
//   }

//     /////////////////////////

//   leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg2_->SetTextFont(62);
//   leg2_->SetBorderSize(0);
//   leg2_->SetLineColor(1);
//   leg2_->SetLineStyle(1);
//   leg2_->SetLineWidth(1);
//   leg2_->SetFillColor(0);
//   leg2_->SetFillStyle(0);
//   leg2_->AddEntry(hist_[1],"Base","l");
//   leg2_->AddEntry(systHist_[0][1],"JERup","l");
//   leg2_->AddEntry(systHist_[1][1],"JERdown","l");
//   leg2_->AddEntry(&ddbkgBands, "stat+method","f");
  
//   leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
//   leg_->SetTextFont(62);
//   leg_->SetBorderSize(0);
//   leg_->SetLineColor(1);
//   leg_->SetLineStyle(1);
//   leg_->SetLineWidth(1);
//   leg_->SetFillColor(0);
//   leg_->SetFillStyle(0);
//   leg_->AddEntry(hist_[1],"Base","l");
//   leg_->AddEntry(systHist_[4][1],"JERup","l");
//   leg_->AddEntry(systHist_[5][1],"JERdown","l");
  
//   hist_[1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[4][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[4][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[5][1]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[5][1]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[4][1]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[1]->SetLineColor(1);
//   systHist_[4][1]->SetLineColor(2);
//   systHist_[5][1]->SetLineColor(3);
//   systHist_[4][1]->Draw("hist");
//   systHist_[5][1]->Draw("histsame");
//   hist_[1]->Draw("histsame");
//   leg_->Draw();
//   canvas_->cd(); 
//   // Order chosen to have good Y axis boundaries

//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.png")).c_str());
//   canvas_->Clear();
//   hist_[1]->SetLineColor(sampleColour_[1]);
  
//   hist_[2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[4][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[4][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[5][2]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[5][2]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[4][2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[2]->SetLineColor(1);
//   systHist_[4][2]->SetLineColor(2);
//   systHist_[5][2]->SetLineColor(3);
//   systHist_[4][2]->Draw("hist");
//   systHist_[5][2]->Draw("histsame");
//   hist_[2]->Draw("histsame");
//   leg_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.png")).c_str());
//   canvas_->Clear();
//   hist_[2]->SetLineColor(sampleColour_[2]);
  
//   systHist_[4][3]->SetFillStyle(0);
//   systHist_[5][3]->SetFillStyle(0);
//   hist_[3]->SetFillStyle(0);
  
//   hist_[3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   hist_[3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[4][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[4][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   systHist_[5][3]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//   systHist_[5][3]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
//   systHist_[4][3]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//   hist_[3]->SetMarkerColor(1);
//   systHist_[4][3]->SetLineColor(2);
//   systHist_[5][3]->SetLineColor(3);
//   systHist_[4][3]->Draw("hist");
//   systHist_[5][3]->Draw("histsame");
//   hist_[3]->Draw("same");
//   ddbkgBands.Draw("2same");
//   leg2_->Draw();
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.png")).c_str());
//   canvas_->Clear();
//   hist_[3]->SetLineColor(sampleColour_[3]);
  
  
//   systHist_[4][3]->SetFillStyle(1001);
//   systHist_[5][3]->SetFillStyle(1001);
//   hist_[3]->SetFillStyle(1001);
  
  
//   for(size_t f=4; f<nSamples_+2; f++){
    
//     systHist_[4][f]->SetFillStyle(0);
//     systHist_[5][f]->SetFillStyle(0);
//     hist_[f]->SetFillStyle(0);
    
//     hist_[f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     hist_[f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[4][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[4][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     systHist_[5][f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     systHist_[5][f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
    
//     systHist_[4][f]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
//     hist_[f]->SetLineColor(1);
//     systHist_[4][f]->SetLineColor(2);
//     systHist_[5][f]->SetLineColor(3);
//     systHist_[4][f]->Draw("hist");
//     systHist_[5][f]->Draw("histsame");
//     hist_[f]->Draw("histsame");
//     leg_->Draw();
//     canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+sampleName_[f].c_str()+string(".pdf")).c_str());
//     canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+sampleName_[f].c_str()+string(".png")).c_str());
//     canvas_->Clear();
    
//     hist_[f]->SetLineColor(sampleColour_[f]);
//     systHist_[4][f]->SetFillStyle(1001);
//     systHist_[5][f]->SetFillStyle(1001);
//     hist_[f]->SetFillStyle(1001);
    
//   }
    
//     //// END SYST

//     //  leg_ = new TLegend(0.3,0.635,0.63,0.93,NULL,"brNDC");
//   leg_ = new TLegend(0.23,0.535,0.62,0.93,NULL,"brNDC");
//     //  leg_ = new TLegend(0.7147651,0.6346154,0.9446309,0.9353147,NULL,"brNDC");
//     //leg_ = new TLegend(0.75,0.6346154,1.,1.,NULL,"brNDC");
    
//   leg_->SetTextFont(62);
//   leg_->SetBorderSize(0);
//   leg_->SetLineColor(1);
//   leg_->SetLineStyle(1);
//   leg_->SetLineWidth(1);
//   leg_->SetFillColor(0);
//   leg_->SetFillStyle(0);
//   leg_->AddEntry(hist_[0],(fancySampleName_[0]).c_str(),"lep");
//   leg_->AddEntry(hist_[1],fancySampleName_[1].c_str(),"l");
//   //  leg_->AddEntry(signalHistHH_,signalSampleNameHH_.c_str(),"f");


//   // Because colour printer is a nobrainer
//   leg_->AddEntry(hist_[3],fancySampleName_[3].c_str(),"f");
//   leg_->AddEntry(hist_[8],fancySampleName_[8].c_str(),"f");
//   leg_->AddEntry(hist_[7],fancySampleName_[7].c_str(),"f");
//   leg_->AddEntry(hist_[6],fancySampleName_[6].c_str(),"f");
//   leg_->AddEntry(hist_[5],fancySampleName_[5].c_str(),"f");
//   leg_->AddEntry(hist_[4],fancySampleName_[4].c_str(),"f");

// //    leg_->AddEntry(hist_[3],fancySampleName_[3].c_str(),"f");
// //    for(size_t f=4; f<nSamples_+2; f++) leg_->AddEntry(hist_[f],fancySampleName_[f].c_str(),"f");
//     // for fig7 // for(size_t f=0; f<nMcSamples_; f++) leg_->AddEntry(mcbkgHist_[f],mcBkgFancySampleName_[f].c_str(),"f");
//   canvas_->cd(); 
//     // Order chosen to have good Y axis boundaries
   
// //   perMassPointSignalShapesToCompareHH_.push_back((TH1*)hist_[2]->Clone(hist_[2]->GetName() +TString("comparison") + massPointName_[currentMassPoint_].c_str()) );
// //   perMassPointSignalShapesToCompareWH_.push_back((TH1*)hist_[1]->Clone(hist_[1]->GetName() +TString("comparison") + massPointName_[currentMassPoint_].c_str()) );
  
// //   TH1* higgsH_ = 0;
// //   double cHiggsBR_ = 0.05; // Perhaps move to cfg file.
// //   double fhh(cHiggsBR_*cHiggsBR_) , fhw( 2*(1-cHiggsBR_)*cHiggsBR_), ftt(1-fhh-fhw);
// //   //  signalHistWH_->Scale(fhw/signalHistWH_->Integral());
// //   //  signalHistHH_->Scale(fhh/signalHistHH_->Integral());
    
// //   //  signalHistWH_->Add(signalHistHH_, fhh);
// //   higgsH_ = hist_[1];
// //   higgsH_->Scale(fhw/higgsH_->Integral());
// //   higgsH_->Add(hist_[2],fhh);
  
// //   perMassPointSignalShapesToCompare_.push_back((TH1*)higgsH_->Clone(higgsH_->GetName() + TString("totComparison") + massPointName_[currentMassPoint_].c_str() ) );    
// //   // Stacked drawing -------------------------------------------
    
    
    
// //   hist_[0]->GetYaxis()->SetTitle("a.u.");
// //   hist_[0]->GetYaxis()->SetTitleOffset(0.85);
// //   hist_[0]->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
// //   hist_[0]->GetXaxis()->SetTitleOffset(0.85);
  
// //   cout << " DATA HIST BIN 0: " << hist_[0]->GetBinContent(0) << " +/- " << hist_[0]->GetBinError(0) << endl;
// //   cout << " DATA HIST BIN 1: " << hist_[0]->GetBinContent(1) << " +/- " << hist_[0]->GetBinError(1) << endl;
  
//   hist_[1]->GetYaxis()->SetTitle("a.u.");
//   hist_[1]->GetYaxis()->SetTitleOffset(0.85);
//   hist_[1]->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
//   hist_[1]->GetXaxis()->SetTitleOffset(0.85);
  
  
  
//   THStack hs("hs","stacked");
  
//   hist_[2]->DrawNormalized("histsame");
//   for(size_t f=3; f<nSamples_+2; f++){
//     // for fig7 // for(size_t f=0; f<nMcSamples_; f++){
//     hist_[f]->GetYaxis()->SetTitle("a.u.");
//     hist_[f]->GetYaxis()->SetTitleOffset(0.85);
//     hist_[f]->GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
//     hist_[f]->GetXaxis()->SetTitleOffset(0.85);
//     hist_[f]->GetXaxis()->SetTitleSize(5);
//     hist_[f]->GetXaxis()->SetRange(displayMin_,displayMax_);    
//     hist_[f]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//     if(isDDbkg_[f] == 0) hs.Add(hist_[f],"hist");
//   }
  
//   for(size_t f=3; f<nSamples_+2; f++){
//     if(isDDbkg_[f]) hs.Add(hist_[f],"hist");
//   }
  
//     //  dataHist_->SetMarkerStyle(1);
//     //  dataHist_->SetMarkerSize(0.8);
  
  
// //   cout << "dd integral: " << hist_[3]->Integral() << endl;
// //   normalize(hs, 1.);
// //   hs.SetMaximum(0.4);
// //   hs.Draw("hist");
// //   hs.GetXaxis()->SetRange(displayMin_,displayMax_);    
// //   hs.GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
// //   hs.GetYaxis()->SetTitle("a.u.");
// //   //  hs.GetYaxis()->SetTitleOffset(1.5);
// //   hs.GetXaxis()->SetTitle("p^{lead.track}/E^{#tau}");
// //     //  hs.GetXaxis()->SetTitleOffset(1.5);
  
// //   hist_[0]->Scale(1./hist_[0]->Integral());
// //   hist_[0]->GetXaxis()->SetRange(displayMin_,displayMax_);    
// //   hist_[0]->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
  
// //   hist_[0]->Draw("same");
// //   higgsH_->Scale(1./higgsH_->Integral());    /// ??? was signalHistWH_->Integral()); instead of higgsH->Integral());
// //   higgsH_->GetXaxis()->SetRange(displayMin_,displayMax_);    
// //   higgsH_->GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
// //   higgsH_->Draw("histsame");
  
//   TGraphErrors myBkgError;
//   getErrorBands(hs, myBkgError);
//   myBkgError.SetFillColor(1);
//   myBkgError.SetFillStyle(3004);
//   myBkgError.GetYaxis()->SetTitle("a.u.");
//   myBkgError.GetYaxis()->SetTitleOffset(0.85);
//   myBkgError.GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
//   myBkgError.GetXaxis()->SetTitleOffset(0.85);
//   myBkgError.GetXaxis()->SetTitleSize(5);
//   myBkgError.GetXaxis()->SetRange(displayMin_,displayMax_);    
//   myBkgError.GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
//   myBkgError.Draw("2");
  
// //   TGraphErrors mySignalError;
// //   getErrorBands(*higgsH_, mySignalError);
// //   mySignalError.SetName("blahSignalError");
// //   mySignalError.SetFillColor(616);
// //   mySignalError.SetFillStyle(3005);
// //   mySignalError.GetYaxis()->SetTitle("a.u.");
// //   mySignalError.GetYaxis()->SetTitleOffset(0.85);
// //   mySignalError.GetXaxis()->SetTitle("p_{T}^{lead.track}/E^{#tau}");
// //   mySignalError.GetXaxis()->SetTitleOffset(0.85);
// //   mySignalError.GetXaxis()->SetTitleSize(5);
// //   mySignalError.GetXaxis()->SetRange(displayMin_,displayMax_);    
// //   mySignalError.GetXaxis()->SetRangeUser(displayMin_,displayMax_);    
// //   mySignalError.Draw("2");
  
//   leg_->AddEntry(myBkgError.GetName(),"bkg total unc.","f");
//   //  leg_->AddEntry(mySignalError.GetName(),"signal total unc.","f");
  
// //   TPaveText *pt1 = new TPaveText(0.17,0.45,0.65,0.5, "brNDC");
// //   pt1->SetBorderSize(1);
// //   pt1->SetFillColor(19);
// //   pt1->SetFillStyle(0);
// //   pt1->SetLineColor(0);
// //   pt1->SetTextFont(132);
// //   pt1->SetTextSize(0.055);
// //   //  TText *text = pt1->AddText("#splitline{m_{H^{#pm}} = 120 GeV/c^{2},}{BR(t #rightarrow H^{+}b) = 0.05}");
// //   TText *text = pt1->AddText("B(t #rightarrow H^{+}b) = 0.05");
// //   text->SetTextAlign(11);
// //   pt1->Draw("same");
  
//   TPaveText *pt = new TPaveText(0.15,0.93,0.9,1.0, "brNDC");
//   pt->SetBorderSize(1);
//   pt->SetFillColor(19);
//   pt->SetFillStyle(0);
//   pt->SetLineColor(0);
//   pt->SetTextFont(132);
//   pt->SetTextSize(0.045);
//   //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.1 fb^{-1}  CMS Preliminary");
//   //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.1 fb^{-1} CMS ");
//   //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  2.2 fb^{-1} CMS Preliminary");
//   //TText *text = pt->AddText("#sqrt{s} = 7 TeV,  1.9 fb^{-1}  CMS Preliminary");
//   //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  1.9 fb^{-1}  CMS ");
//   //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  4.0 fb^{-1} CMS Preliminary");
//   //   TText *text = pt->AddText("#sqrt{s} = 7 TeV,  4.7 fb^{-1} CMS Preliminary");
//   TText *textPrel = pt->AddText("#sqrt{s} = 7 TeV,  4.9 fb^{-1} CMS Preliminary");
//   textPrel->SetTextAlign(11);
//   pt->Draw("same");
  
  
//   // End stacked drawing ---------------------------------------------
  
//   // Normal drawing ----------------------------------------
//   //  signalHistWH_->Draw("hist");
//   //  //  signalHistHH_->DrawNormalized("histsame");
//   //  
//   //  ddbkgHist_->Draw("histsame");
//   //  for(size_t f=0; f<nMcSamples_; f++)
//   //    mcbkgHist_[f]->Draw("histsame");
//   //  dataHist_->Draw("histsame");    
//   // End normal drawing -----------------------------------
  
//   leg_->Draw();
  
//   canvas_->SaveAs((outFolder_+outputFileName+string(".pdf")).c_str());
//   canvas_->SaveAs((outFolder_+outputFileName+string(".png")).c_str());
//   canvas_->cd();
//   canvas_->Clear();
//   /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
//  // End if !produceOnly_
  
//   // Stat filling and histogram putting nonzeroes instead of zeroes (RooFit mojo)
//   // Perhaps do a single loop with systs.
  
  
  
//   for(size_t f=0; f<nSamples_+2; f++){
//     for(int ibin=1; ibin<=hist_[f]->GetNbinsX(); ibin++){ // <= is for overflow
      
//       hist_[nSamples_]->Scale(50.5/hist_[nSamples_]->Integral());      // Normalize to sample
//       hist_[nSamples_+1]->Scale(0.4/hist_[nSamples_+1]->Integral()); // Normalize to sample
      
//       //      if(f == 1 | f == 2)
//       if(ibin==1 && hist_[f]->GetBinContent(ibin) != 0){
// 	hist_[f]->SetBinError(ibin,  hist_[f]->GetBinContent(ibin) * hist_[f]->GetBinError(ibin+1)/hist_[f]->GetBinContent(ibin+1)  );
// 	cout << "bin " << ibin << ", bin error: " << hist_[f]->GetBinError(ibin) << endl;
// 	//	if(f<nSamples_){
// 	//	  cout << "sample " << hist_[f]->GetName();
// 	//	  cout << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	//	  hist_[f]->SetBinError(ibin, sqrt( myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral()     ) );
// 	//	  cout << "done sample " << hist_[f]->GetName() << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	//	} else{
// 	//	  cout << "sample " << hist_[f]->GetName();
// 	//	  cout << ", bincontent scaled " <<    myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral() << endl;
// 	//	  hist_[f]->SetBinError(ibin, sqrt( myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral()     ) );
// 	//	}
//       }

//       if(hist_[f]->GetBinContent(ibin) == 0){ // Protection from RooStat/Fit breaking in combine
// 	hist_[f]->SetBinContent(ibin, 1e-6);
// 	cout << hist_[f]->GetName() << " has 0 bin with content " << hist_[f]->GetBinContent(ibin) << " and error " << hist_[f]->GetBinError(ibin) << endl;
// 	//	hist_[f]->SetBinError(ibin,0);
//       }
//       // Fill stat uncertainty histograms
//       histStatUp_[f]  ->SetBinContent(ibin, std::max( hist_[f]->GetBinContent(ibin) + hist_[f]->GetBinError(ibin) , 1e-6 ) );
//       histStatDown_[f]->SetBinContent(ibin, std::max( hist_[f]->GetBinContent(ibin) - hist_[f]->GetBinError(ibin) , 1e-6 ) );
//       cout << "==================================" << hist_[f]->GetName() << ": statUp " << hist_[f]->GetBinContent(ibin) + hist_[f]->GetBinError(ibin) << ", statDown " << hist_[f]->GetBinContent(ibin) - hist_[f]->GetBinError(ibin) << endl;
//     }
//   }

//   //  hist_[nSamples_]->Scale(50.5/hist_[nSamples_]->Integral());      // Normalize to sample
//   //  hist_[nSamples_+1]->Scale(0.4/hist_[nSamples_+1]->Integral()); // Normalize to sample
  
//   // Rescale stat plots - they must have the same integral as the base ones
//   for(size_t f=0; f<nSamples_+2; f++){
//     histStatUp_[f]->Scale(hist_[f]->Integral()/histStatUp_[f]->Integral());
//     histStatDown_[f]->Scale(hist_[f]->Integral()/histStatDown_[f]->Integral());
//   }
  
//   if(!unsplitUncertainties_){ // the correct thing to do
//     for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
//       for(size_t f=0; f<nSamples_+2; f++){
// 	for(int ibin=1; ibin<=systHist_[a][f]->GetNbinsX(); ibin++){ // <= is for overflow
	  
// 	  if(ibin==1 && systHist_[a][f]->GetBinContent(ibin) != 0){
// 	    systHist_[a][f]->SetBinError(ibin,  systHist_[a][f]->GetBinContent(ibin) * systHist_[a][f]->GetBinError(ibin+1)/systHist_[a][f]->GetBinContent(ibin+1)  );
// 	    //	if(f<nSamples_){
// 	    //	  cout << "sample " << hist_[f]->GetName();
// 	    //	  cout << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	    //	  hist_[f]->SetBinError(ibin, sqrt( myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral()     ) );
// 	    //	  cout << "done sample " << hist_[f]->GetName() << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	    //	} else{
// 	    //	  cout << "sample " << hist_[f]->GetName();
// 	    //	  cout << ", bincontent scaled " <<    myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral() << endl;
// 	    //	  hist_[f]->SetBinError(ibin, sqrt( myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral()     ) );
// 	    //	}
// 	  }
	  
// 	  if(systHist_[a][f]->GetBinContent(ibin) == 0){ // Protection from RooStat/Fit breaking in combine
// 	    systHist_[a][f]->SetBinContent(ibin, 1e-6);
// 	    cout << systHist_[a][f]->GetName() << " has 0 bin error " << systHist_[a][f]->GetBinError(ibin) << endl;
// 	  //	hist_[f]->SetBinError(ibin,0);
// 	  }
// 	}
//       }
//     } // end loop on syst components
//   } // End of the correct way
//   else{ // The finnish way: JES, JER, MET are stacked one over the other like if they were correlated
    
//     // Stack them
//     for(size_t f=0; f<nSamples_+2; f++){
//       systHist_[0][f]->Add(systHist_[2][f],1);
//       systHist_[0][f]->Add(systHist_[4][f],1);
//       systHist_[0][f]->Add(hist_[f],-2);
      
//       systHist_[1][f]->Add(systHist_[3][f],1);
//       systHist_[1][f]->Add(systHist_[5][f],1);
//       systHist_[1][f]->Add(hist_[f],-2);
      
//       systHist_[0][f]->Scale(hist_[f]->Integral()/systHist_[0][f]->Integral());
//       systHist_[1][f]->Scale(hist_[f]->Integral()/systHist_[1][f]->Integral());

//     }

 
//     for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
//       for(size_t f=0; f<nSamples_+2; f++){
// 	for(int ibin=1; ibin<=systHist_[a][f]->GetNbinsX(); ibin++){ // <= is for overflow
	  
// 	  if(ibin==1 && systHist_[a][f]->GetBinContent(ibin) != 0){
// 	    systHist_[a][f]->SetBinError(ibin,  systHist_[a][f]->GetBinContent(ibin) * systHist_[a][f]->GetBinError(ibin+1)/systHist_[a][f]->GetBinContent(ibin+1)  );
// 	    //	if(f<nSamples_){
// 	    //	  cout << "sample " << hist_[f]->GetName();
// 	    //	  cout << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	    //	  hist_[f]->SetBinError(ibin, sqrt( myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral()     ) );
// 	    //	  cout << "done sample " << hist_[f]->GetName() << ", bincontent scaled " <<    myDS_[f]->numEntries()*hist_[f]->GetBinContent(ibin)/hist_[f]->Integral() << endl;
// 	    //	} else{
// 	    //	  cout << "sample " << hist_[f]->GetName();
// 	    //	  cout << ", bincontent scaled " <<    myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral() << endl;
// 	    //	  hist_[f]->SetBinError(ibin, sqrt( myDS_[nSamples_-1]->numEntries()*hist_[nSamples_-1]->GetBinContent(ibin)/hist_[nSamples_-1]->Integral()     ) );
// 	    //	}
// 	  }

	  

	  
// //	  if(systHist_[a][f]->GetBinContent(ibin) == 0){ // Protection from RooStat/Fit breaking in combine
// //	    systHist_[a][f]->SetBinContent(ibin, 1e-6);
// //	    cout << systHist_[a][f]->GetName() << " has 0 bin error " << systHist_[a][f]->GetBinError(ibin) << endl;
// //	    //	hist_[f]->SetBinError(ibin,0);
// //	  }
// 	}
//       }
//     } // end loop on syst components
    
//     for(size_t f=0; f<nSamples_+2; f++){
//       systHist_[0][f]->Add(systHist_[2][f], 1);
//       systHist_[0][f]->Add(systHist_[4][f], 1);
//       systHist_[0][f]->Add(hist_[f], -2);

//       systHist_[1][f]->Add(systHist_[3][f], 1);
//       systHist_[1][f]->Add(systHist_[5][f], 1);
//       systHist_[1][f]->Add(hist_[f], -2);

//       for(int ibin=1; ibin<=systHist_[0][f]->GetNbinsX(); ibin++){ // <= is for overflow
// 	if(systHist_[0][f]->GetBinContent(ibin) == 0){ // Protection from RooStat/Fit breaking in combine
// 	  systHist_[0][f]->SetBinContent(ibin, 1e-6);
// 	}
//       }

//       for(int ibin=1; ibin<=systHist_[1][f]->GetNbinsX(); ibin++){ // <= is for overflow
// 	if(systHist_[1][f]->GetBinContent(ibin) == 0){ // Protection from RooStat/Fit breaking in combine
// 	  systHist_[1][f]->SetBinContent(ibin, 1e-6);
// 	}
//       }

//     }    
    

//   } // End of the finnish way 

//fn   outputFile->Write();

//fn   outputFile->Close();

  
//fn }


void TauDileptonPDFBuilderFitter::DoFit(){
  
  //cout << "INIT: signal tree entries:"<<signalTree_->GetEntries()<<endl;
  
  for(size_t f=0; f<fitType_.size(); f++){
    
    // Set fit settings
    InitFitSettings(f);
    BuildDatasetWithCorrelations(f);
    
    for(size_t i = 0; i< nVars_; i++){
      
      InitPerVariableAmbient(i);
      
      BuildDatasets(i);
      
      BuildPDFs(i);
      
      //DrawSystematics(i);

      DrawTemplates(i);
      
      //      BuildConstrainedModels(i);
      
      //      DoPerVariableFit(i);
      
      //      DrawPerVariableFit(i);
      
      //      DoPerVariableLikelihoodFit(i);
      
    }
    
    //    DoCombinedLikelihoodFit();
    
  }
  
  
  //}
  //  cout.rdbuf(old); // restore   
}
