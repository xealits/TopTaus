#include "LIP/TopTaus/interface/ObjectSelector.hh"

#include "LIP/TopTaus/interface/CommonDefinitions.hh"
#include <iostream>
#include <iomanip>

#include "CondFormats/JetMETObjects/interface/JetResolution.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"

using namespace std;
using namespace commondefinitions;


ObjectSelector::ObjectSelector( double tauPtCut ):
  TAU_PT_MIN_(tauPtCut)
{
  // Set Running conditions... /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // b-tagging working points from https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP#B_tagging_Operating_Points_for_3
  //      BTAGIND_=33; BTAG_CUT_ = 0.679;    // CSVM
  //      BTAGIND_=33; BTAG_CUT_ = 0.244;    // CSVL
  //      BTAGIND_=12; BTAG_CUT_ = 1.7;      // TCHEL
  
  // FIXME: move to cfg
  if( commondefinitions::run2012_ ) { commondefinitions::BTAGIND_=33; commondefinitions::BTAG_CUT_ = 0.679;} // CSVM 
  else { commondefinitions::BTAGIND_=33; commondefinitions::BTAG_CUT_=0.679 ;}  //  CSVM
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  // residual corrections in data ///////////////////////////////////////////////////////////////////////////////////////////////////////
  //resCorPar_      = new JetCorrectorParameters("/lustre/lip.pt/data/cmslocal/samples/CMSSW_4_2_X/mc/JEC/Jec11_V2_AK5PF_L2L3Residual.txt");
  //vector<JetCorrectorParameters> vParam; vParam.push_back(*resCorPar_);
  //resFactors_     = new FactorizedJetCorrector(vParam);  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //  cout << "[ObjectSelector::ObjectSelector] TAU PT MIN: " << TAU_PT_MIN_ << endl;
  SetLeptonPlusJetsSelection();

}

void ObjectSelector::SetLeptonPlusJetsSelection(){

  // by default apply lepton + jet selection
  leptonPlusJetsSelection_=true;
  
  //Vertex //////
  ZMAX_    = 0.5; //1;
  ZTAUMAX_ = 0.2;
  ///////////////
  
  // Jets ///////////////////////////////////////////
  JET_PT_MIN_        = 30; // WARNING : Jet pt set to 20 for trigger studies
  //JET_PT_MIN_        = 30; 
  JET_ETA_MAX_       = 2.4;       // dilepton is 2.5 
  JET_EMF_MIN_       = 0.01;       
  JET_LTK_           = 5.;
  JET_LEPTON_DRMIN_  = 0.3;
  applyJetLtkCut_    = true; // TODAY: false
  //////////////////////////////////////////////////
  
  
  // electron //////////////////////////////////////
  // with 0.05 the agreement is mush better
  E_RELISO_MAX_       = 0.1;//0.05 //test 0.1;//Value to be used(0.1)   // dilepton is 0.15 , //0.1
  
  E_ETA_MAX_          = 2.5;     
  E_ET_MIN_           = 35;      
  E_D0_MAX_           = 0.02;   // dilepton is 0.04
  
  LOOSE_E_RELISO_MAX_ = 0.2;
  LOOSE_E_ETA_MAX_    = 2.5;
  LOOSE_E_ET_MIN_     = 15;
  RHO_AEFF_E_         = 0.24;
  
  //////////////////////////////////////////////////
  
  
  // muon //////////////////////////////////////////
  M_RELISO_MAX_  = 0.12; //0.05;         // dilepton is 0.15
  M_PT_MIN_      = 30; //30;  //WARNING M_PT_MIN_      = 20;               // dilepton is 20 // test on 70
  M_ETA_MAX_     = 2.1;                 // dilepton is 2.5
  M_D0_MAX_      = 0.2;                // dilepton should be 0.2
  
  LOOSE_M_RELISO_MAX_ = 0.2; 
  LOOSE_M_ETA_MAX_    = 2.5; 
  LOOSE_M_PT_MIN_     = 10;
  RHO_AEFF_M_         = 0.112;
  
  if(commondefinitions::run2012_){
    M_RELISO_MAX_  = 0.12; //cone 0.4
  }
  ///////////////////////////////////////////////////
  
  
  // taus /////////////////////////////////////////////////////// 
  TAU_PF_LEADTRKPT_MIN_ = 5;  // WARNING 5;
  TAU_LEADPFPT_MIN_     = 10; // WARNING 10;  // lead track on pt
  TAU_ETA_MAX_          = 2.3; //2.4; 
  TAU_RELISO_           = 0.9;
  TAU_EOP_CUT_          = 0.5;
  TAU_EMF_CUT_          = 0.9;
  TAU_TC_LEADTRKPT_MIN_ = 10; // WARNING 10; 
  TAU_D0_MAX_           = 400;// was 200
  ISO_ECAL_1P_          = 1.5;// was 1
  ISO_ECAL_3P_          = 3;
  TAU_EMF_MAX_          = 2;  //0.95;
  ///////////////////////////////////////////////////////////////
  
}

void ObjectSelector::SetDileptonSelection(){
  
  //Vertex //////
  ZMAX_    = 1;
  ZTAUMAX_ = 0.2;
  ///////////////
  
  
  leptonPlusJetsSelection_=false;        
  
  // Jets///////////////////
  JET_ETA_MAX_       = 2.5;       //
  JET_EMF_MIN_       = 0.;        // which cut to be applied? 
  JET_LTK_           = 5.;       // warning
  applyJetLtkCut_    = true; // today: false;
  JET_LEPTON_DRMIN_  = 0.4;
  
  
  // electron/////////////////
  E_RELISO_MAX_       = 0.15;  
  E_PT_MIN_           = 20;    
  E_D0_MAX_           = 0.02; //0.04; 
  //////////////////////////// 
  
  // muon //////////////
  M_RELISO_MAX_  = 0.15; 
  M_ETA_MAX_     = 2.4;
  /////////////////////
  
}

bool ObjectSelector::LooseElectronVeto(event::Reader* reader, int selectedElectron, const vector<PhysicsObject>& vE){
  bool looseVeto(false);
  for( int i=0;i< (int) vE.size();++i ){
    if(i==selectedElectron){ continue; }
    
    const PhysicsObject * e = &vE[i];
    
    double eEta      = TMath::Abs(e->Eta());
    double eEt       = TMath::Abs(e->Et());
    double eRelIso   = (*e)[18];
    //double eD0       = fabs( (*e)[25] );
    bool   id        = reader->ElectronId( (PhysicsObject &)(*e),0);  // loose working point
    double minDR2mu  = (*e)[14];  
    double scEta    = (*e)[39];        
        
    if(id && eEt > LOOSE_E_ET_MIN_ && eEta < LOOSE_E_ETA_MAX_ && eRelIso < LOOSE_E_RELISO_MAX_ && minDR2mu >0.1){ looseVeto = true;}
  }
  return looseVeto;
}

bool ObjectSelector::LooseMuonVeto( int selectedMuon, const vector<PhysicsObject>& vM){

  bool looseVeto(false);
  
  for(int i=0;i< (int)vM.size();++i){
    
    if( i==selectedMuon ){continue;}
    
    const PhysicsObject * m = &vM[i];
    
    double mEta     = TMath::Abs(m->Eta());
    double mPt      = TMath::Abs(m->Pt());
    //double mD0      = fabs( (*m)[11]);
    double mRelIso  = (*m)[18];
    
    //see if this muon is glogal (how to see if it is only global?)
    bool isGlobalAndTracker ( (int( (*m)[3])>>1 & 0x3) == 3); 
    
    if(! isGlobalAndTracker ) continue;
    
    if( mEta<LOOSE_M_ETA_MAX_  && mPt> LOOSE_M_PT_MIN_ && mRelIso < LOOSE_M_RELISO_MAX_ ){ looseVeto = true; }
    
  }
  
  return looseVeto;
  
}

