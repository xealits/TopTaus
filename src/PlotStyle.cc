#include "LIP/TopTaus/interface/PlotStyle.hh"


using namespace std;


PlotStyle::PlotStyle()
{
  colors[0] = kOrange;
  colors[1] = kAzure-4;
  colors[2] = kRed-6;
  colors[3] = kGreen-6;
  colors[4] = kAzure+3;
  colors[5] = kYellow-10;
}

PlotStyle::~PlotStyle()
{
}


// tdrGrid: Turns the grid lines on (true) or off (false)
void PlotStyle::tdrGrid(bool gridOn) {
  tdrStyle->SetPadGridX(gridOn);
  tdrStyle->SetPadGridY(gridOn);
}

// fixOverlay: Redraws the axis
void PlotStyle::fixOverlay() {
  gPad->RedrawAxis();
}

void PlotStyle::setTDRStyle() {
  tdrStyle = new TStyle("tdrStyle","Style for P-TDR");

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
  tdrStyle->SetPadRightMargin(0.02);

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
  tdrStyle->SetTitleXOffset(0.6);
  tdrStyle->SetTitleYOffset(0.9);
  //  tdrStyle->SetTitleOffset(1.25, "Y"); // Another way to set the Offset

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
  tdrStyle->SetPalette(1);
  // tdrStyle->SetTimeOffset(Double_t toffset);
  // tdrStyle->SetHistMinimumZero(kTRUE);

  tdrStyle->cd();

}

void PlotStyle::setXerrorBars(){
  tdrStyle->SetErrorX();
}
/*-------------------------
  -------------------------*/
TLegend* PlotStyle::ShowInStack(TString name, TString title, TList &plots, Int_t w, Int_t h,TPad *c, TLegend * leg, TString legopt,bool plotSum)
{
  //build the canvas
  if(c!=0) { c->Clear(); }
  else     { 
    TCanvas *cnv = new TCanvas(name,title,w,h); 
    c = (TPad *)cnv->cd();
  }
  c->cd();
  c->SetGridx();
  c->SetGridy();

  if( plots.First()==0) return 0;
  
  //prepare the legend
  if( leg == 0 ) leg = new TLegend(0.75,0.9,0.98,1.0,NULL,"brNDC");
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextFont(42);

  //build the stack
  THStack *hstack = new THStack(name+"Stack",title);  
  TObject *key = 0;
  TH1 *p=(TH1 *) plots.First();
  TH1 *refFrame=(TH1 *) p->Clone(name+"Frame");
  refFrame->SetDirectory(0);
  refFrame->Reset("ICE");  
  
  TIterator *reversePlotsIt = plots.MakeIterator(kIterBackward);
  while ( (key = reversePlotsIt->Next()) ) 
    {
      p = (TH1 *) key;
      refFrame->Add(p);
      leg->AddEntry( p, p->GetTitle(), legopt );      
    }
  refFrame->SetLineColor(1);
  refFrame->SetLineStyle(1);
  refFrame->SetFillStyle(0);
  refFrame->SetFillColor(0);
  refFrame->SetMarkerStyle(0);
  refFrame->SetMarkerColor(1);
  refFrame->GetYaxis()->SetTitleOffset(1.25);
  refFrame->GetXaxis()->SetTitleOffset(0.9);
  
  TIterator *plotsIt = plots.MakeIterator();
  while ( (key = plotsIt->Next()) ) 
    {
      p = (TH1 *) key;
      hstack->Add( p );
    }

  //prepare the frame
  if(!plotSum) refFrame->Reset("ICE");
  FormatAsFrameHisto(refFrame);
  refFrame->SetLineColor(1);
  refFrame->Draw("hist");  
  refFrame->SetTitle("total");

  //draw the stack
  hstack->Draw("histsame"); 
  leg->Draw("same");
  
  c->Update();
  return leg;
}


/*-------------------------
  -------------------------*/
