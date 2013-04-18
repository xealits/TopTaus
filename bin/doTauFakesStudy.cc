
/**   
      \class	    doTauFakesStudy.cc doTauFakesStudy.cc.cc "UserCode/LIP/TopTaus/bin/doTauFakesStudy.cc.cc"                                                                     
      \brief    executable for performing all the tau fakes study
      
      \author   Pietro Vischia
      
      \version  $Id: doTauFakesStudy.cc,v 1.1 2013/03/21 10:55:35 vischia Exp $                                                                                                       
*/

#include "LIP/TopTaus/interface/TauFakesHelper.hh"


// System includes

// CMSSW includes
#include "FWCore/FWLite/interface/AutoLibraryLoader.h"
#include "FWCore/PythonParameterSet/interface/MakeParameterSets.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
// ROOT includes
#include "TSystem.h"



/*
  
@run me like this

doTauFakesStudy 

*/



//std::vector<std::string> actions_;
std::string actions_;

//
void printHelp()
{
  std::cout<< "Options\n" 
	   << "--help      --> print this help and exit.\n" 
	   << "--do        --> list of actions to be performed (they must be comma separated without space)\n\t\tChoices are: qcdTrees, wjetsTrees, trainData, trainMC, monitor, calcFakes" << std::endl;
}
//
int main(int argc, char* argv[])
{

  actions_.clear();

  // load framework libraries
  gSystem->Load( "libFWCoreFWLite" );
  AutoLibraryLoader::enable();
  
  if ( argc < 3 ) {
    std::cout << "Usage : " << argv[0] << " parameters_cfg.py [options]" << std::endl;
    printHelp(); return -1;
  }
  
  // Get input arguments
  for(int i=1;i<argc;i++){
    string arg(argv[i]);
    if(arg.find("--help")        !=string::npos) { printHelp(); return -1;}   
    else if(arg.find("--do")     !=string::npos && i+1<argc) { char* pch = strtok(argv[i+1],",");printf("actions to perform are : "); actions_=argv[i+1]; // Do stuff in order (see below) in order to allow for consistent updates
      //while (pch!=NULL){printf(" %s ",pch); actions_.push_back(pch);  pch = strtok(NULL,",");}printf("\n"); i++;
    }    
    
    //check arguments // FIXME: implement --blah bih
  }
  
  

  
  string parSet(argv[1]);

  const edm::ParameterSet &pSet = edm::readPSetsFrom(parSet)->getParameter<edm::ParameterSet>("TauFakesStudyParSet");
  
  double tauPtCut = pSet.getParameter<double>("tauPtCut");
  TString inputArea = TString(pSet.getParameter<string>("inputArea"));
  TString trainingTreesArea = TString(pSet.getParameter<string>("trainingTreesArea"));
  TString configArea= TString(pSet.getParameter<string>("configArea"));
  TString trainingOutputArea = TString(pSet.getParameter<string>("trainingOutputArea"));
  TString outputArea = TString(pSet.getParameter<string>("outputArea"));
  TString puFileName = TString(pSet.getParameter<string>("puFileName"));
  TString ntuplesArea = TString(pSet.getParameter<string>("ntuplesArea"));
  
  if(actions_.find("new") !=string::npos){
    TauFakesHelper* helper = new TauFakesHelper(
						tauPtCut,
						inputArea,
						trainingTreesArea,
						configArea,
						trainingOutputArea,
						outputArea,
						puFileName,
						ntuplesArea
						);
    //    helper->ProcessEvents(TauFakesHelper::WMUDATA);
    //    helper->ProcessEvents(TauFakesHelper::WMUMC);
    //    helper->ProcessEvents(TauFakesHelper::QCDDATA);
    //    helper->ProcessEvents(TauFakesHelper::QCDMC);
    
    //   helper->Trainer(TauFakesHelper::WMUDATA);
    //   helper->Trainer(TauFakesHelper::WMUMC);
    //   helper->Trainer(TauFakesHelper::QCDDATA);
    //    helper->Trainer(TauFakesHelper::QCDMC);
    
//    helper->Monitor(TauFakesHelper::WMUDATA);
//    helper->Monitor(TauFakesHelper::WMUMC);
//    helper->Monitor(TauFakesHelper::QCDDATA);
//    helper->Monitor(TauFakesHelper::QCDMC);
//    
//    helper->PrepareFiles(TauFakesHelper::WMUDATA);
//    helper->PrepareFiles(TauFakesHelper::WMUMC);
//    helper->PrepareFiles(TauFakesHelper::QCDDATA);
//    helper->PrepareFiles(TauFakesHelper::QCDMC);
//    
// //   std::cout << "WMu tau fakes:" << std::endl;
//    helper->ComputeTauFake("WMu");
// //   std::cout << "QCD tau fakes:" << std::endl;
//    helper->ComputeTauFake("DiJet");
    
    helper->ProduceDataDrivenDistributions();
  }


  //  TString puFileName = TString(pSet.getParameter<string>("puFileName"));
  if(actions_.find("qcdTrees") !=string::npos ){
    FRDataQCDTree* qcdTreeCreator = new FRDataQCDTree( tauPtCut, inputArea, trainingTreesArea /*parSet*/ );
    std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;    
    qcdTreeCreator->processEvents();
  }
  if(actions_.find("wjetsTrees") !=string::npos ){
    FRWJetsTree* wjetsTreeCreator = new FRWJetsTree( tauPtCut, inputArea, trainingTreesArea, puFileName /*parSet*/);
    std::cout << "Analyzer has been set with a cut on tau pt of " << tauPtCut << " GeV/c " << std::endl;    
    wjetsTreeCreator->processEvents();
  }
  
  if( actions_.find("trainMC") !=string::npos )
    std::cout << "Do nothing for now" << std::endl;

  if(actions_.find("trainData") !=string::npos ){
    MyFakesTrainer* myQCDTrainer = new MyFakesTrainer(trainingTreesArea, configArea, trainingOutputArea, "QCD", "Data");
    //    MyFakesTrainer* myWMuTrainer = new MyFakesTrainer(trainingTreesArea, configArea, trainingOutputArea, "WMu", "Data");

  }

  if(actions_.find("monitor") !=string::npos ){
    FakesMonitor qcdData(trainingOutputArea+TString("/train_QCDData/"), "QCD", trainingOutputArea+TString("/Trained_QCDDataPFlow_pt20.mva"), outputArea+TString("QCD")); // QCD data
    FakesMonitor wjetsData(trainingOutputArea+TString("/train_WMuData"), "WMu", trainingOutputArea+TString("/Trained_WMuDataPFlow_pt20.mva"), outputArea+TString("WMu")); // WJets data
    //FakesMonitor qcdMC(trainingOutputArea, configArea+TString("/calibrationFile"), outputArea); // QCD MC
    //FakesMonitor wjetsMC(trainingOutputArea, configArea+TString("/calibrationFile"), outputArea); // WJets MC
  }
  
  if(actions_.find("calcFakes") !=string::npos ){
    FakesComputer myComputer (trainingOutputArea, outputArea);

    // Create intermediate files
    myComputer.PrepareFiles("QCDData");
    std::cout << "Prepared QCDdata files" << std::endl;
    myComputer.PrepareFiles("WMuData");
    std::cout << "Prepared WMuData" << std::endl;
//    myComputer.PrepareFiles("QCDMC");
//    myComputer.PrepareFiles("WMuMC");

    // Compute and display tau fakes values from kNN
    std::cout << "WMu tau fakes:" << std::endl;
    //    myComputer.ComputeTauFake("WMu");
    std::cout << "QCD tau fakes:" << std::endl;
    myComputer.ComputeTauFake("DiJet");
  }
    
  cout << "physicsAnalysis reached its natural end" << endl;
  
  return 0;
  
}

