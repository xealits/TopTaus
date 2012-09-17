#include "LIP/TopTaus/interface/TauDileptonPDFBuilderFitter.h"

//#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

FitVar::FitVar(string varName, double min, double max, double bins, double hmin, double hmax, Int_t unbinned, Int_t smoothOrder)
  varName_(varname),
  min_(min),
  max_(max),
  bins_(bins),
  hmin_(hmin),
  hmax_(hmax),
  unbinned_(unbinned),
  smoothOrder_(smoothOrder)
{
  
}

string FitVar::getVarName(){
  return varName_;
}

double FitVar::getMin(){
  return min_;
}

double FitVar::getMax(){
  return max_;
}

int FitVar::getBins(){
  return bins_;
}

double FitVar::getHmin(){
  return hmin_;
}

double FitVar::getHmax(){
  return hmax_;
}

Int_t FitVar::getUnbinned(){
  return unbinned_;
}

Int_t FitVar::getSmoothOrder(){
  return smoothOrder_;
}


TauDileptonPDFBuilderFitter::TauDileptonPDFBuilderFitter(string parSet):
  parSet_(parSet)
{
  
  using namespace std; 
  using namespace RooFit ;
  
  Init();

  SetOptions();




  buildGenericMultiPDFModel(vars, mins,maxs, bins, hmin, hmax, unbinned, smoothOrder, fitType, signalTree_, signalTreeWH_, signalTreeHH_, ddBkgTree_, ttbarmcBkgTree_, mcBkgTree_, dataTree_);//, canvas_);  

  
}
  
void TauDileptonPDFBuilderFitter::Init(){

  // Clear vectors
  nVars_ = 0;
  fitVars.clear();
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
  const edm::ParameterSet &mFitPars = edm::readPSetsFrom(parSet_)->getParameter<edm::ParameterSet>("mFitPars");

  outFolder_        = mFitPars.getParameter<std::string>("outFolder");
  resultsFileName_  = mFitPars.getParameter<std::string>("resultsFileName");
  baseMCDir_        = mFitPars.getParameter<std::string>("baseMCDir");
  baseDataDir_      = mFitPars.getParameter<std::string>("baseDataDir");
  
  signalFileNameWH_   = mFitPars.getParameter<std::string>("signalFileNameWH");
  signalFileNameHH_   = mFitPars.getParameter<std::string>("signalFileNameHH");
  dataFileName_       = mFitPars.getParameter<std::string>("dataFileName");
  ddBkgFileName_      = mFitPars.getParameter<std::string>("ddBkgFileName");
  ttbarmcBkgFileName_ = mFitPars.getParameter<std::string>("ttbarmcBkgFileName");
  mcBkgFileName_      = mFitPars.getParameter<std::string>("mcBkgFileName");
  
  minitreeSelected_   = mFitPars.getParameter<std::string>("");
  minitreeDataDriven_ = mFitPars.getParameter<std::string>("");

  fitType_     = mFitPars.getParameter<vector<int> >("fitType");
  vars_        = mFitPars.getParameter<vector<string> >("vars");
  mins_        = mFitPars.getParameter<vector<double> >("mins");
  maxs_        = mFitPars.getParameter<vector<double> >("maxs");
  bins_        = mFitPars.getParameter<vector<int> >("bins");
  hmin_        = mFitPars.getParameter<vector<double> >("hmin");
  hmax_        = mFitPars.getParameter<vector<double> >("hmax");
  unbinned_    = mFitPars.getParameter<vector<Int_t> >("unbinned");
  smoothOrder_ = mFitPars.getParameter<vector<Int_t> >("smoothOrder");
  
  
  // Open files and get trees
  // ddBkg is the only to be taken from data driven estimation (tree)
  signalFileWH_   = TFile::Open(baseMCDir_   + signalFileNameWH_  ); signalTreeWH_   = (TTree*) signalFileWH_ ->Get(minitreeSelected_);
  signalFileHH_   = TFile::Open(baseMCDir_   + signalFileNameHH_  ); signalTreeHH_   = (TTree*) signalFileHH_ ->Get(minitreeSelected_);
  ddBkgFile_      = TFile::Open(baseMCDir_   + ddBkgFileName_     ); ddBkgTree_      = (TTree*) ddBkgFile     ->Get(minitreeDataDriven_);
  ttbarmcBkgFile_ = TFile::Open(baseMCDir_   + ttbarmcBkgFileName_); ttbarmcBkgTree_ = (TTree*) ttbarmcBkgFile->Get(minitreeSelected);
  mcBkgFile_      = TFile::Open(baseMCDir_   + mcBkgFileName_     ); mcBkgTree_      = (TTree*) mcBkgFile     ->Get(minitreeSelected);
  dataFile_       = TFile::Open(baseDataDir_ + dataFileName_      ); dataTree_       = (TTree*) dataFile      ->Get(minitreeSelected);


  // Set variables
  //
  nVars_ = vars_.size();
  
  for(size_t i=0; i<nVars_; i++)
    {
      FitVar myVar(vars_[i], mins_[i], maxs_[i], bins_[i], hmin_[i], hmax_[i], unbinned_[i], smoothOrder_[i], );
      fitVars_.push_back(myVar);
    }
  
  // Set canvas
  canvas_ = new TCanvas("canvas","My plots ",0,0,1000,500);
  canvas_->cd();
  
  // Set fit settings
  SetFitSettings();

  resultsFile_.open ((outFolder_+resultsFileName_).c_str());
 
  //  Uncomment following line in order to redirect stdout to file
  //  streamToFile_ = std::cout.rdbuf(resultsFile_.rdbuf());
  
  cout << "Init process complete" << endl;
}

