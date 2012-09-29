#ifndef _Utilities_hh
#define _Utilities_hh


#if !defined(__CINT__) || defined(__MAKECINT__)

// System headers
#include <string>

// #include "EventSelection/test/RunOverMiniEvents.h" // FIXME was included but it is not necessary at all. Remove after successful compilation

#endif

namespace utilities{
  
  class StatUtils { // FIXME: perhaps put them as inline for quick access?
    // Error fractions
    double getErrorFraction( double a,double b);
    double getErrorFraction( double a,double b, double err_a, double err_b);
    double getErrorFractionWithSqrt(double a, double b);
    double getErrorFractionWithSqrt(double a, double b, double err_a, double err_b);
  };
  
  class EditorialUtils {
    // prepare document for pdf
    void prepareDocument(FILE * myfile);
  }; 
  
  
  class GenericUtils {
    
  public : 
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
