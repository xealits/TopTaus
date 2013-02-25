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
  ZMAX_    = 1;
  ZTAUMAX_ = 0.2;
  ///////////////
  
  // Jets ///////////////////////////////////////////
  JET_PT_MIN_        = 30; // WARNING : Jet pt set to 20 for trigger studies
  //JET_PT_MIN_        = 30; 
  JET_ETA_MAX_       = 2.4;       // dilepton is 2.5 
  JET_EMF_MIN_       = 0.01;       
  JET_LTK_           = 5.;
  JET_LEPTON_DRMIN_  = 0.3;
  applyJetLtkCut_    = false;
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
  M_RELISO_MAX_  = 0.2; //0.05;         // dilepton is 0.15
  M_PT_MIN_      = 30; //30;  //WARNING M_PT_MIN_      = 20;               // dilepton is 20 // test on 70
  M_ETA_MAX_     = 2.1;                 // dilepton is 2.5
  M_D0_MAX_      = 0.02;                // dilepton is 0.02
  
  LOOSE_M_RELISO_MAX_ = 0.2; 
  LOOSE_M_ETA_MAX_    = 2.5; 
  LOOSE_M_PT_MIN_     = 10;
  RHO_AEFF_M_         = 0.112;
  
  if(commondefinitions::run2012_){
    M_RELISO_MAX_  = 0.1; //cone 0.4
  }
  ///////////////////////////////////////////////////
  
  
  // taus /////////////////////////////////////////////////////// 
  TAU_PF_LEADTRKPT_MIN_ = 5;  // WARNING 5;
  TAU_LEADPFPT_MIN_     = 10; // WARNING 10;  // lead track on pt
  TAU_ETA_MAX_          = 2.4; 
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
  applyJetLtkCut_    = false;
  JET_LEPTON_DRMIN_  = 0.4;
  
  
  // electron/////////////////
  E_RELISO_MAX_       = 0.15;  
  E_PT_MIN_           = 20;    
  E_D0_MAX_           = 0.04; 
  //////////////////////////// 
  
  // muon //////////////
  M_RELISO_MAX_  = 0.15; 
  M_ETA_MAX_     = 2.4;
  /////////////////////
  
}