void ObjectSelector::PreSelectElectrons( event::Reader* reader, vector<int>* e_i, const vector<PhysicsObject>& vE, PhysicsObject& vertex) {
  
  for(unsigned int i=0;i<vE.size();++i){
    
    const PhysicsObject * e = &vE[i]; 
    double scEta    = (*e)[39];  
    double eEta     = TMath::Abs(e->Eta()); 
    double eEt      = TMath::Abs(e->Et());
    double ePt      = TMath::Abs(e->Pt());
    double eRelIso  = (*e)[18];
    double minDR2mu = (*e)[14];
    double d0       = fabs((*e)[25]);
    //    bool   id       = reader->ElectronId((PhysicsObject &)(*e), 1);    //Tight working point
    //    bool   id       = reader->ElectronId((PhysicsObject &)(*e), 90);  //Loose working point
 

    //Super TightId
    //    bool id = ((*e)[13] > 0.9);
    bool id(false); // = ((*e)[13] > 0.5);

    //   bool superTightId  =   (v >> 4) & 0x1 ; id = id && superTightId ;
    //   cout<<endl<<" electron id : "<<v<<" very loose : "<<(v&0x1)<<" loose : "<<((v>>1)&0x1)<<" medium : "<<((v>>2)&0x1)<<" tight "<<((v>>3)&0x1)<<endl;
    //    cout<<endl<<" electron id : "<<id<<endl;    

    double zvertex   = vertex[7];
    double zelectron = (*e)[29];
    bool   z(false); 
    if ( fabs(zvertex-zelectron) < 1. /* ZMAX_ */ ){ z = true;}
    if(!z){ continue;}
    
    if( leptonPlusJetsSelection_  && ( fabs(scEta)> 1.4442 && fabs(scEta)<1.5660)  ){ continue; }
    if( leptonPlusJetsSelection_  && eEt < E_ET_MIN_ ){ continue; }
    if( !leptonPlusJetsSelection_ && ePt < E_PT_MIN_ ){ continue; }
    
    if (eEta < 0.8) id = ((*e)[13] > 0.94);
    if (eEta >= 0.8 && eEta < 1.479) id = ((*e)[13] > 0.85);
    if (eEta >= 1.479 && eEta < 2.5) id = ((*e)[13] > 0.92);
   
    bool myTrigPresel = false;
    double sigmaIetaIeta = (*e)[4];
    double hadronicOverEm = (*e)[6];
    double TRKISO = (*e)[19];
    double ECALISO = (*e)[20];
    double CALOISO = (*e)[21];
    int nLostHits = (int) (*e)[24];
           
    TRKISO /= ePt;
    ECALISO /= ePt;
    CALOISO /= ePt;

    if(fabs(scEta) < 1.479) {
      if( sigmaIetaIeta < 0.014 &&
	 hadronicOverEm < 0.15 &&
	 TRKISO < 0.2 &&
	 ECALISO < 0.2 &&
	 CALOISO < 0.2 &&
	  nLostHits < 1000) //info[24]=eTrack->numberOfValidHits()+1000*eTrack->trackerExpectedHitsInner().numberOfLostHits();
	myTrigPresel = true;
    }
    else {
      if( sigmaIetaIeta < 0.035 &&
	 hadronicOverEm < 0.10 &&
	 TRKISO < 0.2 &&
	 ECALISO < 0.2 &&
	 CALOISO < 0.2 &&
	  nLostHits < 1000) //info[24]=eTrack->numberOfValidHits()+1000*eTrack->trackerExpectedHitsInner().numberOfLostHits();
	myTrigPresel = true;
    }

    /*    
    cout << "  " << endl;
    cout << "Into ObjectSelector  " << endl;
    cout << "scEta =" << scEta << endl;
    cout << "sigmaIetaIeta =" << sigmaIetaIeta << "   - hadronicOverEm = " << d0 << "   - TRKISO = " << TRKISO << endl;
    cout << "   - ECALISO = " << ECALISO << "   - CALOISO = " << CALOISO << "   - nLostHits = " << nLostHits << endl;
    cout << "myTrigPresel =" << myTrigPresel << endl;
    cout << "Exit ObjectSelector  " << endl;
    cout << "  " << endl;
    */

    /*
    cout << "  " << endl;
    cout << "Into ObjectSelector  " << endl;
    cout << "id = " << id << "   - d0 = " << d0 << "   - eEta = " << eEta << "   - eRelIso = " << eRelIso << "   - minDR2mu = " << minDR2mu << endl;
    cout << "Exit ObjectSelector  " << endl;
    cout << "  " << endl;
    */

    //fn-24-06-13 Before Summer13 - Now d0 cut is included in MVA def. if(id && d0 < E_D0_MAX_ && eEta < E_ETA_MAX_ && eRelIso < E_RELISO_MAX_ && minDR2mu >0.1){ e_i->push_back(i);}
    //    if(id && eEta < E_ETA_MAX_ && eRelIso < E_RELISO_MAX_ && minDR2mu >0.1){ e_i->push_back(i);}
    if(myTrigPresel && id && eEta < E_ETA_MAX_ && eRelIso < E_RELISO_MAX_ && minDR2mu >0.1 && d0 < E_D0_MAX_){ e_i->push_back(i);}
    //if(id && eEta < E_ETA_MAX_ && eRelIso < E_RELISO_MAX_ && minDR2mu >0.1 && d0 < E_D0_MAX_){ e_i->push_back(i);}
  }
  
}

void ObjectSelector::PreSelectMuons( event::Reader* reader, vector<int>* m_i, const vector<PhysicsObject>& vM, PhysicsObject& vertex) {
  
  for( int i=0;i< (int) vM.size();++i){
    
    const PhysicsObject * m = &vM[i];
    
    double mEta     = TMath::Abs(m->Eta());
    double mPt      = TMath::Abs(m->Pt());
    double mD0      = fabs((*m)[11]);
    double mRelIso  = (*m)[18];
    
    bool id = reader->MuonId((PhysicsObject &) (*m));     
    
    
    double zvertex   = vertex[7];
    double zmuon     = (*m)[23];
    bool   z(false); 
    if ( fabs(zvertex-zmuon) < ZMAX_ ){ z = true;}
    if(!z){ continue;}
    
    
    if(id && mD0 < M_D0_MAX_&&  mPt > M_PT_MIN_ && mEta < M_ETA_MAX_ && mRelIso < M_RELISO_MAX_){ m_i->push_back(i);}        
    
  }
  
}

