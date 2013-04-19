
/**   
      \class	    doTauFakesStudy.cc doTauFakesStudy.cc.cc "UserCode/LIP/TopTaus/bin/doTauFakesStudy.cc.cc"                                                                     
      \brief    executable for performing all the tau fakes study
      
      \author   Pietro Vischia
      
      \version  $Id: doTauFakesStudy.cc,v 1.2 2013/04/18 12:51:19 vischia Exp $                                                                                                       
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



std::vector<std::string> actions_;
//std::string actions_;

//
void printHelp()
{
  std::cout<< "Usage\n" 
	   << "--help      --> print this help and exit.\n" 
	   << "--do        --> list of actions to be performed (they must be comma separated without space)\n" 
	   << "\t\t\t Options documentation:\n"
	   << "\t\t\t\t\t all:\n"
	   << "\t\t\t\t\t \t\t do all the following.\n"
	   << "\t\t\t\t\t allTrees wmuTrees qctTrees:\n"
	   << "\t\t\t\t\t \t\t produce training trees.\n"
	   << "\t\t\t\t\t trainAll trainWMuAll trainWMuData trainWMuMC trainQCDAll trainQCDData trainQCDMC:\n"
	   << "\t\t\t\t\t \t\t train with nearest-neighbours algorithm.\n"
	   << "\t\t\t\t\t prepareAll prepareWMuAll prepareWMuData prepareWMuMC prepareQCDAll prepareQCDData prepareQCDMC:\n"
	   << "\t\t\t\t\t \t\t prepare reweighted files for fakes computation.\n"
	   << "\t\t\t\t\t computeAll computeWMuFakes computeQCDFakes:\n"
	   << "\t\t\t\t\t \t\t compute fakes.\n"
	   << "\t\t\t\t\t produceDDfile:\n"
	   << "\t\t\t\t\t \t\t produce final data driven file with data rescaled events.\n"
	   << std::endl;
  
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
    else if(arg.find("--do")     !=string::npos && i+1<argc) { char* pch = strtok(argv[i+1],",");printf("actions to perform are : "); //actions_=argv[i+1]; // Do stuff in order (see below) in order to allow for consistent updates
      while (pch!=NULL){printf(" %s ",pch); actions_.push_back(pch);  pch = strtok(NULL,",");}printf("\n"); i++;
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
  
  for(size_t i=0; i<actions_.size(); i++){
    // Produce training trees
    if(actions_[i] == "wmuTrees" || actions_[i] == "allTrees" || actions_[i] == "all"){
      helper->ProcessEvents(TauFakesHelper::WMUDATA);
      helper->ProcessEvents(TauFakesHelper::WMUMC);
    }
    
    if(actions_[i] == "qcdTrees" || actions_[i] == "allTrees" || actions_[i] == "all"){
      helper->ProcessEvents(TauFakesHelper::QCDDATA);
      helper->ProcessEvents(TauFakesHelper::QCDMC);
    }
    
    // Train and monitor
    if(actions_[i] == "trainWMuData" || actions_[i] == "trainWMuAll" || actions_[i] == "trainAll" || actions_[i] == "all"){
      helper->Trainer(TauFakesHelper::WMUDATA);      
      helper->Monitor(TauFakesHelper::WMUDATA);
    }
    
    if(actions_[i] == "trainWmuMC" || actions_[i] == "trainWmuAll" || actions_[i] == "trainAll" || actions_[i] == "all"){
      helper->Trainer(TauFakesHelper::WMUMC);
      helper->Monitor(TauFakesHelper::WMUMC);
    }
    
    if(actions_[i] == "trainQCDData" || actions_[i] == "trainQCDAll" || actions_[i] == "trainAll" || actions_[i] == "all"){
      helper->Trainer(TauFakesHelper::QCDDATA);
      helper->Monitor(TauFakesHelper::QCDDATA);
    }

    if(actions_[i] == "trainQCDMC" || actions_[i] == "trainQCDAll" || actions_[i] == "trainAll" || actions_[i] == "all"){
      helper->Trainer(TauFakesHelper::QCDMC);
      helper->Monitor(TauFakesHelper::QCDMC);
    }
    
    // Prepare reweighted files for fake computation
    if(actions_[i] == "prepareWMuData" || actions_[i] == "prepareWMuAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::WMUDATA);
    }
    
    if(actions_[i] == "prepareWmuMC" || actions_[i] == "prepareWmuAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::WMUMC);
    }

    if(actions_[i] == "prepareQCDData" || actions_[i] == "prepareQCDAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::QCDDATA);
    }
    
    if(actions_[i] == "prepareQCDMC" || actions_[i] == "prepareQCDAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::QCDMC);
    }

    // Compute fakes  
    if(actions_[i] == "computeWMuFakes" || actions_[i] == "computeAll" || actions_[i] == "all"){
      helper->ComputeTauFake("WMu");
    }
    
    if(actions_[i] == "computeQCDFakes" || actions_[i] == "computeAll" || actions_[i] == "all"){
      helper->ComputeTauFake("DiJet");
    }
    
    // Produce final data driven file with data rescaled events
    if(actions_[i] == "produceDDfile" || actions_[i] == "all"){
      helper->ProduceDataDrivenDistributions();
    }
    
    
  }
    cout << "physicsAnalysis reached its natural end" << endl;
    
  return 0;
  
}

