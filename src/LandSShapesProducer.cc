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
  // unused here. It's from TauDileptonPDFBuilderFitter


  // Reset identifier string
  identifier_ = "";

  cout << "Getting Parameter sets" << endl;

  // Get ParameterSet from cfg file
  const edm::ParameterSet &mFitPars = edm::readPSetsFrom(parSet_)->getParameter<edm::ParameterSet>("LandSShapesProducerParSet");

  outFolder_        = mFitPars.getParameter<std::string>("outFolder");
  outputFileName_  = mFitPars.getParameter<std::string>("outputFileName");
  massPointName_  = mFitPars.getParameter<vector<std::string> >("outputFileNameSuffix");
  nMassPoints_ = massPointName_.size();
  baseMCDir_        = mFitPars.getParameter<std::string>("baseMCDir");
  baseDataDir_      = mFitPars.getParameter<std::string>("baseDataDir");

 
  vector<string>inputFileNameTemp = mFitPars.getParameter<vector<std::string> >("inputFileName");
  for(size_t f=0; f<inputFileNameTemp.size(); f++)
    inputFileName_.push_back( inputFileNameTemp[f] );
  
  sampleName_      = mFitPars.getParameter<vector<std::string> >("sampleName");
  
  fancySampleName_      = mFitPars.getParameter<vector<std::string> >("fancySampleName");
  
  sampleColour_     = mFitPars.getParameter<vector<Int_t> >("sampleColour");
  sampleFillStyle_ = mFitPars.getParameter<vector<Int_t> >("sampleFillStyle");
  
  produceOnly_ = mFitPars.getParameter<bool>("produceOnly");
  
  minitreeSelected_   = mFitPars.getParameter<std::string>("minitreeSelected");
  minitreeDataDriven_ = mFitPars.getParameter<std::string>("minitreeDataDriven");
  
  isFromData_       = mFitPars.getParameter<vector<Int_t> >("isFromData"); // Protect data and DD-bkg from syst
  isDDbkg_       = mFitPars.getParameter<vector<Int_t> >("isDDbkg"); // DD
  isSignal_[f]       = mFitPars.getParameter<vector<Int_t> >("isSignal"); // DD
  
  vector<string>systComponentsTemp = mFitPars.getParameter<vector<std::string> >("systComponents");
  for(size_t f=0; f<systComponentsTemp.size(); f++)
    systComponents_.push_back( systComponentsTemp[f] );
  
  vector<string>systFancyComponentsTemp = mFitPars.getParameter<vector<std::string> >("systFancyComponents");
  for(size_t f=0; f<systFancyComponentsTemp.size(); f++){
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
  nSamples_ = inputFileName_.size();
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
  inputFile_.clear();
  inputTree_.clear();
  
  systTree_.clear();

  perMassPointSignalShapesToCompareHH_.clear();
  perMassPointSignalShapesToCompareWH_.clear();
  perMassPointSignalShapesToCompare_  .clear();
  
  // Get files
  for(size_t f=0; f<nSamples_; f++)
    inputFile_.push_back( TFile::Open(baseDir_   + inputFileName_[f]     ) );

  cout << "Got files" << endl;

  // Get base trees
  for(size_t f=0; f<nSamples_; f++)
    inputTree_.push_back( (TTree*) inputFile_[f]     ->Get(minitree_) );
  
  cout << "Got base trees " << endl;
  // Get syst trees
  for(size_t a=0; a<nSysts_; a++){
    cout << "Getting tree " << systComponents_[a] << endl;
    vector<TTree*> temp;
    temp.clear();
    for(size_t f=0; f<nSamples_; f++){
      if(isFromData_[f]) temp.push_back( (TTree*) inputFile_[f] ->Get(minitree_) ); // dummy fill with base minitree. Will not be used, just needed for vectors size
      temp.push_back( (TTree*) inputFile_[f]     ->Get(minitree_ + systComponents_[a]) );
    }
    systTree_.push_back(temp);
  }
  
  cout << "Mass points set" << endl;
}
 
