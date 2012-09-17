import FWCore.ParameterSet.Config as cms

#from LIP.Top.MVAStudyConfig_cfi import *
#
#datapileup_710_2012=cms.vdouble(1.5451e-05,0.000100788,0.000396626,0.00113891,0.00262914,0.00516402,0.00895023,0.0140333,0.0202608,0.0272874,0.0346216,0.0417011,0.0479804,0.053012,0.0565029,0.058339,0.0585761,0.0574044,0.0550985,0.0519659,0.0483029,0.044365,0.0403523,0.0364078,0.0326242,0.0290553,0.0257273,0.0226496,0.019822,0.0172397,0.0148956,0.0127818,0.0108893,0.00920829,0.00772776,0.0064353,0.00531732,0.00435924,0.00354589,0.00286185,0.0022919,0.00182139,0.00143648,0.0011244,0.000873587,0.000673742,0.000515851,0.000392138,0.00029599,0.00022186,0.000165153,0.000122105,8.96737e-05,6.54205e-05,4.74151e-05,3.41437e-05,2.44304e-05,1.73704e-05,1.22738e-05,8.61939e-06,6.0163e-06,4.17416e-06,2.87891e-06,1.97393e-06,1.34559e-06,9.12007e-07,6.14631e-07,4.11897e-07,2.74504e-07,1.81936e-07,1.19929e-07,7.8631e-08,5.12798e-08,3.32667e-08,2.14686e-08,1.37834e-08,8.80413e-09,5.59523e-09,3.53812e-09,2.22623e-09,1.39391e-09,8.68533e-10,5.38572e-10,3.32375e-10,2.04155e-10,1.24813e-10,7.59525e-11,4.60076e-11,2.77422e-11,1.6653e-11,9.95184e-12,5.92094e-12,3.50729e-12,2.06854e-12,1.21475e-12,7.10314e-13,4.13596e-13,2.39816e-13,1.38476e-13,7.963e-14)
#

mFitPars = cms.PSet(
    outFolder        = cms.string("likelihoodFit/"),
    resultsFileName  = cms.string("likelihoodResults.txt"),
    
    baseMCDir        = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-mc-MU-20GeV/"),
    baseDataDir      = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-data-MU-20GeV/"),
    
    signalFileNameWH   = cms.string("out-wh-pythia-m120.root"),
    signalFileNameHH   = cms.string("out-hh-pythia-m120.root"),
    
    dataFileName       = cms.string("out-data.root"),
    ddBkgFileName      = cms.string("out-data_rescaled.root"),
    ttbarmcBkgFileName = cms.string("out-ttbar_mutau.root"),
    mcBkgFileName      = cms.string("out-mcbkg.root"),
    
    minitreeSelected   = cms.string("lep_tau_Selected"),
    minitreeDataDriven = cms.string("lep_tau_DataDriven"),

    #fitType     = cms.vstring( HIGGS2BKG, HIGGS3BKG, SM2BKG, SM3BKG)
    fitType     = cms.vint32( 0, 1, 2, 3),
    vars        = cms.vstring("rc_t", "pt_l", "pt_met", "multiplicity_j", "btagmultiplicity_j", "Dphi_tau_met"),
    mins        = cms.vdouble( 0,      0,      0,        2,                0,                    0            ),
    maxs        = cms.vdouble( 1.2,    200,    300,      7,                4,                    4.8          ),
    bins        = cms.vint32(    12,     20,     30,       5,                4,                    10           ),
    hmin        = cms.vdouble( 0,      0,      0,        0,                0,                    0            ),
    hmax        = cms.vdouble( 200,    100,    100,      400,              600,                  200          ),
    unbinned    = cms.vint32(  0,      0,      0,        0,                0,                    0            ),
    smoothOrder = cms.vint32(  3,      5,      5,        0,                0,                    3            )


#  vars.push_back("rc_t");       mins.push_back(0);  maxs.push_back(1.2); bins.push_back(12);hmin.push_back(0); hmax.push_back(100); unbinned.push_back(0); smoothOrder.push_back(3);
#  vars.push_back("pt_l");               mins.push_back(0);  maxs.push_back(200); bins.push_back(20);hmin.push_back(0); hmax.push_back(50) ; unbinned.push_back(0); smoothOrder.push_back(5);
#  //  vars.push_back("pt_t");               mins.push_back(0);  maxs.push_back(200); bins.push_back(20);hmin.push_back(0); hmax.push_back(60) ; unbinned.push_back(0); smoothOrder.push_back(5);
#  vars.push_back("pt_met");             mins.push_back(0);  maxs.push_back(200); bins.push_back(20);hmin.push_back(0); hmax.push_back(50) ; unbinned.push_back(0); smoothOrder.push_back(5);
#  vars.push_back("multiplicity_j");     mins.push_back(2);  maxs.push_back(7);   bins.push_back(5); hmin.push_back(0); hmax.push_back(200); unbinned.push_back(0); smoothOrder.push_back(0);
#  vars.push_back("btagmultiplicity_j"); mins.push_back(0);  maxs.push_back(4);   bins.push_back(4); hmin.push_back(0); hmax.push_back(300); unbinned.push_back(0); smoothOrder.push_back(0);
#  vars.push_back("Dphi_tau_met");       mins.push_back(0);  maxs.push_back(4.8); bins.push_back(10);hmin.push_back(0); hmax.push_back(100); unbinned.push_back(0); smoothOrder.push_back(3);
#
#  //  vars.push_back("Dphi_lepton_met");    mins.push_back(0);  maxs.push_back(200); bins.push_back(50); hmin.push_back(0); hmax.push_back(50);
#  //  vars.push_back("eta_t");              mins.push_back(0);  maxs.push_back(2.6); bins.push_back(20);hmin.push_back(0); hmax.push_back(60) ; unbinned.push_back(0); smoothOrder.push_back(5);
#  //  vars.push_back("eta_l");              mins.push_back(0);  maxs.push_back(2.6); bins.push_back(20);hmin.push_back(0); hmax.push_back(60) ; unbinned.push_back(0); smoothOrder.push_back(5);
#  //  vars.push_back("radius_t");               mins.push_back(0);  maxs.push_back(200); bins.push_back(20); hmin.push_back(0); hmax.push_back(60) ; unbinned.push_back(0); smoothOrder.push_back(5);  


#
#    input = cms.string("@input"),
#    outdir = cms.string("@outdir"),
#    kindir = cms.string("@kindir"),
#    isMC = cms.bool(@isMC),
#    xsec = cms.double(@xsec),
#    mctruthmode = cms.int32(@mctruthmode),
#    saveSummaryTree = cms.bool(@saveSummaryTree),
#    runSystematics = cms.bool(@runSystematics),
#    sfMetCut = cms.double(@sfMetCut),
#    ofMetCut = cms.double(@ofMetCut),
#    jetPtCut = cms.double(@jetPtCut),
#    evStart = cms.int32(@evStart),
#    evEnd = cms.int32(@evEnd),
#    dirName = cms.string("evAnalyzer/data"),
#    jesUncFileName = cms.string('${CMSSW_BASE}/src/LIP/Top/data/JEC11_V12_AK5PF_UncertaintySources.txt'),
#    datapileup = datapileup_710_2012,
#    useMVA = cms.bool(@useMVA),
#    tmvaInput =pairSelStudy
    )


