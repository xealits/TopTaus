#ifndef _PlotStyle_hh
#define _PlotStyle_hh

#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include <vector>

// FIXME: move includes to .cc when not needed here
// FIXME: add namespace
#include "TMath.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "THStack.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TString.h"
#include "TObjString.h"
#include "TList.h"
#include "TPaveText.h"
#include "TLine.h"
#include "TLatex.h"
#include "TClass.h"

#endif


class PlotStyle {
  
public:
  
  PlotStyle();
  ~PlotStyle();
  
  TStyle *tdrStyle;
  int colors[6];
  
  void setTDRStyle();
  void fixOverlay();
  void tdrGrid(bool gridOn=true);
  
  /**
     @short retrieves a standard monitor canvas
  */
  TCanvas *GetMonitorCanvas(TString name, TString title,bool setBatch=false);
  
  void FormatLegendForMonitor(TLegend *leg,TString title);

  void FormatForCmsPublic(TCanvas * c, TLegend *leg, TString title, int nsamp=2, float legx=0.9, float legy=0.95, float legw=0.1, float legh=0.05,
			  TString legopt="");

  /**
     @short draws a canvas from a set of histograms to be stacked
  */
  TLegend *ShowInStack(TString name, TString title, TList &plots, Int_t w=400, Int_t h=400, TPad *c=0, TLegend * leg=0, TString legopt="", bool plotSum=true);
  
  /**
     @short draws a canvas from a set of histograms
  */
  TLegend *Show(TString name, TString title, TList &plots,
		TString opt="e1", bool drawNormalized=false, Int_t w=400, Int_t h=400, TPad *c=0, TLegend * leg=0, TString legopt="");
  
  /**
     @short draws a canvas with the ratio of a set of histograms with respect to a reference
  */
  TLegend *ShowRatio(TString name, TString title, TList &plots,
		     TString opt="e1", bool drawNormalized=false, Int_t w=400, Int_t h=400, TPad *c=0, TLegend * leg=0, TString legopt="",bool relative=true);
  
  /**
     @short draws directly MC to data ratio
     @return a list with the ratio, denominator, numerator plots
  */
  TList *GetSummedUpRatio( TList &denplots, TList &numplots );
  
  /**
     @short adds the under/over flows to the plot
  */
  void FixExtremityBins(TH1 *h,bool addOverflow=true, bool addUnderflow=true);
  
  /**
     @short formats histogram
  */
  void FormatHisto(TObject *h, 
		   int color=1, int line=1, int linew=1, int marker=20, int fill=1001, 
		   bool detachFromCurrentDir=false, bool setSumw2=false, 
		   int linecolor=-1, int fillcolor=-1, int markercolor=-1);
  
  void FormatAsFrameHisto(TH1 *f);
  
  /**
     @short finds the frame histogram and re-sets the min and maximum
  */
  void ReadjustCanvasFrame(TCanvas *c);
  
  void ReadjustCanvasFrameForCmsPublic(TCanvas *c);
  
};

#endif // _PlotStyle_hh
