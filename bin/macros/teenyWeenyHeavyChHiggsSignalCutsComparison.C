#include <iostream>
#include <sstream>
#include <iomanip>
#include <TH1F.h>
#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TText.h>
#include <TGraph.h>
#include <TVectorT.h>

void teenyWeenyHeavyChHiggsSignalCutsComparison(){

  // TDR style crap - include from Utilities.cc
  TStyle *tdrStyle = new TStyle("tdrStyle","Style for P-TDR");
  // For the canvas:
  tdrStyle->SetCanvasBorderMode(0);
  tdrStyle->SetCanvasColor(kWhite);
  tdrStyle->SetCanvasDefH(600); //Height of canvas
  tdrStyle->SetCanvasDefW(600); //Width of canvas
  tdrStyle->SetCanvasDefX(0);   //POsition on screen
  tdrStyle->SetCanvasDefY(0);
  
  // For the Pad:
  tdrStyle->SetPadBorderMode(0);
  // tdrStyle->SetPadBorderSize(Width_t size = 1);
  tdrStyle->SetPadColor(kWhite);
  tdrStyle->SetPadGridX(false);
  tdrStyle->SetPadGridY(false);
  tdrStyle->SetGridColor(0);
  tdrStyle->SetGridStyle(3);
  tdrStyle->SetGridWidth(1);
  
  // For the frame:
  tdrStyle->SetFrameBorderMode(0);
  tdrStyle->SetFrameBorderSize(1);
  tdrStyle->SetFrameFillColor(0);
  tdrStyle->SetFrameFillStyle(0);
  tdrStyle->SetFrameLineColor(1);
  tdrStyle->SetFrameLineStyle(1);
  tdrStyle->SetFrameLineWidth(1);
  
  // For the histo:
  // tdrStyle->SetHistFillColor(1);
  // tdrStyle->SetHistFillStyle(0);
  tdrStyle->SetHistLineColor(1);
  tdrStyle->SetHistLineStyle(0);
  tdrStyle->SetHistLineWidth(1);
  // tdrStyle->SetLegoInnerR(Float_t rad = 0.5);
  // tdrStyle->SetNumberContours(Int_t number = 20);
  
  tdrStyle->SetEndErrorSize(2);
  //tdrStyle->SetErrorMarker(20);
  tdrStyle->SetErrorX(0.);
  
  tdrStyle->SetMarkerStyle(20);
  
  //For the fit/function:
  tdrStyle->SetOptFit(1);
  tdrStyle->SetFitFormat("5.4g");
  tdrStyle->SetFuncColor(2);
  tdrStyle->SetFuncStyle(1);
  tdrStyle->SetFuncWidth(1);
  
  //For the date:
  tdrStyle->SetOptDate(0);
  // tdrStyle->SetDateX(Float_t x = 0.01);
  // tdrStyle->SetDateY(Float_t y = 0.01);
  
  // For the statistics box:
  tdrStyle->SetOptFile(0);
  tdrStyle->SetOptStat(0); // To display the mean and RMS:   SetOptStat("mr");
  tdrStyle->SetStatColor(kWhite);
  tdrStyle->SetStatFont(42);
  tdrStyle->SetStatFontSize(0.025);
  tdrStyle->SetStatTextColor(1);
  tdrStyle->SetStatFormat("6.4g");
  tdrStyle->SetStatBorderSize(1);
  tdrStyle->SetStatH(0.1);
  tdrStyle->SetStatW(0.15);
  // tdrStyle->SetStatStyle(Style_t style = 1001);
  // tdrStyle->SetStatX(Float_t x = 0);
  // tdrStyle->SetStatY(Float_t y = 0);

  // Margins:
  tdrStyle->SetPadTopMargin(0.05);
  tdrStyle->SetPadBottomMargin(0.13);
  tdrStyle->SetPadLeftMargin(0.16);
  tdrStyle->SetPadRightMargin(0.04);
  
  // For the Global title:

  tdrStyle->SetOptTitle(0);
  tdrStyle->SetTitleFont(42);
  tdrStyle->SetTitleColor(1);
  tdrStyle->SetTitleTextColor(1);
  tdrStyle->SetTitleFillColor(10);
  tdrStyle->SetTitleFontSize(0.05);
  // tdrStyle->SetTitleH(0); // Set the height of the title box
  // tdrStyle->SetTitleW(0); // Set the width of the title box
  // tdrStyle->SetTitleX(0); // Set the position of the title box
  // tdrStyle->SetTitleY(0.985); // Set the position of the title box
  // tdrStyle->SetTitleStyle(Style_t style = 1001);
  // tdrStyle->SetTitleBorderSize(2);
  
  // For the axis titles:
  
  tdrStyle->SetTitleColor(1, "XYZ");
  tdrStyle->SetTitleFont(42, "XYZ");
  tdrStyle->SetTitleSize(0.06, "XYZ");
  // tdrStyle->SetTitleXSize(Float_t size = 0.02); // Another way to set the size?
  // tdrStyle->SetTitleYSize(Float_t size = 0.02);
  tdrStyle->SetTitleXOffset(0.9);
  tdrStyle->SetTitleYOffset(1.25);
  // tdrStyle->SetTitleOffset(1.1, "Y"); // Another way to set the Offset

  // For the axis labels:

  tdrStyle->SetLabelColor(1, "XYZ");
  tdrStyle->SetLabelFont(42, "XYZ");
  tdrStyle->SetLabelOffset(0.007, "XYZ");
  tdrStyle->SetLabelSize(0.05, "XYZ");

  // For the axis:

  tdrStyle->SetAxisColor(1, "XYZ");
  tdrStyle->SetStripDecimals(kTRUE);
  tdrStyle->SetTickLength(0.03, "XYZ");
  tdrStyle->SetNdivisions(510, "XYZ");
  tdrStyle->SetPadTickX(1);  // To get tick marks on the opposite side of the frame
  tdrStyle->SetPadTickY(1);

  // Change for log plots:
  tdrStyle->SetOptLogx(0);
  tdrStyle->SetOptLogy(0);
  tdrStyle->SetOptLogz(0);

  // Postscript options:
  tdrStyle->SetPaperSize(20.,20.);
  // tdrStyle->SetLineScalePS(Float_t scale = 3);
  // tdrStyle->SetLineStyleString(Int_t i, const char* text);
  // tdrStyle->SetHeaderPS(const char* header);
  // tdrStyle->SetTitlePS(const char* pstitle);

  // tdrStyle->SetBarOffset(Float_t baroff = 0.5);
  // tdrStyle->SetBarWidth(Float_t barwidth = 0.5);
  // tdrStyle->SetPaintTextFormat(const char* format = "g");
  // tdrStyle->SetPalette(Int_t ncolors = 0, Int_t* colors = 0);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->cd();

  // Meat of the stuff

  Double_t mHiggsTBH[6];
  Double_t mHiggsHTB[8];

  Double_t TBHeffCut_met[6];
  Double_t TBHeffCut_btag[6];
  Double_t TBHeffCut_tau[6];
  Double_t TBHeffCut_os[6];

  Double_t HTBeffCut_met[8];
  Double_t HTBeffCut_btag[8];
  Double_t HTBeffCut_tau[8];
  Double_t HTBeffCut_os[8];

  vector<TString> histLabels;

  histLabels.push_back( "E_{T}^{miss} #geq 40GeV" );
  histLabels.push_back( "#geq 1 b-tag" );
  histLabels.push_back( "N_{#tau} = 1");
  histLabels.push_back( "OS");

  // \multicolumn{1}{| c|}{ } &  \multicolumn{1}{ c|}{ $N_{lep}=1$ + $N_{jets}\ge 3$ } & \multicolumn{1}{ c|}{  }  &  \multicolumn{1}{ c |}{ $\ge$ 1btag } & \multicolumn{1}{ c| }{ $N_{\tau}=1$ } & \multicolumn{1}{ c |}{ OS }  & \multicolumn{1}{ c |}{ $\ge$ 2btags } \\ 

  mHiggsTBH[0] = 180.;
  mHiggsTBH[1] = 190.;
  mHiggsTBH[2] = 200.;
  mHiggsTBH[3] = 220.;
  mHiggsTBH[4] = 250.;
  mHiggsTBH[5] = 300.;

  mHiggsHTB[0] = 180.;
  mHiggsHTB[1] = 200.;
  mHiggsHTB[2] = 220.;
  mHiggsHTB[3] = 240.;
  mHiggsHTB[4] = 250.;
  mHiggsHTB[5] = 260.;
  mHiggsHTB[6] = 280.;
  mHiggsHTB[7] = 300.;
  
  TBHeffCut_met[0] = 3768.1  / 4223.7  ;  
  TBHeffCut_met[1] = 1874.2  / 2094.3  ;  
  TBHeffCut_met[2] = 1075.1  / 1198.4  ;  
  TBHeffCut_met[3] =  531.5  /  587.3  ;  
  TBHeffCut_met[4] =  225.2  /  245.6  ;  
  TBHeffCut_met[5] =  152.8  /  164.9  ;  
  
  TBHeffCut_btag[0] =   2670.9   / 3768.1   ;
  TBHeffCut_btag[1] =  1322.8    / 1874.2   ;
  TBHeffCut_btag[2] =    753.6   / 1075.1   ;
  TBHeffCut_btag[3] =   372.5    /  531.5   ;
  TBHeffCut_btag[4] =   156.7    /  225.2   ;
  TBHeffCut_btag[5] =    105.9   /  152.8   ;

  TBHeffCut_tau[0] =   341.9   /  2670.9 ;
  TBHeffCut_tau[1] =   168.1   / 1322.8  ;
  TBHeffCut_tau[2] =    94.2   /   753.6 ;
  TBHeffCut_tau[3] =    44.9   /  372.5  ;
  TBHeffCut_tau[4] =    18.0   /  156.7  ;
  TBHeffCut_tau[5] =    11.6   /   105.9 ;

  TBHeffCut_os[0] =  327.2  / 341.9 ;
  TBHeffCut_os[1] =  163.7  / 168.1 ;
  TBHeffCut_os[2] =   90.6  /  94.2 ;
  TBHeffCut_os[3] =   43.5  /  44.9 ;
  TBHeffCut_os[4] =   17.1  /  18.0 ;
  TBHeffCut_os[5] =   10.9  /  11.6 ;


  HTBeffCut_met[0] =   27232.4  / 36098.7   ;  
  HTBeffCut_met[1] =   28110.1  / 37094.3   ;  
  HTBeffCut_met[2] =   28455.3  / 36942.4   ;  
  HTBeffCut_met[3] =   28843.6  / 36411.5   ;  
  HTBeffCut_met[4] =   27540.4  / 33740.4   ;  
  HTBeffCut_met[5] =   27143.8  / 32958.1   ;  
  HTBeffCut_met[6] =   27281.1  / 32696.3   ;  
  HTBeffCut_met[7] =   27281.1  / 32696.3   ;  
  
  HTBeffCut_btag[0] =  25424.2 /  27232.4    ;
  HTBeffCut_btag[1] =  26135.2 /  28110.1    ;
  HTBeffCut_btag[2] =  25996.0 /  28455.3    ;
  HTBeffCut_btag[3] =  25514.3 /  28843.6    ;
  HTBeffCut_btag[4] =  23904.4 /  27540.4    ;
  HTBeffCut_btag[5] =  22847.4 /  27143.8    ;
  HTBeffCut_btag[6] =  22896.0 /  27281.1    ;
  HTBeffCut_btag[7] =  22896.0 /  27281.1    ;

  HTBeffCut_tau[0] =  1822.8  / 25424.2     ;
  HTBeffCut_tau[1] =  2002.2  / 26135.2     ;
  HTBeffCut_tau[2] =  1891.6  / 25996.0     ;
  HTBeffCut_tau[3] =  1960.3  / 25514.3     ;
  HTBeffCut_tau[4] =  1409.6  / 23904.4     ;
  HTBeffCut_tau[5] =  1272.6  / 22847.4     ;
  HTBeffCut_tau[6] =  1457.0  / 22896.0     ;
  HTBeffCut_tau[7] =  1457.0  / 22896.0     ;

  HTBeffCut_os[0] =   1798.7  / 1822.8    ;
  HTBeffCut_os[1] =   1975.3  / 2002.2    ;
  HTBeffCut_os[2] =   1851.7  / 1891.6    ;
  HTBeffCut_os[3] =   1893.5  / 1960.3    ;
  HTBeffCut_os[4] =   1399.0  / 1409.6    ;
  HTBeffCut_os[5] =   1248.8  / 1272.6    ;
  HTBeffCut_os[6] =   1431.0  / 1457.0    ;
  HTBeffCut_os[7] =   1431.0  / 1457.0    ;


  TGraph* TBH_met  = new TGraph(6, mHiggsTBH, TBHeffCut_met );
  TGraph* TBH_btag = new TGraph(6, mHiggsTBH, TBHeffCut_btag);
  TGraph* TBH_tau  = new TGraph(6, mHiggsTBH, TBHeffCut_tau );
  TGraph* TBH_os   = new TGraph(6, mHiggsTBH, TBHeffCut_os  );

  TGraph* HTB_met  = new TGraph(8, mHiggsHTB, HTBeffCut_met );
  TGraph* HTB_btag = new TGraph(8, mHiggsHTB, HTBeffCut_btag);
  TGraph* HTB_tau  = new TGraph(8, mHiggsHTB, HTBeffCut_tau );
  TGraph* HTB_os   = new TGraph(8, mHiggsHTB, HTBeffCut_os  );


  TBH_met  ->SetLineColor(1);
  TBH_btag ->SetLineColor(2);
  TBH_tau  ->SetLineColor(4);
  TBH_os   ->SetLineColor(6);
  
  TBH_met  ->SetMarkerColor(1);
  TBH_btag ->SetMarkerColor(2);
  TBH_tau  ->SetMarkerColor(4);
  TBH_os   ->SetMarkerColor(6);

  HTB_met  ->SetLineColor(1);
  HTB_btag ->SetLineColor(2);
  HTB_tau  ->SetLineColor(4);
  HTB_os   ->SetLineColor(6);
  
  HTB_met  ->SetMarkerColor(1);
  HTB_btag ->SetMarkerColor(2);
  HTB_tau  ->SetMarkerColor(4);
  HTB_os   ->SetMarkerColor(6);


  TBH_met  ->SetLineWidth(3);
  TBH_btag ->SetLineWidth(3);
  TBH_tau  ->SetLineWidth(3);
  TBH_os   ->SetLineWidth(3);

  HTB_met  ->SetLineWidth(3);
  HTB_btag ->SetLineWidth(3);
  HTB_tau  ->SetLineWidth(3);
  HTB_os   ->SetLineWidth(3);

  TBH_met  ->SetMarkerSize(5);
  TBH_btag ->SetMarkerSize(5);
  TBH_tau  ->SetMarkerSize(5);
  TBH_os   ->SetMarkerSize(5);

  HTB_met  ->SetMarkerSize(5);
  HTB_btag ->SetMarkerSize(5);
  HTB_tau  ->SetMarkerSize(5);
  HTB_os   ->SetMarkerSize(5);

  
  //  (const Double_t)4.83393830509829703e-01
 
  TLegend* leg_ = new TLegend(0.15,0.35,0.62,0.65,NULL,"brNDC");
  leg_->SetTextFont(42);
  leg_->SetBorderSize(0);
  leg_->SetLineColor(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColor(0);
  leg_->SetFillStyle(0);
  leg_->AddEntry(TBH_met , histLabels[0], "lp");
  leg_->AddEntry(TBH_btag, histLabels[1], "lp");
  leg_->AddEntry(TBH_tau , histLabels[2], "lp");
  leg_->AddEntry(TBH_os  , histLabels[3], "lp");

  TPaveText *pt1 = new TPaveText(0.17,0.30,0.5,0.35, "brNDC");
  pt1->SetBorderSize(1);
  pt1->SetFillColor(19);
  pt1->SetFillStyle(0);
  pt1->SetLineColor(0);
  pt1->SetTextFont(132);
  pt1->SetTextSize(0.033);
  //  TText *text = pt1->AddText("BR(t #rightarrow H^{+}b) = 0.05");

  TBH_met->GetXaxis()->SetTitle("H^{+} mass [GeV]");
  HTB_met->GetXaxis()->SetTitle("H^{+} mass [GeV]");

  TBH_met->GetYaxis()->SetTitle("Cut efficiency (%)");
  HTB_met->GetYaxis()->SetTitle("Cut efficiency (%)");


  TCanvas* myCanva = new TCanvas("signalCutEffs","signalCutEffs",2000,2000);
  
  TBH_met->GetYaxis()->SetRangeUser(0,1.);
  HTB_met->GetYaxis()->SetRangeUser(0,1.);

  myCanva->cd();
  TBH_met  ->Draw("LPA");
  TBH_btag ->Draw("LPsame");
  TBH_tau  ->Draw("LPsame");
  TBH_os   ->Draw("LPsame");
  leg_->Draw();
  pt1->Draw("same");
  myCanva->SaveAs("TBHcutEffs.pdf");
  myCanva->SaveAs("TBHcutEffs.png");

  myCanva->Clear();

  myCanva->cd();
  HTB_met  ->Draw("LPA");
  HTB_btag ->Draw("LPsame");
  HTB_tau  ->Draw("LPsame");
  HTB_os   ->Draw("LPsame");
  leg_->Draw();
  pt1->Draw("same");
  myCanva->SaveAs("HTBcutEffs.pdf");
  myCanva->SaveAs("HTBcutEffs.png");

  myCanva->Clear();
  

}
