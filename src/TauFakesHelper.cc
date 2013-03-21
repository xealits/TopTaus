/**   
      \class	    TauFakesHelper TauFakesHelper.cc "UserCode/LIP/TopTaus/src/TauFakesHelper.cc"                                                                     
      \brief    Class for managing the operations needed in order to estimate tauFakes :)
      
      \author   Pietro Vischia
      
      \version  $Id: TauFakesHelper.cc,v 0.1 2013/03/19 17:08:18 vischia Exp $                                                                                                       
*/




/*

   FRDataQCDTree q(20);
   q.processEvents();

*/
 
#include "LIP/TopTaus/interface/TauFakesHelper.hh"


using namespace event; // FIXME: define inside the code to avoid ambiguities
FRDataQCDTree::FRDataQCDTree(double tauPtCut, TString inputArea, TString outputArea) : ObjectSelector(tauPtCut),
										       inputArea_(inputArea),
										       outputArea_(outputArea)
{
  
  DRMIN_JET_E_ = 0.3;
  DRMIN_JET_M_ = 0.3;
  DRMIN_T_E_   = 0.3;
  DRMIN_T_M_   = 0.3;
  DRMIN_T_J_   = 0.3;
  MIN_JET_PT_CUT_ = 20.0;
  MIN_TAU_PT_CUT_ = 20.0;
  
  TAUPRONGS_ = 0;    
  
  JES_=0.0;
  
}

void FRDataQCDTree::processEvents(){
  ComputeFakeRate(inputArea_+TString("Jet.root"), "PFlow", outputArea_+TString("TauFRDataPassing_pt20"), true);
  ComputeFakeRate(inputArea_+TString("Jet.root"), "PFlow", outputArea_+TString("TauFRDataFailing_pt20"), false);
  
  //    ComputeFakeRate("/lustre/data3/cmslocal/vischia/tau_dilepton/fakerate2011/inputSamples/Jet.root", "PFlow", "TauFRDataPassing_pt20_BTag", true, true);
  //    ComputeFakeRate("/lustre/data3/cmslocal/vischia/tau_dilepton/fakerate2011/inputSamples/Jet.root", "PFlow", "TauFRDataFailing_pt20_BTag", false, true);
  //
  //    
  //    ComputeFakeRate("/lustre/data3/cmslocal/vischia/tau_dilepton/fakerate2011/inputSamples/Jet.root", "PFlowM", "TauFRDataPassing_pt20_MIso", true);
  //    ComputeFakeRate("/lustre/data3/cmslocal/vischia/tau_dilepton/fakerate2011/inputSamples/Jet.root", "PFlowM", "TauFRDataFailing_pt20_MIso", false);
  
  //gApplication->Terminate(0);
}

