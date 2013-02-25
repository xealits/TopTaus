import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to physicsAnalysis
#########

PhysicsAnalysisParSet = cms.PSet(
    tauPtCut = cms.double(20.0),
    noUncertainties = cms.bool(True),
    doWPlusJetsAnalysis = cms.bool(False)
    )


