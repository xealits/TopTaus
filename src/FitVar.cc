#include "LIP/TopTaus/interface/FitVar.hh"

// System includes

// CMSSW includes

// ROOT includes

using namespace std;

FitVar::FitVar(string varName, double min, double max, double bins, double hmin, double hmax, Int_t unbinned, Int_t smoothOrder):
  varName_(varName),
  min_(min),
  max_(max),
  bins_(bins),
  hmin_(hmin),
  hmax_(hmax),
  unbinned_(unbinned),
  smoothOrder_(smoothOrder)
{
  
}

string FitVar::getVarName(){
  return varName_;
}

double FitVar::getMin(){
  return min_;
}

double FitVar::getMax(){
  return max_;
}

int FitVar::getBins(){
  return bins_;
}

double FitVar::getHmin(){
  return hmin_;
}

double FitVar::getHmax(){
  return hmax_;
}

Int_t FitVar::getUnbinned(){
  return unbinned_;
}

Int_t FitVar::getSmoothOrder(){
  return smoothOrder_;
}