TLegend* PlotStyle::Show(TString name, TString title, TList &plots, TString opt, bool drawNormalized, Int_t w, Int_t h, TPad *c, TLegend *leg, TString legopt)
{  
  //build the canvas
  if(c!=0) { c->Clear(); }
  else     { 
    TCanvas *cnv = new TCanvas(name,title,w,h); 
    c = (TPad *)cnv->cd();
  }
  c->SetGridx();
  c->SetGridy();
 
  //prepare the legend
  // TLegend *leg = new TLegend(0.6,0.9,0.98,1.0,NULL,"brNDC");
  if( leg == 0 ) leg = new TLegend(0.6,0.9,0.98,1.0,NULL,"brNDC");
  leg->SetBorderSize(1);
  leg->SetFillColor(0);
  leg->SetTextFont(42);

  //prepare the frame
  TObject *p= plots.At(0);
  if( p == 0 ) { return 0; }
  bool is2d = p->IsA()->InheritsFrom(TH2::Class());
  bool isgraph = p->IsA()->InheritsFrom(TGraph::Class());
  if( !is2d && !isgraph ) { // the frame gives problems when plotting TH2s and TGraphs
    //    TH1 *f = (TH1 *) p->Clone(name+"Frame");
    // f->SetDirectory(0);
    //f->Reset("ICE");
    //f->Draw();
  } else if(!isgraph) {
    
    TH1 *f = (TH1 *)p;
    if(!drawNormalized) f->Draw( opt );
    else 
      {
	f->Sumw2();
	f->DrawNormalized( opt );
      }
    leg->AddEntry(f,f->GetTitle(),legopt);
  }

  double xmin(1e13),xmax(-1e13);

  TIterator *plotsIt = plots.MakeIterator();
  TObject *key = 0;
  //plotsIt->Next();
  Int_t iplot(0);
  while ( (key = plotsIt->Next()) ) 
    {
      iplot++;
      if (isgraph){
	TGraph *gr = (TGraph *)key;
	gr->Draw((iplot==1 ? "a"+opt : opt));
	leg->AddEntry(gr,gr->GetTitle(),legopt);
	if(gr->GetMinimum()<xmin) xmin=gr->GetMinimum();
	if(gr->GetMaximum()>xmax) xmax=gr->GetMaximum()*1.5;
      }
      else
	{
	  TH1 *f = (TH1 *) key;
	  TString  drawOpt(opt);
	  if(iplot>1) drawOpt += "same";
	  if(!drawNormalized) f->Draw( drawOpt );
	  else 
	    {
	      f->Sumw2();
	      f->DrawNormalized( drawOpt );
	    }
	  leg->AddEntry(f,f->GetTitle(),legopt);
	  if(f->GetMinimum()<xmin) xmin=f->GetMinimum();
	  if(f->GetMaximum()>xmax) xmax=f->GetMaximum()*1.5;
	}
    }
  
  if(!isgraph)
    {
      TH1 *f = (TH1 *)p;
      if(xmin==0) xmin=0.1;
      if(xmin==xmax) xmax++;
      if( !is2d ) {  
	f->GetYaxis()->SetRangeUser(xmin,xmax); 
      }
      FormatAsFrameHisto(f);
    }
  leg->Draw("same");
  
  c->Update();
  return leg;
}

/*-----------------------------------
  -----------------------------------*/
TLegend* PlotStyle::ShowRatio(TString name, TString title, TList &plots, TString opt, bool drawNormalized, Int_t w, Int_t h, TPad *c, TLegend *leg, TString legopt, bool relative)
{
  //Pad positioning stuff
  const float xpad[2] = {0.,1};
  const float ypad[4] = {0.,0.3,0.3,1.0};


  //build the canvas
  if(c!=0) { c->Clear(); }
  else     {
    TCanvas *cnv = new TCanvas(name,title,w,h); 
    c = (TPad *)cnv->cd();
  }
  c->Divide(1,2);
  TPad *p = (TPad *)c->cd(1);
  p->SetBottomMargin(0);
  p->SetPad(xpad[0],ypad[2],xpad[1],ypad[3]);
  p->SetGridx();
  p->SetGridy();
  leg=Show(name,title,plots,opt,drawNormalized,w,h,p,leg,legopt);

  //compute the ratio of histograms
  TList ratiosH;
  TIterator *plotsIt = plots.MakeIterator();
  TH1 *ref = (TH1 *) plotsIt->Next();
  TObject *key = 0;
  while ( (key = plotsIt->Next()) ) 
    {
      TH1 *p = (TH1 *) key;
      if(p==0) continue;
      TString name=p->GetName() + TString("_ratio");
      TString title= TString("(")+p->GetTitle();
      if(relative) title+= TString("-")+ref->GetTitle();
      title += TString(")/")+ref->GetTitle();
      
      TH1D *iRatio = (TH1D *) p->Clone(name);
      if(relative) 
	{
	  iRatio->Add(ref,-1);
	  iRatio->GetYaxis()->SetTitle("Relative difference");
	}
      else
	  iRatio->GetYaxis()->SetTitle("Ratio");
      iRatio->SetTitle(title);
      iRatio->Divide(ref);      
      ratiosH.Add(iRatio);
    }
  
  //draw the canvas
  p = (TPad *)c->cd(2);
  p->SetTopMargin(0);
  p->SetBottomMargin(0.3);
  p->SetPad(xpad[0],ypad[0],xpad[1],ypad[1]);
  p->SetGridx();
  p->SetGridy();
  TLegend *leg2=Show(name,title,ratiosH,"e1",false,w,h,p,0,legopt);
  leg2->Delete();

  const float yscale = (ypad[3]-ypad[2])/(ypad[1]-ypad[0]);
  plotsIt = ratiosH.MakeIterator();
  while ( (key = plotsIt->Next()) ) 
    {
      TH1 *p = (TH1 *) key;
      if(p==0) continue;
      if(relative)  p->GetYaxis()->SetRangeUser(-1.1,1.1);
      p->GetXaxis()->SetTitleOffset(0.85);
      p->GetXaxis()->SetLabelSize(0.04 * yscale);
      p->GetXaxis()->SetTitleSize(0.05 * yscale);
      p->GetXaxis()->SetTickLength( 0.03 * yscale );
      p->GetYaxis()->SetTitleOffset(0.5);
      p->GetYaxis()->SetLabelSize(0.04 * yscale);
      p->GetYaxis()->SetTitleSize(0.04 * yscale);
    }
  p->Modified();
  p->Update();

  if(relative)
    {
      TLine *line = new TLine(((TH1 *)ratiosH.At(0))->GetXaxis()->GetXmin(),0,
			      ((TH1 *)ratiosH.At(0))->GetXaxis()->GetXmax(),0);
      line->SetLineColor(14);
      line->SetLineWidth(2);
      line->Draw("same");
    }

  c->cd();
  c->Update();
  c->cd(1);
  return leg;
}