void TauDileptonPDFBuilderFitter::SetOptions(){
  gStyle->SetOptStat(0);
}

void TauDileptonPDFBuilderFitter::SetFitSettings(){
  baseIdentifier_="";
  
  // FIXME: hardcoded
  signalStatError_=8.3; // 6.7; // 8.3 and 0.2 propagation // 6.7 (wh) and 0.2 (hh) propagation  
  ddbkgEstimate_ = 207.85; //165.4;
  ddbkgStatError_ = 49.52; //23.7;
  
  switch(fitType_){
  case SM2BKG :
    includeSignal_=false;
    standaloneTTbar_=false;
    baseIdentifier_.append("SM2BKG");
    mcbkgStatError_ = 6.09; //3.48;
    break;
  case SM3BKG:
    includeSignal_=false;
    standaloneTTbar_=true;
    baseIdentifier_.append("SM3BKG");
    ttbarmcbkgStatError_ = 3.4; // 3.0
    mcbkgStatError_ = 5.05; // 1.76
    break;
  case HIGGS2BKG:
    includeSignal_=true;
    standaloneTTbar_=false;
    baseIdentifier_.append("HIGGS2BKG");
    mcbkgStatError_ = 6.09; // 3.48;
    break;
  case HIGGS3BKG:
    includeSignal_=true;
    standaloneTTbar_=true;
    baseIdentifier_.append("HIGGS3BKG");
    ttbarmcbkgStatError_ = 3.4; //3.0;
    mcbkgStatError_ = 5.05; //1.76;
    break;
  default : // Dummy - should never arrive here
    cout<<"Type of fit not available. Check your options motherfucker"<<endl;
    gApplication->Terminate(1);
  }
}

void TauDileptonPDFBuilderFitter::InitPerVariableAmbient(size_t i){
  
  // String identifiers
  switch(fitVars_[i].getUnbinned()){
  case 1 : // Unbinned
    identifier_=baseIdentifier_+string("_unbinned_");
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    identifier_=baseIdentifier_+string("_binned_");
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
    gApplication->Terminate(1);
  }

  // Binned fit variable
  myvar           = new RooRealVar(fitVars_[i].getVarName().c_str(), fitVars_[i].getVarName().c_str(), fitVars_[i].getMin(), fitVars_[i].getMax());  myvar->setBins(fitVars_[i].getBins()); 
  myvar_weights   = new RooRealVar("weight","weight",0,1000);
  isOSvar         = new RooRealVar("is_os","is_os",0,2);
  

  //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  mySignalDSName = fitVars_[i].getVarName().c_str() + string("_mySignalDS");
  myDDBkgDSName  = fitVars_[i].getVarName().c_str() + string("_myDDBkgDS");
  myTTBARMCBkgDSName  = fitVars_[i].getVarName().c_str() + string("_myTTBARMCBkgDS");
  myMCBkgDSName  = fitVars_[i].getVarName().c_str() + string("_myMCBkgDS");
  myDataDSName   = fitVars_[i].getVarName().c_str() + string("_myDataDS");
  mySignalDS      = 0; //= new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyDDBkgDS      = 0; //= new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyTTBARMCBkgDS = 0; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyMCBkgDS      = 0; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
  unrMyDataDS       = 0; //= new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   *myvar );
  myDDBkgDS      = 0; 
  myTTBARMCBkgDS = 0; 
  myMCBkgDS      = 0; 
  myDataDS       = 0; 

  // Define model names
  signalModelName_           = fitVars_[i].getVarName() + string("_SignalModel");
  signalConstrainedName_     = fitVars_[i].getVarName() + string("_signalConstrained");   string signalConstraintName =fitVars_[i].getVarName()+string("_signalConstraint");
  ddbkgModelName_            = fitVars_[i].getVarName() + string("_ddbkgModel");  
  ddbkgConstrainedName_      = fitVars_[i].getVarName() + string("_ddbkgConstrained");   string ddbkgConstraintName =fitVars_[i].getVarName()+string("_ddbkgConstraint");
  ttbarmcbkgModelName_       = fitVars_[i].getVarName() + string("_ttbarmcbkgModel");  
  ttbarmcbkgConstrainedName_ = fitVars_[i].getVarName() + string("_ttbarmcbkgConstrained");   string ttbarmcbkgConstraintName =fitVars_[i].getVarName()+string("_ttbarmcbkgConstraint");
  mcbkgModelName_            = fitVars_[i].getVarName() + string("_mcbkgModel");  
  mcbkgConstrainedName_      = fitVars_[i].getVarName() + string("_mcbkgConstrained");   string mcbkgConstraintName =fitVars_[i].getVarName()+string("_mcbkgConstraint");
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // Define var names
  signalVarName_     =fitVars_[i].getVarName()+string("_signalVar");string signalMeanVarName  =fitVars_[i].getVarName()+string("_signalMeanVar");              string signalSigmaVarName     =fitVars_[i].getVarName()+string("_signalSigmaVar");
  ddbkgVarName_      =fitVars_[i].getVarName()+string("_ddbkgVar"); string ddbkgMeanVarName  =fitVars_[i].getVarName()+string("_ddbkgMeanVar");                string ddbkgSigmaVarName      =fitVars_[i].getVarName()+string("_ddbkgSigmaVar");
  ttbarmcbkgVarName_ =fitVars_[i].getVarName()+string("_ttbarmcbkgVar"); string ttbarmcbkgMeanVarName  =fitVars_[i].getVarName()+string("_ttbarmcbkgMeanVar"); string ttbarmcbkgSigmaVarName =fitVars_[i].getVarName()+string("_ttbarmcbkgSigmaVar");
  mcbkgVarName_      =fitVars_[i].getVarName()+string("_mcbkgVar"); string mcbkgMeanVarName  =fitVars_[i].getVarName()+string("_mcbkgMeanVar");                string mcbkgSigmaVarName      =fitVars_[i].getVarName()+string("_mcbkgSigmaVar");

  // PDF models
  b_signalModel     = 0;
  b_ddbkgModel      = 0;
  b_ttbarmcbkgModel = 0;  
  b_mcbkgModel      = 0;  
  u_signalModel     = 0; 
  u_ddbkgModel      = 0;  
  u_ttbarmcbkgModel = 0;  
  u_mcbkgModel      = 0;  
  
}

