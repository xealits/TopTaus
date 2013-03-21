#include "LIP/TopTaus/interface/UncertaintyCalculator.hh"

#include "TRandom3.h"

double UncertaintyCalculator::jetMETScaling( vector<double> & jerFactors, double jes,JetCorrectionUncertainty * junc, vector<PhysicsObject> & vJ , double missetX, double missetY) {


  for (unsigned int i=0; i<vJ.size(); i++){

    double jerF(0);

    if(jerFactors.size() !=0) jerF = jerFactors[i];

    double jetPt  =   getJetPt(vJ[i] ,junc, jerF, jes );

    double varyPx = 0;
    double varyPy = 0;
    double varyPz = 0;

    missetX += vJ[i].Px(); 
    missetY += vJ[i].Py();
    varyPx   = (1+jes)*(vJ[i].Px());
    varyPy   = (1+jes)*(vJ[i].Py());
    varyPz   = (1+jes)*(vJ[i].Pz());
    // Propagate the scale variation to the z component to keep the eta constant
    TLorentzVector j(varyPx,varyPy,varyPz,pow( varyPx*varyPx + varyPy* varyPy + varyPz*varyPz ,0.5) );
    missetX -=  j.Px();
    missetY -=  j.Py();
  }
 
  double missEt =  pow(missetX*missetX + missetY*missetY , 0.5);

  return missEt;

}



double UncertaintyCalculator::jetMETUnclustered( vector<double> & jerFactors, PhysicsObject * obj, double unc, vector<PhysicsObject> & vJ, double missetX, double missetY){




  for (unsigned int i=0; i<vJ.size(); i++){
    if( jerFactors.size() != 0 ){    missetX += ( vJ[i][5] )*(vJ[i].Px())*jerFactors[i];  missetY += ( vJ[i][5] )*(vJ[i].Py())*jerFactors[i]; }
    else{                            missetX += ( vJ[i][5] )*(vJ[i].Px());                missetY += ( vJ[i][5] )*(vJ[i].Py());               }
  }
  
  missetX += obj->Px(); missetY += obj->Py();
  missetX *= (1+unc);   missetY *= (1+unc);
  missetX -= obj->Px(); missetY -= obj->Py();

  for (unsigned int i=0; i<vJ.size(); i++){
    if(jerFactors.size()!=0){ missetX -= (vJ[i][5])*(vJ[i].Px())*jerFactors[i]; missetY -= (vJ[i][5])*(vJ[i].Py())*jerFactors[i];}
    else{                     missetX -= (vJ[i][5])*(vJ[i].Px());               missetY -= (vJ[i][5])*(vJ[i].Py());              }
  }

  double missEt =  pow(missetX*missetX + missetY*missetY , 0.5);

  return missEt;

}



double UncertaintyCalculator::jetMETResolution( vector<double> & jerFactors , vector<PhysicsObject> & vJ, double missetX, double missetY){

 for (unsigned int i=0; i<vJ.size(); i++){

    missetX += ( vJ[i][5] )*(vJ[i].Px()); 
    missetY += ( vJ[i][5] )*(vJ[i].Py());
  }

 for (unsigned int i=0; i<vJ.size(); i++){
   if(jerFactors.size() != 0){
      missetX -= (vJ[i][5])*(vJ[i].Px())*jerFactors[i]; 
      missetY -= (vJ[i][5])*(vJ[i].Py())*jerFactors[i];
   }else{
     missetX -= (vJ[i][5])*(vJ[i].Px()); 
     missetY -= (vJ[i][5])*(vJ[i].Py());
   }
 }

  double missEt =  pow(missetX*missetX + missetY*missetY , 0.5);

  return missEt;


}

double UncertaintyCalculator::getJetPt( PhysicsObject &j ,JetCorrectionUncertainty * junc, double jerFactor, double jes ){

  double corr(0);
  double jetPt  = j.Pt();
  double jetEta = j.Eta();

  // jer factor ///////////////////////
  if(jerFactor) jetPt = jerFactor*jetPt;
  /////////////////////////////////////

  if(junc){
    if( jes ){ junc->setJetEta(jetEta); junc->setJetPt(jetPt); corr=junc->getUncertainty(true); }
    if( jes >0  ){ } else if( jes <0 ){ corr =(-1)*corr;}
  }
  else{
    if( jes ){ corr= sqrt( jes*jes +  ( (0.02)*jetEta *(0.02)*jetEta ) ) ;}
    if( jes > 0  ){ jes = corr;} else if( jes <0 ){ jes =(-1)*corr;}
  }
    
     
  //apply jes correction
  jetPt = (1+corr)*jetPt;
  //////////////////////

  return jetPt;

}


