import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to runTauDileptonPDFBuilderFitter and produceLandSShapes
#########

LandSShapesProducerParSet = cms.PSet(
    outFolder        = cms.string("shapesForDatacard/"),
    outputFileName  = cms.string("shapes"),

    baseMCDir        = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-mc-MU-20GeV/"),
    baseDataDir      = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-data-MU-20GeV/"),

#    baseMCDir        = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3_bakNew/nomtcut/nomt-2011-V1-mc-MU-20GeV/"),
#    baseDataDir      = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3_bakNew/nomtcut/nomt-2011-V1-data-MU-20GeV/"),


    signalFileNameWH   = cms.vstring("out-wh-pythia-m80.root", "out-wh-pythia-m100.root", "out-wh-pythia-m120.root", "out-wh-pythia-m140.root", "out-wh-pythia-m150.root", "out-wh-pythia-m155.root", "out-wh-pythia-m160.root",),
    signalFileNameHH   = cms.vstring("out-hh-pythia-m80.root", "out-hh-pythia-m100.root", "out-hh-pythia-m120.root", "out-hh-pythia-m140.root", "out-hh-pythia-m150.root", "out-hh-pythia-m155.root", "out-hh-pythia-m160.root"),

    outputFileNameSuffix   = cms.vstring("m80", "m100", "m120", "m140", "m150", "m155", "m160"),
    
    dataFileName       = cms.string("out-data.root"),
    ddBkgFileName      = cms.string("out-data_rescaled.root"),
    mcBkgFileName      = cms.vstring(    "out-ttbar_mutau.root", "out-ttbar_other.root", "out-singletop.root", "out-dibosons.root"      ),

    signalSampleNameWH   = cms.string("WH3"),
    signalSampleNameHH   = cms.string("HH3"),
    
    dataSampleName       = cms.string("data_obs"),
    ddBkgSampleName      = cms.string("tau-fake3"),
    mcBkgSampleName      = cms.vstring( "tt->ltau3", "tt~->ll3", "singleTop3", "di-boson3", "Z->ee,mumu3", "Z->tautau3" ),
    
    osCutEff               = cms.double(0.69*220.92/242.915), #    osCutEff               = cms.double(0.577749097), #223.82/387.4),
    #    "t#bar{t} #rightarrow #mu#tau_{h}", "other t#bar{t}", "Single t", "Diboson"

    minitreeSelected   = cms.string("m_tau_Selected"),
    minitreeDataDriven = cms.string("m_tau_DataDriven"),

    uncSources         = cms.vstring("jes", "unc" ),
    
    #fitType     = cms.vstring( HIGGS2BKG, HIGGS3BKG, SM2BKG, SM3BKG)
    #    fitType     = cms.vint32( 0),
    #, 2, 3, 0),

#    vars        = cms.vstring("rc_t", "pt_l", "pt_met", "multiplicity_j", "btagmultiplicity_j", "Dphi_tau_met"    ),
#    mins        = cms.vdouble(     0,      0,      0,        2,                0,                    0            ),
#    maxs        = cms.vdouble(   1.2,    200,    300,      7,                4,                    4.8            ),
#    bins        = cms.vint32(     12,     20,     30,       5,                4,                    10            ),
#    hmin        = cms.vdouble(     0,      0,      0,        0,                0,                    0            ),
#    hmax        = cms.vdouble(   200,    100,    100,      400,              600,                  200            ),
#    unbinned    = cms.vint32(      0,      0,      0,        0,                0,                    0            ),
#    smoothOrder = cms.vint32(      3,      5,      5,        0,                0,                    3            )

    vars        = cms.vstring( "pt_l", "pt_met", "multiplicity_j", "btagmultiplicity_j","rc_t", "Dphi_tau_met"    ),
    mins        = cms.vdouble(      0,      0,        2,                0,                   0,      0            ),
    maxs        = cms.vdouble(    200,    300,      7,                4,                   1.2,    4.8            ),
    bins        = cms.vint32(      20,     30,       5,                4,                   12,     10            ),
    hmin        = cms.vdouble(      0,      0,        0,                0,                   0,      0            ),
    hmax        = cms.vdouble(    100,    100,      400,              600,                 200,    200            ),
    unbinned    = cms.vint32(       0,      0,        0,                0,                   0,      0            ),
    smoothOrder = cms.vint32(       5,      5,        0,                0,                   3,      3            )

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
    
    minitreeSelected   = cms.string("m_tau_Selected"),
    minitreeDataDriven = cms.string("m_tau_DataDriven"),

    #fitType     = cms.vstring( HIGGS2BKG = 2, HIGGS3BKG = 3, SM2BKG = 0, SM3BKG = 1)
    fitType     = cms.vint32( 3),
    #, 2, 3, 0),
    useOS       = cms.bool(True),

    osCutEff               = cms.double(0.69*220.92/242.915),
    cHiggsBR               = cms.double(0.1),
    
    vars        = cms.vstring( "pt_l",  "multiplicity_j", "btagmultiplicity_j",  "Dphi_tau_met"    ),
    mins        = cms.vdouble(      0,       2,                0,                     0            ),
    maxs        = cms.vdouble(    200,     7,                4,                     4.8            ),
    bins        = cms.vint32(      20,      5,                4,                     10            ),
    hmin        = cms.vdouble(      0,       0,                0,                     0            ),
    hmax        = cms.vdouble(    100,     400,              600,                   200            ),
    unbinned    = cms.vint32(       0,       0,                0,                     0            ),
    smoothOrder = cms.vint32(       5,       0,                0,                     2            )

#"pt_met",
#     0,  
#   300,  
#    30,  
#     0,  
#   100,  
#     0,  
#     5,  

#"rc_t",
#     0,
#   1.2,
#    12,
#     0,
#   200,
#     0,
#     3,

    )


