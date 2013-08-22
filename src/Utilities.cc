#include "LIP/TopTaus/interface/Utilities.hh"

// System headers
#include <iostream>
#include <cmath>


//ROOT headers
#include <TStyle.h>
#include <TList.h>
#include <THStack.h>
#include <TGraphErrors.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>

namespace utilities{

  double PhysicsUtils::ttbarReweight(double genTPt, double genTbarPt){
    return ( (genTPt>400 || genTbarPt>400) ? 1.0 : sqrt( exp( 0.148-0.00129*genTPt + 0.148-0.00129*genTbarPt ) )         );
    // dilepton values at 8 TeV (https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting#MC_SFs_Reweighting)                                                                     
  }


  
  double StatUtils::getErrorFraction( double a,double b){
    double ret(0);
    if (b){  
      double temp = fabs(a)/(b*b)+ fabs( (a*a)/(b*b*b)) ;
      ret = sqrt(temp);
    }
    return ret;
  }
  
  
  double StatUtils::getErrorFraction( double a,double b, double err_a, double err_b){
    double ret(0);
    if (b){  
      double temp = (err_a*err_a)/(b*b)+ ( (a*a)/(b*b*b*b) )*( err_b*err_b )  ;
      ret = sqrt(temp);
    }
    return ret;
  }



  double StatUtils::getErrorFractionWithSqrt(double a, double b){
  
    double ret(0); 
    if(b){
      double temp = fabs(a/b) + (1./4.)*(a/b)*(a/b);
      ret= sqrt(temp);
    }   
  
    return ret;
  }

  double StatUtils::getErrorFractionWithSqrt(double a, double b, double err_a, double err_b){
  
    double ret(0); 
    if(b){
      double temp = (err_a*err_a)/b + (1./4.)*((a*a)/(b*b*b))*(err_b*err_b);
      ret= sqrt(temp);
    }   
  
    return ret;
  }

  void StatUtils::normalize(THStack& hstack, double norm){
    
    double total(0);
        
    TList * list = hstack.GetHists();
    
    int listSize = list->GetSize();
    
    for(int i=0;i< listSize;i++){
      TH1 *h = (TH1*)list->At(i);
      if( h ) { total += h->Integral();}
    }
    float factor(0);
    if( total ){ factor=norm/total;}
    for(int i=0;i< listSize;i++){
      TH1 *h = (TH1*)list->At(i);
      if( h ) { h->Scale(factor);}
    }  
    
    //////////////////////////////////////////////////
  }
  
  void StatUtils::getErrorBands(THStack& hstack, TGraphErrors& myError){
    
    TList * list = hstack.GetHists();
    
    int listSize = list->GetSize();
    TH1* errorH = 0;
    for(int j=0;j< listSize;j++){
      TH1 *histo = (TH1*)list->At(j);
      
      TH1 * temp = (TH1 *) histo->Clone();  
      if(j==0) errorH = ( TH1 * ) histo->Clone("myErrors"); errorH->Sumw2();

      int numberOfBins = temp->GetNbinsX();
      for(int n=1;n<=numberOfBins; n++){
	double value  = temp->GetBinContent(n);
	double errorN = temp->GetBinError(n);
	
	double myGraphError = myError.GetErrorY(n);
	
	double errorContribution2 =  value *0.05 * value*0.05;
	temp->SetBinError( n, sqrt( errorContribution2 + errorN*errorN ));
	if(j==0) errorH->SetBinError( n, sqrt( errorContribution2 + errorN*errorN ));
	myError.SetPoint(n, temp->GetBinCenter(n), errorH->GetBinContent(n)  );
	myError.SetPointError(n, temp->GetBinWidth(n)/2, errorH->GetBinError(n) ); //sqrt( myGraphError*myGraphError + temp->GetBinError(n) * temp->GetBinError(n) ));
	
      }
      if(j!=0) errorH->Add(temp);
    }
    for(int n=1;n<=errorH->GetNbinsX(); n++){
    myError.SetPoint(n, errorH->GetBinCenter(n), errorH->GetBinContent(n)  );
    myError.SetPointError(n, errorH->GetBinWidth(n)/2, errorH->GetBinError(n) ); //sqrt( myGraphError*myGraphError + temp->GetBinError(n) * temp->GetBinError(n) ));
    }
  }

  void StatUtils::getErrorBands(TH1& histo, TGraphErrors& myError){
    
    int numberOfBins = histo.GetNbinsX();
    for(int n=1;n<=numberOfBins; n++){
      double value  = histo.GetBinContent(n);
      double errorN = histo.GetBinError(n);
      
      double myGraphError = myError.GetErrorY(n);
      
      double errorContribution2 =  value *0.05 * value*0.05;
      histo.SetBinError( n, sqrt( errorContribution2 + errorN*errorN ));
      myError.SetPoint(n, histo.GetBinCenter(n), histo.GetBinContent(n)  );
      myError.SetPointError(n, histo.GetBinWidth(n)/2, histo.GetBinError(n) ); //sqrt( myGraphError*myGraphError + temp->GetBinError(n) * temp->GetBinError(n) ));
    }
  }