//MC smearing
PhysicsObject UncertaintyCalculator::smearedJet(const PhysicsObject &origJet, double genJetPt, int mode) // Perhaps it would be better to create a class or a namespace METUtils
{
  genJetPt = origJet[34];
  // genJet info:
  // info[34] = j->genJet()->pt();
  // info[35] = j->genJet()->eta();
  // info[36] = j->genJet()->phi();
   

  if(genJetPt<=0) return origJet;
  
  //smearing factors are described in https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
  //Moriond2013 values
  double eta=fabs(origJet.Eta());
  
  double ptSF(1.0), ptSF_err(0.06);
  if(eta<0.5) {
    ptSF=1.066;
    ptSF_err=sqrt(pow(0.007,2)+pow(0.5*(0.07+0.072),2)); 
  } 
  else if(eta>=0.5 && eta<1.7) {
    ptSF=1.191;
    ptSF_err=sqrt(pow(0.019,2)+pow(0.5*(0.06+0.062),2)); 
  }
  else if(eta>=1.7 && eta<2.3) { 
    ptSF=1.096;
    ptSF_err=sqrt(pow(0.030,2)+pow(0.5*(0.08+0.085),2)); 
  }
  else if(eta>=2.3 && eta<5.0) {
    ptSF=1.166;
    ptSF_err=sqrt(pow(0.050,2)+pow(0.5*(0.19+0.199),2)); 
  }
  
  
  if(mode==1) ptSF += ptSF_err; // JerUp
  if(mode==2) ptSF -= ptSF_err; // JerDown
  ptSF=max(0.,(genJetPt+ptSF*(origJet.Pt()-genJetPt)))/origJet.Pt();
  // Deterministic version (now the Recommended version)
  
  /// Random smearing is now NOT recommended
  ///double sin_phi = sin(origJet.Phi()*1000000);
  ///double seed = abs(static_cast<int>(sin_phi*100000));
  ///TRandom3 myRandom(seed);
  ///
  ///// A seed value of "0" means that the seed is authomatically calculated from memory data, garanteeing that it's unique in space and time, according to documentation of TRandom3
  ///// Still, the sinphi method is valid even if the TRandom3 convention changes eventually
  /////  TRandom3 myRandom(0); 
  ///
  ///ptSF=max(0.,(genJetPt+myRandom.Gaus(ptSF,ptSF_err)*(origJet.Pt()-genJetPt)))/origJet.Pt();
  /////deterministic version
  ///if(ptSF<=0) return origJet;

  double px(origJet.Px()*ptSF), py(origJet.Py()*ptSF),
    pz(origJet.Pz()), mass(origJet.M());
  double en = sqrt(mass*mass+px*px+py*py+pz*pz);
  
  PhysicsObject toReturn = origJet;
  //  toReturn.SetCoordinates(px, py, pz, en);
  toReturn.SetPtEtaPhiE(px, py, pz, en);
  
  return toReturn;
  
  //return new kinematics
  //    return LorentzVector(px,py,pz,en);
}
///
 //
