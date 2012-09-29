#include "LIP/TopTaus/interface/Utilities.hh"

// System headers
#include <iostream>
#include <cmath>

namespace utilities{
  
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