/*-----------------------------------
  -----------------------------------*/
void PlotStyle::ReadjustCanvasFrameForCmsPublic(TCanvas *c)
{
  if(c==0) return;
  TIter next(c->GetListOfPrimitives());
  TH1 *f=0;
  double max = -1e+23;  
  double min = 1e+23;
  TObject *obj=0;
  while ( (obj = next()) )
    {
      if(obj==0) continue;
      TString className = obj->ClassName();
      if(className.Contains("TH1")==false) continue;
      TString name = obj->GetName();
      if(name.Contains("Frame") ) { f = (TH1 *) obj; continue; }

      double imax=((TH1 *)obj)->GetMaximum()*1.05;
      double imin=((TH1 *)obj)->GetMinimum();
      if(imax>max) max=imax;
      if(imin<min) min=imin;
    }
  if(f==0) return;
  if(max<min) return;

  if(min==0) min=0.5;
  if(max==min) max++;
  
  double fmax = f->GetMaximum();
  double fmin = f->GetMinimum();
  f->GetXaxis()->CenterTitle(false);
  if( c->GetLogy() ) fmax *= 10;
  f->GetYaxis()->SetRangeUser( (min < fmin ? min : fmin ), (max > fmax ? max : fmax) );
  c->cd();
  f->Draw("axissame");
  c->Update();
}


/*-----------------------------------
  -----------------------------------*/
void PlotStyle::ReadjustCanvasFrame(TCanvas *c)
{
  if(c==0) return;
  TIter next(c->GetListOfPrimitives());
  TH1 *f=0;
  double max = -1e+23;  
  double min = 1e+23;
  TObject *obj=0;
  while ( (obj = next()) )
    {
      if(obj==0) continue;
      TString className = obj->ClassName();
      if(className.Contains("TH1")==false) continue;
      TString name = obj->GetName();
      if(name.Contains("Frame") ) { f = (TH1 *) obj; continue; }

      double imax=((TH1 *)obj)->GetMaximum()*1.05;
      double imin=((TH1 *)obj)->GetMinimum();
      if(imax>max) max=imax;
      if(imin<min) min=imin;
    }
  if(f==0) return;
  if(max<min) return;

  if(c->GetLogy() && min==0) min=0.5;
  else if( !c->GetLogy() ) min=0;
  if(max==min) max++;
  
  double fmax = f->GetMaximum();
  double fmin = f->GetMinimum();
  f->GetYaxis()->SetRangeUser( (min < fmin ? min : fmin ), (max > fmax ? max : fmax) );
  c->Update();
}