///// Propagate met variation
///void computeVariation(vector<PhysicsObject>& jets, 
///		      vector<PhysicsObject> &leptons, // not really needed for now
///		      PhysicsObject& met, // 
///		      vector<PhysicsObjectCollection>& jetsVar, 
///		      vector<PhysicsObject>& metsVar,
///		      JetCorrectionUncertainty *jecUnc)
///// Old data types for input parameters  
/////  PhysicsObjectJetCollection& jets, 
/////  PhysicsObjectLeptonCollection &leptons,
/////  LorentzVector& met,
/////  std::vector<PhysicsObjectJetCollection>& jetsVar, 
/////  LorentzVectorCollection& metsVar,
/////  JetCorrectionUncertainty *jecUnc)
///{
///  jetsVar.clear();
///  metsVar.clear();
///  
///  //  int vars[]={JER, JER_UP, JER_DOWN, JES_UP, JES_DOWN, UMET_UP,UMET_DOWN,LES_UP,LES_DOWN};
///  int vars[]={UncertaintyCalculator::JER}; // support only JER for now
///  for(size_t ivar=0; ivar<sizeof(vars)/sizeof(int); ivar++)
///    {
///      PhysicsObjectCollection newJets;
///      PhysicsObject newMet(met),jetDiff(0,0,0,0),lepDiff(0,0,0,0), unclustDiff(0,0,0,0), clusteredFlux(0,0,0,0);
///      for(size_t ijet=0; ijet<jets.size(); ijet++)
///	{
///	  if(ivar==JER) // || ivar==JER_UP || ivar==JER_DOWN)
///	    {
///	      PhysicsObject iSmearJet=UncertaintyCalculator::smearedJet(jets[ijet],jets[ijet].genPt,ivar);
///	      jetDiff += (iSmearJet-jets[ijet]);
///	      newJets.push_back( iSmearJet );
///	    }
///	  //	  else if(ivar==JES_UP || ivar==JES_DOWN)
///	  //	    {
///	  //	      double varSign=(ivar==JES_UP ? 1.0 : -1.0 );
///	  //	      double jetScale(1.0);
///	  //	      try{
///	  //		jecUnc->setJetEta(jets[ijet].eta());
///	  //		jecUnc->setJetPt(jets[ijet].pt());
///	  //		jetScale = 1.0 + varSign*fabs(jecUnc->getUncertainty(true));  
///	  //	      }
///	  //	      catch(std::exception &e){
///	  //		cout << "[METUtils::computeVariation]" << e.what() << ijet << " " << jets[ijet].pt() << endl;
///	  //	      }
///	  //	      PhysicsObject_Jet iScaleJet(jets[ijet]);  iScaleJet *= jetScale;
///	  //	      jetDiff += (iScaleJet-jets[ijet]);
///	  //	      newJets.push_back(iScaleJet);
///	  //	    }
///	  //	  else if(ivar==UMET_UP || ivar==UMET_DOWN)  clusteredFlux += jets[ijet];
///	}
///      
///      //      if(ivar==UMET_UP || ivar==UMET_DOWN || ivar==LES_UP || ivar==LES_DOWN)
///      //	{
///      //	  for(size_t ilep=0; ilep<leptons.size(); ilep++)
///      //	    {
///      //	      if(ivar==UMET_UP || ivar==UMET_DOWN)  clusteredFlux +=leptons[ilep];
///      //	      else if(ivar==LES_UP  || ivar==LES_DOWN)
///      //		{
///      //		  LorentzVector iScaleLepton=leptons[ilep];
///      //		  double varSign=(ivar==LES_UP ? 1.0 : -1.0);
///      //		  if(fabs(leptons[ilep].id)==13)          iScaleLepton *= (1.0+varSign*0.01);
///      //		  else if(fabs(leptons[ilep].id)==11)
///      //		    {
///      //		      if(fabs(leptons[ilep].eta())<1.442) iScaleLepton *= (1.0+varSign*0.02);
///      //		      else                                iScaleLepton *= (1.0+varSign*0.05);
///      //		    }
///      //		  lepDiff += (iScaleLepton-leptons[ilep]);
///      //		}
///      //	    }
///      //	}
///      
///      //      //vary unclustered component
///      //      if(ivar==UMET_UP || ivar==UMET_DOWN)
///      //	{
///      //	  unclustDiff=(met+clusteredFlux);
///      //	  double varSign=(ivar==UMET_UP ? 1.0 : -1.0);
///      //	  unclustDiff *= (varSign*0.10);
///      //	}
///      
///      //add new met
///      newMet -= jetDiff; 
///      newMet -= lepDiff; 
///      newMet -= unclustDiff; 
///      metsVar.push_back(newMet);
///      
///      //add new jets (if some change has occured)
///      jetsVar.push_back(newJets);
///      
///      //SOME DEBUGING PRINTOUT... CAN BE SAFELY REMOVED
///      //        if(ivar==JER && newMet.pt()>2000){
///      //           printf("%f - %f - %f : ",newMet.pt(), met.pt(), jetDiff.pt());
///      //           for(size_t ijet=0; ijet<jets.size(); ijet++){
///      //              LorentzVector iSmearJet=METUtils::smearedJet(jets[ijet],genjetsPt[ijet],mode);
///      //              LorentzVector iSmearJet=METUtils::smearedJet(jets[ijet],jets[ijet].genPt,mode);
///      //              printf("Jet%i %f(%f)>%f  ", (int)ijet, jets[ijet].pt(), genjetsPt[ijet], iSmearJet.pt());
///      //              printf("Jet%i %f(%f)>%f  ", (int)ijet, jets[ijet].pt(), jets[ijet].genPt, iSmearJet.pt());
///      //           }printf("\n");
///      //       }
///
///    }
///}
///