void TauDileptonPDFBuilderFitter::BuildDatasets(size_t i){
  
  // Signal dataset from WH and HH
  if(includeSignal_){
    //  mySignalDS     = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
    mySignalDS         = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(),              RooArgSet(*myvar,*myvar_weights), "weight"); // This constructor does not accept the cut parameter
    
    // Temp variables for setting branch addresses
    double myVarAllocator, myVarWeightAllocator;
    double isOSsig;
    
    // Cross section
    // FIXME: hardcoded
    double fhh(0.05*0.05) , fhw( 2*(1-0.05)*0.05) ;      
    double sumWeights(0);
    
    // Get WH events
    signalTreeWH_->SetBranchAddress(fitVars_[i].getVarName().c_str(), &myVarAllocator);
    signalTreeWH_->SetBranchAddress("weight", &myVarWeightAllocator);
    signalTreeWH_->SetBranchAddress("is_os", &isOSsig);
    cout << "getentries " << signalTreeWH_->GetEntries() << endl;
    for(unsigned int ev=0; ev<signalTreeWH_->GetEntries(); ev++){
      cout << "ev: " << ev << endl;
      signalTreeWH_->GetEntry(ev);
      cout << "isOSsig: " << isOSsig << endl;//	if(isOSsig < 0.5) continue;
      if(isOSsig==0) cout << "peggio" << endl;
      myvar_->setVal(myVarAllocator);
      if(fitVars_[i].getVarName() == "rc_t" && myVarAllocator > 1) cout<< "myVar: " << myVarAllocator<<endl;
      sumWeights += myVarWeightAllocator;
      cout << " arrive here" ;
      myvar_weights_->setVal(fhw*myVarWeightAllocator);
      mySignalDS->add(RooArgSet(*myvar_,*myvar_weights_),fhw*myVarWeightAllocator);
    }
    cout << " strt HH" << endl;
    // Get HH events
    signalTreeHH_->SetBranchAddress(fitVars_[i].getVarName().c_str(), &myVarAllocator);
    cout << "getAllocator      ";
    signalTreeHH_->SetBranchAddress("weight", &myVarWeightAllocator);
    cout << "getWeightAllocator      ";
    signalTreeHH_->SetBranchAddress("is_os", &isOSsig);
    cout << "getIsoS      ";
    for(unsigned int ev=0; ev<signalTreeHH_->GetEntries(); ev++){
      signalTreeHH_->GetEntry(ev);
      if(isOSsig < 0.5) continue;
      myvar_->setVal(myVarAllocator);
      if(fitVars_[i].getVarName() == "rc_t" && myVarAllocator > 1) cout<< "myVar: " << myVarAllocator<<endl;
      sumWeights += myVarWeightAllocator;
      myvar_weights_->setVal(fhh*myVarWeightAllocator);
      mySignalDS->add(RooArgSet(*myvar_,*myvar_weights_),fhh*myVarWeightAllocator);
    }
    cout<<"Signal ok"<<endl;
  }
  string myOsCut = "is_os>0.5";
  unrMyDDBkgDS      = new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
  myDDBkgDS = (RooDataSet*) unrMmyDDBkgDS.reduce(RooArgSet(*myvar_,*myvar_weights_));
  cout<<"DDBkg ok"<<endl;
  if(standaloneTTbar){
    unrMyTTBARMCBkgDS = new RooDataSet(myTTBARMCBkgDSName.c_str(), myTTBARMCBkgDSName.c_str(),  ttbarmcBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
    myTTBARMCBkgDS = (RooDataSet*) unrMyTTBARMCBkgDS.reduce(RooArgSet(*myvar_,*myvar_weights_));
  }
  cout<<"TTbar ok"<<endl;
  unrMyMCBkgDS      = new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar_,*myvar_weights_,*isOSvar_),myOsCut.c_str(),"weight" );
  myMCBkgDS = (RooDataSet*) unrMyMCBkgDS.reduce(RooArgSet(*myvar_,*myvar_weights_));
  cout<<"MCBkg ok"<<endl;
  unrMyDataDS       = new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   RooArgSet(*myvar_,*isOSvar_), myOsCut.c_str() );
  myDataDS = (RooDataSet*) unrMyDataDS.reduce(RooArgSet(*myvar_,*myvar_weights_));
  cout<<"Data ok"<<endl;

  // Build binned clones
  if(includeSignal_)
    signalHisto_ = mySignalDS_->binnedClone();
  ddbkgHisto_ = myDDBkgDS_->binnedClone();
  if(standaloneTTbar_)
    ttbarmcbkgHisto_  = myTTBARMCBkgDS_->binnedClone(); 
  mcbkgHisto_ = myMCBkgDS_->binnedClone();
  dataHisto_  = myDataDS_ ->binnedClone();
  

}

