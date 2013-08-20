import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to physicsAnalysis
#########

PhysicsAnalysisParSet = cms.PSet(
    tauPtCut = cms.double(20.0),
    noUncertainties = cms.bool(False),
    doWPlusJetsAnalysis = cms.bool(True),
    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/merged/"),
    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test/"),
    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_All_73500.root"),
    runRange = cms.string("ABCD"),
    brHtaunu = cms.vdouble(
    0.831852,
    0.407966,
    0.228564,
    0.106446,
    0.04129783,
    0.02514242
#    0.831852,   
#    0.407966,   
#    0.228564,   
#    0.106446,   
#    0.04902523, 
#    0.04129783, 
#    0.0361988,  
#    0.02984434, 
#    0.02514242,     
    ),
    brHtb = cms.vdouble(
    #    1-0.831852,
    #    1-0.407966,
    #    1-0.228564,
    #    1-0.106446,
    #    1-0.04129783,
    #    1-0.02514242
    1-0.831852,   
    1-0.228564,   
    1-0.106446,   
    1-0.04902523, 
    1-0.04129783, 
    1-0.0361988,  
    1-0.02984434, 
    1-0.02514242     
    )
    
    )

#########
# Parameter file to give as input to doTauFakesStudy
#########
TauFakesStudyParSet = cms.PSet(
    # base is fakerate
    # percentage test is fakerate_fractionsEstimate
    # full tree extraction is fakerate_fullExtraction
    # test POG is the test with inverted weights files (computeAll only)
    tauPtCut = cms.double(20.0),
#    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
#    trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate_testPOG/trees/"),
#    configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate_testPOG/config/"),
#    trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate_testPOG/weights/"),
#    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate_testPOG/output/"),
#    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_All_73500.root"),
#    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
#    ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/")


    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
    trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/trees/"),
    configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/config/"),
    trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/weights/"),
    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/output/"),
    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_All_73500.root"),
    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
    ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/")


    )
