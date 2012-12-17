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

void teenyWeenySignalCutsComparison(){

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

  Double_t mHiggs[7];

  Double_t HHeffCut_met[7];
  Double_t HHeffCut_btag[7];
  Double_t HHeffCut_tau[7];
  Double_t HHeffCut_os[7];

  Double_t WHeffCut_met[7];
  Double_t WHeffCut_btag[7];
  Double_t WHeffCut_tau[7];
  Double_t WHeffCut_os[7];

  vector<TString> histLabels;

  histLabels.push_back( "E_{T}^{miss} #geq 40GeV" );
  histLabels.push_back( "#geq 1 b-tag" );
  histLabels.push_back( "N_{#tau} = 1");
  histLabels.push_back( "OS");

  // \multicolumn{1}{| c|}{ } &  \multicolumn{1}{ c|}{ $N_{lep}=1$ + $N_{jets}\ge 3$ } & \multicolumn{1}{ c|}{  }  &  \multicolumn{1}{ c |}{ $\ge$ 1btag } & \multicolumn{1}{ c| }{ $N_{\tau}=1$ } & \multicolumn{1}{ c |}{ OS }  & \multicolumn{1}{ c |}{ $\ge$ 2btags } \\ 

  mHiggs[0] =  80.;
  mHiggs[1] = 100.;
  mHiggs[2] = 120.;
  mHiggs[3] = 140.;
  mHiggs[4] = 150.;
  mHiggs[5] = 155.;
  mHiggs[6] = 160.;
  
  HHeffCut_met[0] =  14136.5 / 18284.0 ;  
  HHeffCut_met[1] =  15155.3 / 19912.9 ;  
  HHeffCut_met[2] =  16565.5 / 21142.8 ;  
  HHeffCut_met[3] =  15147.7 / 18699.4 ;  
  HHeffCut_met[4] =  12471.2 / 14960.9 ;  
  HHeffCut_met[5] =  11295.6 / 13235.8 ;  
  HHeffCut_met[6] =   9657.7 / 11071.8 ;  
  
  HHeffCut_btag[0] =   13200.7 / 14136.5  ;
  HHeffCut_btag[1] =   14015.2 / 15155.3  ;
  HHeffCut_btag[2] =   14927.6 / 16565.5  ;
  HHeffCut_btag[3] =   12708.1 / 15147.7  ;
  HHeffCut_btag[4] =    9192.6 / 12471.2  ;
  HHeffCut_btag[5] =    7503.1 / 11295.6  ;
  HHeffCut_btag[6] =    5723.0 /  9657.7  ;

  HHeffCut_tau[0] =    1280.1 / 13200.7  ;
  HHeffCut_tau[1] =    1479.7 / 14015.2  ;
  HHeffCut_tau[2] =    1635.1 / 14927.6  ;
  HHeffCut_tau[3] =    1303.1 / 12708.1  ;
  HHeffCut_tau[4] =     740.3 /  9192.6  ;
  HHeffCut_tau[5] =     712.1 /  7503.1  ;
  HHeffCut_tau[6] =     442.4 /  5723.0  ;

  HHeffCut_os[0] =  1264.8 / 1280.1    ;
  HHeffCut_os[1] =  1448.5 / 1479.7    ;
  HHeffCut_os[2] =  1625.3 / 1635.1    ;
  HHeffCut_os[3] =  1290.4 / 1303.1    ;
  HHeffCut_os[4] =   732.9 /  740.3    ;
  HHeffCut_os[5] =   706.4 /  712.1    ;
  HHeffCut_os[6] =   432.1 /  442.4    ;


  WHeffCut_met[0] =   27232.4  / 36098.7   ;  
  WHeffCut_met[1] =   28110.1  / 37094.3   ;  
  WHeffCut_met[2] =   28455.3  / 36942.4   ;  
  WHeffCut_met[3] =   28843.6  / 36411.5   ;  
  WHeffCut_met[4] =   27540.4  / 33740.4   ;  
  WHeffCut_met[5] =   27143.8  / 32958.1   ;  
  WHeffCut_met[6] =   27281.1  / 32696.3   ;  
  
  WHeffCut_btag[0] =  25424.2 /  27232.4    ;
  WHeffCut_btag[1] =  26135.2 /  28110.1    ;
  WHeffCut_btag[2] =  25996.0 /  28455.3    ;
  WHeffCut_btag[3] =  25514.3 /  28843.6    ;
  WHeffCut_btag[4] =  23904.4 /  27540.4    ;
  WHeffCut_btag[5] =  22847.4 /  27143.8    ;
  WHeffCut_btag[6] =  22896.0 /  27281.1    ;

  WHeffCut_tau[0] =  1822.8  / 25424.2     ;
  WHeffCut_tau[1] =  2002.2  / 26135.2     ;
  WHeffCut_tau[2] =  1891.6  / 25996.0     ;
  WHeffCut_tau[3] =  1960.3  / 25514.3     ;
  WHeffCut_tau[4] =  1409.6  / 23904.4     ;
  WHeffCut_tau[5] =  1272.6  / 22847.4     ;
  WHeffCut_tau[6] =  1457.0  / 22896.0     ;

  WHeffCut_os[0] =   1798.7  / 1822.8    ;
  WHeffCut_os[1] =   1975.3  / 2002.2    ;
  WHeffCut_os[2] =   1851.7  / 1891.6    ;
  WHeffCut_os[3] =   1893.5  / 1960.3    ;
  WHeffCut_os[4] =   1399.0  / 1409.6    ;
  WHeffCut_os[5] =   1248.8  / 1272.6    ;
  WHeffCut_os[6] =   1431.0  / 1457.0    ;


  TGraph* HH_met  = new TGraph(7, mHiggs, HHeffCut_met );
  TGraph* HH_btag = new TGraph(7, mHiggs, HHeffCut_btag);
  TGraph* HH_tau  = new TGraph(7, mHiggs, HHeffCut_tau );
  TGraph* HH_os   = new TGraph(7, mHiggs, HHeffCut_os  );

  TGraph* WH_met  = new TGraph(7, mHiggs, WHeffCut_met );
  TGraph* WH_btag = new TGraph(7, mHiggs, WHeffCut_btag);
  TGraph* WH_tau  = new TGraph(7, mHiggs, WHeffCut_tau );
  TGraph* WH_os   = new TGraph(7, mHiggs, WHeffCut_os  );


  HH_met  ->SetLineColor(1);
  HH_btag ->SetLineColor(2);
  HH_tau  ->SetLineColor(4);
  HH_os   ->SetLineColor(6);
  
  HH_met  ->SetMarkerColor(1);
  HH_btag ->SetMarkerColor(2);
  HH_tau  ->SetMarkerColor(4);
  HH_os   ->SetMarkerColor(6);

  WH_met  ->SetLineColor(1);
  WH_btag ->SetLineColor(2);
  WH_tau  ->SetLineColor(4);
  WH_os   ->SetLineColor(6);
  
  WH_met  ->SetMarkerColor(1);
  WH_btag ->SetMarkerColor(2);
  WH_tau  ->SetMarkerColor(4);
  WH_os   ->SetMarkerColor(6);


  HH_met  ->SetLineWidth(3);
  HH_btag ->SetLineWidth(3);
  HH_tau  ->SetLineWidth(3);
  HH_os   ->SetLineWidth(3);

  WH_met  ->SetLineWidth(3);
  WH_btag ->SetLineWidth(3);
  WH_tau  ->SetLineWidth(3);
  WH_os   ->SetLineWidth(3);

  HH_met  ->SetMarkerSize(5);
  HH_btag ->SetMarkerSize(5);
  HH_tau  ->SetMarkerSize(5);
  HH_os   ->SetMarkerSize(5);

  WH_met  ->SetMarkerSize(5);
  WH_btag ->SetMarkerSize(5);
  WH_tau  ->SetMarkerSize(5);
  WH_os   ->SetMarkerSize(5);

  
  //  (const Double_t)4.83393830509829703e-01
 
  TLegend* leg_ = new TLegend(0.15,0.35,0.62,0.65,NULL,"brNDC");
  leg_->SetTextFont(42);
  leg_->SetBorderSize(0);
  leg_->SetLineColor(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColor(0);
  leg_->SetFillStyle(0);
  leg_->AddEntry(HH_met , histLabels[0], "lp");
  leg_->AddEntry(HH_btag, histLabels[1], "lp");
  leg_->AddEntry(HH_tau , histLabels[2], "lp");
  leg_->AddEntry(HH_os  , histLabels[3], "lp");

  TPaveText *pt1 = new TPaveText(0.17,0.30,0.5,0.35, "brNDC");
  pt1->SetBorderSize(1);
  pt1->SetFillColor(19);
  pt1->SetFillStyle(0);
  pt1->SetLineColor(0);
  pt1->SetTextFont(132);
  pt1->SetTextSize(0.033);
  //  TText *text = pt1->AddText("BR(t #rightarrow H^{+}b) = 0.05");

  HH_met->GetXaxis()->SetTitle("H^{+} mass [GeV]");
  WH_met->GetXaxis()->SetTitle("H^{+} mass [GeV]");

  HH_met->GetYaxis()->SetTitle("Cut efficiency (%)");
  WH_met->GetYaxis()->SetTitle("Cut efficiency (%)");


  TCanvas* myCanva = new TCanvas("signalCutEffs","signalCutEffs",2000,2000);
  
  HH_met->GetYaxis()->SetRangeUser(0,1.);
  WH_met->GetYaxis()->SetRangeUser(0,1.);

  myCanva->cd();
  HH_met  ->Draw("LPA");
  HH_btag ->Draw("LPsame");
  HH_tau  ->Draw("LPsame");
  HH_os   ->Draw("LPsame");
  leg_->Draw();
  pt1->Draw("same");
  myCanva->SaveAs("HHcutEffs.pdf");
  myCanva->SaveAs("HHcutEffs.png");

  myCanva->Clear();

  myCanva->cd();
  WH_met  ->Draw("LPA");
  WH_btag ->Draw("LPsame");
  WH_tau  ->Draw("LPsame");
  WH_os   ->Draw("LPsame");
  leg_->Draw();
  pt1->Draw("same");
  myCanva->SaveAs("WHcutEffs.pdf");
  myCanva->SaveAs("WHcutEffs.png");

  myCanva->Clear();
  

}
