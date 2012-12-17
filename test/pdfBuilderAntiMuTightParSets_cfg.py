import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to runTauDileptonPDFBuilderFitter and produceLandSShapes
#########


TauDileptonPDFBuilderFitterParSet = cms.PSet(
    outFolder        = cms.string("likelihoodFit_antiMuTight/"),
    resultsFileName  = cms.string("likelihoodResults"),
    
    baseMCDir        = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/newRun/nomt-2011-V1-mc-MU-20GeV/"),
    baseDataDir      = cms.string("/lustre/data3/cmslocal/vischia/tau_dilepton/newRun/nomt-2011-V1-data-MU-20GeV/"),

    signalFileNameWH   = cms.string("out-wh-pythia-m120.root"),
    signalFileNameHH   = cms.string("out-hh-pythia-m120.root"),
    
    dataFileName       = cms.string("out-data.root"),
    ddBkgFileName      = cms.string("out-data_rescaled.root"),
    ttbarmcBkgFileName = cms.string("out-ttbar_mutau.root"),
    mcBkgFileName      = cms.string("out-mcbkg.root"),
    
    minitreeSelected   = cms.string("m_tau_Selected"),
    minitreeDataDriven = cms.string("m_tau_DataDriven"),

    #fitType     = cms.vstring( HIGGS2BKG = 2, HIGGS3BKG = 3, SM2BKG = 0, SM3BKG = 1)
    fitType     = cms.vint32( 1),
    #, 2, 3, 0),
    useOS       = cms.bool(True),

#    osCutEff               = cms.double(0.69 * 220.92/242.915),
#    osCutEff               = cms.double(0.69 * 220.9/16.725),
    osCutEff               = cms.double(0.69 * 220.9/16.725),
    cHiggsBR               = cms.double(0.01),
    
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