//
TCanvas* PlotStyle::GetMonitorCanvas(TString name, TString title,bool setBatch)
{
  
  if(tdrStyle!=0)
    {
      //      tdrStyle->SetCanvasDefH( (int)(700 *sqrt(2)/2) ); //Height of canvas *sqrt(2)/2
      // tdrStyle->SetCanvasDefW(700); //Width of canvas
      tdrStyle->SetCanvasDefH(600);
      tdrStyle->SetCanvasDefW(600);
      tdrStyle->SetOptStat(0);
      tdrStyle->SetHatchesSpacing(1.0);
    }
  TCanvas *c1 = new TCanvas(name,title,1);
  if(setBatch)  c1->SetBatch();
  c1->SetLeftMargin(0.15);
  c1->SetRightMargin(0.04);
  c1->SetBottomMargin(0.1);
  c1->SetTopMargin(0.05);
  c1->SetGridx();
  c1->SetGridy();
  return c1;
}


//
void PlotStyle::FormatHisto(TObject *h, 
		 int color, int line, int linew, int marker, int fill, 
		 bool detachFromCurrentDir, bool setSumw2, 
		 int linecolor, int fillcolor, int markercolor)
{
  if(h==0) return;
  if( linecolor < 0 ) linecolor=color;  
  if( fillcolor < 0 ) fillcolor=color; 
  if( markercolor < 0 ) markercolor=color;
  if( h->IsA()->InheritsFrom(TAttLine::Class()) ) {
    TAttLine * l = dynamic_cast<TAttLine *>(h);
    l->SetLineStyle(line);   l->SetLineWidth(linew); l->SetLineColor(linecolor);
  }
  if( h->IsA()->InheritsFrom(TAttMarker::Class()) ) {
    TAttMarker * m =  dynamic_cast<TAttMarker *>(h); 
    m->SetMarkerColor(markercolor);  m->SetMarkerStyle(marker); 
  }
  if( h->IsA()->InheritsFrom(TAttFill::Class()) ) {
    TAttFill * f = dynamic_cast<TAttFill *>(h); 
    f->SetFillColor(fillcolor); f->SetFillStyle(fill);
  }
  if( h->IsA()->InheritsFrom(TH1::Class()) ) {
    TH1 * h1 = (TH1 *) h; 
    h1->SetLineStyle(line);          h1->SetLineWidth(linew); h1->SetLineColor(linecolor);
    h1->SetMarkerColor(markercolor); h1->SetMarkerStyle(marker); 
    h1->SetFillColor(fillcolor);     h1->SetFillStyle(fill);
    if( setSumw2 ) h1->Sumw2();
    if( detachFromCurrentDir ) h1->SetDirectory(0);
  }
}

//
void PlotStyle::FixExtremityBins(TH1 *h,bool addOverflow, bool addUnderflow)
{
  if(h==0) return;

  if(addUnderflow)
    {
      double fbin  = h->GetBinContent(0) + h->GetBinContent(1);
      double fbine = sqrt(h->GetBinError(0)*h->GetBinError(0)
			  + h->GetBinError(1)*h->GetBinError(1));
      h->SetBinContent(1,fbin);
      h->SetBinError(1,fbine);
      h->SetBinContent(0,0);
      h->SetBinError(0,0);
    }
  
  if(addOverflow)
    {  
      int nbins = h->GetNbinsX();
      double fbin  = h->GetBinContent(nbins) + h->GetBinContent(nbins+1);
      double fbine = sqrt(h->GetBinError(nbins)*h->GetBinError(nbins) 
			  + h->GetBinError(nbins+1)*h->GetBinError(nbins+1));
      h->SetBinContent(nbins,fbin);
      h->SetBinError(nbins,fbine);
      h->SetBinContent(nbins+1,0);
      h->SetBinError(nbins+1,0);
    }
}
//
void PlotStyle::FormatAsFrameHisto(TH1 *f)
{
  if(f==0) return;
  f->GetYaxis()->SetTitleOffset(1.2);
  f->GetYaxis()->SetTitleSize(0.04);
  f->GetYaxis()->SetLabelSize(0.04);
  f->GetYaxis()->SetLabelFont(42);
  f->GetYaxis()->SetTitleFont(62);
  f->GetXaxis()->SetTitleOffset(0.9);
  f->GetXaxis()->SetLabelFont(42);
  f->GetXaxis()->SetTitleFont(62);
  f->GetXaxis()->SetLabelSize(0.04);
  f->GetXaxis()->SetTitleSize(0.04);
}


