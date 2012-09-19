import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to runTauDileptonPDFBuilderFitter and produceLandSShapes
#########

LandSShapesProducerParSet = cms.PSet(
    outFolder        = cms.string("shapesForDatacard/"),
    outputFileName  = cms.string("shapes"),

    baseMCDir        = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-mc-MU-20GeV/"),
    baseDataDir      = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-data-MU-20GeV/"),

    signalFileNameWH   = cms.string("out-wh-pythia-m120.root"),
    signalFileNameHH   = cms.string("out-hh-pythia-m120.root"),
    
    dataFileName       = cms.string("out-data.root"),
    ddBkgFileName      = cms.string("out-data_rescaled.root"),
    mcBkgFileName      = cms.vstring("out-mcbkg.root"),

    signalSampleNameWH   = cms.string("wh-pythia-m120"),
    signalSampleNameHH   = cms.string("hh-pythia-m120"),
    
    dataSampleName       = cms.string("data"),
    ddBkgSampleName      = cms.string("data_rescaled"),
    mcBkgSampleName      = cms.vstring("mcbkg"),

    minitreeSelected   = cms.string("lep_tau_Selected"),
    minitreeDataDriven = cms.string("lep_tau_DataDriven"),

        #fitType     = cms.vstring( HIGGS2BKG, HIGGS3BKG, SM2BKG, SM3BKG)
    fitType     = cms.vint32( 3, 2, 3, 0),
    vars        = cms.vstring("rc_t", "pt_l", "pt_met", "multiplicity_j", "btagmultiplicity_j", "Dphi_tau_met"),
    mins        = cms.vdouble( 0,      0,      0,        2,                0,                    0            ),
    maxs        = cms.vdouble( 1.2,    200,    300,      7,                4,                    4.8          ),
    bins        = cms.vint32(    12,     20,     30,       5,                4,                    10           ),
    hmin        = cms.vdouble( 0,      0,      0,        0,                0,                    0            ),
    hmax        = cms.vdouble( 200,    100,    100,      400,              600,                  200          ),
    unbinned    = cms.vint32(  0,      0,      0,        0,                0,                    0            ),
    smoothOrder = cms.vint32(  3,      5,      5,        0,                0,                    3            )

    )

TauDileptonPDFBuilderFitterParSet = cms.PSet(
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
    fitType     = cms.vint32( 3, 2, 3, 0),
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