  void StatUtils::getErrorBands(TH1& histo, TH1& varUp, TH1& varDown, TGraphAsymmErrors& myError){
    
    int numberOfBins = histo.GetNbinsX();
    for(int n=1;n<=numberOfBins; n++){
      double value  = histo.GetBinContent(n);
      double errorN = histo.GetBinError(n);
      
      double myGraphError = myError.GetErrorY(n);
      
      double errorContributionUp =   varUp.GetBinContent(n) - value;
      double errorContributionDown = varDown.GetBinContent(n) - value;

      std::cout << "contribution up " << errorContributionUp <<  ", contribution down " << errorContributionDown << ", integral base " << histo.Integral() << ", integral up " << varUp.Integral() << ", integral down " << varDown.Integral() << std::endl;

      
      //      histo.SetBinError( n, sqrt( errorContribution2 + errorN*errorN ));
      myError.SetPoint(n, histo.GetBinCenter(n), histo.GetBinContent(n)  );
      //      myError.SetPointError(n, histo.GetBinWidth(n)/2, histo.GetBinError(n) ); //sqrt( myGraphError*myGraphError + temp->GetBinError(n) * temp->GetBinError(n) ));
      
      myError.SetPointEXhigh(n, histo.GetBinWidth(n)/2);
      myError.SetPointEXlow( n, histo.GetBinWidth(n)/2);
      myError.SetPointEYhigh(n, sqrt( errorContributionUp*errorContributionUp   + errorN*errorN ));
      myError.SetPointEYlow( n, sqrt( errorContributionDown*errorContributionDown + errorN*errorN ));
      
    }
  }
  
//    }     
//  }
//  //normalize(errorH, i);
//  errorH->SetFillColor(4);
//    //    errorH->SetFillStyle(3001);
//    errorH->SetFillStyle(3004);
////    errorH->SetLineColor(616);
////    errorH->SetLineWidth(4);
////    errorH->SetLineStyle(7);
//
//    myError->SetFillColor(1);
//    //    errorH->SetFillStyle(3001);
//    myError->SetFillStyle(3004);
//
//    ///    errorH->Draw("e2p same");
//    myError->Draw("2same");
//
//    double StatUtils::getError2OnSystematics( TString sampName, double value ){
//      
//      double xsecError(0);
//      double puError(0);
//      double btagError(0);
//      double jesError(0);
//      double tauIdError(0);
//      double leptonError(0);
//      double effError(0);
//      double osError(0.12);
//      
//      //  leptonError=0.03; // electron case
//      leptonError=0.02; // muon case
//      
//      
//      
//      if( sampName.Contains("QCD")                  ){ xsecError = 0.50; puError= 0.05; btagError=0.05; jesError=0.05 ; tauIdError=0.15;                }
//      if( sampName.Contains("Single t")             ){ xsecError = 0.08; puError= 0.02; btagError=0.07; jesError=0.06 ; tauIdError=0.06;                }
//      if( sampName.Contains("t#bar{t} #rightarrow") ){ xsecError = 0.10; puError= 0.02; btagError=0.05; jesError=0.05 ; tauIdError=0.06; effError=0.04; }
//      if( sampName.Contains("other t#bar{t}")       ){ xsecError = 0.10; puError= 0.08; btagError=0.04; jesError=0.04 ; tauIdError=0.06;                } 
//      if( sampName.Contains("DY+jets")              ){ xsecError = 0.04; puError= 0.04; btagError=0.08; jesError=0.05 ; tauIdError=0.15;                }
//      if( sampName.Contains("Diboson")              ){ xsecError = 0.04; puError= 0.04; btagError=0.08; jesError=0.05 ; tauIdError=0.15;                }
//      if( sampName.Contains("W+jets" )              ){ xsecError = 0.05; puError= 0.08; btagError=0.08; jesError=0.06 ; tauIdError=0.06;                }
//      
//      // if btag plot
//      //btagError=0; 
//      //tauIdError=0; osError=0;
//      
//      double totalError2 = pow(value*btagError,2)+ pow(value*LUM_ERR,2)+pow(value*xsecError,2)+pow(value*puError,2)+pow(value*tauIdError,2)+pow(value*jesError,2)+pow(value*leptonError,2) + pow(value*effError,2);
//      
//      return totalError2;
//      
//      
//    }
//    
//    
//    

    void EditorialUtils::SetTDRStyle(){
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


  }
  void EditorialUtils::prepareDocument(FILE * myfile){


    //fprintf(myfile,"\\documentclass[landscape,twocolumn,letterpaper]{article} \n");
    fprintf(myfile,"\\documentclass[landscape,letterpaper]{article} \n");
    fprintf(myfile,"\\pagestyle{empty} \n");
    fprintf(myfile,"\\usepackage{epsfig} \n");
    fprintf(myfile,"\\usepackage{amsmath} \n");
    fprintf(myfile,"\\usepackage{array} \n");
    fprintf(myfile,"\\usepackage[cm]{fullpage} \n");
    //    fprintf(myfile,"\\parindent 0.25in \n");
    //    fprintf(myfile,"\\oddsidemargin -0.90in \n");
    //    fprintf(myfile,"\\evensidemargin -0.90in \n");
    //    fprintf(myfile,"\\columnsep 0.2in \n");
    //    fprintf(myfile,"\\topmargin=-0.25in \n");
    //    fprintf(myfile,"\\headheight=0.1in \n");
    //    fprintf(myfile,"\\headsep=0.2in \n");
    //    fprintf(myfile,"\\footskip=0.3in \n");
    fprintf(myfile,"\\textheight = 8.in \n");
    fprintf(myfile,"\\textwidth 7.0in \n");                             
    fprintf(myfile,"\\begin{document} \n");


}

} // namespace utils
