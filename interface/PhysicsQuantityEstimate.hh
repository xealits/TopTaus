#ifndef _PhysicsQuantityEstimate_hh
#define _PhysicsQuantityEstimate_hh
/**                                                                                                                                                                              
																						 \class    PhysicsQuantityEstimate PhysicsQuantityEstimate.cc "UserCode/LIP/TopTaus/src/PhysicsQuantityEstimate.cc"                                                                     
  \brief    class for managing the estimate of a physics value
  
  \author   Pietro Vischia, pietro.vischia@gmail.com

  \version  $Id: PhysicsQuantityEstimate.hh,v 1.2 2012/09/23 14:21:56 vischia Exp $                                                                                                       
*/

//#if !defined(__CINT__) || defined(__MAKECINT__)
//
//#ifndef __CINT__
////#include "RooGlobalFunc.h"
//#endif

// System headers
#include <string>


// ROOT headers
#include "TString.h"

//#endif

class PhysicsQuantityEstimate{
  
public:
  PhysicsQuantityEstimate(std::string&);
  PhysicsQuantityEstimate(std::string&, double&, double&);
  PhysicsQuantityEstimate(std::string&, double&, double&, double&);
  PhysicsQuantityEstimate(std::string&, double&, double&, std::vector<double>&);
  std::string getName();
  double getStatError();
  double getSystError();
  double getTotalError();
  std::vector<double> getSystErrorComponents();
  
  void setName(std::string&);
  void setStatError (double&);
  void setSystError (double&);
  void setTotalError(double&);
  void setSystErrorComponents(std::vector<double>&);
  void addSystErrorComponent(double&);
  
private:
  std::string name_;
  double value_;
  double statError_;
  int systError_;
  std::vector<double> systErrorComponents_;

};

#endif //_PhysicsQuantityEstimate_hh