void FRDataQCDTree::ComputeFakeRate(TString url,  TString myKey, TString histoFiles_, bool passing, bool isAntiBTag)
{
  
  //Analysis 
  
  //objects to use
  unsigned int jetAlgo(event::AK5),metAlgo(event::CALOAK5), tauType(CALOTAU), leptonType(event::STDLEPTON);
  if(myKey=="TC") { jetAlgo=event::AK5JPT, metAlgo=event::TC, tauType = CALOTAU;}
  else if(myKey=="PF") { jetAlgo=event::AK5PF, metAlgo=event::PF, tauType = PFTAU;}
  else if(myKey.Contains("TaNC")) { jetAlgo=event::AK5PF, metAlgo=event::PF, tauType = PFTAU;}
  else if(myKey.Contains("HPS")) { jetAlgo=event::AK5PF, metAlgo=event::PF, tauType = HPSTAU;}
  //else if(myKey=="PFlow") { jetAlgo=event::AK5PFLOW, metAlgo=event::PFLOWMET, tauType = PFLOWTAU; leptonType = event::PFLOWLEPTON;}
  //else if(myKey=="PFlowM") { jetAlgo=event::AK5PFLOW, metAlgo=event::PFLOWMET, tauType = PFLOWTAU; leptonType = event::PFLOWLEPTON;}
  else if(myKey.Contains("PFlow")) { jetAlgo=event::AK5PFLOW, metAlgo=event::PFLOWMET, tauType = PFLOWTAU; leptonType = event::PFLOWLEPTON;}

  cout << "Events will be taken from "  << url << endl
       << "and analyzed with jet-algo="<< jetAlgo << " met-algo=" << metAlgo << endl;
  
  
  //open file with mini events
  evReader = new event::Reader();
  TFile *f = TFile::Open(url);
  if(f==0) return ;
  if(f->IsZombie()) { f->Close(); return; }  
  TString tag("data");
  int nEntries = evReader->AssignMiniEventTreeFrom(f,tag,"myEventSelector"); 
  cout << nEntries << " events are available" << endl;
  if( nEntries == 0) { f->Close(); return; }

  //create the file ////////////////////////////////
  TString Filename_ = histoFiles_ +"_"+myKey+".root";
  TFile *outFile_ = TFile::Open( Filename_, "RECREATE" );
  //////////////////////////////////////////////////

  TTree *tree = new TTree("tree", "tree");
  tree->Branch("JetPt", &JetPt, "JetPt/D");
  tree->Branch("AbsJetEta", &AbsJetEta, "AbsJetEta/D");
  tree->Branch("JetWidth", &JetWidth, "JetWidth/D");
  tree->Branch("__WEIGHT__", &__WEIGHT__, "__WEIGHT__/D");
  tree->Branch("__TARGET__", &__TARGET__, "__TARGET__/I");

  int nTriggEvent = 0;
  for(int i=0; i<nEntries; ++i)
    {
      //
      // GET OBJECTS FOR NEW EVENT (nb. if kin results are available include them)
      //

      event::MiniEvent_t *ev = evReader->GetNewMiniEvent(i,tag);      
      if(ev==0) continue;
      
      //trigger
      TVectorD *trig = (TVectorD *)ev->triggerColl->At(0);
      bool hasJettrig = ((*trig)[2]>0);

      if(!hasJettrig) continue;  
      nTriggEvent++;

      //Collect Physics Objects
      std::vector<PhysicsObject> vertices                 = evReader->GetVertexCollection(ev);
      std::vector<PhysicsObject> jets_without_arbitration = evReader->GetPhysicsObjectsFrom(ev,event::JET, jetAlgo  ); 
      std::vector<PhysicsObject> jets = evReader->GetJetsWithArbitration( jets_without_arbitration, event::Reader::VTXCONSTRAINED );
      std::vector<PhysicsObject> muons = evReader->GetPhysicsObjectsFrom(ev,event::MUON);
      std::vector<PhysicsObject> electrons = evReader->GetPhysicsObjectsFrom(ev,event::ELECTRON);
      std::vector<PhysicsObject> tausColl = evReader->GetPhysicsObjectsFrom(ev,event::TAU);
      std::vector<PhysicsObject> taus; 
      for(size_t iorigtau=0; iorigtau<tausColl.size(); iorigtau++){
	if(tausColl[iorigtau][17] == tauType){
	  taus.push_back(tausColl[iorigtau]);
	}
      }

      if(vertices.size()==0){ cout<<endl<<" Vertex was zero ???????"<<endl; continue; }
      PhysicsObject & primaryVertex = vertices[0];

      JetCorrectionUncertainty * junc(0);
      vector<double> jerFactors; jerFactors.clear();
      // preselect objects /////////////////////////////////
      DisableLtkCutOnJets(); Pt_Jet(MIN_JET_PT_CUT_); // select hard jets
      //pt_Tau(MIN_TAU_PT_CUT_);  //select tau pT
      vector<int> e_init, m_init, j_init, t_init;
      PreSelectMuons( evReader,&m_init,muons, primaryVertex ); 
      PreSelectElectrons(  evReader,&e_init,electrons, primaryVertex );
      PreSelectJets(true, jerFactors, JES_, junc, jetAlgo,&j_init,jets);
      PreSelectTaus( &t_init,taus, TAUPRONGS_, myKey, primaryVertex);
      //////////////////////////////////////////////////////

      // only accept jets if dr > drmin in respect to electrons and muons /////////////////////
      vector<int> emptyColl, j_toRemove; 
      vector<int> j_afterLeptonRemoval;
      ProcessCleaning(&j_init, &j_toRemove, &e_init, &emptyColl, jets, electrons, DRMIN_JET_E_ );
      ProcessCleaning(&j_init, &j_toRemove, &m_init, &emptyColl, jets, muons,     DRMIN_JET_M_ );
      ApplyCleaning(&j_init, &j_toRemove, &j_afterLeptonRemoval);
      /////////////////////////////////////////////////////////////////////////////////////////
	
      // only accept taus if dr > drmin in respect to electrons and muons /////////////////////
      vector<int> t_toRemove;
      vector<int> t_final; 
      ProcessCleaning(&t_init, &t_toRemove, &e_init, &emptyColl, taus, electrons, DRMIN_T_E_ );
      ProcessCleaning(&t_init, &t_toRemove, &m_init, &emptyColl, taus, muons,     DRMIN_T_M_ );
      ApplyCleaning(&t_init, &t_toRemove, &t_final);
      /////////////////////////////////////////////////////////////////////////////////////////
	
      //remove jet matching to the trigger////////////////////////////////////////////////////// 
      vector<int> j_final;
      int nTrigJet = 0;
      for(size_t ijet = 0; ijet < j_afterLeptonRemoval.size(); ijet++){
	int ind =  j_afterLeptonRemoval[ijet];
	if(jets[ind][4] > 30)nTrigJet++;
      }
      //if(nTrigJet)cout<<"nTrig Jet "<<nTrigJet<<endl;
      for(size_t ijet = 0; ijet < j_afterLeptonRemoval.size(); ijet++){
	int ind =  j_afterLeptonRemoval[ijet];
	if(nTrigJet == 1 && jets[ind][4] > 30) continue; //not to use trigger matched Jet
	j_final.push_back(ind);
      }
      //////////////////////////////////////////////////////////////////////////////////////////
      
      //Fill Histograms
      for(size_t ijet = 0; ijet < j_final.size(); ijet++){
	int ind_jet = j_final[ijet];

	if(isAntiBTag && (jets[ind_jet][33] > 0.679)) continue; //anti-btag CSVL

	double jetPt = GetJetResidualPt(jets[ind_jet]);
	double Rjet = ((jets[ind_jet][29]+jets[ind_jet][30]) > 0) ? sqrt(jets[ind_jet][29]+jets[ind_jet][30]) : 0.;
	
	//fill for selected taus
	double mindr = 0.5; int jtau = -1;
	for(size_t itau = 0; itau < t_final.size(); itau++){
	  int ind_tau = t_final[itau];
	  double idr=taus[ind_tau].DeltaR(jets[ind_jet]);
	  if(idr < mindr){
	    mindr = idr;
	    jtau = ind_tau;
	  }
	}
	if(passing && mindr < DRMIN_T_J_){
	  JetPt = jetPt;
	  AbsJetEta = fabs(jets[ind_jet].Eta());
	  JetWidth = Rjet;
	  __WEIGHT__ = 1.0;
	  __TARGET__ = 1;

	  tree->Fill();
	}
	else if(!passing && mindr > DRMIN_T_J_){
	  JetPt = jetPt;
	  AbsJetEta = fabs(jets[ind_jet].Eta());
	  JetWidth = Rjet;
	  __WEIGHT__ = 1.0;
	  __TARGET__ = 0;
	  
	  tree->Fill();
	}
      }

    }
  cout<<" no. of triggered Events "<<nTriggEvent<<endl;
   
  f->Close();
  outFile_->Write();
  outFile_->Close();
}