void LandSShapesProducer::InitPerVariableAmbient(size_t i){
  
  // Base trees
  for(size_t f=0; f<nSamples_; f++) inputTree_[f] ->ResetBranchAddresses()  ;

  // Syst trees
  for(size_t a=0; a<nSysts_; a++){
    for(size_t f=0; f<nSamples_; f++){
      cout << "variable " << i << ", syst " << a << ", sample " << f << ", entries " << systTree_[a][f]->GetEntries() << endl;
      mcBkgSystTree_[a][f]->ResetBranchAddresses();
    }
  }
  
  histo_.clear();

  myDSName_.clear();
  myDS_.clear();

  for(size_t a=0; a<nSysts_; a++){
    mySystDSName_.clear();
  }


  myDS_.clear();

  mySystDS_    .clear();

  hist_.clear();
  histStatUp_.clear();
  histStatDown_.clear();
  systHist_.clear();

  histo_.clear();
  systHisto_.clear();

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
  for(size_t f=0; f<nSamples_; f++){
    myDSName_.push_back( fitVars_[i]->getVarName().c_str() + sampleName_[f] + massPointName_[currentMassPoint_] );
  }

  // Define syst data sets
  for(size_t a=0; a<nSysts_; a++){
    vector<string> temp;
    temp.clear();
    for(size_t f=0; f<nSamples_; f++){
      temp.push_back( myDSName_[f] + string(systComponents_[a]) );
     }
     mySystDSName_.push_back(temp);
  }
  
  sumWeights_ = 0;
  cout << "per-variable ambient initialized" << endl;
}

void LandSShapesProducer::BuildDatasets(size_t i){

  
  // Temp variables for setting branch addresses
  double myVarAllocator, myVarWeightAllocator;
  double isOSsig;
  
  mySignalDSWH_         = new RooDataSet(mySignalDSNameWH_.c_str(),mySignalDSNameWH_.c_str(),              RooArgSet(*myvar_,*myvar_weights_), "weight"); // This constructor does not accept the cut parameter  
  
  for(size_t f=0; f<nSamples_; f++){
    myDS_.push_back( new RooDataSet(myDSName_[f].c_str(),myDSName_[f].c_str(), RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    // Get events
    inputTree_[f]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
    inputTree_[f]->SetBranchAddress("weight", &myVarWeightAllocator);
    inputTree_[f]->SetBranchAddress("is_os", &isOSsig);
    //    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<inputTree_[f]->GetEntries(); ev++){
      inputTree_[f]->GetEntry(ev);
      if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
      myvar_->setVal(myVarAllocator);
      sumWeights_ += myVarWeightAllocator;
      if(isDDbkg_[f]){
	myvar_weights_->setVal(osCutEff_*myVarWeightAllocator);
	myMCBkgDS_[f]->add(RooArgSet(*myvar_,*myvar_weights_),osCutEff_*myVarWeightAllocator);
      }
      else{
	myvar_weights_->setVal(myVarWeightAllocator);
	myMCBkgDS_[f]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
      }
    }
  }

  cout << "Got base datasets" << endl;
  
  ///// Syst get
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    vector<RooDataSet*> temp;
    temp.clear();
    for(size_t f=0; f<nSamples_; f++){
      temp.push_back( new RooDataSet(mySystDSName_[a][f].c_str(),mySystDSName_[a][f].c_str(), RooArgSet(*myvar_,*myvar_weights_), "weight") ); // This constructor does not accept the cut parameter
    }
    mySystDS_.push_back(temp);
    for(size_t f=0; f<nSamples_; f++){
      // Get MCBkg events
      systTree_[a][f]->SetBranchAddress(fitVars_[i]->getVarName().c_str(), &myVarAllocator);
      systTree_[a][f]->SetBranchAddress("weight", &myVarWeightAllocator);
      systTree_[a][f]->SetBranchAddress("is_os", &isOSsig);
      //    cout << "getIsoS      ";
      for(unsigned int ev=0; ev<systTree_[a][f]->GetEntries(); ev++){
	systTree_[a][f]->GetEntry(ev);
	if(isOSsig < 0.5 || myVarAllocator < fitVars_[i]->getMin()) continue;
	myvar_->setVal(myVarAllocator);
	sumWeights_ += myVarWeightAllocator;
	if(isDDbkg_[f]){
	  myvar_weights_->setVal(osCutEff_*myVarWeightAllocator);
	  mySystDS_[a][f]->add(RooArgSet(*myvar_,*myvar_weights_),osCutEff_*myVarWeightAllocator);
	}
	else{
	  myvar_weights_->setVal(myVarWeightAllocator);
	  mySystDS_[a][f]->add(RooArgSet(*myvar_,*myvar_weights_),myVarWeightAllocator);
	}
      }
    }
    
  } // End loop on syst components
  ///// End syst get
  
  cout << "Got syst datasets" << endl;

  
  cout << "All datasets built" << endl;
  
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
  for(size_t f=0; f<nSamples_; f++) histo_.push_back( myDS_[f]->binnedClone());

  cout << "Binned clones built" << endl;

  for(size_t f=0; f<nSamples_; f++)
    cout << myDSName_[f] << " unbinned entries: " << myDS_[f]->numEntries() << ". weighted entries: " << histo_[f]->sum(kFALSE) << endl;
  
  cout << "BuildDatasets successful" << endl;



  /// Build binned clones - syst case
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    vector<RooDataHist*> temp;
    temp.clear();
    for(size_t f=0; f<nSamples_; f++){
      temp.push_back( mySystDS_[a][f]->binnedClone() );
    }
    systHisto_.push_back(temp);
    cout << "Syst binned clones built" << endl;
    
    for(size_t f=0; f<nSamples_; f++)
      cout << mySystDSName_[a][f] << " unbinned entries: " << mySystDS_[a][f]->numEntries() << ". weighted entries: " << systHisto_[a][f]->sum(kFALSE) << endl;
   
  } // End syst loop
    /// End build binned clones - syst case
  
  cout << "BuildDatasets successful" << endl;
}

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