void TauDileptonPDFBuilderFitter::BuildPDFs(size_t i){
  
  switch(fitVars_[i].getUnbinned()){
  case 1 : // Unbinned
    if(includeSignal_) u_signalModel_ = new RooKeysPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *mySignalDS_, RooKeysPdf::MirrorBoth,2);
    u_ddbkgModel_  = new RooKeysPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(),  *myvar_, *myDDBkgDS_ , RooKeysPdf::MirrorBoth,2);
    if(standaloneTTbar_) u_ttbarmcbkgModel_  = new RooKeysPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *myTTBARMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
    u_mcbkgModel_  = new RooKeysPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *myMCBkgDS_ , RooKeysPdf::MirrorBoth,2);
    break;
  case 0:  // Binned (w/ or w/out smoothing)
    if(includeSignal_) b_signalModel_ = new RooHistPdf(signalModelName_.c_str(), signalModelName_.c_str(), *myvar_, *signalHisto_, fitVars_[i].getSmoothOrder());
    b_ddbkgModel_  = new RooHistPdf(ddbkgModelName_.c_str(),  ddbkgModelName_.c_str(), *myvar_, *ddbkgHisto_ , fitVars_[i].getSmoothOrder());
    if(standaloneTTbar_) b_ttbarmcbkgModel_ = new RooHistPdf(ttbarmcbkgModelName_.c_str(),  ttbarmcbkgModelName_.c_str(),  *myvar_, *ttbarmcbkgHisto_  , fitVars_[i].getSmoothOrder()); 
    b_mcbkgModel_  = new RooHistPdf(mcbkgModelName_.c_str(),  mcbkgModelName_.c_str(),  *myvar_, *mcbkgHisto_  , fitVars_[i].getSmoothOrder()); 
    break;
  default : // Dummy - should never arrive here
    cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
    gApplication->Terminate(1);
  }
}

