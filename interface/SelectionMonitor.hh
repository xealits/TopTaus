#ifndef _SelectionMonitor_hh
#define _SelectionMonitor_hh

// -*- C++ -*-
//
// Package:    SelectionMonitor
// Class:      SelectionMonitor
//
/**\class SelectionMonitor SelectionMonitor.cc UserCode/LIP/TopTaus/interface/SelectionMonitor.cc

Description: Class for monitoring different selections

Implementation:
<Notes on implementation>
*/
//
// Original Author:  Pedro Silva
// Adapted. Pietro Vischia pietro.vischia@gmail.com
//         Created:  Mon Dec 28 17:39:00 WEST 2009
// $Id: SelectionMonitor.hh,v 0.1 2013/01/31 20:01:49 vischia Exp $
//
//



// system include files
#include<iostream>
#include<string>
#include<map>
#include<algorithm>
#include<vector>
#include<memory>

// user include files
#include "TH1D.h"
#include "TH2D.h"
#include "TString.h"

class SelectionMonitor {
  
public:

  /**
     CTOR
   */
  SelectionMonitor() 
  { 
  }

  //types
  typedef std::map<TString, TH1 *> Monitor_t;
  typedef std::map<TString, Monitor_t> StepMonitor_t;

  /**
     @short getters
   */
  inline StepMonitor_t &getAllMonitors() { return allMonitors_; }
  inline Monitor_t &getMonitorsAtStep(TString tag)
  {
    if(allMonitors_.find(tag) == allMonitors_.end()) initMonitorForStep(tag);
    return allMonitors_[tag];
  }
  inline TH1 *getHisto(TString name,TString tag="base") 
  {
    if( !hasStep(tag) ) return 0;
    if(allMonitors_[tag].find(name) == allMonitors_[tag].end()) return 0;
    return allMonitors_[tag][name];
  }

  /**
     @short checks if category exists
   */
  inline bool hasStep(TString tag) 
  {
    return ( allMonitors_.find(tag) != allMonitors_.end() );
  }

  /**
     @short takes care of filling an histogram
   */
  inline bool fillHisto(TString name, TString tag, double val, double weight=1,bool useBinWidth=false)
  {
    TH1 *h = getHisto(name,tag);
    if(h==0) return false;
    if(useBinWidth)  
      {
	int ibin =h->FindBin(val);
	double width = h->GetBinWidth(ibin);
	weight /= width;
      }
    h->Fill(val,weight);
    return true;
  }

  /**
     @short takes care of filling an histogram
   */
  inline bool fill2DHisto(TString name, TString tag, double valx, double valy, double weight=1,bool useBinWidth=false)
  {
    TH2 *h = (TH2 *)getHisto(name,tag);
    if(h==0) return false;
    if(useBinWidth)  
      {
	int ibin =h->FindBin(valx,valy);
	double width = h->GetBinWidth(ibin);
	weight /= width;
      }
    h->Fill(valx,valy,weight);
    return true;
  }
  
  /**
     @short inits the monitor plots for a new step
   */
  inline void initMonitorForStep(TString tag)
  {
    //clone all base histograms to the new monitor
    Monitor_t newMonitor;
    int monitorCtr = allMonitors_.size();
    for(Monitor_t::iterator hit = allMonitors_["base"].begin();
	hit != allMonitors_["base"].end();
	hit++)
      {
	TString baseName = hit->second->GetName();
	TString name = baseName + "_"; name +=  monitorCtr;
	TH1 *h = (TH1 *) hit->second->Clone(name);
	h->Reset("ICE");
	newMonitor[baseName] = h;
      }

    //add new monitor to the map
    allMonitors_[tag]=newMonitor;
  }
  
  /**
     @short add new histogram
   */
  TH1 * addHistogram(TH1 *h, TString tag) 
  { 
    allMonitors_["base"][tag] = h; 
    /*
      TString className = h->ClassName();
      if(className.Contains("TH1D" ) ) ((TH1D *)h)->Sumw2();
      if(className.Contains("TH1F" ) ) ((TH1F *)h)->Sumw2();
      if(className.Contains("TH1I" ) ) ((TH1I *)h)->Sumw2();
      if(className.Contains("TH2D" ) ) ((TH2D *)h)->Sumw2();
      if(className.Contains("TH2F" ) ) ((TH2F *)h)->Sumw2();
      if(className.Contains("TH2I" ) ) ((TH2I *)h)->Sumw2();
    */
    return allMonitors_["base"][tag];
  }
  
  /**
     DTOR
  */
  ~SelectionMonitor() { }

private:

  //all the selection step monitors
  StepMonitor_t allMonitors_;
};

#endif