void LandSShapesProducer::DrawTemplates(size_t i){

  // Open file for storing shapes
  string outputFileName = outputFileName_ + string("_") + massPointName_[currentMassPoint_] + string("_") + fitVars_[i]->getVarName();
  TFile* outputFile;
  if(produceOnly_)
    outputFile = new TFile((outFolder_+outputFileName+string(".root")).c_str(), "RECREATE");
  
  // Reset canvas
  canvas_->cd();
  canvas_->Clear();




  ddbkgHistUp_ =   (TH1*) ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("Up") );
  ddbkgHistDown_ = (TH1*) ddbkgHist_->Clone(ddbkgHist_->GetName() + TString("Down") );



  //  /////////////////////////////////////////////////////////////////////
  
  // mc bkg histogram ////////////////////////////////////////////////
  for(size_t f=0; f<nSamples_; f++){
    hist_.push_back( histo_[f]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
    hist_[f]->Sumw2();
    if(isSignal_[f]){
      hist_[f]->SetOption("0000"); // What did that do, again? LoL
      hist_[f]->SetLineColour(sampleColour_[f]);
    }
    else
      hist_[f]->SetFillColour(sampleColour_[f]);
    hist_[f]->SetLineWidth(3);
    hist_[f]->SetFillStyle(sampleFillStyle_[f]); //1001 for background and data, 0 for signal // 3017);



    cout << "sample " << sampleName_[f] << ", integral " << hist_[f]->Integral() << endl;
    if(isDDbkg_[f]){
      hist_[f]->Scale(222/ddbkgHist_->Integral());
      ddbkgHistUp_ =   (TH1*) hist_[f]->Clone(hist_[f]->GetName() + TString("Up") );
      ddbkgHistDown_ = (TH1*) hist_[f]->Clone(hist_[f]->GetName() + TString("Down") );
      
      ddbkgHistUp_->SetFillColour(sampleColour_[f]);
      ddbkgHistUp_->SetLineWidth(3);
      ddbkgHistUp_->SetFillStyle(sampleFillStyle_[f]);//3017);
      
      ddbkgHistDown_->SetFillColour(sampleColour_[f]);
      ddbkgHistDown_->SetLineWidth(3);
      ddbkgHistDown_->SetFillStyle(sampleFillStyle_[f]);//3017);
      
      ddbkgHistUp_->Scale((222+11.4)/ddbkgHistUp_->Integral());
      ddbkgHistDown_->Scale((222-11.4)/ddbkgHistDown_->Integral());
      cout << "sample " << sampleName_[f] << ", integral " << hist_[f]->Integral() << " after rescaling " << endl;
    }
  }
  //    ///////////////////////////////////////////////////////////////////
    
  hist_.push_back( (TH1*)hist_[nSamples_-1]->Clone( sampleName_[nSamples_].c_str())   ); // nSamples-1 Must always be di-bosons
  hist_.push_back( (TH1*)hist_[nSamples_-1]->Clone( sampleName_[nSamples_+1].c_str()) ); // nSamples-1 Must always be di-bosons
  hist_[nSamples_]->Scale(0.4/hist_[nSamples_]->Integral());      // Normalize to sample
  hist_[nSamples_+1]->Scale(50.6/hist_[nSamples_+1]->Integral()); // Normalize to sample

  for(size_t f=0; f<nSamples_+2; f++){
    if(f>nSamples_-1){ // Repeat colours for newly cloned histos
      if(isSignal_[f]){
	hist_[f]->SetOption("0000"); // What did that do, again? LoL
	hist_[f]->SetLineColour(sampleColour_[f]);
      }
      else
	hist_[f]->SetFillColour(sampleColour_[f]);
      hist_[f]->SetLineWidth(3);
      hist_[f]->SetFillStyle(sampleFillStyle_[f]); //1001 for background and data, 0 for signal // 3017);
    }

    hist_[f]->Sumw2();
    histStatUp_  .push_back( (TH1*)hist_[f]->Clone(hist_[f]->GetName() + TString("StatUp")));
    histStatDown_.push_back( (TH1*)hist_[f]->Clone(hist_[f]->GetName() + TString("StatDown")));
  }

  
  /// Produce plots - syst case
  for(size_t a=0; a<nSysts_; a++){ // Loop on syst components
    
    // mc bkg histogram ////////////////////////////////////////////////
    vector<TH1*> temp;
    temp.clear();
    for(size_t f=0; f<nSamples_; f++){
      temp.push_back( systHisto_[a][f]->createHistogram(fitVars_[i]->getVarName().c_str(),fitVars_[i]->getBins() ) );
    }
    systHist_.push_back(temp);
    
    for(size_t f=0; f<nSamples_; f++){
      systHist_[a][f]->Sumw2();
      systHist_[a][f]->SetFillColour(sampleColour_[f]);
      systHist_[a][f]->SetFillStyle(sampleFillStyle_[f]);
      if(isDDbkg_[f]){    
	if(a == 0 || a == 2 || a == 4)
	  systHist_[a][f]->Scale((222+11.4/5)/systHist_[a][f]->Integral());
	else
	  systHist_[a][f]->Scale((222-11.4/5)/systHist_[a][f]->Integral());
      }
    }
    //    ///////////////////////////////////////////////////////////////////
    
    systHist_[a].push_back( (TH1*)systHist_[a][nSamples_-1]->Clone(  sampleName_[nSamples_].c_str()) );
    systHist_[a].push_back( (TH1*)systHist_[a][nSamples_-1]->Clone(  sampleName_[nSamples_+1].c_str()) );
    systHist_[a][nSamples_]->Scale(0.4/systHist_[a][nSamples_]->Integral());
    systHist_[a][nSamples_+1]->Scale(50.6/systHist_[a][nSamples_+1]->Integral());
    
    for(size_t f=0; f<nSamples_+2; f++){
      if(f>nSamples_-1){ // Repeat colours for newly cloned histos
	systHist_[a][f]->Sumw2();
	if(isSignal_[f]){
	  systHist_[a][f]->SetOption("0000"); // What did that do, again? LoL
	  systHist_[a][f]->SetLineColour(sampleColour_[f]);
	}
	else
	  systHist_[a][f]->SetFillColour(sampleColour_[f]);
	systHist_[a][f]->SetLineWidth(3);
	systHist_[a][f]->SetFillStyle(sampleFillStyle_[f]); //1001 for background and data, 0 for signal // 3017);
      }
    }
  } // End loop on systs
  
  /// End produce plots - syst case



  if(!produceOnly_){ // For now raw indexes. This will improve.
    TGraphAsymmErrors ddbkgBands;
    getErrorBands(*(hist_[3]), *ddbkgHistUp_, *ddbkgHistDown_, ddbkgBands);

    ddbkgBands.SetFillColour(1);
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
    leg2_->SetLineColour(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColour(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(hist_[1],"Base","l");
    leg2_->AddEntry(systHist_[0][1],"JESup","l");
    leg2_->AddEntry(systHist_[1][1],"JESdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");

    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColour(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColour(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(hist_[1],"Base","l");
    leg_->AddEntry(systHist_[0][1],"JESup","l");
    leg_->AddEntry(systHist_[1][1],"JESdown","l");

    hist_[1]->GetXaxis()->SetRange(0.001,1.001);    
    hist_[1]->GetXaxis()->SetRangeUser(0.001,1.001);    
    systHist_[0][1]->GetXaxis()->SetRange(0.001,1.001);    
    systHist_[0][1]->GetXaxis()->SetRangeUser(0.001,1.001);    
    systHist_[1][1]->GetXaxis()->SetRange(0.001,1.001);    
    systHist_[1][1]->GetXaxis()->SetRangeUser(0.001,1.001);    
    
    systHist_[0][1]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    hist_[1]->SetLineColour(1);
    systHist_[0][1]->SetLineColour(2);
    systHist_[1][1]->SetLineColour(3);
    systHist_[0][1]->Draw("hist");
    systHist_[1][1]->Draw("histsame");
    histWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColour(signalSampleColour_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[0]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[0]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[1]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[1]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[0]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColour(1);
    signalSystHistHH_[0]->SetLineColour(2);
    signalSystHistHH_[1]->SetLineColour(3);
    signalSystHistHH_[0]->Draw("hist");
    signalSystHistHH_[1]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColour(signalSampleColour_);
    
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
    ddbkgHist_->SetMarkerColour(1);
    ddbkgSystHist_[0]->SetLineColour(2);
    ddbkgSystHist_[1]->SetLineColour(3);
    ddbkgSystHist_[0]->Draw("hist");
    ddbkgSystHist_[1]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColour(ddBkgSampleColour_);

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
      mcbkgHist_[f]->SetLineColour(1);
      mcbkgSystHist_[0][f]->SetLineColour(2);
      mcbkgSystHist_[1][f]->SetLineColour(3);
      mcbkgSystHist_[0][f]->Draw("hist");
      mcbkgSystHist_[1][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_jes_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColour(mcBkgSampleColour_[f]);
      mcbkgSystHist_[0][f]->SetFillStyle(1001);
      mcbkgSystHist_[1][f]->SetFillStyle(1001);
      mcbkgHist_[f]->SetFillStyle(1001);

    }


    ////////////////////////////
    leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg2_->SetTextFont(62);
    leg2_->SetBorderSize(0);
    leg2_->SetLineColour(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColour(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(signalHistWH_,"Base","l");
    leg2_->AddEntry(signalSystHistWH_[0],"METup","l");
    leg2_->AddEntry(signalSystHistWH_[1],"METdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");


    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColour(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColour(0);
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
    signalHistWH_->SetLineColour(1);
    signalSystHistWH_[2]->SetLineColour(2);
    signalSystHistWH_[3]->SetLineColour(3);
    signalSystHistWH_[2]->Draw("hist");
    signalSystHistWH_[3]->Draw("histsame");
    signalHistWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColour(signalSampleColour_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[2]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[2]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[3]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[3]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[2]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColour(1);
    signalSystHistHH_[2]->SetLineColour(2);
    signalSystHistHH_[3]->SetLineColour(3);
    signalSystHistHH_[2]->Draw("hist");
    signalSystHistHH_[3]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColour(signalSampleColour_);
    
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
    ddbkgHist_->SetMarkerColour(1);
    ddbkgSystHist_[2]->SetLineColour(2);
    ddbkgSystHist_[3]->SetLineColour(3);
    ddbkgSystHist_[2]->Draw("hist");
    ddbkgSystHist_[3]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColour(ddBkgSampleColour_);
    
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
      mcbkgHist_[f]->SetLineColour(1);
      mcbkgSystHist_[2][f]->SetLineColour(2);
      mcbkgSystHist_[3][f]->SetLineColour(3);
      mcbkgSystHist_[2][f]->Draw("hist");
      mcbkgSystHist_[3][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_met_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColour(mcBkgSampleColour_[f]);
      mcbkgSystHist_[2][f]->SetFillStyle(1001);
      mcbkgSystHist_[3][f]->SetFillStyle(1001);
      mcbkgHist_[f]->SetFillStyle(1001);

    }

    /////////////////////////

    leg2_  = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg2_->SetTextFont(62);
    leg2_->SetBorderSize(0);
    leg2_->SetLineColour(1);
    leg2_->SetLineStyle(1);
    leg2_->SetLineWidth(1);
    leg2_->SetFillColour(0);
    leg2_->SetFillStyle(0);
    leg2_->AddEntry(signalHistWH_,"Base","l");
    leg2_->AddEntry(signalSystHistWH_[0],"JERup","l");
    leg2_->AddEntry(signalSystHistWH_[1],"JERdown","l");
    leg2_->AddEntry(&ddbkgBands, "stat+method","f");

    leg_ = new TLegend(0.23,0.65,0.62,0.80,NULL,"brNDC");
    leg_->SetTextFont(62);
    leg_->SetBorderSize(0);
    leg_->SetLineColour(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColour(0);
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
    signalHistWH_->SetLineColour(1);
    signalSystHistWH_[4]->SetLineColour(2);
    signalSystHistWH_[5]->SetLineColour(3);
    signalSystHistWH_[4]->Draw("hist");
    signalSystHistWH_[5]->Draw("histsame");
    signalHistWH_->Draw("histsame");
    leg_->Draw();
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries

    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsWH.png")).c_str());
    canvas_->Clear();
    signalHistWH_->SetLineColour(signalSampleColour_);

    signalHistHH_->GetXaxis()->SetRange(0.001,1.001);    
    signalHistHH_->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[4]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[4]->GetXaxis()->SetRangeUser(0.001,1.001);    
    signalSystHistHH_[5]->GetXaxis()->SetRange(0.001,1.001);    
    signalSystHistHH_[5]->GetXaxis()->SetRangeUser(0.001,1.001);    

    signalSystHistHH_[4]->GetXaxis()->SetTitle("R = p_{T}^{lead.track}/E^{#tau}");
    signalHistHH_->SetLineColour(1);
    signalSystHistHH_[4]->SetLineColour(2);
    signalSystHistHH_[5]->SetLineColour(3);
    signalSystHistHH_[4]->Draw("hist");
    signalSystHistHH_[5]->Draw("histsame");
    signalHistHH_->Draw("histsame");
    leg_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsHH.png")).c_str());
    canvas_->Clear();
    signalHistHH_->SetLineColour(signalSampleColour_);
    
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
    ddbkgHist_->SetMarkerColour(1);
    ddbkgSystHist_[4]->SetLineColour(2);
    ddbkgSystHist_[5]->SetLineColour(3);
    ddbkgSystHist_[4]->Draw("hist");
    ddbkgSystHist_[5]->Draw("histsame");
    ddbkgHist_->Draw("same");
    ddbkgBands.Draw("2same");
    leg2_->Draw();
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.pdf")).c_str());
    canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariationsDD.png")).c_str());
    canvas_->Clear();
    ddbkgHist_->SetLineColour(ddBkgSampleColour_);

    
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
      mcbkgHist_[f]->SetLineColour(1);
      mcbkgSystHist_[4][f]->SetLineColour(2);
      mcbkgSystHist_[5][f]->SetLineColour(3);
      mcbkgSystHist_[4][f]->Draw("hist");
      mcbkgSystHist_[5][f]->Draw("histsame");
      mcbkgHist_[f]->Draw("histsame");
      leg_->Draw();
      canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+mcBkgSampleName_[f].c_str()+string(".pdf")).c_str());
      canvas_->SaveAs((outFolder_+outputFileName+string("_jer_sigVariations")+mcBkgSampleName_[f].c_str()+string(".png")).c_str());
      canvas_->Clear();
      
      mcbkgHist_[f]->SetLineColour(mcBkgSampleColour_[f]);
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
    leg_->SetLineColour(1);
    leg_->SetLineStyle(1);
    leg_->SetLineWidth(1);
    leg_->SetFillColour(0);
    leg_->SetFillStyle(0);
    leg_->AddEntry(dataHist_,(dataFancySampleName_).c_str(),"lep");
    leg_->AddEntry(signalHistWH_,signalFancySampleNameWH_.c_str(),"l");
    //  leg_->AddEntry(signalHistHH_,signalSampleNameHH_.c_str(),"f");
    leg_->AddEntry(ddbkgHist_,ddBkgFancySampleName_.c_str(),"f");
    for(size_t f=0; f<nMcSamples_+2; f++) leg_->AddEntry(mcbkgHist_[f],mcBkgFancySampleName_[f].c_str(),"f");
    // for fig7 // for(size_t f=0; f<nMcSamples_; f++) leg_->AddEntry(mcbkgHist_[f],mcBkgFancySampleName_[f].c_str(),"f");
    canvas_->cd(); 
    // Order chosen to have good Y axis boundaries
    
    

    perMassPointSignalShapesToCompareHH_.push_back((TH1*)signalHistHH_->Clone(signalHistHH_->GetName() +TString("comparison") + massPointName_[currentMassPoint_].c_str()) );
    perMassPointSignalShapesToCompareWH_.push_back((TH1*)signalHistWH_->Clone(signalHistWH_->GetName() +TString("comparison") + massPointName_[currentMassPoint_].c_str()) );
    
    TH1* higgsH_ = 0;
    double cHiggsBR_ = 0.05; // Perhaps move to cfg file.
    double fhh(cHiggsBR_*cHiggsBR_) , fhw( 2*(1-cHiggsBR_)*cHiggsBR_), ftt(1-fhh-fhw);
    //  signalHistWH_->Scale(fhw/signalHistWH_->Integral());
    //  signalHistHH_->Scale(fhh/signalHistHH_->Integral());
    
    //  signalHistWH_->Add(signalHistHH_, fhh);
    higgsH_ = signalHistWH_;
    higgsH_->Scale(fhw/higgsH_->Integral());
    higgsH_->Add(signalHistHH_,fhh);
    
    perMassPointSignalShapesToCompare_.push_back((TH1*)higgsH_->Clone(higgsH_->GetName() + TString("totComparison") + massPointName_[currentMassPoint_].c_str() ) );    
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
    myBkgError.SetFillColour(1);
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
    mySignalError.SetFillColour(616);
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
    pt1->SetFillColour(19);
    pt1->SetFillStyle(0);
    pt1->SetLineColour(0);
    pt1->SetTextFont(132);
    pt1->SetTextSize(0.033);
    //  TText *text = pt1->AddText("#splitline{m_{H^{#pm}} = 120 GeV/c^{2},}{BR(t #rightarrow H^{+}b) = 0.05}");
    TText *text = pt1->AddText("BR(t #rightarrow H^{+}b) = 0.05");
    text->SetTextAlign(11);
    pt1->Draw("same");
    
    TPaveText *pt = new TPaveText(0.15,0.93,0.9,1.0, "brNDC");
    pt->SetBorderSize(1);
    pt->SetFillColour(19);
    pt->SetFillStyle(0);
    pt->SetLineColour(0);
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


  
  // Stat filling and histogram putting nonzeroes instead of zeroes (RooFit mojo)
  // Perhaps do a single loop with systs.
  for(size_t f=0; f<nSamples_+2; f++){
    for(int ibin=0; ibin<hist_[f]->GetNbinsX(); ibin++){
      histStatUp_[f]  ->SetBinContent(ibin, hist_[f]->GetBinContent(ibin) + hist_[f]->GetBinError(ibin) );
      histStatDown_[f]->SetBinContent(ibin, hist_[f]->GetBinContent(ibin) - hist_[f]->GetBinError(ibin) );
    }

  }
  
  // Rescale stat plots
  signalHistWHStatUp_->Scale(signalHistWH_->Integral()/signalHistWHStatUp_->Integral());
  signalHistHHStatUp_->Scale(signalHistHH_->Integral()/signalHistHHStatUp_->Integral());
  ddbkgHistStatUp_->Scale(ddbkgHist_->Integral()/ddbkgHistStatUp_->Integral());
  for(size_t f=0; f<nMcSamples_+2; f++)
    mcbkgHistStatUp_[f]->Scale(mcbkgHist_[f]->Integral()/mcbkgHistStatUp_[f]->Integral());
  dataHistStatUp_->Scale(dataHist_->Integral()/dataHistStatUp_->Integral());

  signalHistWHStatDown_->Scale(signalHistWH_->Integral()/signalHistWHStatDown_->Integral());
  signalHistHHStatDown_->Scale(signalHistHH_->Integral()/signalHistHHStatDown_->Integral());
  ddbkgHistStatDown_->Scale(ddbkgHist_->Integral()/ddbkgHistStatDown_->Integral());
  for(size_t f=0; f<nMcSamples_+2; f++)
    mcbkgHistStatDown_[f]->Scale(mcbkgHist_[f]->Integral()/mcbkgHistStatDown_[f]->Integral());
  dataHistStatDown_->Scale(dataHist_->Integral()/dataHistStatDown_->Integral());
  
  
  
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
    perMassPointSignalShapesToCompare_[i]  ->SetLineColour(currentMassPoint_);
    perMassPointSignalShapesToCompareHH_[i]->SetLineColour(currentMassPoint_);
    perMassPointSignalShapesToCompareWH_[i]->SetLineColour(currentMassPoint_);
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
  leg_->SetLineColour(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColour(0);
  leg_->SetFillStyle(0);
  
  for(size_t s=0; s<signalShapesToCompare_.size(); s++){
    //    for(size_t i=0; i<signalShapesToCompare_[s].size(); i++)
    leg_->AddEntry(signalShapesToCompare_[s][0],(massPointName_[s]).c_str(),"l");
  }  
  
  // Draw them
  for(size_t i=0; i<nVars_; i++){
    canvas_->cd();
    for(size_t s=0; s<signalShapesToCompare_.size(); s++){     
      cout << "DEBUG: signalShapesToCompare: " << signalShapesToCompare_[s].size() << endl;
      signalShapesToCompare_[s][i]->SetLineColour(s);
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
      signalShapesToCompareHH_[s][i]->SetLineColour(s);
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
      signalShapesToCompareWH_[s][i]->SetLineColour(s);
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