void ObjectSelector::PreSelectJets( bool isData, vector<double>& jerFactors, double jes, JetCorrectionUncertainty* junc, int jetAlgo, vector<int>* j_i, const vector<PhysicsObject>& vJ) {
  
  for(unsigned int i=0;i<vJ.size();++i){
    
    double jetEta     = TMath::Abs(vJ[i].Eta());
    double jetPt      = TMath::Abs(vJ[i].Pt());
    double minDRToLep = TMath::Abs(vJ[i][15]);
    
    if(jerFactors.size() !=0 ) jetPt = jerFactors[i]*jetPt;
    
    if(isData) jetPt = GetJetResidualPt(vJ[i]);
    
    double corr(0);
    
    // JES corrections ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //old proceadure
    if(junc==0){
      if( jes     ){ corr = sqrt( jes*jes +  ( (0.02)*jetEta *(0.02)*jetEta ) ) ;}
      if( jes >0  ){ jes = corr;} else if( jes <0 ){ jes =(-1)*corr;             }
    }
    
    //new proceadure
    else{
      if( jes     ){ junc->setJetEta(jetEta); junc->setJetPt(jetPt); corr=junc->getUncertainty(true); }
      if( jes >0  ){ jes = corr;} else if( jes <0 ){ jes =(-1)*corr;}
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //apply jes correction
    jetPt = (1+jes)*jetPt;
    //////////////////////
    
    
    // emf is different for different types
    // NOTE jetEmf is computed differently according to jet Algo
    double jetLtk  = vJ[i][25]; 
    
    if( applyJetLtkCut_  &&  jetLtk<JET_LTK_ ) continue;  // WARNING !!
    if( minDRToLep < JET_LEPTON_DRMIN_       ) continue;
    
    
    if(jetPt > JET_PT_MIN_ && jetEta < JET_ETA_MAX_ /*&& jetEmf > JET_EMF_MIN*/ ){ j_i->push_back(i);}
    
  }
  
}

void ObjectSelector::PreSelectTaus( vector<int>* t_i, const vector<PhysicsObject>& vT, int requiredProngs, TString myKey, PhysicsObject& vertex, double tes){
  
  // cout<<endl<<" tau collection size is : "<<vT.size()<<endl;
  
  bool applyTanc(false);
  bool applyHPS(false);
  
  if( myKey =="PFlow"   ){ applyHPS =true; HPS_ISO_ = HPS_MEDIUMISO; }  // MEDIUM WORKING POINT  
  else if(      myKey =="TaNC0"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_ONE;    }
  else if( myKey =="TaNC1"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_HALF;   }
  else if( myKey =="TaNC2"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_QUARTER;} 
  else if( myKey =="TaNC3"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_TENTH;  } 
  else if( myKey =="HPS0"    ){ applyHPS =true; HPS_ISO_ = HPS_LOOSEISO;   } 
  else if( myKey =="HPS1"    ){ applyHPS =true; HPS_ISO_ = HPS_MEDIUMISO;  } 
  else if( myKey =="HPS2"    ){ applyHPS =true; HPS_ISO_ = HPS_TIGHTISO;   } 
  //else if( myKey =="PFlow" ){ applyHPS =true; HPS_ISO_ = HPS_LOOSEISO;  }  // LOOSE WORKING POINT WARNING
  
  
  
  for(unsigned int i=0;i<vT.size();++i){
    
    
    double tauEta            = TMath::Abs(vT[i].Eta());
    double tauPt             = TMath::Abs(vT[i].Pt())*(1+tes);
    double tauLtk            = vT[i][4];   // LEAD track /charged hadron pt
    double tauLeadPF         = vT[i][5];   // LEAD PF PT
    double tauEmf            = vT[i][15];  // EMF
    int    numberOfProngs    = vT[i][3];
    //double absIso          = vT[i][6];  //ISO_ABSISOPT
    int    type              = vT[i][17]; 
    //double tauJetPt        = vT[i][7];
    //double relIso          = (tauJetPt-absIso)/tauJetPt;
    double d0                = fabs( vT[i][11] );
    int charge               = fabs( vT[i][0]  );
    
    // isobits
    
    // position 0 (default) , position 1 (delta beta corrections), position 2 (combined iso)
    int isobits              = vT[i][26];  int tau_dis_iso =  (isobits >> 2) & 0x1;
    // position 0 (default) , position 1 (delta beta corrections), CombinedIsolationDeltaBetaCorr3Hits
    //    int isobits              = vT[i][20];  int tau_dis_iso =  isobits & 0x4; 

    double tau_dis_muon      = vT[i][21];
    double tau_dis_electron  = vT[i][22];
    int decayMode            = vT[i][28];  
    
    double zvertex   = vertex[7];
    double ztau     = vT[i][30];
    bool   z(false); 
    if ( fabs(zvertex-ztau) < ZTAUMAX_ ){ z = true; }
    if(!z){ continue;}

    //if(tau_dis_muon && tau_dis_electron && )
    //cout<<endl<<" tauLtk : "<<tauLtk<<" lead PF "<<tauLeadPF<<" number p. "<<" tau_dis_iso "<<tau_dis_iso<<" tau_dis_muon "<<tau_dis_muon<<" tau_dis_electron "<< tau_dis_electron<<endl;
    
    //cout<<endl<<" \n ========= ";
    //cout<<endl<<" tau dis iso = "<<tau_dis_iso<<" dis_muon = "<<tau_dis_electron<<" charge = "<<charge<<" numb prongs : "<<numberOfProngs<<endl;
    
    // The number of prongs is only checked for PF/TC Taus not for HPS or Tanc /////////////
    if( !applyTanc && ! applyHPS ){
      
      
      if     ( requiredProngs == 0 ){ if (numberOfProngs!=1 && numberOfProngs!=3) continue; }
      else if( requiredProngs == 1 ){ if (numberOfProngs!=1)                      continue; }
      else if( requiredProngs == 3 ){ if (numberOfProngs!=3)                      continue; }
      // require sign
      if( charge != 15 ){ continue; } 
    }
    ///////////////////////////////////////////////////////////////////////////////////////
    
    
    // In HPS we cannot access to signal tracks => not possible to compute d0
    if( ! applyHPS ){ if( d0>TAU_D0_MAX_) continue; }
    /////////////////////////////////////////////////////////////////////////
    
    
//fn     // Require discriminators on leptons and tau pt and eta ///////////////////////////////////////////////////////////////////////
//fn    if( tau_dis_muon == 0 || tau_dis_electron == 0 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;    // LooseLoose
//fn    if( tau_dis_muon == 0 || tau_dis_electron < 2 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;        //LooseMedium
//fn     //if( tau_dis_muon < 2 || tau_dis_electron < 3 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;       //TightTight
//fn    if( tau_dis_muon < 3 || tau_dis_electron < 2 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;        //TightMedium
//fn     // If putting electron descrimination against electron to tight no muons pass...  

/*
  cout << "Inside preselect taus" << endl;
  cout << "dis_mu_2 = " << tau_dis_muon << "    - dis_el_mva = " << tau_dis_electron<<endl;
  cout << "dis_mu_old = " << vT[i][39] << "    - dis_el_old = " << vT[i][38] <<endl;
  cout << "Inside preselect taus" << endl;
*/

    if( tau_dis_muon < 6 || tau_dis_electron < 5 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;        //Tight2-MVAMedium 

    // if( tau_dis_muon == 0 || tau_dis_electron < 2.5 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    
    if( tauEmf >= TAU_EMF_MAX_ ) continue;
    
    
    if(type == PFTAU) {   
      
      // extra tanc variables
      if( applyTanc ){
	double discByFR   =vT[i][TANC_FRDISCRIMINATOR_]; 
        
	//WARNING 1): which value of TANC to be used ???
	//double discByTaNC =vT[i][23];
	//double TANC_BYTANC_=0.5; //?????????
	if( /*discByTaNC < TANC_BYTANC_ || */ discByFR < 1. ){ continue; }
	//WARNING 2): Shall we also use lead track and lead pf pf for tanc?
	//if( tauLtk > TAU_PF_LEADTRKPT_MIN_ && tauLeadPF > TAU_LEADPFPT_MIN_){t_i->push_back(i);}
	t_i->push_back(i);
      }
      else if(tau_dis_iso && tauLtk > TAU_PF_LEADTRKPT_MIN_ && tauLeadPF > TAU_LEADPFPT_MIN_ /*&& relIso > TAU_RELISO_*/){ t_i->push_back(i);}
      
      
    }
    else if(type == CALOTAU){
      
      double isoTkPt       = vT[i][13] ;     //iso tkpt sum
      double isoEcalSumPt  = vT[i][14] ;     //iso ecal et sum
      
      double eop(0);
      double e = TMath::Abs(vT[i].E());
      double p = vT[i][16]; // LEAD track P
      if(p) eop = e/p ;
      
      if( isoTkPt != 0 || (numberOfProngs == 1 && isoEcalSumPt > ISO_ECAL_1P_) || (numberOfProngs == 3 && isoEcalSumPt > ISO_ECAL_3P_) ){ continue; }
      
      
      // apply tau muon discrimination only since electron discrimination is applyied by default to tc taus
      if( tau_dis_iso && tauLtk > TAU_TC_LEADTRKPT_MIN_ /* && emf < TAU_EMF_CUT_ */ && eop >TAU_EOP_CUT_ ){ t_i->push_back(i);}
      
      
    }
    else if( type == HPSTAU  ){
      // WARNING DISCRIMINATOR BY DECAY MODE FINDING in 38x = LeadingTrackFinding > 0.5 (this is applied by default to all)
      /*double discByDecayModeFinding = vT[i][xxx]; */ 
      int discByIsolation = ((int) vT[i][HPS_ISO_]) & 0x1;
      
      if( /* discByDecayModeFinding ==0 ||*/  discByIsolation == 0){ continue; }     
      //// Delta Beta:
      //if( /* discByDecayModeFinding ==0 ||*/  discByIsolation < 2){ continue; }            
      
      //decayMode() == (N_charged - 1)*5 + N_piZeros
      //So:
      //0 = 1prong0pi0 ,1 = 1prong1pi0, 2 = 1prong2pi0 ... 10 = 3prong0pi0
      /*
        if( requiredProngs == 1 ){ if( decayMode != 0  && decayMode != 1  && decayMode != 2                                                           ) continue; }
        if( requiredProngs == 3 ){ if(                                                          decayMode != 10 && decayMode != 11 && decayMode != 12 ) continue; }
        if( requiredProngs == 0 ){ if( decayMode != 0  && decayMode != 1  && decayMode != 2  && decayMode != 10 && decayMode != 11 && decayMode != 12 ) continue; }
	
        // use tight electron discrimination
        if( tau_dis_electron < 3 ){ continue;}
	
        if     ( requiredProngs == 0 ){ if (numberOfProngs!=1 && numberOfProngs!=3) continue; }
        else if( requiredProngs == 1 ){ if (numberOfProngs!=1)                      continue; }
        else if( requiredProngs == 3 ){ if (numberOfProngs!=3)                      continue; }
      */
      t_i->push_back(i);
      //cout<<" decayMode "<<decayMode <<endl;
    }
    else if( type == PFLOWTAU  ){
      // WARNING DISCRIMINATOR BY DECAY MODE FINDING in 38x = LeadingTrackFinding > 0.5 (this is applied by default to all)
      /*double discByDecayModeFinding = vT[i][xxx]; */ 

      int veelkbits = vT[i][HPS_ISO_];
      //      int discByIsolation = ((int) vT[i][HPS_ISO_]) & 0x1; // 8hits
      int discByIsolation = ((int) vT[i][HPS_ISO_] >> 2) & 0x1; // 3hits
      
//       cout << "Start of new event" << endl;
//       cout << setprecision(6) << "type = " << type << " tau_dis_iso =" << discByIsolation << " --    isobits =" << veelkbits <<
// 	" --    isobitsBinary =" << ((veelkbits >> 3) & 0x1) << ((veelkbits >> 2) & 0x1) << ((veelkbits >> 1) & 0x1) << (veelkbits & 0x1) <<  endl;
//       cout << "End of new event" << endl;
//       cout << "---------------" << endl;
//       cout << " " << endl;
//       cout << " " << endl;
//       cout << " " << endl;
    
      if( /* discByDecayModeFinding ==0 ||*/  discByIsolation == 0){ continue; }            
      //	// Delta Beta:
      //        if( /* discByDecayModeFinding ==0 ||*/  discByIsolation < 2){ continue; }            
      
      //decayMode() == (N_charged - 1)*5 + N_piZeros
      //So:
      //0 = 1prong0pi0 ,1 = 1prong1pi0, 2 = 1prong2pi0 ... 10 = 3prong0pi0
      /*
        if( requiredProngs == 1 ){ if( decayMode != 0  && decayMode != 1  && decayMode != 2                                                           ) continue; }
        if( requiredProngs == 3 ){ if(                                                          decayMode != 10 && decayMode != 11 && decayMode != 12 ) continue; }
        if( requiredProngs == 0 ){ if( decayMode != 0  && decayMode != 1  && decayMode != 2  && decayMode != 10 && decayMode != 11 && decayMode != 12 ) continue; }
	
        // use tight electron discrimination
        if( tau_dis_electron < 3 ){ continue;}

        if     ( requiredProngs == 0 ){ if (numberOfProngs!=1 && numberOfProngs!=3) continue; }
        else if( requiredProngs == 1 ){ if (numberOfProngs!=1)                      continue; }
        else if( requiredProngs == 3 ){ if (numberOfProngs!=3)                      continue; }
      */
      t_i->push_back(i);
      //cout<<" decayMode "<<decayMode <<endl;
    }
  }
}

void ObjectSelector::GetObjectsBasedOnPt( 
					 bool isData,
					 vector<double>& jerFactors ,
					 double es,  JetCorrectionUncertainty* junc,
					 int type, vector<int>& obj_output, vector<int>& obj_input,
					 const vector<PhysicsObject>& v, double ptmin, double ptmax 
					 ){
  
  
  for(unsigned int i=0;i<obj_input.size();++i){
    int obj_ind = obj_input[i];
    double objPt  = TMath::Abs(v[obj_ind].Pt());
    
    // take into account jes(eta) with a 2% dependencie as recommended by the jes met group ///
    if( type == ObjectSelector::JETS_TYPE ){
      double corr(0);
      double jetEta = TMath::Abs(v[obj_ind].Eta());
      double jetPt = TMath::Abs(v[obj_ind].Pt());
      
      if(isData) jetPt = GetJetResidualPt( v[obj_ind]);
      
      if(jerFactors.size() !=0 ) jetPt = jetPt*jerFactors[obj_ind];
      
      
      if(junc==0){
	if( es ){    corr= sqrt( es*es +  ( (0.02)*jetEta *(0.02)*jetEta ) ) ;}
	if( es >0  ){ es = corr;} else if( es <0 ){ es =(-1)*corr;}
      }else{
	if( es     ){ junc->setJetEta(jetEta); junc->setJetPt(objPt); corr=junc->getUncertainty(true); }
	if( es >0  ){ es = corr;} else if( es <0 ){ es =(-1)*corr;}
      }
      
    }
    ////////////////////////////////////////////////////////////////////////////////////////////
    
    objPt = (1+es)*objPt;
    
    if(objPt<ptmin) continue;
    
    if     ( ptmax == 0    ){ obj_output.push_back(obj_ind); }
    else if( objPt < ptmax ){ obj_output.push_back(obj_ind); }
    
  }       
  
}

void ObjectSelector::ProcessSelfCleaning(  const vector<PhysicsObject>& vParticle, vector<int>* vi_old, vector<int>* vi_new, double DR ){

  vector<int>::iterator itv1, itv2;
  map< int, vector<int> * >::iterator itm1;
  
  // map of non isolated particles and vector of particles in their neighborhood 
  map< int, vector<int> * > nonIso;  
  
  //Build map of non isolated particules and fill new index of isolated ones //////////
  for(itv1 = vi_old->begin(); itv1 != vi_old->end(); ++itv1){
    
    vector<int> * p = new vector<int>;
    
    for(itv2 = vi_old->begin(); itv2 != vi_old->end(); ++itv2){ 
      if (  *itv1 != *itv2 ){
	double dr =vParticle[(*itv1)].DeltaR(vParticle[(*itv2)]);
	if(dr<DR){  p->push_back(*itv2); }
      }
    }
    if ( p->size() !=0 ) { nonIso[(*itv1)] = p;  }
    else{ delete p; vi_new->push_back(*itv1); }  
  }
  
}

void ObjectSelector::ProcessCleaning( 
				     vector<int>* va_old, vector<int>* a_remove, vector<int>* vb_old, vector<int>* b_remove,  
				     const vector<PhysicsObject>& aParticles, const vector<PhysicsObject>& bParticles, double DR ){  
  
  vector<int>::iterator itv1,itv2;
  
  for(itv1 = va_old->begin(); itv1 != va_old->end(); ++itv1){
    if( find(a_remove->begin(),a_remove->end(), *itv1 ) ==  a_remove->end() ){
      
      
      for( itv2 = vb_old->begin(); itv2 != vb_old->end(); ++itv2){
	
	if( find(b_remove->begin(),b_remove->end(), *itv2 ) ==  b_remove->end() ){
	  double dr =aParticles[(*itv1)].DeltaR(bParticles[(*itv2)]);
	  if(dr<DR){  a_remove->push_back(*itv1); }
	}
      }
    }
    
  }
  
}
 

void ObjectSelector::ApplyCleaning( vector<int>* i_old , vector<int>* i_remove, vector<int>* i_new){
  vector<int>::iterator itv1,itv2; 
  for(itv1 = i_old->begin(); itv1 != i_old->end(); ++itv1){
    if( find( i_remove->begin(), i_remove->end(), *itv1) == i_remove->end()){  i_new->push_back(*itv1); }
  }
}

void ObjectSelector::LeadElectronTriggerEfficiency(const vector<PhysicsObject>& electrons, const int& ind, double& electrontriggerefficiency){
  // Electron trigger/ID/Iso scale factors for efficiency from https://twiki.cern.ch/twiki/bin/view/CMS/KoPFAElectronTagAndProbe
  PhysicsObject electron = electrons[ind];
  
  double abseta = fabs( electron.Eta() );
  double pt = electron.Pt();
  
//   if (abseta < 0.8) {
//     if (pt>=30 && pt<40)    electrontriggerefficiency=0.987 /* +0.012-0.017 */ ;
//     if (pt>=40 && pt<50)    electrontriggerefficiency=0.997 /* +0.001-0.001 */ ;
//     if (pt>=50 && pt<200)   electrontriggerefficiency=0.998 /* +0.002-0.002 */ ;
//   }
  
//   if (abseta >= 0.8 && abseta < 1.478) {
//     if (pt>=30 && pt<40)    electrontriggerefficiency=0.964 /* +0.002-0.001 */ ;
//     if (pt>=40 && pt<50)    electrontriggerefficiency=0.980 /* +0.001-0.001 */ ;
//     if (pt>=50 && pt<200)   electrontriggerefficiency=0.988 /* +0.002-0.002 */ ;
//   }
  
//   if (abseta >= 1.478 && abseta < 2.5) {
//     if (pt>=30 && pt<40)    electrontriggerefficiency=1.004 /* +0.006-0.006 */ ;
//     if (pt>=40 && pt<50)    electrontriggerefficiency=1.033 /* +0.007-0.007 */ ;
//     if (pt>=50 && pt<200)   electrontriggerefficiency=0.976 /* +0.015-0.012 */ ;
//   }
 
  if (abseta < 0.8) {
    if (pt>=30 && pt<40)    electrontriggerefficiency=0.964 /* +0.012-0.017 */ ;
    if (pt>=40 && pt<50)    electrontriggerefficiency=0.976 /* +0.001-0.001 */ ;
    if (pt>=50)   electrontriggerefficiency=0.977 /* +0.002-0.002 */ ;
  }
  
  if (abseta >= 0.8 && abseta < 1.442) {
    if (pt>=30 && pt<40)    electrontriggerefficiency=0.941 /* +0.002-0.001 */ ;
    if (pt>=40 && pt<50)    electrontriggerefficiency=0.963 /* +0.001-0.001 */ ;
    if (pt>=50)   electrontriggerefficiency=0.964 /* +0.002-0.002 */ ;
  }
  
  if (abseta >= 1.556 && abseta < 2.) {
    if (pt>=30 && pt<40)    electrontriggerefficiency=0.919 /* +0.006-0.006 */ ;
    if (pt>=40 && pt<50)    electrontriggerefficiency=0.958 /* +0.007-0.007 */ ;
    if (pt>=50)   electrontriggerefficiency=0.966 /* +0.015-0.012 */ ;
  }

  if (abseta >= 2. && abseta < 2.5) {
    if (pt>=30 && pt<40)    electrontriggerefficiency=0.972 /* +0.006-0.006 */ ;
    if (pt>=40 && pt<50)    electrontriggerefficiency=0.978 /* +0.007-0.007 */ ;
    if (pt>=50)   electrontriggerefficiency=0.98 /* +0.015-0.012 */ ;
  }
 
}


void ObjectSelector::LeadElectronIDIsoEfficiency(const vector<PhysicsObject>& electrons, const int& ind, double& electronidisoefficiency){
  // Electron trigger/ID/Iso scale factors for efficiency from https://twiki.cern.ch/twiki/bin/view/CMS/KoPFAElectronTagAndProbe
  PhysicsObject electron = electrons[ind];
  
  double abseta = fabs( electron.Eta() );
  double pt = electron.Pt();
  
  if (abseta < 0.8) {
    if (pt>=20 && pt<30)    electronidisoefficiency=0.949 /* +0.007-0.007 */ ;
    if (pt>=30 && pt<40)    electronidisoefficiency=0.939 /* +0.003-0.003 */ ;
    if (pt>=40 && pt<50)    electronidisoefficiency=0.950 /* +0.001-0.001 */ ;
    if (pt>=50 && pt<200)   electronidisoefficiency=0.957 /* +0.001-0.001 */ ;
  }
  
  if (abseta >= 0.8 && abseta < 1.478) {
    if (pt>=20 && pt<30)    electronidisoefficiency=0.900 /* +0.010-0.009 */ ;
    if (pt>=30 && pt<40)    electronidisoefficiency=0.920 /* +0.002-0.000 */ ;
    if (pt>=40 && pt<50)    electronidisoefficiency=0.949 /* +0.002-0.002 */ ;
    if (pt>=50 && pt<200)   electronidisoefficiency=0.959 /* +0.003-0.003 */ ;
  }
  
  if (abseta >= 1.478 && abseta < 2.5) {
    if (pt>=20 && pt<30)    electronidisoefficiency=0.857 /* +0.009-0.010 */ ;
    if (pt>=30 && pt<40)    electronidisoefficiency=0.907 /* +0.005-0.005 */ ;
    if (pt>=40 && pt<50)    electronidisoefficiency=0.937 /* +0.008-0.008 */ ;
    if (pt>=50 && pt<200)   electronidisoefficiency=0.954 /* +0.011-0.010 */ ;
  }
 
}


void ObjectSelector::LeadMuonTriggerEfficiency(const vector<PhysicsObject>& muons, const int& ind, double& muontriggerefficiency){
  // Muon trigger/ID/Iso scale factors for efficiency are taken from https://twiki.cern.ch/twiki/bin/viewauth/CMS/MuonReferenceEffs
  // FIXME: implement non-pt dependent and implement non-fabs dependent
  PhysicsObject muon = muons[ind];

  double abseta = fabs( muon.Eta() );
  double pt = muon.Pt();

  // TOTAL // pt>500 assumed flat and uses 140.<pt<500. SF.                                                                                                                              
  if(abseta>=0. && abseta <0.9){ // ABCD                                                                                                                                                 
    if(pt>=140. /*&& pt<500.*/) muontriggerefficiency=0.98041749810533507;
    if(pt>=25.  && pt<30.)      muontriggerefficiency=0.98372524384334614;
    if(pt>=30.  && pt<35.)      muontriggerefficiency=0.98406344315477012;
    if(pt>=35.  && pt<40.)      muontriggerefficiency=0.98391658181685537;
    if(pt>=40.  && pt<50.)      muontriggerefficiency=0.98345252700570363;
    if(pt>=50.  && pt<60.)      muontriggerefficiency=0.98429177039157478;
    if(pt>=60.  && pt<90.)      muontriggerefficiency=0.98467201842489449;
    if(pt>=90.  && pt<140.)     muontriggerefficiency=0.98091711658069591;
  }
  if(abseta>=0.9 && abseta <1.2){ // ABCD                                                                                                                                                
    if(pt>=140. /*&& pt<500.*/) muontriggerefficiency=0.97127896196175556;
    if(pt>=25.  && pt<30.)      muontriggerefficiency=0.96838127559931908;
    if(pt>=30.  && pt<35.)      muontriggerefficiency=0.96538054889610103;
    if(pt>=35.  && pt<40.)      muontriggerefficiency=0.96696514151670487;
    if(pt>=40.  && pt<50.)      muontriggerefficiency=0.96667958160832501;
    if(pt>=50.  && pt<60.)      muontriggerefficiency=0.96273957552501865;
    if(pt>=60.  && pt<90.)      muontriggerefficiency=0.95952416834753307;
    if(pt>=90.  && pt<140.)     muontriggerefficiency=0.96444182461126438;
  }
  if(abseta>=1.2 && abseta <2.1){ // ABCD                                                                                                                                                
    if(pt>=140. /*&& pt<500.*/) muontriggerefficiency=0.99416866829048334;
    if(pt>=25.  && pt<30.)      muontriggerefficiency=1.0051991254438037;
    if(pt>=30.  && pt<35.)      muontriggerefficiency=1.0013781590159485;
    if(pt>=35.  && pt<40.)      muontriggerefficiency=0.99616640424792002;
    if(pt>=40.  && pt<50.)      muontriggerefficiency=0.99425410141043047;
    if(pt>=50.  && pt<60.)      muontriggerefficiency=0.99054467301217797;
    if(pt>=60.  && pt<90.)      muontriggerefficiency=0.98829374192885855;
    if(pt>=90.  && pt<140.)     muontriggerefficiency=0.98187598993908232;
  }

  
  
  /* Naked values

  // RUN A
  if(abseta>=0. && abseta <0.9){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta<0.9_2012A']
  {'140_500':'data/mc': {'efficiency_ratio': 1.0039855865250082,                         'err_hi': 0.022499183084688845,                         'err_low': 0.029995941945261614,
  '25_30'  'data/mc': {'efficiency_ratio': 0.93813730368945492,                       'err_hi': 0.0044022371718614199,                       'err_low': 0.0044354815653162025,   
  '30_35':  'data/mc': {'efficiency_ratio': 0.94234693090055577,                       'err_hi': 0.0028007879009532481,                       'err_low': 0.0028208720486102776,
  '35_40':  'data/mc': {'efficiency_ratio': 0.9511673822775496,                       'err_hi': 0.0019217206048403684,                       'err_low': 0.0019351915905614554,
  '40_50':  'data/mc': {'efficiency_ratio': 0.96009375278418141,                       'err_hi': 0.0011143882954913547,                       'err_low': 0.0011179007238696897,
  '50_60':  'data/mc': {'efficiency_ratio': 0.97343957235480161,                       'err_hi': 0.0024411350920320106,                       'err_low': 0.0024754942326703422,
  '60_90':  'data/mc': {'efficiency_ratio': 0.97573663054250859,                       'err_hi': 0.0035828589659443228,                       'err_low': 0.0036549437474093912,
  '90_140':  'data/mc': {'efficiency_ratio': 0.98029069760538523,                        'err_hi': 0.012366158027736435,                        'err_low': 0.013260673665312696,
  
  
  }
  if(abseta>=0.9 && abseta <1.2){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta0.9-1.2_2012A']
  {'140_500':  'data/mc': {'efficiency_ratio': 0.87925933693202241,                         'err_hi': 0.09659576917260633,                         'err_low': 0.11584311792663776,
  '25_30':  'data/mc': {'efficiency_ratio': 0.94044745125239582,                       'err_hi': 0.0091767215066666675,                       'err_low': 0.0092635075891679113,
  '30_35':  'data/mc': {'efficiency_ratio': 0.94175636773857052,                       'err_hi': 0.0068800427455579663,                       'err_low': 0.0069361952719798189,
  '35_40':  'data/mc': {'efficiency_ratio': 0.96184390382781271,                       'err_hi': 0.0050058891385179978,                       'err_low': 0.0050470262459905374,
  '40_50':  'data/mc': {'efficiency_ratio': 0.95880861862761002,                       'err_hi': 0.0028604289424100783,                       'err_low': 0.0028768404443189955,
  '50_60':  'data/mc': {'efficiency_ratio': 0.94396680918599263,                       'err_hi': 0.0069651492894596927,                       'err_low': 0.0070602979471116618,
  '60_90':  'data/mc': {'efficiency_ratio': 0.94402326294141492,                       'err_hi': 0.01060753235806736,                       'err_low': 0.010818706427484965,
  '90_140':  'data/mc': {'efficiency_ratio': 0.98555155845578823,                        'err_hi': 0.029074568385132889,                        'err_low': 0.031287356179248285,
  
  }
  if(abseta>=1.2 && abseta <2.1){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta1.2-2.1_2012A']
  {'140_500':  'data/mc': {'efficiency_ratio': 1.0276661017688036,                         'err_hi': 0.058613958853563478,                         'err_low': 0.071021543276337876,
  '25_30':  'data/mc': {'efficiency_ratio': 0.98008815033950636,                       'err_hi': 0.0058749122809747352,                       'err_low': 0.0059037530583840132,
  '30_35':  'data/mc': {'efficiency_ratio': 0.98934704759544734,                       'err_hi': 0.0045785677725535805,                       'err_low': 0.0046011603775876582,
  '35_40':  'data/mc': {'efficiency_ratio': 0.98463970628770603,                       'err_hi': 0.0036470820310153121,                       'err_low': 0.003663238973677829,
  '40_50':   'data/mc': {'efficiency_ratio': 0.97977118571541111,                       'err_hi': 0.002256573606508046,                       'err_low': 0.0022619617032217866,
  '50_60':  'data/mc': {'efficiency_ratio': 0.98223280251110145,                       'err_hi': 0.0053736555765784471,                       'err_low': 0.0054196536658200579,
  '60_90':  'data/mc': {'efficiency_ratio': 0.96524157990770376,                       'err_hi': 0.0083434128724738637,                       'err_low': 0.0084409501663191694,
  '90_140':  'data/mc': {'efficiency_ratio': 0.97760874126163566,                        'err_hi': 0.029743004651914488,                        'err_low': 0.03121370649559679,
  }
  
  
  
  
  // RUN B
  if(abseta>=0. && abseta <0.9){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta<0.9_2012B']
  {'140_500':  'data/mc': {'efficiency_ratio': 0.97440457563913907,                         'err_hi': 0.014256560782020932,                         'err_low': 0.015275754946814704,
  '25_30':  'data/mc': {'efficiency_ratio': 0.97885805202380882,                       'err_hi': 0.0019851847371091013,                       'err_low': 0.0019931662231116409,
  '30_35':  'data/mc': {'efficiency_ratio': 0.97980323349334553,                       'err_hi': 0.0012453102988839982,                       'err_low': 0.0012497557832992071,
  '35_40':  'data/mc': {'efficiency_ratio': 0.97975530836897795,                       'err_hi': 0.00085702693603589158,                       'err_low': 0.00086003584517788925,
  '40_50':   'data/mc': {'efficiency_ratio': 0.97968142272864767,                       'err_hi': 0.0004943362934782293,                       'err_low': 0.00024819327312176692,
  '50_60':  'data/mc': {'efficiency_ratio': 0.97870686775661053,                       'err_hi': 0.0011392030775206204,                       'err_low': 0.001146432417345856,
  '60_90':  'data/mc': {'efficiency_ratio': 0.98110330744549257,                       'err_hi': 0.0016872169812545313,                       'err_low': 0.0017032973425868255,
  '90_140':  'data/mc': {'efficiency_ratio': 0.98042031652949257,                        'err_hi': 0.0056548504354232837,                        'err_low': 0.0058207886942260225,
  }
  if(abseta>=0.9 && abseta <1.2){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta0.9-1.2_2012B']
  {'140_500':  'data/mc': {'efficiency_ratio': 0.95360674495560072,                         'err_hi': 0.045847507937714443,                         'err_low': 0.050015414181808653,
  '25_30':  'data/mc': {'efficiency_ratio': 0.97007008718709498,                       'err_hi': 0.0043723050165507121,                       'err_low': 0.0043928231586872497,
  '30_35':  'data/mc': {'efficiency_ratio': 0.96269379501996166,                       'err_hi': 0.0031856425422058347,                       'err_low': 0.003198565152114299,
  '35_40':  'data/mc': {'efficiency_ratio': 0.96023531079758973,                       'err_hi': 0.00244009260220135,                       'err_low': 0.0024487492905615532,
  '40_50':  'data/mc': {'efficiency_ratio': 0.96214880574476191,                       'err_hi': 0.0013954557209484328,                       'err_low': 0.001401656713597709,
  '50_60':  'data/mc': {'efficiency_ratio': 0.95908612627283873,                       'err_hi': 0.0033425420704125655,                       'err_low': 0.0033631128064578282,
  '60_90':  'data/mc': {'efficiency_ratio': 0.95217276446962162,                       'err_hi': 0.0050683327810917035,                       'err_low': 0.0051125913184399303,
  '90_140':  'data/mc': {'efficiency_ratio': 0.94918636153681046,                        'err_hi': 0.016794425935082069,                        'err_low': 0.01726967001934817,
  }
  if(abseta>=1.2 && abseta <2.1){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta1.2-2.1_2012B']
  {'140_500':  'data/mc': {'efficiency_ratio': 0.98286615824757273,                         'err_hi': 0.043720961605029501,                         'err_low': 0.046117181616762426,
  '25_30':  'data/mc': {'efficiency_ratio': 0.99501274847385413,                       'err_hi': 0.0028283579570503891,                       'err_low': 0.0028340040981053908,
  '30_35':  'data/mc': {'efficiency_ratio': 0.98649349917357299,                       'err_hi': 0.002233236508377224,                       'err_low': 0.0022372876535592299,
  '35_40':  'data/mc': {'efficiency_ratio': 0.98149970931934127,                       'err_hi': 0.0018194402833553954,                       'err_low': 0.0018227440996326791,
  '40_50':  'data/mc': {'efficiency_ratio': 0.97870186746514398,                       'err_hi': 0.0011350367205615391,                       'err_low': 0.0011358914921148053,
  '50_60':  'data/mc': {'efficiency_ratio': 0.97745007778784843,                       'err_hi': 0.0027039814950265023,                       'err_low': 0.0027130658944350679,
  '60_90':  'data/mc': {'efficiency_ratio': 0.97572759209438797,                       'err_hi': 0.0040970458403296215,                       'err_low': 0.0041195117858496581,
  '90_140':  'data/mc': {'efficiency_ratio': 0.95520425403413389,                        'err_hi': 0.013386396743115671,                        'err_low': 0.01362169423675119,
  }
  
  
  //RUN C
  if(abseta>=0. && abseta <0.9){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta<0.9']
  {'140_500':  'data/mc': {'efficiency_ratio': 0.98331488030612646,                         'err_hi': 0.0121594366990965,                         'err_low': 0.013165510795077145,
  '25_30':  'data/mc': {'efficiency_ratio': 0.98605163783412098,                       'err_hi': 0.0013942452628413233,                       'err_low': 0.0014020800585459675,
  '30_35':  'data/mc': {'efficiency_ratio': 0.98551473615861951,                       'err_hi': 0.00090516284233322665,                       'err_low': 0.00090956689404318822,
  '35_40':  'data/mc': {'efficiency_ratio': 0.98494669206714758,                       'err_hi': 0.00064516806298976249,                       'err_low': 0.00064877503730224092,
  '40_50':  'data/mc': {'efficiency_ratio': 0.98321323751374934,                       'err_hi': 0.00053949010189997511,                       'err_low': 0.00053949010189997511,
  '50_60':  'data/mc': {'efficiency_ratio': 0.98380139407493405,                       'err_hi': 0.00091790757839555306,                       'err_low': 0.00092294379257880062,
  '60_90':  'data/mc': {'efficiency_ratio': 0.98339711419280051,                       'err_hi': 0.0013854763027156304,                       'err_low': 0.0013967579304166784,
  '90_140':  'data/mc': {'efficiency_ratio': 0.97708648648494312,                        'err_hi': 0.0048207587940254258,                        'err_low': 0.0049507189011278388,
  }
  if(abseta>=0.9 && abseta <1.2){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta0.9-1.2']
  {'140_500':  'data/mc': {'efficiency_ratio': 1.014598415522407,                         'err_hi': 0.038009914795107277,                         'err_low': 0.041868767454545423,
  '25_30':  'data/mc': {'efficiency_ratio': 0.97610027144634715,                       'err_hi': 0.0035318036056163978,                       'err_low': 0.0035499948993381342,
  '30_35':  'data/mc': {'efficiency_ratio': 0.96957505872620087,                       'err_hi': 0.0026421144608112968,                       'err_low': 0.0026538760490568795,
  '35_40':  'data/mc': {'efficiency_ratio': 0.97172722186989646,                       'err_hi': 0.0020470584115986497,                       'err_low': 0.0020547205420876338,
  '40_50':  'data/mc': {'efficiency_ratio': 0.96855088549646673,                       'err_hi': 0.0009274726479718379,                       'err_low': 0.00093428028163270881,
  '50_60':  'data/mc': {'efficiency_ratio': 0.96689267029367632,                       'err_hi': 0.0028357838374024179,                       'err_low': 0.0028515267995841602,
  '60_90':  'data/mc': {'efficiency_ratio': 0.95999872287817567,                       'err_hi': 0.0042727494870127766,                       'err_low': 0.0043027753489072789,
  '90_140':  'data/mc': {'efficiency_ratio': 0.95932231979340665,                        'err_hi': 0.014274024008399628,                        'err_low': 0.014685692866276922,
  }
  if(abseta>=1.2 && abseta <2.1){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta1.2-2.1']
  '140_500':  'data/mc': {'efficiency_ratio': 0.99805498588548336,                         'err_hi': 0.036102881278942782,                         'err_low': 0.040416111226777102,
  '25_30':  'data/mc': {'efficiency_ratio': 1.0091911468596282,                       'err_hi': 0.0024097844241964205,                       'err_low': 0.0024154834432019187,
  '30_35':  'data/mc': {'efficiency_ratio': 1.0051776462431439,                       'err_hi': 0.0018841769147249416,                       'err_low': 0.0018875065748520203,
  '35_40':  'data/mc': {'efficiency_ratio': 1.0024159561631731,                       'err_hi': 0.0015550332829383185,                       'err_low': 0.0015576645386781234,
  '40_50':   'data/mc': {'efficiency_ratio': 1.0016974912730539,                       'err_hi': 0.00097388377843326982,                       'err_low': 0.00092317537162422069,
  '50_60':  'data/mc': {'efficiency_ratio': 0.99924439802206022,                       'err_hi': 0.0022931577193718157,                       'err_low': 0.0023007860598461011,
  '60_90':  'data/mc': {'efficiency_ratio': 0.99415925403956484,                       'err_hi': 0.0034877157906004035,                       'err_low': 0.0035051608536696482,
  '90_140':  'data/mc': {'efficiency_ratio': 1.0018625421366663,                        'err_hi': 0.011647383620774592,                        'err_low': 0.011843439025869167,
  }
  // RUN D
  if(abseta>=0. && abseta <0.9){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta<0.9_2012D']
  if(pt>=140.  && pt<500. )      muontriggerefficiency=0.98601151721013425; // 'data/mc': {'efficiency_ratio': 0.98601151721013425, 'err_hi': 0.013062714238057683, 'err_low': 0.014073601453910352, 
  if(pt>=25. && pt<30.)              muontriggerefficiency=0.97914496622610225; // 'data/mc': {'efficiency_ratio': 0.97914496622610225, 'err_hi': 0.0018008858554421543, 'err_low': 0.001806929370962707,
  if(pt>=30. && pt<35.)              muontriggerefficiency=0.98019485553878438; // 'data/mc': {'efficiency_ratio': 0.98019485553878438, 'err_hi': 0.0011196120617616768, 'err_low': 0.0011229899299581472,
  if(pt>=35. && pt<40.)              muontriggerefficiency=0.98121815376364718; // 'data/mc': {'efficiency_ratio': 0.98121815376364718, 'err_hi': 0.0007626607730832082, 'err_low': 0.00076461153531109168,
  if(pt>=40. && pt<50.)              muontriggerefficiency=0.98231741596833355; // 'data/mc': {'efficiency_ratio': 0.98231741596833355, 'err_hi': 0.078803443945036702, 'err_low': 0.00036329303125574918,
  if(pt>=50. && pt<60.)	       muontriggerefficiency=0.98179265165696761; // 'data/mc': {'efficiency_ratio': 0.98179265165696761, 'err_hi': 0.00099006266924005432, 'err_low': 0.00099558740718984763,
  if(pt>=60. && pt<90.)	       muontriggerefficiency=0.98526154045830983; // 'data/mc': {'efficiency_ratio': 0.98526154045830983, 'err_hi': 0.0014681423182645698, 'err_low': 0.0014806677503566158,
  if(pt>=90. && pt<140.)	       muontriggerefficiency=0.98529848220688254; // 'data/mc': {'efficiency_ratio': 0.98529848220688254, 'err_hi': 0.0048957147462200925, 'err_low': 0.0050279411182655599,
  }
  if(abseta>=0.9 && abseta <1.2){// dict['IsoMu24_eta2p1_TightIso']['ptabseta0.9-1.2_2012D']
  if(pt>=140. && pt<500. )       muontriggerefficiency=0.99289045846971669; // 'data/mc': {'efficiency_ratio': 0.99289045846971669, 'err_hi': 0.034047432416510044,   'err_low': 0.037621730879919629,
  if(pt>=25. && pt<30.)              muontriggerefficiency=0.96261309489587965; // 'data/mc': {'efficiency_ratio': 0.96261309489587965, 'err_hi': 0.0039150467122432073, 'err_low': 0.0039295292214824992,
  if(pt>=30. && pt<35.)              muontriggerefficiency=0.95900676188305922; // 'data/mc': {'efficiency_ratio': 0.95900676188305922, 'err_hi': 0.0028826197641733985, 'err_low': 0.0028917465174494511,
  if(pt>=35. && pt<40.)              muontriggerefficiency=0.96179985528446077; // 'data/mc': {'efficiency_ratio': 0.96179985528446077, 'err_hi': 0.0021886297596684532, 'err_low': 0.002195247383850036,
  if(pt>=40. && pt<50.)              muontriggerefficiency=0.96293779887804887; // 'data/mc': {'efficiency_ratio': 0.96293779887804887, 'err_hi': 0.0012496849632170402, 'err_low': 0.0012519048627461281,
  if(pt>=50. && pt<60.)              muontriggerefficiency=0.96029757743629218; // 'data/mc': {'efficiency_ratio': 0.96029757743629218, 'err_hi': 0.0029620477233797118, 'err_low': 0.0029778291697058414,
  if(pt>=60. && pt<90.)              muontriggerefficiency=0.95672179799515245; // 'data/mc': {'efficiency_ratio': 0.95672179799515245, 'err_hi': 0.0045008915161213444, 'err_low': 0.0045338834116438118,
  if(pt>=90. && pt<140.)             muontriggerefficiency=0.9623387481411757 ; // 'data/mc': {'efficiency_ratio': 0.9623387481411757,  'err_hi': 0.014502317240007626,  'err_low': 0.014870736802545031,
  }
  if(abseta>=1.2 && abseta <2.1){ // dict['IsoMu24_eta2p1_TightIso']['ptabseta1.2-2.1_2012D']
  if(pt>=140. && pt<500.)            muontriggerefficiency=0.94409610419248469; // 'data/mc': {'efficiency_ratio': 0.94409610419248469, 'err_hi': 0.03915721520872615,   'err_low': 0.040781109792725333,
  if(pt>=25. && pt<30.)              muontriggerefficiency=1.0047243862175275;  // 'data/mc': {'efficiency_ratio': 1.0047243862175275,  'err_hi': 0.0025421804311439788, 'err_low': 0.0025460048398088719,
  if(pt>=30. && pt<35.)              muontriggerefficiency=1.0019383989167374;  // 'data/mc': {'efficiency_ratio': 1.0019383989167374,  'err_hi': 0.0019821971393773671, 'err_low': 0.0019852404459558166,
  if(pt>=35. && pt<40.)              muontriggerefficiency=0.99821177731560617; // 'data/mc': {'efficiency_ratio': 0.99821177731560617, 'err_hi': 0.0016227869401149063, 'err_low': 0.0016253110660227157,
  if(pt>=40. && pt<50.)              muontriggerefficiency=0.99658256171319515; // 'data/mc': {'efficiency_ratio': 0.99658256171319515, 'err_hi': 0.001005524945783521,  'err_low': 0.0010073667951948028,
  if(pt>=50. && pt<60.)              muontriggerefficiency=0.99109764417153334; // 'data/mc': {'efficiency_ratio': 0.99109764417153334, 'err_hi': 0.002383366530969413,  'err_low': 0.0023910677931727117,
  if(pt>=60. && pt<90.)              muontriggerefficiency=0.98379899771732682; // 'data/mc': {'efficiency_ratio': 0.98379899771732682, 'err_hi': 0.0035900842092497962, 'err_low': 0.0036072104737469349,
    if(pt>=90. && pt<140.)             muontriggerefficiency=0.98342842226423999; // 'data/mc': {'efficiency_ratio': 0.98342842226423999, 'err_hi': 0.011993403076338283,  'err_low': 0.012173910417085737,
    }

    //fn   abseta<1.442 || abseta>1.566 e --> tau fake
    
  */ 

//   // TOTAL // pt>500 assumed flat and uses 140.<pt<500. SF.
//   if(abseta>=0. && abseta <0.9){ // ABCD
//     if(pt>=25. && pt<30.)              muontriggerefficiency=1.04811; //0.979543;
//     if(pt>=30. && pt<35.)              muontriggerefficiency=1.04876; //0.980155;
//     if(pt>=35. && pt<40.)              muontriggerefficiency=1.01997; //0.980745;
//     if(pt>=40. && pt<50.)              muontriggerefficiency=1.02018; //0.980943;
//     if(pt>=50. && pt<60.)	       muontriggerefficiency=0.981394;
//     if(pt>=60. && pt<90.)	       muontriggerefficiency=0.983171;
//     if(pt>=90. && pt<140.)	       muontriggerefficiency=0.931941; //0.98099 ;
//     if(pt>=140. && pt<250.)            muontriggerefficiency=0.884933; //0.983259;
//     if(pt>=250.)                   muontriggerefficiency=0.786607; //0.983259;
//   }
//   if(abseta>=0.9 && abseta <1.2){ // ABCD
//     if(pt>=25. && pt<30.)              muontriggerefficiency=1.03583; //0.968072;
//     if(pt>=30. && pt<35.)              muontriggerefficiency=1.03019; //0.962795;
//     if(pt>=35. && pt<40.)              muontriggerefficiency=1.00359; //0.964999;
//     if(pt>=40. && pt<50.)              muontriggerefficiency=1.00314; //0.964565;
//     if(pt>=50. && pt<60.)              muontriggerefficiency=0.961581;
//     if(pt>=60. && pt<90.)              muontriggerefficiency=0.956228;
//     if(pt>=90. && pt<140.)             muontriggerefficiency=0.911404; //0.959373;
//     if(pt>=140. && pt<250. )           muontriggerefficiency=0.88746;  //0.986067;
//     if(pt>=250.)                   muontriggerefficiency=0.788853; //0.986067;
//   }
//   if(abseta>=1.2 && abseta <2.1){ // ABCD
//     if(pt>=140. /* && pt<250. */)      muontriggerefficiency=0.976435;
//     if(pt>=25. && pt<30.)              muontriggerefficiency=1.00288 ;
//     if(pt>=30. && pt<35.)              muontriggerefficiency=0.998932;
//     if(pt>=35. && pt<40.)              muontriggerefficiency=0.995211;
//     if(pt>=40. && pt<50.)              muontriggerefficiency=0.993481;
//     if(pt>=50. && pt<60.)              muontriggerefficiency=0.990445;
//     if(pt>=60. && pt<90.)              muontriggerefficiency=0.984746;
//     if(pt>=90. && pt<140.)             muontriggerefficiency=0.98326 ;
//   }

}









