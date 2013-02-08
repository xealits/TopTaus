#ifndef _Utilities_hh
#define _Utilities_hh


#if !defined(__CINT__) || defined(__MAKECINT__)

// System headers
#include <string>
#include <cstdlib>

// #include "EventSelection/test/RunOverMiniEvents.h" // FIXME was included but it is not necessary at all. Remove after successful compilation

class THStack;
class TGraphErrors;
class TGraphAsymmErrors;
class TH1;

#endif

namespace utilities{
  
  class StatUtils { // FIXME: perhaps put them as inline for quick access?

  protected:
    // Error fractions

    double getErrorFraction( double a,double b);
    double getErrorFraction( double a,double b, double err_a, double err_b);
    double getErrorFractionWithSqrt(double a, double b);
    double getErrorFractionWithSqrt(double a, double b, double err_a, double err_b);
    void normalize(THStack& hstack, double norm);
    void getErrorBands(THStack& stack, TGraphErrors& myError);
    void getErrorBands(TH1& histo, TGraphErrors& myError);
    void getErrorBands(TH1& histo, TH1& varUp, TH1& varDown, TGraphAsymmErrors& myError);
  };
  
  class EditorialUtils {
  protected:
    // prepare document for pdf
    void prepareDocument(FILE * myfile);

    void SetTDRStyle();
  }; 
  
  
  class GenericUtils {
    
  public :  // FIXME: protected?
    inline std::string get_env_var( std::string const & key ) {                                 
      char * val;                                                                        
      val = getenv( key.c_str() );                                                       
      std::string retval = "";                                                           
      if (val != NULL) { retval = val;}                                                                                  
      return retval;                                                                        
    }
  };
}

#endif // _Utilities_hh
