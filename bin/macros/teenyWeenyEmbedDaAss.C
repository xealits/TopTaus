#include <iostream>
#include <iomanip>

#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>


void teenyWeenyEmbedDaAss(){

  // Acquire files
  TFile* embeddedData = new TFile("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_2/nomt-2012-V1-data-MU-20GeV/out-embedded-data.root");
  TFile* ztautau      = new TFile("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_2/nomt-2012-V1-mc-MU-20GeV/out-mc.root");
  
  // Acquire simple cutflow histos
  TH1F* emd_cutflow = (TH1F*) embeddedData->Get("PFlow/Yields/m_tau/cutflow_yields_3");
  emd_cutflow->SetName("emd_cutflow");
  TH1F* ztt_cutflow = (TH1F*) ztautau     ->Get("PFlow/Yields/m_tau/cutflow_yields_3");
  ztt_cutflow->SetName("ztt_cutflow");
  
  // Acquire MC cutflow per-channel
  TH2F* ztt_mccutflow = (TH2F*) ztautau->Get("PFlow/YieldsMC/cutflow_yields_mc_1");
  ztt_mccutflow->SetName("ztt_mccutflow");
  
  // Acquire sample distribution for shape analysis
  TH1F* emd_taupt = (TH1F*) embeddedData->Get("PFlow/RecoSteps/m_tau/OS/pt_t_31");  
  emd_taupt->SetName("emd_taupt");
  TH1F* ztt_taupt = (TH1F*) ztautau     ->Get("PFlow/RecoSteps/m_tau/OS/pt_t_31");
  ztt_taupt->SetName("ztt_taupt");

  
  cout << "Preliminary output: " << endl;
  cout << "-------------------- " << endl;
  cout << "embedded: " << emd_cutflow->GetBinContent(11) << endl;
  cout << "ztautau:  " << ztt_cutflow->GetBinContent(11) << endl;
  cout << "--------------------" << endl;
  cout << "MC flow info:" << endl;
  cout << "Z->(ll):     " << ztt_mccutflow->GetBinContent(11,11) << endl;
  cout << "Z->(tautau): " << ztt_mccutflow->GetBinContent(11,12) << endl;
  cout << "--------------------" << endl;

  //std::ostringstream strs;
  double ks = ztt_taupt->KolmogorovTest(emd_taupt);
  double chi2p = ztt_taupt->Chi2Test(emd_taupt,"");
  double nchi2p = ztt_taupt->Chi2Test(emd_taupt,"UU NORM");
  double chi2c = ztt_taupt->Chi2Test(emd_taupt,"CHI2");
  double chi2cn = ztt_taupt->Chi2Test(emd_taupt,"CHI2/NDF");
  double nchi2c = ztt_taupt->Chi2Test(emd_taupt,"UU NORM CHI2");
  double nchi2cn = ztt_taupt->Chi2Test(emd_taupt,"UU NORM CHI2/NDF");


  cout << "ks is " << std::setprecision(2) << ks <<  endl;
  cout << "pvalue for chi2 is " << std::setprecision(2) << chi2p <<  endl;
  cout << "pvalue for norm-chi2 is " << std::setprecision(2) << nchi2p <<  endl;
  cout << "chi2 for chi2 is " << std::setprecision(2) << chi2c <<  endl;
  cout << "chi2/NDOF for chi2 is " << std::setprecision(2) << chi2cn <<  endl;
  cout << "chi2 for norm-chi2 is " << std::setprecision(2) << nchi2c <<  endl;
  cout << "chi2/NDOF for norm-chi2 is " << std::setprecision(2) << nchi2cn <<  endl;



  //  strs << std::setprecision(2) << ks;
//  std::string str = "p-value from KS test: " + strs.str();
//
//  TText *text = pt1->AddText( str.c_str() );
//  text->SetTextAlign(11);
//
//
//
//  TCanvas* myCanva = new TCanvas("closureTest","closureTest",2000,2000);
//  myCanva->cd();
//  tauFake->Draw("e4");
//  subtracted->Draw("e4same");
//
//  leg_->Draw();
//  pt1->Draw("same");
//  myCanva->SaveAs("closureTest.pdf");
//  myCanva->SaveAs("closureTest.png");

  
  
}