void buildGenericMultiPDFModel( 
			       unsigned int & fitType,
			       TTree* signalTree_, 
			       TTree* signalTreeWH_, 
			       TTree* signalTreeHH_, 
			       TTree* ddBkgTree_ ,
			       TTree* ttbarmcBkgTree_,
			       TTree* mcBkgTree_,
			       TTree* dataTree_
			       //			       TCanvas* canvas2_
			       ){
  
  cout << "INIT: signal tree entries:"<<signalTree_->GetEntries()<<endl;
  
  
  for(size_t i = 0; i< nVars_; i++){

    InitPerVariableAmbient(i);
    
    BuildDatasets();

    BuildPDFs();


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    //Generate histograms and Models ///////////////////////////////////////////////////////////////////////////////////////////
    

    
    //signal histogram ////////////////////////////////////////////////////////////
    TH1 * signalHist;
    if(includeSignal){
      signalHist = signalHisto->createHistogram(fitVars_[i].getVarName().c_str(),fitVars_[i].getBins() );
      signalHist->SetOption("0000");
      signalHist->SetLineWidth(3);
      signalHist->SetTitle("");
      signalHist->GetYaxis()->SetTitle("a.u.");
      signalHist->GetYaxis()->SetTitleOffset(1.5);
      signalHist->SetLineColor(kGreen);
      signalHist->SetFillColor(kGreen);
    }
    ///////////////////////////////////////////////////////////////////////////////
    
    
    // dd bkg histogram /////////////////////////////////////////////////
    TH1 * ddbkgHist = ddbkgHisto->createHistogram(fitVars_[i].getVarName().c_str(),fitVars_[i].getBins() );
    ddbkgHist->SetLineColor(kRed);
    ddbkgHist->SetFillColor(kRed);
    ddbkgHist->SetLineWidth(3);
    ddbkgHist->SetFillStyle(3017);
    /////////////////////////////////////////////////////////////////////

    // ttbar mc bkg histogram ////////////////////////////////////////////////
    TH1 * ttbarmcbkgHist;
    if(standaloneTTbar){
      ttbarmcbkgHist = ttbarmcbkgHisto->createHistogram(fitVars_[i].getVarName().c_str(),fitVars_[i].getBins() );   
      ttbarmcbkgHist->SetLineColor(kYellow);
      ttbarmcbkgHist->SetFillColor(kYellow);
      ttbarmcbkgHist->SetLineWidth(3);
      ttbarmcbkgHist->SetFillStyle(3017);
    }
    ///////////////////////////////////////////////////////////////////
    
    // mc bkg histogram ////////////////////////////////////////////////
    TH1 * mcbkgHist = mcbkgHisto->createHistogram(fitVars_[i].getVarName().c_str(),fitVars_[i].getBins() );   
    mcbkgHist->SetLineColor(kBlack);
    mcbkgHist->SetFillColor(kBlack);
    mcbkgHist->SetLineWidth(3);
    mcbkgHist->SetFillStyle(3017);
    ///////////////////////////////////////////////////////////////////
    
    TLegend *leg = new TLegend(0.3,0.665,0.85,0.86,NULL,"brNDC");
    leg->SetTextFont(62);
    leg->SetBorderSize(0);
    leg->SetLineColor(1);
    leg->SetLineStyle(1);
    leg->SetLineWidth(1);
    leg->SetFillColor(0);
    leg->SetFillStyle(1001);
    if(includeSignal) leg->AddEntry(signalHist,"signal template","f");
    leg->AddEntry(ddbkgHist,"dd bkg template","f");
    if(standaloneTTbar) leg->AddEntry(ttbarmcbkgHist,"irr mc bkg template","f");
    leg->AddEntry(mcbkgHist,"mc bkg template","f");
    
    canvas_->cd(); 
    if(standaloneTTbar){
      if(fitVars_[i].getHmax()){ ttbarmcbkgHist->SetMaximum(fitVars_[i].getHmax()); ttbarmcbkgHist->SetMinimum(fitVars_[i].getHmin());} 
      ttbarmcbkgHist->DrawNormalized("hist");
      mcbkgHist->DrawNormalized("histsame");    
    }
    else {
      if(fitVars_[i].getHmax()){ mcbkgHist->SetMaximum(fitVars_[i].getHmax()); mcbkgHist->SetMinimum(fitVars_[i].getHmin());} 
      mcbkgHist->DrawNormalized("hist");
    }
    
    if(includeSignal)
      signalHist->DrawNormalized("histsame");
    ddbkgHist->DrawNormalized("histsame");
    
    if(standaloneTTbar) ttbarmcbkgHist->DrawNormalized("histsame"); // in order for it to be on top and thus viewable for discriminating it from higgs in rc_t plots
    
    

    
    
    leg->Draw();
    string shapes= string("shapes_")+identifier+fitVars_[i].getVarName();
    canvas_->SaveAs((outFolder+shapes+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder+shapes+string(".png")).c_str());
    canvas_->cd();
    canvas_->Clear();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    // Get total entries from Histograms ////////////////////////////////////////////////
    cout<<endl<<endl<<"******************************************************************************************"<<endl;
    
    double sig_N   ;  double nsig   ;
    double ddbkg_N ;  double nddbkg ;
    double ttbarmcbkg_N ;  double nttbarmcbkg ; 
    double mcbkg_N ;  double nmcbkg ; 
    double data_N  ;   
    
    
    if(includeSignal){ sig_N   = mySignalDS->numEntries(); nsig   = signalHisto->sum(kFALSE); }
    ddbkg_N = myDDBkgDS->numEntries();  nddbkg = ddbkgHisto->sum(kFALSE);
    if(standaloneTTbar){ ttbarmcbkg_N = myTTBARMCBkgDS->numEntries();  nttbarmcbkg = ttbarmcbkgHisto->sum(kFALSE); }
    mcbkg_N = myMCBkgDS->numEntries();  nmcbkg = mcbkgHisto->sum(kFALSE); 
    data_N  = myDataDS->numEntries();   
    
    ///    switch(unbinned[i]){
    ///    case 1 : // Unbinned
    ///      nsig   = mySignalDS->numEntries();
    ///      nddbkg = myDDBkgDS ->numEntries(); 
    ///      nmcbkg = myMCBkgDS ->numEntries(); 
    ///      break;
    ///    case 0:  // Binned (w/ or w/out smoothing)
    ///      nsig   = signalHisto->sum(kFALSE);
    ///      nddbkg = ddbkgHisto ->sum(kFALSE); 
    ///      nmcbkg = mcbkgHisto ->sum(kFALSE); 
    ///      break;
    ///    default : // Dummy - should never arrive here
    ///      nsig   = 0;
    ///      nddbkg = 0;
    ///      nmcbkg = 0;
    ///    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    cout<<endl<<"Identifier: "<<identifier<<", meaning that includeSignal="<<includeSignal<<", standaloneTTbar="<<standaloneTTbar<<endl;
    cout<<endl<<" PDF data set Entries for fitVars_[i].getVarName()ribution : "<<fitVars_[i].getVarName();    
    cout<<endl<<"WARNING: values taken just from unbinned datasets"<<endl;
    cout<<endl<<"Sum weights for signal is " << sumWeights << endl;
    double separateSigEntries = signalTreeWH_->GetEntries() + signalTreeHH_->GetEntries();
    if(includeSignal) cout<<endl<<" sig_N    = "<<sig_N<<  " , root entries : "<< separateSigEntries<<" weights : "<<nsig;
    cout<<endl<<" ddbkg_N  = "<<ddbkg_N<<" , root entries : "<<ddBkgTree_->GetEntries()<<" weights : "<<nddbkg;
    if(standaloneTTbar) cout<<endl<<" ttbarmcbkg_N  = "<<ttbarmcbkg_N<<" , root entries : "<<ttbarmcBkgTree_->GetEntries()<<" weights : "<<nttbarmcbkg;
    cout<<endl<<" mcbkg_N  = "<<mcbkg_N<<" , root entries : "<<mcBkgTree_->GetEntries()<<" weights : "<<nmcbkg;
    cout<<endl<<" data_N   = "<<data_N<< " , root entries : "<<dataTree_->GetEntries()<<endl;
    cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
    /////////////////////////////////////////////////////////////////////////////////////
    
    
    // Building the constrained models for signal mc bkg ///////////////////////////////////////////////////////////////////////////////////////////////
    
    double nsignalMean, nsignalSigma, nttbarmcbkgMean, nttbarmcbkgSigma;
    if(includeSignal) nsignalMean=nsig; nsignalSigma=signalStatError;
    double nddbkgMean(ddbkgEstimate); double nddbkgSigma(ddbkgStatError);
    nddbkg = ddbkgEstimate;
    if(standaloneTTbar) nttbarmcbkgMean=nttbarmcbkg; nttbarmcbkgSigma=ttbarmcbkgStatError;
    double nmcbkgMean(nmcbkg); double nmcbkgSigma(mcbkgStatError);
    
    
    ///resultsFile<<" ******************************************************************************************* "<<endl;
    ///resultsFile<<"Variable " << fitVars_[i].getVarName()<<endl;
    ///resultsFile<<endl<<"Constraints to background number of events"<<endl;
    ///double integral, integralError; integral = mcbkgHist->IntegralAndError(0,mcbkgHist->GetNbinsX()-1,integralError);
    ///resultsFile<<"mcbkg: "<<nmcbkgMean<<"+/-"<<nmcbkgSigma<<", est from hist: " << integral << "+/-" << integralError<<endl;
    ///resultsFile<<"ddbkg: "<<nddbkgMean<<"+/-"<<nddbkgSigma<<endl;
    ///resultsFile<<endl<<endl<<" ******************************************************************************************* "<<endl;
    
    if(includeSignal){
      if( ! sigVar_         ) sigVar_         = new RooRealVar( "globalSignalVarName",       "globalSignalVarName",         nsig,   0, nsig*2.5);
      else{ sigVar_->setMin("",0);   sigVar_->setMax("",nsig*3.5);     sigVar_->setVal(nsig); }
      
      if( ! sigMeanVar_   ) sigMeanVar_   = new RooRealVar( "globalSignalMeanVarName",    "globalSignalMeanVarName",      nsignalMean); 
      else{ sigMeanVar_->setVal(nsignalMean);   }
      
      if( ! sigSigmaVar_  ) sigSigmaVar_  = new RooRealVar( "globalSignalSigmaVarName",   "globalSignalSigmaVarName",     nsignalSigma ); 
      else{ sigSigmaVar_->setVal(nsignalSigma); }
    }
    
    // FIXME?    
    //    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkgMean, 0, nddbkgMean*1.5); 
    //    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkgMean*3.5);     ddbkgVar_->setVal(nddbkgMean);}
    
    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkg, 0, nddbkg*1.5); 
    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkg*1.5);     ddbkgVar_->setVal(nddbkg);}
    
    
    if(! ddbkgMeanVar_)     ddbkgMeanVar_   = new RooRealVar( "globalDDBkgMeanVarName", "globalDDBkgMeanVarName",         nddbkgMean); 
    else{ ddbkgMeanVar_->setVal(nddbkgMean);   }
    
    if(! ddbkgSigmaVar_)    ddbkgSigmaVar_  = new RooRealVar( "globalDDBkgSigmaVarName","globalDDBkgSigmaVarName",        nddbkgSigma);  
    else{ ddbkgSigmaVar_->setVal(nddbkgSigma);   }

    
    if(standaloneTTbar){
      if( ! ttbarmcbkgVar_       ) ttbarmcbkgVar_       = new RooRealVar( "globalTTbarMcBkgVarName",        "globalTTbarMcBkgVarName",          nttbarmcbkg, 0, nttbarmcbkg*1.5); 
      else{ ttbarmcbkgVar_->setMin("",0); ttbarmcbkgVar_->setMax("",nttbarmcbkg*3.5); ttbarmcbkgVar_->setVal(nttbarmcbkg); }
      
      if( ! ttbarmcbkgMeanVar_   ) ttbarmcbkgMeanVar_   = new RooRealVar( "globalTTbarMcBkgMeanVarName",    "globalTTbarMcBkgMeanVarName",      nttbarmcbkgMean); 
      else{ ttbarmcbkgMeanVar_->setVal(nttbarmcbkgMean);   }
      
      if( ! ttbarmcbkgSigmaVar_  ) ttbarmcbkgSigmaVar_  = new RooRealVar( "globalTTbarMcBkgSigmaVarName",   "globalTTbarMcBkgSigmaVarName",     nttbarmcbkgSigma ); 
      else{ ttbarmcbkgSigmaVar_->setVal(nttbarmcbkgSigma); }
    }
    
    if( ! mcbkgVar_       ) mcbkgVar_       = new RooRealVar( "globalMcBkgVarName",        "globalMcBkgVarName",          nmcbkg, 0, nmcbkg*1.5); 
    else{ mcbkgVar_->setMin("",0); mcbkgVar_->setMax("",nmcbkg*3.5); mcbkgVar_->setVal(nmcbkg); }
    
    if( ! mcbkgMeanVar_   ) mcbkgMeanVar_   = new RooRealVar( "globalMcBkgMeanVarName",    "globalMcBkgMeanVarName",      nmcbkgMean); 
    else{ mcbkgMeanVar_->setVal(nmcbkgMean);   }
    
    if( ! mcbkgSigmaVar_  ) mcbkgSigmaVar_  = new RooRealVar( "globalMcBkgSigmaVarName",   "globalMcBkgSigmaVarName",     nmcbkgSigma ); 
    else{ mcbkgSigmaVar_->setVal(nmcbkgSigma); }
    
    
    RooGaussian * signalConstraint;
    RooGaussian * ttbarmcbkgConstraint;
    if(includeSignal) signalConstraint = new RooGaussian( signalConstraintName.c_str(), signalConstraintName.c_str(), *sigVar_,*sigMeanVar_, *sigSigmaVar_);
    RooGaussian * ddbkgConstraint = new RooGaussian( ddbkgConstraintName.c_str(), ddbkgConstraintName.c_str(), *ddbkgVar_, *ddbkgMeanVar_, *ddbkgSigmaVar_);   
    if(standaloneTTbar) ttbarmcbkgConstraint = new RooGaussian( ttbarmcbkgConstraintName.c_str(), ttbarmcbkgConstraintName.c_str(), *ttbarmcbkgVar_,*ttbarmcbkgMeanVar_, *ttbarmcbkgSigmaVar_);
    RooGaussian * mcbkgConstraint = new RooGaussian( mcbkgConstraintName.c_str(), mcbkgConstraintName.c_str(), *mcbkgVar_,*mcbkgMeanVar_, *mcbkgSigmaVar_);
    
    
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    // build the sum model and model with constrains ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    string sumModelName = fitVars_[i].getVarName()+string("_sumModel");
    
    //    string sumModelExp  = signalModelName+string("+")+ddbkgModelName+string("+")+mcbkgModelName;
    string sumModelExp = "";
    if(includeSignal) sumModelExp.append(signalModelName+string("+"));
    sumModelExp.append(ddbkgModelName+string("+"));
    if(standaloneTTbar) sumModelExp.append(ttbarmcbkgModelName+string("+"));
    sumModelExp.append(mcbkgModelName);
        
    
    //    RooAddPdf * sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *signalModel, *ddbkgModel, *mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
    RooAddPdf* sumModel;
    string sumModelConstrainedName = fitVars_[i].getVarName()+string("_sumConstrainedModel");
    string sumModelConstrainedExp  = sumModelName+string("*")+ddbkgConstraintName+string("*")+mcbkgConstraintName;
    RooProdPdf * model;
    RooFitResult * constrainedModelFit;

    //Fit model to data
    //    RooFitResult * constrainedModelFit = model->fitTo( 
    //						      *dataHisto, 
    //						      //						      *myDataDS,
    //      Minos(), 
    //      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
    //      Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max) 
    //    );

    switch(unbinned[i]){
    case 1 : // Unbinned
      if(includeSignal){
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel, *u_ddbkgModel, *u_ttbarmcbkgModel, *u_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel, *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
      } else{
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel, *u_ttbarmcbkgModel, *u_mcbkgModel ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
      }
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      if(includeSignal){
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_ttbarmcbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
      } else{
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel, *b_ttbarmcbkgModel, *b_mcbkgModel ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	}
      }
      break;
    default : // Dummy - should never arrive here
      cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
      gApplication->Terminate(1);
    }
    
    
    


    gStyle->SetOptStat(0);
    string plotWithModelFit   = string("modelFit_"+identifier)+fitVars_[i].getVarName();
    
    canvas_->cd();  
    canvas_->Clear();  
    RooPlot * myFrame = myvar->frame();
    myFrame->SetTitle("");
    myFrame->GetXaxis()->SetTitle(fitVars_[i].getVarName().c_str());
    myFrame->GetYaxis()->SetTitle("Events");
    dataHisto->plotOn(myFrame);
    model->plotOn(myFrame);
    //    model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*signalModel), RooFit::LineColor(kGreen));   
    if(includeSignal) 
      switch(unbinned[i]){
      case 1 : // Unbinned
	model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*u_signalModel), RooFit::LineColor(kGreen));   
	break;
      case 0:  // Binned (w/ or w/out smoothing)
	model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*b_signalModel), RooFit::LineColor(kGreen));   
	break;
      default : // Dummy - should never arrive here
	cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
	gApplication->Terminate(1);
      }
    
    myFrame->Draw();
    canvas_->SaveAs((outFolder+plotWithModelFit+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder+plotWithModelFit+string(".png")).c_str());
    canvas_->cd();
    canvas_->Clear();
    
    // Test on the fit ////////////////////////////////////////////////////////////
    //    RooNLLVar * nll = (RooNLLVar *) model->createNLL(
    //						     *dataHisto,
    //						     //*myDataDS,
    //      RooFit::CloneData(kTRUE),
    //      Extended(kTRUE),
    //      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
    //      Range(min,max)
    //    );
    
    RooNLLVar * nll;
    
    switch(unbinned[i]){
    case 1: // Unbinned
      if(includeSignal)
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	else nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
      else
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	else nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      if(includeSignal)
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	else nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
      else
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
	else nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(fitVars_[i].getMin(),fitVars_[i].getMax()));
      break;
    default : // Dummy - should never arrive here
      cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
      gApplication->Terminate(1);
    }
    
    likelihoodVector[i] = nll; //.push_back(nll); 
    
    RooMinuit minuit( *nll );
    minuit.migrad();
    minuit.setErrorLevel(0.5);
    minuit.hesse();
    
    RooFitResult* myTempRes = minuit.save();
    cout<<endl<<"*******************************************************************************"<<endl;
    cout<<      "*******************************************************************************"<<endl;
    cout<<      "************* IDENTIFIER: "<< identifier << "****************************"<<endl;
    cout<<endl<<"FIT RESULTS for variable"<<fitVars_[i].getVarName()<<endl<<endl;
    myTempRes->Print("v");
    cout<<endl<<"*******************************************************************************"<<endl;
    cout<<      "*******************************************************************************"<<endl;
    
    canvas_->cd();
    canvas_->Clear();
    RooPlot * contourPlot;
    if(includeSignal){
      contourPlot = minuit.contour( *ddbkgVar_, *sigVar_,1,2,3);
      contourPlot->GetYaxis()->SetTitle("N(H+), m_{H} = 120 GeV/c2");
      contourPlot->GetYaxis()->SetRangeUser(0,600);
    }
    else 
      if(standaloneTTbar){
	contourPlot = minuit.contour( *ddbkgVar_, *ttbarmcbkgVar_,1,2,3);
	contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
	contourPlot->GetYaxis()->SetRangeUser(0,400);
      }
      else{
	contourPlot = minuit.contour( *ddbkgVar_, *mcbkgVar_,1,2,3);
	contourPlot->GetYaxis()->SetTitle("N^{MCdriven}_{Bkg}");
	contourPlot->GetYaxis()->SetRangeUser(0,200);
      }
    contourPlot->SetTitle("");
    contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
    contourPlot->GetXaxis()->SetRange(0,400);
    contourPlot->Draw();
    string contourPlotName =  string("contour_")+identifier+fitVars_[i].getVarName();
    canvas_->SaveAs((outFolder+contourPlotName+string(".pdf")).c_str());
    canvas_->SaveAs((outFolder+contourPlotName+string(".png")).c_str());
    canvas_->cd();
    canvas_->Clear();
    ///////////////////////////////////////////////////////////
  }
  
  //  RooAddition* combll;
  //  RooFitResult* myRes;
  //  RooPlot* contourPlot;
  //  for(int dim=0; dim<NVARS; dim++){
  RooArgSet llSet;
  cout<<"FINAL FIT NUMBER " << NVARS << endl;    
  // fill arg set with a
  for(size_t i=0; i<NVARS;i++){ llSet.add( *(likelihoodVector[i]) ); }
  //  for(int i=0; i<likelihoodVector.size();i++){ llSet.add( *(likelihoodVector[i]) ); }
  
  //create a rooAddition
  RooAddition *    combll = new RooAddition("combll","combll",llSet);
  
  //minimize
  RooMinuit minuit(*combll);
  minuit.setVerbose(false);
  minuit.setErrorLevel(0.5); //otherwise RooFit seems to assume the chi^2 default
  minuit.hesse();
  minuit.migrad();
  minuit.setErrorLevel(0.5);
  minuit.hesse();
  minuit.minos();
  minuit.save();
  RooFitResult*    myRes = minuit.save();
  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;
  cout<<      "********************** IDENTIFIER: " << identifier << "**************************" << endl;
  cout<<endl<<"FINAL FIT RESULTS"<<endl<<endl;
  myRes->Print("v");//myRes->printToStream(resultsFile);
  cout<<endl<<"*******************************************************************************"<<endl;
  cout<<      "*******************************************************************************"<<endl;
  
  canvas_->Clear();
  RooPlot* contourPlot;
  
  if(includeSignal){
    contourPlot = minuit.contour( *ddbkgVar_ , *sigVar_,1,2,3);
    contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
    contourPlot->GetYaxis()->SetRangeUser(0,600);
  }
  else{
    if(standaloneTTbar){
      contourPlot = minuit.contour( *ddbkgVar_ , *ttbarmcbkgVar_,1,2,3);
      contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
      contourPlot->GetYaxis()->SetRangeUser(0,400);
    }
    else{
      contourPlot = minuit.contour( *ddbkgVar_ , *mcbkgVar_,1,2,3);
      contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
      contourPlot->GetYaxis()->SetRangeUser(0,200);
    }
  }
  contourPlot->SetTitle("");
  contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
  contourPlot->GetXaxis()->SetRangeUser(0,400);
  contourPlot->Draw();
  
  
  std::ostringstream oss;
  oss << NVARS;//dim;
  
  string contourPlotName =  string("contour_final_")+baseIdentifier+oss.str()+string("vars");
  canvas_->SaveAs((outFolder+contourPlotName+string(".pdf")).c_str());
  canvas_->SaveAs((outFolder+contourPlotName+string(".png")).c_str());
  ///////////////////////////////////////////////////////////
  
  delete canvas_;
  delete signalTree_;
  delete ddBkgTree_;
  delete ttbarmcBkgTree_;
  delete mcBkgTree_;
  delete dataTree_;
  delete likelihoodVector;
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
  delete myvar;
  delete myvar_weights;
  delete mySignalDS ;
  delete myDDBkgDS  ;
  delete myTTBARMCBkgDS;
  delete myMCBkgDS  ; 
  delete myDataDS   ; 
  delete signalHisto    ;
  delete ttbarmcbkgHisto;
  delete mcbkgHisto     ;
  delete ddbkgHisto     ;
  delete dataHisto      ;
  delete b_signalModel;
  delete b_ddbkgModel;
  delete b_ttbarmcbkgModel;  
  delete b_mcbkgModel;  
  delete u_signalModel; 
  delete u_ddbkgModel;  
  delete u_ttbarmcbkgModel;  
  delete u_mcbkgModel;  
  delete signalConstraint;
  delete ttbarmcbkgConstraint;
  delete ddbkgConstraint;
  delete mcbkgConstraint;
  delete sumModel;
  delete model;
  delete constrainedModelFit;
  delete myFrame;
  delete nll;
  delete myTempRes;
  delete contourPlot;
  delete combll;
  delete myRes;
  delete contourPlot;
  
  
  //}
  //  cout.rdbuf(old); // restore   
}
