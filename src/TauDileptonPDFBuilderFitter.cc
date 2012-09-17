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
  fitVars.clear();
  vars_.clear();
  mins_.clear(); 
  maxs_.clear();
  bins_.clear();
  hmin_.clear();
  hmax_.clear();
  unbinned_.clear(); 
  smoothOrder_.clear();
  
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
  for(size_t i=0; i<vars_.size(); i++)
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
  
  cout << "Init process complete" << endl;
}

void TauDileptonPDFBuilderFitter::SetOptions(){
  gStyle->SetOptStat(0);
}

void TauDileptonPDFBuilderFitter::SetFitSettings(){
  baseIdentifier_="";
  
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

void buildGenericMultiPDFModel( 
			       vector<string> & distVector,
			       vector<double> & minVector,
			       vector<double> & maxVector,
			       vector<double> & numbBinsVector,
			       vector<double> & hminVector,
			       vector<double> & hmaxVector,
			       vector<Int_t> & unbinned,
			       vector<Int_t> & smoothOrder,
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



  
  
  
 
  
  //
  //  std::streambuf * old = std::cout.rdbuf(resultsFile.rdbuf());
  //  // do here output to std::cout
  
  //vector with the likelihoods
  RooNLLVar *  likelihoodVector[NVARS];
  // global variables // must define them in each subroutine, now
  RooRealVar* sigVar_             = 0;              
  RooRealVar* sigMeanVar_         = 0;              
  RooRealVar* sigSigmaVar_        = 0;              
  RooRealVar* ttbarmcbkgVar_      = 0;
  RooRealVar* ttbarmcbkgMeanVar_  = 0;
  RooRealVar* ttbarmcbkgSigmaVar_ = 0;
  RooRealVar* mcbkgVar_           = 0;               
  RooRealVar* mcbkgMeanVar_       = 0;         
  RooRealVar* mcbkgSigmaVar_      = 0;         
  RooRealVar* ddbkgVar_           = 0;              
  RooRealVar* ddbkgMeanVar_       = 0;         
  RooRealVar* ddbkgSigmaVar_      = 0;        
  ////////////////////
  
  for(size_t i = 0; i< NVARS; i++){

    string identifier = "";
    switch(unbinned[i]){
    case 1 : // Unbinned
      identifier=baseIdentifier+string("_unbinned_");
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      identifier=baseIdentifier+string("_binned_");
      break;
    default : // Dummy - should never arrive here
      cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
      gApplication->Terminate(1);
    }
    
    string dist = distVector[i];
    int numbBins = (int) numbBinsVector[i];
    //    numbBins = numbBins*4;
    double min  = minVector[i];// - 4* maxVector[i];
    double max  = maxVector[i];
    double hMin = hminVector[i];
    double hMax = 2* hmaxVector[i];
    
    // define binned variable ////////////////////////////////////////////////////////////////////////////////
    RooRealVar * myvar           = new RooRealVar(dist.c_str(),dist.c_str(),min,max);  myvar->setBins(numbBins); 
    RooRealVar * myvar_weights   = new RooRealVar("weight","weight",0,1000);
    RooRealVar * isOSvar         = new RooRealVar("is_os","is_os",0,2);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    string mySignalDSName = dist + string("_mySignalDS");
    string myDDBkgDSName  = dist + string("_myDDBkgDS");
    string myTTBARMCBkgDSName  = dist + string("_myTTBARMCBkgDS");
    string myMCBkgDSName  = dist + string("_myMCBkgDS");
    string myDataDSName   = dist + string("_myDataDS");
    RooDataSet * mySignalDS ; //= new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
    RooDataSet * RmyDDBkgDS  ; //= new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
    RooDataSet * RmyTTBARMCBkgDS  ; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
    RooDataSet * RmyMCBkgDS  ; //= new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
    RooDataSet * RmyDataDS   ; //= new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   *myvar );
    
    if(includeSignal){
      //  mySignalDS     = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
      mySignalDS         = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(),              RooArgSet(*myvar,*myvar_weights), "weight"); // This constructor does not accept the cut parameter
      double myVarAllocator, myVarWeightAllocator;
      double isOSsig;
      double fhh(0.05*0.05) , fhw( 2*(1-0.05)*0.05) ;      

      double sumWeights(0);

      // WH      
      signalTreeWH_->SetBranchAddress(dist.c_str(), &myVarAllocator);
      signalTreeWH_->SetBranchAddress("weight", &myVarWeightAllocator);
      signalTreeWH_->SetBranchAddress("is_os", &isOSsig);
      cout << "getentries " << signalTreeWH_->GetEntries() << endl;
      for(unsigned int ev=0; ev<signalTreeWH_->GetEntries(); ev++){
	cout << "ev: " << ev << endl;
	signalTreeWH_->GetEntry(ev);
	cout << "isOSsig: " << isOSsig << endl;//	if(isOSsig < 0.5) continue;
	if(isOSsig==0) cout << "peggio" << endl;
	myvar->setVal(myVarAllocator);
	if(dist == "rc_t" && myVarAllocator > 1) cout<< "myVar: " << myVarAllocator<<endl;
	sumWeights += myVarWeightAllocator;
	cout << " arrive here" ;
	myvar_weights->setVal(fhw*myVarWeightAllocator);
	mySignalDS->add(RooArgSet(*myvar,*myvar_weights),fhw*myVarWeightAllocator);
      }
      cout << " strt HH" << endl;
      //HH
      signalTreeHH_->SetBranchAddress(dist.c_str(), &myVarAllocator);
      cout << "getAllocator      ";
      signalTreeHH_->SetBranchAddress("weight", &myVarWeightAllocator);
      cout << "getWeightAllocator      ";
      signalTreeHH_->SetBranchAddress("is_os", &isOSsig);
      cout << "getIsoS      ";
      for(unsigned int ev=0; ev<signalTreeHH_->GetEntries(); ev++){
	signalTreeHH_->GetEntry(ev);
	if(isOSsig < 0.5) continue;
	myvar->setVal(myVarAllocator);
	if(dist == "rc_t" && myVarAllocator > 1) cout<< "myVar: " << myVarAllocator<<endl;
	sumWeights += myVarWeightAllocator;
	myvar_weights->setVal(fhh*myVarWeightAllocator);
	mySignalDS->add(RooArgSet(*myvar,*myvar_weights),fhh*myVarWeightAllocator);
      }
      
      
      //  higgsH->Add(wh,0.095);
      //  higgsH->Add(hh,0.025);
      
      //    : psilva 1.1 for(int i=0; i<em.nEvents; i++)
      //	86 : {
      //	87 : psilva 1.5 recoMass->setVal( em.evMasses[i] );
      //	88 :     category->setVal( em.evCategories[i] );
      //	89 :     inclusiveData->add(RooArgSet(*recoMass,*category));
      //	90 : psilva 1.1 }
//
      cout<<"Signal ok"<<endl;
    }
    string myOsCut = "is_os>0.5";
    RmyDDBkgDS      = new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights,*isOSvar),myOsCut.c_str(),"weight" );
    RooDataSet* myDDBkgDS = (RooDataSet*) RmyDDBkgDS.reduce(RooArgSet(*myvar,*myvar_weights));
    cout<<"DDBkg ok"<<endl;
    if(standaloneTTbar){
      RmyTTBARMCBkgDS = new RooDataSet(myTTBARMCBkgDSName.c_str(), myTTBARMCBkgDSName.c_str(),  ttbarmcBkgTree_,  RooArgSet(*myvar,*myvar_weights,*isOSvar),myOsCut.c_str(),"weight" );
      RooDataSet* myTTBARMCBkgDS = (RooDataSet*) RmyTTBARMCBkgDS.reduce(RooArgSet(*myvar,*myvar_weights));
    }
    cout<<"TTbar ok"<<endl;
    RmyMCBkgDS      = new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights,*isOSvar),myOsCut.c_str(),"weight" );
    RooDataSet* myMCBkgDS = (RooDataSet*) RmyMCBkgDS.reduce(RooArgSet(*myvar,*myvar_weights));
    cout<<"MCBkg ok"<<endl;
    RmyDataDS       = new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   RooArgSet(*myvar,*isOSvar), myOsCut.c_str() );
    RooDataSet* myDataDS = (RooDataSet*) RmyDataDS.reduce(RooArgSet(*myvar,*myvar_weights));
    cout<<"Data ok"<<endl;
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Model Names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    string signalModelName      = dist + string("_SignalModel");
    string signalConstrainedName = dist + string("_signalConstrained");   string signalConstraintName =dist+string("_signalConstraint");
    string ddbkgModelName       = dist + string("_ddbkgModel");  
    string ddbkgConstrainedName = dist + string("_ddbkgConstrained");   string ddbkgConstraintName =dist+string("_ddbkgConstraint");
    string ttbarmcbkgModelName       = dist + string("_ttbarmcbkgModel");  
    string ttbarmcbkgConstrainedName = dist + string("_ttbarmcbkgConstrained");   string ttbarmcbkgConstraintName =dist+string("_ttbarmcbkgConstraint");
    string mcbkgModelName       = dist + string("_mcbkgModel");  
    string mcbkgConstrainedName = dist + string("_mcbkgConstrained");   string mcbkgConstraintName =dist+string("_mcbkgConstraint");
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //Var names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    string signalVarName     =dist+string("_signalVar");string signalMeanVarName  =dist+string("_signalMeanVar"); string signalSigmaVarName =dist+string("_signalSigmaVar");
    string ddbkgVarName      =dist+string("_ddbkgVar"); string ddbkgMeanVarName  =dist+string("_ddbkgMeanVar"); string ddbkgSigmaVarName =dist+string("_ddbkgSigmaVar");
    string ttbarmcbkgVarName      =dist+string("_ttbarmcbkgVar"); string ttbarmcbkgMeanVarName  =dist+string("_ttbarmcbkgMeanVar"); string ttbarmcbkgSigmaVarName =dist+string("_ttbarmcbkgSigmaVar");
    string mcbkgVarName      =dist+string("_mcbkgVar"); string mcbkgMeanVarName  =dist+string("_mcbkgMeanVar"); string mcbkgSigmaVarName =dist+string("_mcbkgSigmaVar");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    
    //Generate histograms and Models ///////////////////////////////////////////////////////////////////////////////////////////
    RooDataHist * signalHisto    ; //= mySignalDS->binnedClone(); 
    RooDataHist * ttbarmcbkgHisto;
    RooDataHist * mcbkgHisto     ; //= myMCBkgDS ->binnedClone();
    RooDataHist * ddbkgHisto     ; //= myDDBkgDS ->binnedClone();
    RooDataHist * dataHisto      ; //= myDataDS  ->binnedClone();
    
    if(includeSignal)
      signalHisto = mySignalDS->binnedClone();
    cout << "Done binned clone for signal" << endl;
    ddbkgHisto  = myDDBkgDS ->binnedClone();
    if(standaloneTTbar)
      ttbarmcbkgHisto  = myTTBARMCBkgDS ->binnedClone(); 
    mcbkgHisto  = myMCBkgDS ->binnedClone();
    dataHisto   = myDataDS  ->binnedClone();
    


    // Binned
    RooHistPdf * b_signalModel;
    RooHistPdf * b_ddbkgModel;
    RooHistPdf * b_ttbarmcbkgModel;  
    RooHistPdf * b_mcbkgModel;  
    
			       
    // Unbinned	       
    RooKeysPdf * u_signalModel; 
    RooKeysPdf * u_ddbkgModel;  
    RooKeysPdf * u_ttbarmcbkgModel;  
    RooKeysPdf * u_mcbkgModel;  
    
    
    switch(unbinned[i]){
    case 1 : // Unbinned
      if(includeSignal) u_signalModel = new RooKeysPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *mySignalDS, RooKeysPdf::MirrorBoth,2);
      u_ddbkgModel  = new RooKeysPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(),  *myvar, *myDDBkgDS , RooKeysPdf::MirrorBoth,2);
      if(standaloneTTbar) u_ttbarmcbkgModel  = new RooKeysPdf(ttbarmcbkgModelName.c_str(),  ttbarmcbkgModelName.c_str(),  *myvar, *myTTBARMCBkgDS , RooKeysPdf::MirrorBoth,2);
      u_mcbkgModel  = new RooKeysPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *myMCBkgDS , RooKeysPdf::MirrorBoth,2);
      
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      if(includeSignal) b_signalModel = new RooHistPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *signalHisto , smoothOrder[i] );
      b_ddbkgModel  = new RooHistPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(), *myvar, *ddbkgHisto  , smoothOrder[i] );
      if(standaloneTTbar) b_ttbarmcbkgModel = new RooHistPdf(ttbarmcbkgModelName.c_str(),  ttbarmcbkgModelName.c_str(),  *myvar, *ttbarmcbkgHisto  , smoothOrder[i] ); 
      b_mcbkgModel  = new RooHistPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *mcbkgHisto  , smoothOrder[i] ); 
      break;
    default : // Dummy - should never arrive here
       cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
       gApplication->Terminate(1);
    }
    
    //signal histogram ////////////////////////////////////////////////////////////
    TH1 * signalHist;
    if(includeSignal){
      signalHist = signalHisto->createHistogram(dist.c_str(),numbBins );
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
    TH1 * ddbkgHist = ddbkgHisto->createHistogram(dist.c_str(),numbBins );
    ddbkgHist->SetLineColor(kRed);
    ddbkgHist->SetFillColor(kRed);
    ddbkgHist->SetLineWidth(3);
    ddbkgHist->SetFillStyle(3017);
    /////////////////////////////////////////////////////////////////////

    // ttbar mc bkg histogram ////////////////////////////////////////////////
    TH1 * ttbarmcbkgHist;
    if(standaloneTTbar){
      ttbarmcbkgHist = ttbarmcbkgHisto->createHistogram(dist.c_str(),numbBins );   
      ttbarmcbkgHist->SetLineColor(kYellow);
      ttbarmcbkgHist->SetFillColor(kYellow);
      ttbarmcbkgHist->SetLineWidth(3);
      ttbarmcbkgHist->SetFillStyle(3017);
    }
    ///////////////////////////////////////////////////////////////////
    
    // mc bkg histogram ////////////////////////////////////////////////
    TH1 * mcbkgHist = mcbkgHisto->createHistogram(dist.c_str(),numbBins );   
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
      if(hMax){ ttbarmcbkgHist->SetMaximum(hMax); ttbarmcbkgHist->SetMinimum(hMin);} 
      ttbarmcbkgHist->DrawNormalized("hist");
      mcbkgHist->DrawNormalized("histsame");    
    }
    else {
      if(hMax){ mcbkgHist->SetMaximum(hMax); mcbkgHist->SetMinimum(hMin);} 
      mcbkgHist->DrawNormalized("hist");
    }
    
    if(includeSignal)
      signalHist->DrawNormalized("histsame");
    ddbkgHist->DrawNormalized("histsame");
    
    if(standaloneTTbar) ttbarmcbkgHist->DrawNormalized("histsame"); // in order for it to be on top and thus viewable for discriminating it from higgs in rc_t plots
    
    

    
    
    leg->Draw();
    string shapes= string("shapes_")+identifier+dist;
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
    cout<<endl<<" PDF data set Entries for distribution : "<<dist;    
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
    ///resultsFile<<"Variable " << dist<<endl;
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
    string sumModelName = dist+string("_sumModel");
    
    //    string sumModelExp  = signalModelName+string("+")+ddbkgModelName+string("+")+mcbkgModelName;
    string sumModelExp = "";
    if(includeSignal) sumModelExp.append(signalModelName+string("+"));
    sumModelExp.append(ddbkgModelName+string("+"));
    if(standaloneTTbar) sumModelExp.append(ttbarmcbkgModelName+string("+"));
    sumModelExp.append(mcbkgModelName);
        
    
    //    RooAddPdf * sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *signalModel, *ddbkgModel, *mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
    RooAddPdf* sumModel;
    string sumModelConstrainedName = dist+string("_sumConstrainedModel");
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
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel, *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
      } else{
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel, *u_ttbarmcbkgModel, *u_mcbkgModel ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *myDataDS, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
      }
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      if(includeSignal){
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_ttbarmcbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *signalConstraint, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
      } else{
	if(standaloneTTbar){
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel, *b_ttbarmcbkgModel, *b_mcbkgModel ), RooArgList(*ddbkgVar_, *ttbarmcbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *ttbarmcbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
	else{
	  sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *ddbkgVar_, *mcbkgVar_) );
	  model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstraint, *mcbkgConstraint));
	  constrainedModelFit = model->fitTo( *dataHisto, Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
	}
      }
      break;
    default : // Dummy - should never arrive here
      cout<<"Neither binned not unbinned. Check your options motherfucker."<<endl;
      gApplication->Terminate(1);
    }
    
    
    


    gStyle->SetOptStat(0);
    string plotWithModelFit   = string("modelFit_"+identifier)+dist;
    
    canvas_->cd();  
    canvas_->Clear();  
    RooPlot * myFrame = myvar->frame();
    myFrame->SetTitle("");
    myFrame->GetXaxis()->SetTitle(dist.c_str());
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
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(min,max));
	else nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(min,max));
      else
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(min,max));
	else nll = (RooNLLVar *) model->createNLL( *myDataDS, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
      break;
    case 0:  // Binned (w/ or w/out smoothing)
      if(includeSignal)
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(min,max));
	else nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*sigVar_,*ddbkgVar_,*mcbkgVar_)), Range(min,max));
      else
	if(standaloneTTbar) nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*ttbarmcbkgVar_,*mcbkgVar_)), Range(min,max));
	else nll = (RooNLLVar *) model->createNLL( *dataHisto, RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
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
    cout<<endl<<"FIT RESULTS for variable"<<dist<<endl<<endl;
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
    string contourPlotName =  string("contour_")+identifier+dist;
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///void build3ComponentsMultiPDFModel( 
///				   vector<string> & distVector,
///				   vector<double> & minVector,
///				   vector<double> & maxVector,
///				   vector<double> & numbBinsVector,
///				   vector<double> & hminVector,
///				   vector<double> & hmaxVector,
///				   vector<Int_t> & unbinned,
///				   vector<Int_t> & smoothOrder,
///				   TTree* signalTree_, 
///				   TTree* ddBkgTree_ ,
///				   TTree* mcBkgTree_,
///				   TTree* dataTree_,
///				   TCanvas* canvas_
///				   ){
///  
///  ofstream resultsFile;
///  resultsFile.open ("likelihoodResults.txt");
/////
/////  std::streambuf * old = std::cout.rdbuf(resultsFile.rdbuf());
/////  // do here output to std::cout
///
///  
///  //vector with the likelihoods
///  RooNLLVar *  likelihoodVector[NVARS];
///  // global variables // must define them in each subroutine, now
///  RooRealVar* sigVar_         = 0;              
///  RooRealVar* mcbkgVar_       = 0;               
///  RooRealVar* mcbkgMeanVar_   = 0;         
///  RooRealVar* mcbkgSigmaVar_  = 0;         
///  RooRealVar* ddbkgVar_       = 0;              
///  RooRealVar* ddbkgMeanVar_   = 0;         
///  RooRealVar* ddbkgSigmaVar_  = 0;        
///  ////////////////////
///  
///  for(size_t i = 0; i< NVARS; i++){
///
///    string dist = distVector[i];
///    int numbBins = (int) numbBinsVector[i];
///    //    numbBins = numbBins*4;
///    double min  = minVector[i];// - 4* maxVector[i];
///    double max  = maxVector[i];
///    double hMin = hminVector[i];
///    double hMax = 2* hmaxVector[i];
///   
///    // define binned variable ////////////////////////////////////////////////////////////////////////////////
///    RooRealVar * myvar           = new RooRealVar(dist.c_str(),dist.c_str(),min,max);  myvar->setBins(numbBins); 
///    RooRealVar * myvar_weights   = new RooRealVar("weight","weight",0,1000);
///    //////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string mySignalDSName = dist + string("_mySignalDS");
///    string myDDBkgDSName  = dist + string("_myDDBkgDS");
///    string myMCBkgDSName  = dist + string("_myMCBkgDS");
///    string myDataDSName   = dist + string("_myDataDS");
///    RooDataSet * mySignalDS = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myDDBkgDS  = new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myMCBkgDS  = new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myDataDS   = new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   *myvar );
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    // Model Names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string signalModelName      = dist + string("_SignalModel");
///    string mcbkgModelName       = dist + string("_mcbkgModel");  
///    string mcbkgConstrainedName = dist + string("_mcbkgConstrained");   string mcbkgConstrainName =dist+string("_mcbkgConstrain");
///    string ddbkgModelName       = dist + string("_ddbkgModel");  
///    string ddbkgConstrainedName = dist + string("_ddbkgConstrained");   string ddbkgConstrainName =dist+string("_ddbkgConstrain");
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    //Var names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string signalVarName     =dist+string("_signalVar");
///    string mcbkgVarName      =dist+string("_mcbkgVar"); string mcbkgMeanVarName  =dist+string("_mcbkgMeanVar"); string mcbkgSigmaVarName =dist+string("_mcbkgSigmaVar");
///    string ddbkgVarName      =dist+string("_ddbkgVar"); string ddbkgMeanVarName  =dist+string("_ddbkgMeanVar"); string ddbkgSigmaVarName =dist+string("_ddbkgSigmaVar");
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    //Generate histograms and Models ///////////////////////////////////////////////////////////////////////////////////////////
///    RooDataHist * signalHisto = mySignalDS->binnedClone(); 
///    RooDataHist * mcbkgHisto  = myMCBkgDS ->binnedClone();
///    RooDataHist * ddbkgHisto  = myDDBkgDS ->binnedClone();
///    RooDataHist * dataHisto   = myDataDS  ->binnedClone();
///
///
///    // Binned
///    RooHistPdf * b_signalModel;
///    RooHistPdf * b_ddbkgModel;
///    RooHistPdf * b_mcbkgModel;  
///			       
///    // Unbinned	       
///    RooKeysPdf * u_signalModel; 
///    RooKeysPdf * u_ddbkgModel;  
///    RooKeysPdf * u_mcbkgModel;  
///    
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      u_signalModel = new RooKeysPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *mySignalDS, RooKeysPdf::MirrorBoth,2);
///      u_ddbkgModel  = new RooKeysPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(),  *myvar, *myDDBkgDS , RooKeysPdf::MirrorBoth,2);
///      u_mcbkgModel  = new RooKeysPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *myMCBkgDS , RooKeysPdf::MirrorBoth,2);
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      b_signalModel = new RooHistPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *signalHisto , smoothOrder[i] );
///      b_ddbkgModel  = new RooHistPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(),  *myvar, *ddbkgHisto  , smoothOrder[i] );
///      b_mcbkgModel  = new RooHistPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *mcbkgHisto  , smoothOrder[i] ); 
///      break;
///    default : // Dummy - should never arrive here
///      b_signalModel = 0;
///      b_ddbkgModel  = 0;
///      b_mcbkgModel  = 0;  
///      u_signalModel = 0; 
///      u_ddbkgModel  = 0;  
///      u_mcbkgModel  = 0;  
///    }
///
///    //signal histogram ////////////////////////////////////////////////////////////
///    TH1 * signalHist = signalHisto->createHistogram(dist.c_str(),numbBins );
///    signalHist->SetOption("0000");
///    signalHist->SetLineWidth(3);
///    signalHist->SetTitle("");
///    signalHist->GetYaxis()->SetTitle("a.u.");
///    signalHist->GetYaxis()->SetTitleOffset(1.5);
///    signalHist->SetLineColor(kGreen);
///    signalHist->SetFillColor(kGreen);
///    if(hMax){ signalHist->SetMaximum(hMax); signalHist->SetMinimum(hMin);}
///    ///////////////////////////////////////////////////////////////////////////////
///    
///    
///    // dd bkg histogram /////////////////////////////////////////////////
///    TH1 * ddbkgHist = ddbkgHisto->createHistogram(dist.c_str(),numbBins );
///    ddbkgHist->SetLineColor(kRed);
///    ddbkgHist->SetFillColor(kRed);
///    ddbkgHist->SetLineWidth(3);
///    ddbkgHist->SetFillStyle(3017);
///    /////////////////////////////////////////////////////////////////////
///
///
///    // mc bkg histogram ////////////////////////////////////////////////
///    TH1 * mcbkgHist = mcbkgHisto->createHistogram(dist.c_str(),numbBins );   
///    mcbkgHist->SetLineColor(kBlack);
///    mcbkgHist->SetFillColor(kBlack);
///    mcbkgHist->SetLineWidth(3);
///    mcbkgHist->SetFillStyle(3017);
///    ///////////////////////////////////////////////////////////////////
///
///    TLegend *leg = new TLegend(0.3,0.665,0.85,0.86,NULL,"brNDC");
///    leg->SetTextFont(62);
///    leg->SetBorderSize(0);
///    leg->SetLineColor(1);
///    leg->SetLineStyle(1);
///    leg->SetLineWidth(1);
///    leg->SetFillColor(0);
///    leg->SetFillStyle(1001);
///    leg->AddEntry(signalHist,"irr bkg template","f");
///    leg->AddEntry(ddbkgHist,"dd bkg template","f");
///    leg->AddEntry(mcbkgHist,"mc bkg template","f");
///
///    canvas_->cd(); 
///    signalHist->DrawNormalized("hist");
///    ddbkgHist->DrawNormalized("histsame");
///    mcbkgHist->DrawNormalized("histsame");
///    leg->Draw();
///    string shapes= string("shapes_binned_")+dist+string(".pdf");
///    canvas_->SaveAs(shapes.c_str());
///    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    // Get total entries from Histograms ////////////////////////////////////////////////
///    cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    double sig_N   = mySignalDS->numEntries(); double nsig   = signalHisto->sum(kFALSE); 
///    double ddbkg_N = myDDBkgDS->numEntries();  double nddbkg = ddbkgHisto->sum(kFALSE);
///    double mcbkg_N = myMCBkgDS->numEntries();  double nmcbkg = mcbkgHisto->sum(kFALSE); 
///    double data_N  = myDataDS->numEntries();   
///   
//////    switch(unbinned[i]){
//////    case 1 : // Unbinned
//////      nsig   = mySignalDS->numEntries();
//////      nddbkg = myDDBkgDS ->numEntries(); 
//////      nmcbkg = myMCBkgDS ->numEntries(); 
//////      break;
//////    case 0:  // Binned (w/ or w/out smoothing)
//////      nsig   = signalHisto->sum(kFALSE);
//////      nddbkg = ddbkgHisto ->sum(kFALSE); 
//////      nmcbkg = mcbkgHisto ->sum(kFALSE); 
//////      break;
//////    default : // Dummy - should never arrive here
//////      nsig   = 0;
//////      nddbkg = 0;
//////      nmcbkg = 0;
//////    }
///
///    /////////////////////////////////////////////////////////////////////////////////////
///    cout<<endl<<" PDF data set Entries for distribution : "<<dist;    
///    cout<<endl<<"WARNING: values taken just from unbinned datasets"<<endl;
///    cout<<endl<<" sig_N    = "<<sig_N<<  " , root entries : "<<signalTree_->GetEntries()<<" weights : "<<nsig;
///    cout<<endl<<" ddbkg_N  = "<<ddbkg_N<<" , root entries : "<<ddBkgTree_->GetEntries()<<" weights : "<<nddbkg;
///    cout<<endl<<" mcbkg_N  = "<<mcbkg_N<<" , root entries : "<<mcBkgTree_->GetEntries()<<" weights : "<<nmcbkg;
///    cout<<endl<<" data_N   = "<<data_N<< " , root entries : "<<dataTree_->GetEntries()<<endl;
///    cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    /////////////////////////////////////////////////////////////////////////////////////
///    
///    
///    
///    
///    // Building the constrained models for signal mc bkg ///////////////////////////////////////////////////////////////////////////////////////////////
///
///    double nmcbkgMean(nmcbkg); double nmcbkgSigma(1.76);//nmcbkg*0.10);//1.76);// double nmcbkgSigma(nmcbkg*0.10);
///    //    double nddbkgMean(nddbkg); double nddbkgSigma(nddbkg*0.14);
///    double nddbkgMean(165.4); double nddbkgSigma(23.7);
///
///    resultsFile<<" ******************************************************************************************* "<<endl;
///    resultsFile<<"Variable " << dist<<endl;
///    resultsFile<<endl<<"Constraints to background number of events"<<endl;
///    double integral, integralError; integral = mcbkgHist->IntegralAndError(0,mcbkgHist->GetNbinsX()-1,integralError);
///    resultsFile<<"mcbkg: "<<nmcbkgMean<<"+/-"<<nmcbkgSigma<<", est from hist: " << integral << "+/-" << integralError<<endl;
///    resultsFile<<"ddbkg: "<<nddbkgMean<<"+/-"<<nddbkgSigma<<endl;
///    resultsFile<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    
///    if( ! sigVar_         ) sigVar_         = new RooRealVar( "globalSignalVarName",       "globalSignalVarName",         nsig,   0, nsig*2.5);
///    else{ sigVar_->setMin("",0);   sigVar_->setMax("",nsig*3.5);     sigVar_->setVal(nsig); }
///    
///    if( ! mcbkgVar_       ) mcbkgVar_       = new RooRealVar( "globalMcBkgVarName",        "globalMcBkgVarName",          nmcbkg, 0, nmcbkg*1.5); 
///    else{ mcbkgVar_->setMin("",0); mcbkgVar_->setMax("",nmcbkg*3.5); mcbkgVar_->setVal(nmcbkg); }
///    
///    if( ! mcbkgMeanVar_   ) mcbkgMeanVar_   = new RooRealVar( "globalMcBkgMeanVarName",    "globalMcBkgMeanVarName",      nmcbkgMean); 
///    else{ mcbkgMeanVar_->setVal(nmcbkgMean);   }
///    
///    if( ! mcbkgSigmaVar_  ) mcbkgSigmaVar_  = new RooRealVar( "globalMcBkgSigmaVarName",   "globalMcBkgSigmaVarName",     nmcbkgSigma ); 
///    else{ mcbkgSigmaVar_->setVal(nmcbkgSigma); }
///    
///    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkgMean, 0, nddbkgMean*1.5); 
///    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkgMean*3.5);     ddbkgVar_->setVal(nddbkgMean);}
///
/////    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkg, 0, nddbkg*1.5); 
/////    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkg*3.5);     ddbkgVar_->setVal(nddbkg);}
///    
///    
///    if(! ddbkgMeanVar_)     ddbkgMeanVar_   = new RooRealVar( "globalDDBkgMeanVarName", "globalDDBkgMeanVarName",         nddbkgMean); 
///    else{ ddbkgMeanVar_->setVal(nddbkgMean);   }
///    
///    if(! ddbkgSigmaVar_)    ddbkgSigmaVar_  = new RooRealVar( "globalDDBkgSigmaVarName","globalDDBkgSigmaVarName",        nddbkgSigma);  
///    else{ ddbkgSigmaVar_->setVal(nddbkgSigma);   }
///    
///    RooGaussian * mcbkgConstrain = new RooGaussian( mcbkgConstrainName.c_str(), mcbkgConstrainName.c_str(), *mcbkgVar_,*mcbkgMeanVar_, *mcbkgSigmaVar_);
///    RooGaussian * ddbkgConstrain = new RooGaussian( ddbkgConstrainName.c_str(), ddbkgConstrainName.c_str(), *ddbkgVar_, *ddbkgMeanVar_, *ddbkgSigmaVar_);
///    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    
///    
///    
///    
///    // build the sum model and model with constrains ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string sumModelName = dist+string("_sumModel");
///    string sumModelExp  = signalModelName+string("+")+ddbkgModelName+string("+")+mcbkgModelName;
///    //    RooAddPdf * sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *signalModel, *ddbkgModel, *mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///    RooAddPdf* sumModel;
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel, *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///      break;
///    default : // Dummy - should never arrive here
///      sumModel=0;
///    }
///    
///    
///    string sumModelConstrainedName = dist+string("_sumConstrainedModel");
///    string sumModelConstrainedExp  = sumModelName+string("*")+ddbkgConstrainName+string("*")+mcbkgConstrainName;
///    RooProdPdf * model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstrain, *mcbkgConstrain));
///
///    //Fit model to data
///    //    RooFitResult * constrainedModelFit = model->fitTo( 
///    //						      *dataHisto, 
///    //						      //						      *myDataDS,
///    //      Minos(), 
///    //      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
///    //      Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max) 
///    //    );
///    
///    RooFitResult * constrainedModelFit;
///
///    cout<<"Start fitting variable"<<endl;
///    switch(unbinned[i]){
///    case 1: // Unbinned
///      constrainedModelFit = model->fitTo( *myDataDS, 
///					  Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      constrainedModelFit = model->fitTo( *dataHisto, 
///					  Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
///      break;
///    default : // Dummy - should never arrive here
///      constrainedModelFit = 0;
///    }
///    
///
///    gStyle->SetOptStat(0);
///    string plotWithModelFit   = string("plot_binned_modelFit_")+dist+string(".pdf");
///    
///    canvas_->cd();  
///    canvas_->Clear();  
///    RooPlot * myFrame = myvar->frame();
///    myFrame->SetTitle("");
///    myFrame->GetXaxis()->SetTitle(dist.c_str());
///    myFrame->GetYaxis()->SetTitle("Events");
///    dataHisto->plotOn(myFrame);
///    model->plotOn(myFrame);
///    //    model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*signalModel), RooFit::LineColor(kGreen));   
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*u_signalModel), RooFit::LineColor(kGreen));   
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*b_signalModel), RooFit::LineColor(kGreen));   
///      break;
///    default : // Dummy - should never arrive here
///      constrainedModelFit = 0;
///    }
///    
///
///   
///    myFrame->Draw();
///    canvas_->SaveAs(plotWithModelFit.c_str());
///    canvas_->cd();
///    canvas_->Clear();
///
///    // Test on the fit ////////////////////////////////////////////////////////////
/////    RooNLLVar * nll = (RooNLLVar *) model->createNLL(
/////						     *dataHisto,
/////						     //*myDataDS,
/////      RooFit::CloneData(kTRUE),
/////      Extended(kTRUE),
/////      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
/////      Range(min,max)
/////    );
///    
///    RooNLLVar * nll;
///    
///    switch(unbinned[i]){
///    case 1: // Unbinned
///      nll = (RooNLLVar *) model->createNLL( *myDataDS,
///					    RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      nll = (RooNLLVar *) model->createNLL( *dataHisto,
///					    RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
///      break;
///    default : // Dummy - should never arrive here
///      nll = 0;
///    }
///    
///    
///    
///    
///    likelihoodVector[i] = nll; //.push_back(nll); 
///
///    RooMinuit minuit( *nll );
///    minuit.migrad();
///    minuit.setErrorLevel(0.5);
///    minuit.hesse();
///
///    RooFitResult* myTempRes = minuit.save();
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    cout<<endl<<"FIT RESULTS for variable"<<dist<<endl<<endl;
///    myTempRes->Print("v");
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///
///    canvas_->cd();
///    canvas_->Clear();
///    RooPlot * contourPlot= minuit.contour( *ddbkgVar_, *sigVar_,1,2,3);
///    contourPlot->SetTitle("");
///    contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
///    contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
///    contourPlot->GetYaxis()->SetRangeUser(0,600);
///    contourPlot->GetXaxis()->SetRange(0,400);
///    contourPlot->Draw();
///    string contourPlotName =  string("contour_")+dist+string(".pdf");
///    canvas_->SaveAs(contourPlotName.c_str());
///    ///////////////////////////////////////////////////////////
///
///
///  }
///
/////  RooAddition* combll;
/////  RooFitResult* myRes;
/////  RooPlot* contourPlot;
/////  for(int dim=0; dim<NVARS; dim++){
///    RooArgSet llSet;
///    cout<<"FINAL FIT NUMBER " << NVARS << endl;    
///    // fill arg set with a
///    for(size_t i=0; i<NVARS;i++){ llSet.add( *(likelihoodVector[i]) ); }
///    //  for(int i=0; i<likelihoodVector.size();i++){ llSet.add( *(likelihoodVector[i]) ); }
///    
///    //create a rooAddition
///    RooAddition *    combll = new RooAddition("combll","combll",llSet);
///    
///    //minimize
///    RooMinuit minuit(*combll);
///    minuit.setVerbose(false);
///    minuit.setErrorLevel(0.5); //otherwise RooFit seems to assume the chi^2 default
///    minuit.hesse();
///    minuit.migrad();
///    minuit.setErrorLevel(0.5);
///    minuit.hesse();
///    minuit.minos();
///    minuit.save();
///    RooFitResult*    myRes = minuit.save();
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    cout<<endl<<"FINAL FIT RESULTS"<<endl<<endl;
///    myRes->Print("v");//myRes->printToStream(resultsFile);
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    
///    canvas_->Clear();
///    RooPlot *     contourPlot= minuit.contour( *ddbkgVar_ , *sigVar_,1,2,3);
///    contourPlot->SetTitle("");
///    contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
///    contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
///    contourPlot->GetYaxis()->SetRangeUser(0,600);
///    contourPlot->GetXaxis()->SetRangeUser(0,400);
///    contourPlot->Draw();
///    
///    
///    std::ostringstream oss;
///    oss << NVARS;//dim;
///    
///    string contourPlotName =  string("contour_binned_final_")+oss.str()+string(".pdf");
///    canvas_->SaveAs(contourPlotName.c_str());
///    ///////////////////////////////////////////////////////////
///    
///
///    
///    //}
///    //  cout.rdbuf(old); // restore   
///}
///
///
///void build4ComponentsMultiPDFModel( 
///			       vector<string> & distVector,
///			       vector<double> & minVector,
///			       vector<double> & maxVector,
///			       vector<double> & numbBinsVector,
///			       vector<double> & hminVector,
///			       vector<double> & hmaxVector,
///			       vector<Int_t> & unbinned,
///			       vector<Int_t> & smoothOrder,
///			       TTree* signalTree_, 
///			       TTree* ddBkgTree_ ,
///			       TTree* mcBkgTree_,
///			       TTree* dataTree_,
///			       TCanvas* canvas_
///			       ){
///  
///  ofstream resultsFile;
///  resultsFile.open ("likelihoodResults.txt");
/////
/////  std::streambuf * old = std::cout.rdbuf(resultsFile.rdbuf());
/////  // do here output to std::cout
///
///  
///  //vector with the likelihoods
///  RooNLLVar *  likelihoodVector[NVARS];
///  // global variables // must define them in each subroutine, now
///  RooRealVar* sigVar_         = 0;              
///  RooRealVar* mcbkgVar_       = 0;               
///  RooRealVar* mcbkgMeanVar_   = 0;         
///  RooRealVar* mcbkgSigmaVar_  = 0;         
///  RooRealVar* ddbkgVar_       = 0;              
///  RooRealVar* ddbkgMeanVar_   = 0;         
///  RooRealVar* ddbkgSigmaVar_  = 0;        
///  ////////////////////
///  
///  for(size_t i = 0; i< NVARS; i++){
///
///    string dist = distVector[i];
///    int numbBins = (int) numbBinsVector[i];
///    //    numbBins = numbBins*4;
///    double min  = minVector[i];// - 4* maxVector[i];
///    double max  = maxVector[i];
///    double hMin = hminVector[i];
///    double hMax = 2* hmaxVector[i];
///   
///    // define binned variable ////////////////////////////////////////////////////////////////////////////////
///    RooRealVar * myvar           = new RooRealVar(dist.c_str(),dist.c_str(),min,max);  myvar->setBins(numbBins); 
///    RooRealVar * myvar_weights   = new RooRealVar("weight","weight",0,1000);
///    //////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    //Define data sets /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string mySignalDSName = dist + string("_mySignalDS");
///    string myDDBkgDSName  = dist + string("_myDDBkgDS");
///    string myMCBkgDSName  = dist + string("_myMCBkgDS");
///    string myDataDSName   = dist + string("_myDataDS");
///    RooDataSet * mySignalDS = new RooDataSet(mySignalDSName.c_str(),mySignalDSName.c_str(), signalTree_, RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myDDBkgDS  = new RooDataSet(myDDBkgDSName.c_str(), myDDBkgDSName.c_str(),  ddBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myMCBkgDS  = new RooDataSet(myMCBkgDSName.c_str(), myMCBkgDSName.c_str(),  mcBkgTree_,  RooArgSet(*myvar,*myvar_weights),0,"weight" );
///    RooDataSet * myDataDS   = new RooDataSet(myDataDSName.c_str(),  myDataDSName.c_str(),   dataTree_,   *myvar );
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    // Model Names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string signalModelName      = dist + string("_SignalModel");
///    string mcbkgModelName       = dist + string("_mcbkgModel");  
///    string mcbkgConstrainedName = dist + string("_mcbkgConstrained");   string mcbkgConstrainName =dist+string("_mcbkgConstrain");
///    string ddbkgModelName       = dist + string("_ddbkgModel");  
///    string ddbkgConstrainedName = dist + string("_ddbkgConstrained");   string ddbkgConstrainName =dist+string("_ddbkgConstrain");
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    //Var names ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string signalVarName     =dist+string("_signalVar");
///    string mcbkgVarName      =dist+string("_mcbkgVar"); string mcbkgMeanVarName  =dist+string("_mcbkgMeanVar"); string mcbkgSigmaVarName =dist+string("_mcbkgSigmaVar");
///    string ddbkgVarName      =dist+string("_ddbkgVar"); string ddbkgMeanVarName  =dist+string("_ddbkgMeanVar"); string ddbkgSigmaVarName =dist+string("_ddbkgSigmaVar");
///    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    //Generate histograms and Models ///////////////////////////////////////////////////////////////////////////////////////////
///    RooDataHist * signalHisto = mySignalDS->binnedClone(); 
///    RooDataHist * mcbkgHisto  = myMCBkgDS ->binnedClone();
///    RooDataHist * ddbkgHisto  = myDDBkgDS ->binnedClone();
///    RooDataHist * dataHisto   = myDataDS  ->binnedClone();
///
///
///    // Binned
///    RooHistPdf * b_signalModel;
///    RooHistPdf * b_ddbkgModel;
///    RooHistPdf * b_mcbkgModel;  
///			       
///    // Unbinned	       
///    RooKeysPdf * u_signalModel; 
///    RooKeysPdf * u_ddbkgModel;  
///    RooKeysPdf * u_mcbkgModel;  
///    
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      u_signalModel = new RooKeysPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *mySignalDS, RooKeysPdf::MirrorBoth,2);
///      u_ddbkgModel  = new RooKeysPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(),  *myvar, *myDDBkgDS , RooKeysPdf::MirrorBoth,2);
///      u_mcbkgModel  = new RooKeysPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *myMCBkgDS , RooKeysPdf::MirrorBoth,2);
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      b_signalModel = new RooHistPdf(signalModelName.c_str(), signalModelName.c_str(), *myvar, *signalHisto , smoothOrder[i] );
///      b_ddbkgModel  = new RooHistPdf(ddbkgModelName.c_str(),  ddbkgModelName.c_str(),  *myvar, *ddbkgHisto  , smoothOrder[i] );
///      b_mcbkgModel  = new RooHistPdf(mcbkgModelName.c_str(),  mcbkgModelName.c_str(),  *myvar, *mcbkgHisto  , smoothOrder[i] ); 
///      break;
///    default : // Dummy - should never arrive here
///      b_signalModel = 0;
///      b_ddbkgModel  = 0;
///      b_mcbkgModel  = 0;  
///      u_signalModel = 0; 
///      u_ddbkgModel  = 0;  
///      u_mcbkgModel  = 0;  
///    }
///
///    //signal histogram ////////////////////////////////////////////////////////////
///    TH1 * signalHist = signalHisto->createHistogram(dist.c_str(),numbBins );
///    signalHist->SetOption("0000");
///    signalHist->SetLineWidth(3);
///    signalHist->SetTitle("");
///    signalHist->GetYaxis()->SetTitle("a.u.");
///    signalHist->GetYaxis()->SetTitleOffset(1.5);
///    signalHist->SetLineColor(kGreen);
///    signalHist->SetFillColor(kGreen);
///    if(hMax){ signalHist->SetMaximum(hMax); signalHist->SetMinimum(hMin);}
///    ///////////////////////////////////////////////////////////////////////////////
///    
///    
///    // dd bkg histogram /////////////////////////////////////////////////
///    TH1 * ddbkgHist = ddbkgHisto->createHistogram(dist.c_str(),numbBins );
///    ddbkgHist->SetLineColor(kRed);
///    ddbkgHist->SetFillColor(kRed);
///    ddbkgHist->SetLineWidth(3);
///    ddbkgHist->SetFillStyle(3017);
///    /////////////////////////////////////////////////////////////////////
///
///
///    // mc bkg histogram ////////////////////////////////////////////////
///    TH1 * mcbkgHist = mcbkgHisto->createHistogram(dist.c_str(),numbBins );   
///    mcbkgHist->SetLineColor(kBlack);
///    mcbkgHist->SetFillColor(kBlack);
///    mcbkgHist->SetLineWidth(3);
///    mcbkgHist->SetFillStyle(3017);
///    ///////////////////////////////////////////////////////////////////
///
///    TLegend *leg = new TLegend(0.3,0.665,0.85,0.86,NULL,"brNDC");
///    leg->SetTextFont(62);
///    leg->SetBorderSize(0);
///    leg->SetLineColor(1);
///    leg->SetLineStyle(1);
///    leg->SetLineWidth(1);
///    leg->SetFillColor(0);
///    leg->SetFillStyle(1001);
///    leg->AddEntry(signalHist,"irr bkg template","f");
///    leg->AddEntry(ddbkgHist,"dd bkg template","f");
///    leg->AddEntry(mcbkgHist,"mc bkg template","f");
///
///    canvas_->cd(); 
///    signalHist->DrawNormalized("hist");
///    ddbkgHist->DrawNormalized("histsame");
///    mcbkgHist->DrawNormalized("histsame");
///    leg->Draw();
///    string shapes= string("shapes_binned_")+dist+string(".pdf");
///    canvas_->SaveAs(shapes.c_str());
///    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///
///    // Get total entries from Histograms ////////////////////////////////////////////////
///    cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    double sig_N   = mySignalDS->numEntries(); double nsig   = signalHisto->sum(kFALSE); 
///    double ddbkg_N = myDDBkgDS->numEntries();  double nddbkg = ddbkgHisto->sum(kFALSE);
///    double mcbkg_N = myMCBkgDS->numEntries();  double nmcbkg = mcbkgHisto->sum(kFALSE); 
///    double data_N  = myDataDS->numEntries();   
///   
//////    switch(unbinned[i]){
//////    case 1 : // Unbinned
//////      nsig   = mySignalDS->numEntries();
//////      nddbkg = myDDBkgDS ->numEntries(); 
//////      nmcbkg = myMCBkgDS ->numEntries(); 
//////      break;
//////    case 0:  // Binned (w/ or w/out smoothing)
//////      nsig   = signalHisto->sum(kFALSE);
//////      nddbkg = ddbkgHisto ->sum(kFALSE); 
//////      nmcbkg = mcbkgHisto ->sum(kFALSE); 
//////      break;
//////    default : // Dummy - should never arrive here
//////      nsig   = 0;
//////      nddbkg = 0;
//////      nmcbkg = 0;
//////    }
///
///    /////////////////////////////////////////////////////////////////////////////////////
///    cout<<endl<<" PDF data set Entries for distribution : "<<dist;    
///    cout<<endl<<"WARNING: values taken just from unbinned datasets"<<endl;
///    cout<<endl<<" sig_N    = "<<sig_N<<  " , root entries : "<<signalTree_->GetEntries()<<" weights : "<<nsig;
///    cout<<endl<<" ddbkg_N  = "<<ddbkg_N<<" , root entries : "<<ddBkgTree_->GetEntries()<<" weights : "<<nddbkg;
///    cout<<endl<<" mcbkg_N  = "<<mcbkg_N<<" , root entries : "<<mcBkgTree_->GetEntries()<<" weights : "<<nmcbkg;
///    cout<<endl<<" data_N   = "<<data_N<< " , root entries : "<<dataTree_->GetEntries()<<endl;
///    cout<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    /////////////////////////////////////////////////////////////////////////////////////
///    
///    
///    
///    
///    // Building the constrained models for signal mc bkg ///////////////////////////////////////////////////////////////////////////////////////////////
///
///    double nmcbkgMean(nmcbkg); double nmcbkgSigma(1.76);//nmcbkg*0.10);//1.76);// double nmcbkgSigma(nmcbkg*0.10);
///    //    double nddbkgMean(nddbkg); double nddbkgSigma(nddbkg*0.14);
///    double nddbkgMean(165.4); double nddbkgSigma(23.7);
///
///    resultsFile<<" ******************************************************************************************* "<<endl;
///    resultsFile<<"Variable " << dist<<endl;
///    resultsFile<<endl<<"Constraints to background number of events"<<endl;
///    double integral, integralError; integral = mcbkgHist->IntegralAndError(0,mcbkgHist->GetNbinsX()-1,integralError);
///    resultsFile<<"mcbkg: "<<nmcbkgMean<<"+/-"<<nmcbkgSigma<<", est from hist: " << integral << "+/-" << integralError<<endl;
///    resultsFile<<"ddbkg: "<<nddbkgMean<<"+/-"<<nddbkgSigma<<endl;
///    resultsFile<<endl<<endl<<" ******************************************************************************************* "<<endl;
///    
///    if( ! sigVar_         ) sigVar_         = new RooRealVar( "globalSignalVarName",       "globalSignalVarName",         nsig,   0, nsig*2.5);
///    else{ sigVar_->setMin("",0);   sigVar_->setMax("",nsig*3.5);     sigVar_->setVal(nsig); }
///    
///    if( ! mcbkgVar_       ) mcbkgVar_       = new RooRealVar( "globalMcBkgVarName",        "globalMcBkgVarName",          nmcbkg, 0, nmcbkg*1.5); 
///    else{ mcbkgVar_->setMin("",0); mcbkgVar_->setMax("",nmcbkg*3.5); mcbkgVar_->setVal(nmcbkg); }
///    
///    if( ! mcbkgMeanVar_   ) mcbkgMeanVar_   = new RooRealVar( "globalMcBkgMeanVarName",    "globalMcBkgMeanVarName",      nmcbkgMean); 
///    else{ mcbkgMeanVar_->setVal(nmcbkgMean);   }
///    
///    if( ! mcbkgSigmaVar_  ) mcbkgSigmaVar_  = new RooRealVar( "globalMcBkgSigmaVarName",   "globalMcBkgSigmaVarName",     nmcbkgSigma ); 
///    else{ mcbkgSigmaVar_->setVal(nmcbkgSigma); }
///    
///    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkgMean, 0, nddbkgMean*1.5); 
///    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkgMean*3.5);     ddbkgVar_->setVal(nddbkgMean);}
///
/////    if(! ddbkgVar_        ) ddbkgVar_       = new RooRealVar( "globalDDBkgVarName",        "globalDDBkgVarName",          nddbkg, 0, nddbkg*1.5); 
/////    else{ ddbkgVar_->setMin("",0);   ddbkgVar_->setMax("",nddbkg*3.5);     ddbkgVar_->setVal(nddbkg);}
///    
///    
///    if(! ddbkgMeanVar_)     ddbkgMeanVar_   = new RooRealVar( "globalDDBkgMeanVarName", "globalDDBkgMeanVarName",         nddbkgMean); 
///    else{ ddbkgMeanVar_->setVal(nddbkgMean);   }
///    
///    if(! ddbkgSigmaVar_)    ddbkgSigmaVar_  = new RooRealVar( "globalDDBkgSigmaVarName","globalDDBkgSigmaVarName",        nddbkgSigma);  
///    else{ ddbkgSigmaVar_->setVal(nddbkgSigma);   }
///    
///    RooGaussian * mcbkgConstrain = new RooGaussian( mcbkgConstrainName.c_str(), mcbkgConstrainName.c_str(), *mcbkgVar_,*mcbkgMeanVar_, *mcbkgSigmaVar_);
///    RooGaussian * ddbkgConstrain = new RooGaussian( ddbkgConstrainName.c_str(), ddbkgConstrainName.c_str(), *ddbkgVar_, *ddbkgMeanVar_, *ddbkgSigmaVar_);
///    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    
///    
///    
///    
///    // build the sum model and model with constrains ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///    string sumModelName = dist+string("_sumModel");
///    string sumModelExp  = signalModelName+string("+")+ddbkgModelName+string("+")+mcbkgModelName;
///    //    RooAddPdf * sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *signalModel, *ddbkgModel, *mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///    RooAddPdf* sumModel;
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *u_signalModel, *u_ddbkgModel, *u_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      sumModel= new RooAddPdf( sumModelName.c_str(),sumModelExp.c_str(), RooArgList( *b_signalModel, *b_ddbkgModel, *b_mcbkgModel ), RooArgList( *sigVar_, *ddbkgVar_, *mcbkgVar_) );
///      break;
///    default : // Dummy - should never arrive here
///      sumModel=0;
///    }
///    
///    
///    string sumModelConstrainedName = dist+string("_sumConstrainedModel");
///    string sumModelConstrainedExp  = sumModelName+string("*")+ddbkgConstrainName+string("*")+mcbkgConstrainName;
///    RooProdPdf * model = new RooProdPdf(sumModelConstrainedName.c_str(),sumModelConstrainedExp.c_str(), RooArgSet(*sumModel, *ddbkgConstrain, *mcbkgConstrain));
///
///    //Fit model to data
///    //    RooFitResult * constrainedModelFit = model->fitTo( 
///    //						      *dataHisto, 
///    //						      //						      *myDataDS,
///    //      Minos(), 
///    //      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
///    //      Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max) 
///    //    );
///    
///    RooFitResult * constrainedModelFit;
///
///    cout<<"Start fitting variable"<<endl;
///    switch(unbinned[i]){
///    case 1: // Unbinned
///      constrainedModelFit = model->fitTo( *myDataDS, 
///					  Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      constrainedModelFit = model->fitTo( *dataHisto, 
///					  Minos(), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Save(kTRUE),PrintLevel(-1),Verbose(false),Range(min,max));
///      break;
///    default : // Dummy - should never arrive here
///      constrainedModelFit = 0;
///    }
///    
///
///    gStyle->SetOptStat(0);
///    string plotWithModelFit   = string("plot_binned_modelFit_")+dist+string(".pdf");
///    
///    canvas_->cd();  
///    canvas_->Clear();  
///    RooPlot * myFrame = myvar->frame();
///    myFrame->SetTitle("");
///    myFrame->GetXaxis()->SetTitle(dist.c_str());
///    myFrame->GetYaxis()->SetTitle("Events");
///    dataHisto->plotOn(myFrame);
///    model->plotOn(myFrame);
///    //    model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*signalModel), RooFit::LineColor(kGreen));   
///    switch(unbinned[i]){
///    case 1 : // Unbinned
///      model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*u_signalModel), RooFit::LineColor(kGreen));   
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      model->plotOn(myFrame, RooFit::LineStyle(kDashed), RooFit::Components(*b_signalModel), RooFit::LineColor(kGreen));   
///      break;
///    default : // Dummy - should never arrive here
///      constrainedModelFit = 0;
///    }
///    
///
///   
///    myFrame->Draw();
///    canvas_->SaveAs(plotWithModelFit.c_str());
///    canvas_->cd();
///    canvas_->Clear();
///
///    // Test on the fit ////////////////////////////////////////////////////////////
/////    RooNLLVar * nll = (RooNLLVar *) model->createNLL(
/////						     *dataHisto,
/////						     //*myDataDS,
/////      RooFit::CloneData(kTRUE),
/////      Extended(kTRUE),
/////      Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)),
/////      Range(min,max)
/////    );
///    
///    RooNLLVar * nll;
///    
///    switch(unbinned[i]){
///    case 1: // Unbinned
///      nll = (RooNLLVar *) model->createNLL( *myDataDS,
///					    RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
///      break;
///    case 0:  // Binned (w/ or w/out smoothing)
///      nll = (RooNLLVar *) model->createNLL( *dataHisto,
///					    RooFit::CloneData(kTRUE), Extended(kTRUE), Constrain(RooArgSet(*ddbkgVar_,*mcbkgVar_)), Range(min,max));
///      break;
///    default : // Dummy - should never arrive here
///      nll = 0;
///    }
///    
///    
///    
///    
///    likelihoodVector[i] = nll; //.push_back(nll); 
///
///    RooMinuit minuit( *nll );
///    minuit.migrad();
///    minuit.setErrorLevel(0.5);
///    minuit.hesse();
///
///    RooFitResult* myTempRes = minuit.save();
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    cout<<endl<<"FIT RESULTS for variable"<<dist<<endl<<endl;
///    myTempRes->Print("v");
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///
///    canvas_->cd();
///    canvas_->Clear();
///    RooPlot * contourPlot= minuit.contour( *ddbkgVar_, *sigVar_,1,2,3);
///    contourPlot->SetTitle("");
///    contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
///    contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
///    contourPlot->GetYaxis()->SetRangeUser(0,600);
///    contourPlot->GetXaxis()->SetRange(0,400);
///    contourPlot->Draw();
///    string contourPlotName =  string("contour_")+dist+string(".pdf");
///    canvas_->SaveAs(contourPlotName.c_str());
///    ///////////////////////////////////////////////////////////
///
///
///  }
///
/////  RooAddition* combll;
/////  RooFitResult* myRes;
/////  RooPlot* contourPlot;
/////  for(int dim=0; dim<NVARS; dim++){
///    RooArgSet llSet;
///    cout<<"FINAL FIT NUMBER " << NVARS << endl;    
///    // fill arg set with a
///    for(size_t i=0; i<NVARS;i++){ llSet.add( *(likelihoodVector[i]) ); }
///    //  for(int i=0; i<likelihoodVector.size();i++){ llSet.add( *(likelihoodVector[i]) ); }
///    
///    //create a rooAddition
///    RooAddition *    combll = new RooAddition("combll","combll",llSet);
///    
///    //minimize
///    RooMinuit minuit(*combll);
///    minuit.setVerbose(false);
///    minuit.setErrorLevel(0.5); //otherwise RooFit seems to assume the chi^2 default
///    minuit.hesse();
///    minuit.migrad();
///    minuit.setErrorLevel(0.5);
///    minuit.hesse();
///    minuit.minos();
///    minuit.save();
///    RooFitResult*    myRes = minuit.save();
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    cout<<endl<<"FINAL FIT RESULTS"<<endl<<endl;
///    myRes->Print("v");//myRes->printToStream(resultsFile);
///    cout<<endl<<"*******************************************************************************"<<endl;
///    cout<<      "*******************************************************************************"<<endl;
///    
///    canvas_->Clear();
///    RooPlot *     contourPlot= minuit.contour( *ddbkgVar_ , *sigVar_,1,2,3);
///    contourPlot->SetTitle("");
///    contourPlot->GetXaxis()->SetTitle("N^{DD}_{Bkg}");
///    contourPlot->GetYaxis()->SetTitle("N(t#bar{t}#rightarrow l#tau)");
///    contourPlot->GetYaxis()->SetRangeUser(0,600);
///    contourPlot->GetXaxis()->SetRangeUser(0,400);
///    contourPlot->Draw();
///    
///    
///    std::ostringstream oss;
///    oss << NVARS;//dim;
///    
///    string contourPlotName =  string("contour_binned_final_")+oss.str()+string(".pdf");
///    canvas_->SaveAs(contourPlotName.c_str());
///    ///////////////////////////////////////////////////////////
///    
///    delete canvas_;
///    
///    //}
///    //  cout.rdbuf(old); // restore   
///}
///