//
TList* PlotStyle::GetSummedUpRatio( TList &denplots, TList &numplots )
{
  if(denplots.GetEntries()==0 || numplots.GetEntries()==0) return 0;

  TIterator *plotsIt = denplots.MakeIterator();
  TH1 *mcsum = (TH1 *) plotsIt->Next();
  TString name = mcsum->GetName() + TString("_mcsum");
  mcsum = (TH1 *)mcsum->Clone(name);
  mcsum->SetName("Simulation");
  TObject *key = 0;
  while ( (key = plotsIt->Next()) ) 
    {
      TH1 *p = (TH1 *) key;
      if(p==0) continue;
      mcsum->Add(p);
    }


  plotsIt = numplots.MakeIterator();
  TH1 *datasum = (TH1 *) plotsIt->Next();
  name = datasum->GetName() + TString("_datasum");
  datasum = (TH1 *)datasum->Clone(name);
  datasum->SetName("Data");
  key = 0;
  while ( (key = plotsIt->Next()) ) 
    {
      TH1 *p = (TH1 *) key;
      if(p==0) continue;
      datasum->Add(p);
    }

  name = datasum->GetName() + TString("_ratio");
  TH1 *ratio =(TH1 *) datasum->Clone( name );
  ratio->Reset("ICE");
  ratio->Add(datasum);
  ratio->Add(mcsum,-1); 
  ratio->Divide(mcsum);
  ratio->GetYaxis()->SetTitle("Relative difference");
  ratio->GetYaxis()->SetRangeUser(-1.1,1.1);
  ratio->GetXaxis()->SetLabelSize(0.09);
  ratio->GetXaxis()->SetTitleSize(0.12);
  ratio->GetXaxis()->SetTitleOffset(0.5);
  ratio->GetYaxis()->SetLabelSize(0.09);
  ratio->GetYaxis()->SetTitleSize(0.08);
  ratio->GetYaxis()->SetTitleOffset(0.5);

  TList *res = new TList;
  res->Add(ratio);
  res->Add(mcsum);
  res->Add(datasum);

  return res;
}


//
void PlotStyle::FormatForCmsPublic(TCanvas * c, TLegend *leg, TString title, int nsamp, float legx, float legy, float legw, float legh, TString legopt)
{
  // float legx1=0.56, legx2=0.91;
  float legx1=legx, legx2=legx+legw;
  // float lowy = 0.91;
  float legy1 = legy, legy2 = legy;
  if(title.Length()!=0)
    {
      TObjArray * tokens = title.Tokenize("\\\\");
      int nt = tokens->GetEntries();
      // legy1 -= 0.05*(float)nt;
      legy1 -= legh*(float)nt;
      TPaveText *pave = new TPaveText(legx1,legy1,legx2,legy2,"NDC");
      pave->SetLineColor(kWhite);
      for(int it=0; it<nt; ++it) {
	TObjString * t = (TObjString *)tokens->At(it);
	TText *l1 = pave->AddText(t->GetString());
	l1->SetTextAlign(11);
      }
      pave->SetBorderSize(1);  
      pave->SetLineWidth(1); 
      pave->SetFillColor(kWhite);
      pave->SetFillStyle(0);      
      pave->Draw("same");
    }

  if(c)
    {
      c->SetLeftMargin(0.15);
      c->SetRightMargin(0.05);
      c->SetTopMargin(0.05);
      c->SetGridx(false);
      c->SetGridy(false);
    }

  if(leg==0)
    {
      leg = new TLegend(legx1,legy1-nsamp*legh,legx2,legy1,NULL,"brNDC");
      leg->SetBorderSize(1);
      leg->SetFillColor(0);
      leg->SetTextFont(42);
    }
  leg->SetEntryOption(legopt);
  leg->SetX1NDC(legx1);
  leg->SetY1NDC(legy1-nsamp*legh);
  leg->SetX2NDC(legx2);
  leg->SetY2NDC(legy1);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetLineWidth(1);
}


//
void PlotStyle::FormatLegendForMonitor(TLegend *leg,TString title)
{
  if(title.Length()!=0)
    {
      TPaveText *pave = new TPaveText(0.76,0.85,0.97,0.95,"NDC");
      TText *l1 = pave->AddText(title);
      l1->SetTextAlign(22);
      l1->SetTextSize(0.04);
      l1->SetTextColor(kBlue+3);
      pave->SetBorderSize(1);  
      pave->SetLineWidth(1); 
      pave->SetFillColor(kWhite);
      pave->SetFillStyle(0);      
      pave->Draw("same");
    }
  
  if(leg==0)
    {
      leg = new TLegend(0.65,0.9,0.98,1.0,NULL,"brNDC");
      leg->SetBorderSize(1);
      leg->SetFillColor(0);
      leg->SetTextFont(42);
    }
  leg->SetX1NDC(0.76);
  leg->SetY1NDC(0.45);
  leg->SetX2NDC(0.97);
  leg->SetY2NDC(0.84);
  leg->SetFillColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->SetLineWidth(1);
}
