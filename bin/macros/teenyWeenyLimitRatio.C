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

void teenyWeenyLimitRatio(){

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

  histLabels.push_back( "ME_{T} #ge 40GeV" );
  histLabels.push_back( "#ge 1 btag" );
  histLabels.push_back( "N_{#tau} = 1");
  histLabels.push_back( "OS");
  
  // \multicolumn{1}{| c|}{ } &  \multicolumn{1}{ c|}{ $N_{lep}=1$ + $N_{jets}\ge 3$ } & \multicolumn{1}{ c|}{  }  &  \multicolumn{1}{ c |}{ $\ge$ 1btag } & \multicolumn{1}{ c| }{ $N_{\tau}=1$ } & \multicolumn{1}{ c |}{ OS }  & \multicolumn{1}{ c |}{ $\ge$ 2btags } \\ 
  TGraph* graph = new TGraph(7);
  graph->SetName("Graph");
  graph->SetTitle("Graph");
  graph->SetFillColor(1);
  graph->SetLineColor(2);
  graph->SetLineStyle(2);
  graph->SetLineWidth(3);
  graph->SetMarkerColor(2);
  graph->SetMarkerStyle(21);
  graph->SetMarkerSize(3);
  
  graph->SetPoint(0, 80.,  double(0.0247838 / 0.05808) );
  graph->SetPoint(1,100., double(0.0254861 / 0.04671) );
  graph->SetPoint(2,120., double(0.0331729 / 0.06168) );
  graph->SetPoint(3,140., double(0.0206885 / 0.05729) );
  graph->SetPoint(4,150., double(0.0404898 / 0.09177) );
  graph->SetPoint(5,155., double(0.0446807 / 0.11246) );
  graph->SetPoint(6,160., double(0.0432696 / 0.11826) );
  









  // Combination
//   graph = new TGraph(7);
//   graph->SetName("tg_exp");
//   graph->SetTitle("Graph");
//   graph->SetFillColor(1);
//   graph->SetLineColor(kBlack);
//   graph->SetLineStyle(2);
//   graph->SetLineWidth(3);
//   graph->SetMarkerColor(kBlack);
//   graph->SetMarkerStyle(21);
//   graph->SetMarkerSize(1.4);
//   graph->SetPoint(0,80,0.0294745);
//   graph->SetPoint(1,100,0.0237506);
//   graph->SetPoint(2,120,0.0200001);
//   graph->SetPoint(3,140,0.0143644);
//   graph->SetPoint(4,150,0.013179);
//   graph->SetPoint(5,155,0.0124245);
//   graph->SetPoint(6,160,0.0112734);


  mHiggs[0] =  80.;
  mHiggs[1] = 100.;
  mHiggs[2] = 120.;
  mHiggs[3] = 140.;
  mHiggs[4] = 150.;
  mHiggs[5] = 155.;
  mHiggs[6] = 160.;
  
  
  //  (const Double_t)4.83393830509829703e-01
 
  TLegend* leg_ = new TLegend(0.55,0.75,0.99,0.95,NULL,"brNDC");
  leg_->SetTextFont(42);
  leg_->SetBorderSize(0);
  leg_->SetLineColor(1);
  leg_->SetLineStyle(1);
  leg_->SetLineWidth(1);
  leg_->SetFillColor(0);
  leg_->SetFillStyle(0);
  leg_->AddEntry(graph , "new limit / paper limit" , "lp");

  TPaveText *pt1 = new TPaveText(0.55,0.70,0.99,0.75, "brNDC");
  pt1->SetBorderSize(1);
  pt1->SetFillColor(19);
  pt1->SetFillStyle(0);
  pt1->SetLineColor(0);
  pt1->SetTextFont(132);
  pt1->SetTextSize(0.055);
  TText *text = pt1->AddText("#mu#tau channel");

  


  TCanvas* myCanva = new TCanvas("signalCutEffs","signalCutEffs",2000,2000);
  
  graph->GetYaxis()->SetRangeUser(0,1);


  myCanva->cd();
  graph  ->Draw("LPA");
  leg_->Draw();
  pt1->Draw("same");
  myCanva->SaveAs("limitsRatio.pdf");
  myCanva->SaveAs("limitsRatio.png");

  

}