bool ObjectSelector::LooseElectronVeto(event::Reader* reader, int selectedElectron, const vector<PhysicsObject>& vE){
  bool looseVeto(false);
  for( int i=0;i< (int) vE.size();i++ ){
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
  
  for(int i=0;i< (int)vM.size();i++){
    
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
  
  for(unsigned int i=0;i<vE.size();i++){
    
    const PhysicsObject * e = &vE[i]; 
    double scEta    = (*e)[39];  
    double eEta     = TMath::Abs(e->Eta()); 
    double eEt      = TMath::Abs(e->Et());
    double ePt      = TMath::Abs(e->Pt());
    double eRelIso  = (*e)[18];
    double minDR2mu = (*e)[14];
    double d0       = fabs((*e)[25]);
    bool   id       = reader->ElectronId((PhysicsObject &)(*e), 1);    //Tight working point
    //bool   id       = reader->ElectronId((PhysicsObject &)(*e), 0);  //Loose working point
    
    //Super TightId
    int v = (*e)[12];
    bool superTightId  =   (v >> 4) & 0x1 ; id = id && superTightId ;
    //cout<<endl<<" electron id : "<<v<<" very loose : "<<(v&0x1)<<" loose : "<<((v>>1)&0x1)<<" medium : "<<((v>>2)&0x1)<<" tight "<<((v>>3)&0x1)<<endl;
    
    
    double zvertex   = vertex[7];
    double zelectron = (*e)[29];
    bool   z(false); 
    if ( fabs(zvertex-zelectron) < ZMAX_ ){ z = true;}
    if(!z){ continue;}
    
    if( leptonPlusJetsSelection_  && ( fabs(scEta)> 1.4442 && fabs(scEta)<1.5660)  ){ continue; }
    if( leptonPlusJetsSelection_  && eEt < E_ET_MIN_ ){ continue; }
    if( !leptonPlusJetsSelection_ && ePt < E_PT_MIN_ ){ continue; }
    
    
    if(id && d0 < E_D0_MAX_ && eEta < E_ETA_MAX_ && eRelIso < E_RELISO_MAX_ && minDR2mu >0.1){ e_i->push_back(i);}
    
  }
  
}

void ObjectSelector::PreSelectMuons( event::Reader* reader, vector<int>* m_i, const vector<PhysicsObject>& vM, PhysicsObject& vertex) {
  
  for( int i=0;i< (int) vM.size();i++){
    
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
  
  for(unsigned int i=0;i<vJ.size();i++){
    
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

void ObjectSelector::PreSelectTaus( vector<int>* t_i, const vector<PhysicsObject>& vT, int requiredProngs, TString myKey, PhysicsObject& vertex){
  
  // cout<<endl<<" tau collection size is : "<<vT.size()<<endl;
  
  bool applyTanc(false);
  bool applyHPS(false);
  
  if(      myKey =="TaNC0"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_ONE;    }
  else if( myKey =="TaNC1"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_HALF;   }
  else if( myKey =="TaNC2"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_QUARTER;} 
  else if( myKey =="TaNC3"   ){ applyTanc=true; TANC_FRDISCRIMINATOR_ = TANCFR_TENTH;  } 
  else if( myKey =="HPS0"    ){ applyHPS =true; HPS_ISO_ = HPS_LOOSEISO;   } 
  else if( myKey =="HPS1"    ){ applyHPS =true; HPS_ISO_ = HPS_MEDIUMISO;  } 
  else if( myKey =="HPS2"    ){ applyHPS =true; HPS_ISO_ = HPS_TIGHTISO;   } 
  //else if( myKey =="PFlow" ){ applyHPS =true; HPS_ISO_ = HPS_LOOSEISO;  }  // LOOSE WORKING POINT WARNING
  else if( myKey =="PFlow"   ){ applyHPS =true; HPS_ISO_ = HPS_MEDIUMISO; }  // MEDIUM WORKING POINT
  
  
  for(unsigned int i=0;i<vT.size();i++){
    
    
    double tauEta            = TMath::Abs(vT[i].Eta());
    double tauPt             = TMath::Abs(vT[i].Pt());
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
    int isobits              = vT[i][20];  int tau_dis_iso =  isobits & 0x1; 
    
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
    
    
    // Require discriminators on leptons and tau pt and eta ///////////////////////////////////////////////////////////////////////
    //if( tau_dis_muon == 0 || tau_dis_electron == 0 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;    // LooseLoose
    if( tau_dis_muon == 0 || tau_dis_electron < 2 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;        //LooseMedium
    //if( tau_dis_muon < 2 || tau_dis_electron < 3 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;       //TightTight
    if( tau_dis_muon < 3 || tau_dis_electron < 2 || tauPt < TAU_PT_MIN_ || tauEta > TAU_ETA_MAX_ ) continue;        //TightMedium
    // If putting electron descrimination against electron to tight no muons pass...  
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
      int discByIsolation = ((int) vT[i][HPS_ISO_]) & 0x1;
      
      
      
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
  
  
  for(unsigned int i=0;i<obj_input.size();i++){
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
  for(itv1 = vi_old->begin(); itv1 != vi_old->end(); itv1++){
    
    vector<int> * p = new vector<int>;
    
    for(itv2 = vi_old->begin(); itv2 != vi_old->end(); itv2++){ 
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
  
  for(itv1 = va_old->begin(); itv1 != va_old->end(); itv1++){
    if( find(a_remove->begin(),a_remove->end(), *itv1 ) ==  a_remove->end() ){
      
      
      for( itv2 = vb_old->begin(); itv2 != vb_old->end(); itv2++){
	
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
  for(itv1 = i_old->begin(); itv1 != i_old->end(); itv1++){
    if( find( i_remove->begin(), i_remove->end(), *itv1) == i_remove->end()){  i_new->push_back(*itv1); }
  }
}





