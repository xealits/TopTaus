
/**   
      \class	    doTauFakesStudy.cc doTauFakesStudy.cc.cc "UserCode/LIP/TopTaus/bin/doTauFakesStudy.cc.cc"                                                                     
      \brief    executable for performing all the tau fakes study
      
      \author   Pietro Vischia
      
      \version  $Id: doTauFakesStudy.cc,v 1.4 2013/04/22 16:58:36 vischia Exp $                                                                                                       
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
    
    if(actions_[i] == "trainWMuMC" || actions_[i] == "trainWMuAll" || actions_[i] == "trainAll" || actions_[i] == "all"){
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
    
    if(actions_[i] == "prepareWMuMC" || actions_[i] == "prepareWMuAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::WMUMC);
    }

    if(actions_[i] == "prepareQCDData" || actions_[i] == "prepareQCDAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::QCDDATA);
    }
    
    if(actions_[i] == "prepareQCDMC" || actions_[i] == "prepareQCDAll" || actions_[i] == "prepareAll" || actions_[i] == "all"){
      helper->PrepareFiles(TauFakesHelper::QCDMC);
    }

    // Compute fakes  
    vector<double> wmuValues;
    vector<double> qcdValues;

    if(actions_[i] == "computeWMuFakes" || actions_[i] == "computeAll" || actions_[i] == "all"){
      helper->ComputeTauFake("WMu", wmuValues);
    }
    
    if(actions_[i] == "computeQCDFakes" || actions_[i] == "computeAll" || actions_[i] == "all"){
      helper->ComputeTauFake("DiJet", qcdValues);
    }
    
    if( (actions_[i] == "computeWMuFakes" && actions_[i] == "computeQCDFakes")
	|| actions_[i] == "computeAll" || actions_[i] == "all"){

      double tempErr;

      double wmuExp(657.76), wmuExpErr(10.); // FIXME: hardcoded
      double qcdExp(82.4), qcdExpErr(19.);
      
      double wmuData(wmuValues[0]), wmuDataErr(wmuValues[1]);
      double wmuMC  (wmuValues[2]), wmuMCErr  (wmuValues[3]);
      double wmuRes (wmuValues[4]), wmuResErr (wmuValues[5]);

      double qcdData(qcdValues[0]), qcdDataErr(qcdValues[1]);
      double qcdMC  (qcdValues[2]), qcdMCErr  (qcdValues[3]);
      double qcdRes (qcdValues[4]), qcdResErr (qcdValues[5]);
      
      wmuData = wmuData - wmuRes;
      tempErr = sqrt(wmuDataErr*wmuDataErr + wmuResErr*wmuResErr);
      wmuDataErr = tempErr;

      wmuMC = wmuMC - wmuRes;
      tempErr = sqrt(wmuMCErr*wmuMCErr + wmuResErr*wmuResErr);
      wmuMCErr = tempErr;

      qcdData = qcdData - qcdRes;
      tempErr = sqrt(qcdDataErr*qcdDataErr + qcdResErr*qcdResErr);
      qcdDataErr = tempErr;

      qcdMC = qcdMC - qcdRes;
      tempErr = sqrt(qcdMCErr*qcdMCErr + qcdResErr*qcdResErr);
      qcdMCErr = tempErr;

      
      double data( (wmuData+qcdData)/2. ), dataErr( fabs(wmuData-qcdData)/2. );
      double mc( (wmuMC+qcdMC)/2. ), mcErr( fabs(wmuMC-qcdMC)/2. );
      double res( (wmuRes+qcdRes)/2. ), resErr( fabs(wmuRes-qcdRes)/2. );
      
      double expFinal(wmuExp+qcdExp), expErrFinal( sqrt(wmuExpErr*wmuExpErr + qcdExpErr*qcdExpErr) );
      //      double dataFinal(data-res ), dataErrFinal( sqrt(dataErr*dataErr + resErr*resErr) );
      //      double mcFinal(mc-res), mcErrFinal( sqrt(mcErr*mcErr + resErr*resErr) );
      
      cout << "exp: " << expFinal << " +/- " << expErrFinal << endl;
      cout << "mc: " << mc << " +/- " << mcErr << endl;
      cout << "data: " << data << " +/- " << dataErr << endl;
      
      cout<<"--------------------------------------- LATEX SUMMARY  --------------------------------------" << endl<<endl;
      cout<<"\\begin{tabular}{c|c|c|c}"<<endl;
      cout<<"\\multicolumn{4}{c}{ } \\\\"<< endl;
      cout<<"\\hline"<<endl;
      cout<<"Sample &  MC expectation & Estimated from MC & Estimated from data  \\\\\\hline"<<endl;
      cout<<"\\hline"<<endl;
      cout<<"QCD multi-jet & \\multirow{3}{*}{"<<expFinal<<"$\\pm$"<<expErrFinal<<"} & "<<qcdMC<<" & "<<qcdData<<" \\\\\\cline{1-1} \\cline{3-4}"<<endl;
      cout<<"W+jets &   & "<<wmuMC<<" & "<<wmuData<<" \\\\\\cline{1-1} \\cline{3-4}"<<endl;
      cout<<"Average &  & "<<mc<<"$\\pm$"<<100*mcErr/mc<<"\\% & "<<data<<"$\\pm$"<<100*dataErr/data<<"\\%  \\\\\\hline"<< endl;
      cout<<"\\hline"<<endl;
      cout<<"\\end{tabular}"<< endl;
    }
    
    // Produce final data driven file with data rescaled events
    if(actions_[i] == "produceDDfile" || actions_[i] == "all"){
      helper->ProduceDataDrivenDistributions();
    }
    
    
  }
    cout << "physicsAnalysis reached its natural end" << endl;
    
  return 0;
  
}

