#include <TString.h>
#include <TFile.h>
#include <TH1D.h>
#include <iostream>
#include <sstream>
#include <iomanip>

void teenyWeenySearchNtot(){



  // data location in the tree ///////////////////////
  TString ndir("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/merged/");
  TString hdir("/lustre/ncg.ingrid.pt/cmslocal/vischia/ttbarDileptonAnalysis/topMassAnalysis/tDilbh/");
  TString cutflow("myEventSelector/Selection/cutflow");
  TString tag("data"); 
  TString id("myEventSelector");
  vector<TString> base;
  vector<double> max;
  vector<int> fromZero;
  vector<double> compareWith;
  base.push_back(    "ttbar_")       ;max.push_back(18); fromZero.push_back(0); compareWith.push_back(8228517.   );
  base.push_back("stopbar_s_")       ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(139974.	 );
  base.push_back("WJetsToLNu_")      ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(43636521.	 );
  base.push_back("dy_from50_")       ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(6084984.	 );
  base.push_back("dy_10_50_")	     ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(20751565.	 );
  base.push_back("WW_")	             ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(9000414.	 );
  base.push_back("WZ_")	             ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(9050268.	 );
  base.push_back("ZZ_") 	     ;max.push_back(50); fromZero.push_back(1);	compareWith.push_back(9022326.	 );
  base.push_back("htb-pythia-m180_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(300000.	 );
  base.push_back("htb-pythia-m200_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(299999.	 );
  base.push_back("htb-pythia-m220_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(299999.	 );
  base.push_back("htb-pythia-m240_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(300000.	 );
  base.push_back("htb-pythia-m250_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(299668.	 );
  base.push_back("htb-pythia-m260_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(299686.	 );
  base.push_back("htb-pythia-m280_") ;max.push_back(30); fromZero.push_back(1);	compareWith.push_back(300000.	 );
  base.push_back("htb-pythia-m300_") ;max.push_back(30); fromZero.push_back(1); compareWith.push_back(300000.    );


  for(size_t i=0; i<base.size(); ++i){
    cout << "Base: " << base[i] << endl;
    TString dir( base[i].Contains("htb-pythia") ? hdir : ndir);
    double nevts(0);
    if(fromZero[i]){
      for(size_t k=0; k<max[i]; ++k){
	stringstream sidx;
	sidx<<k;
	string idx=sidx.str();

	TFile* f= new TFile(dir + base[i] + TString(idx) + TString(".root"));
	TH1D* h = (TH1D*) f->Get(cutflow);
	if(h) nevts+=h->GetBinContent(1);
	else cout << "Damaged file " << k << endl;
      }
     
    }
    else{
      for(size_t k=1; k<=max[i]; ++k){
	stringstream sidx;
	sidx<<k;
	string idx=sidx.str();

	TFile* f= new TFile(dir + base[i] + TString(idx) + TString(".root"));
	TH1D* h = (TH1D*) f->Get(cutflow);
	if(h) nevts+=h->GetBinContent(1);
	else cout << "Damaged file " << i << endl;
      }
    }

    stringstream sghi;
    sghi<<compareWith[i];
    string ghi=sghi.str();
	
    TString result( (nevts== compareWith[i]) ? "OK" : "BAD: code has "+ghi+"events"  );
    cout << setprecision(10) << "events total: " << nevts << ", which is " << result << endl;
  }


}