
import FWCore.ParameterSet.Config as cms

#########
# Parameter file to give as input to physicsAnalysis
#########

PhysicsAnalysisParSet = cms.PSet(
    tauPtCut = cms.double(20.0),
    noUncertainties = cms.bool(False),
    doWPlusJetsAnalysis = cms.bool(True),
    #    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/merged/"),
    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_9_def/"),
    # inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/top_forTttbarReweighting_notWorking/"),
    # new
    #    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_preapp2/"),
    ## outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/"),
    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/olek/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_3/"),
    #outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/olek/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_electrons/"),
    #basepreapp
    #outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test2/"),
    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/olek/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/testNewPileup.root"),
    # puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins_true.root"),
    runRange = cms.string("ABCD"),
    brHtaunu = cms.vdouble(
    1,
    1,
    1,
    1,
    1,
    1
    #0.831852,
    #0.407966,
    #0.228564,
    #0.106446,
    #0.04129783,
    #0.02514242

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

    1,
    1,
    1,
    1,
    1,
    1,
    1,
    #1,
    1,
    1,
    1,
    1,
    1
    #1-0.831852,   
    #1-0.228564,   
    #1-0.106446,   
    #1-0.04902523, 
    #1-0.04129783, 
    #1-0.0361988,  
    #1-0.02984434, 
    #1-0.02514242     
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


    # inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
    # trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/trees/"),
    # configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/config/"),
    # trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/weights/"),
    # outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/chiggs/fakerate/output/"),
    # puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
    # #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
    # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test2/")

    # def #
    # def #    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
    # def #    trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/trees/"),
    # def #    configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/config/"),
    # def #    trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/weights/"),
    # def #    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/output/"),
    # def #    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
    # def #    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
    # def #    # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/")
    # def #    ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew/")
    # def #
    
    # test della roba di federico
    
# def 2 #    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
# def 2 #    trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/trees/"),
# def 2 #    configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/config/"),
# def 2 #    trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/"),
# def 2 #    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/out/"),
# def 2 #    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
# def 2 #    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
# def 2 #    # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/")
# def 2 #    ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_2/")


    inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
    trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/bbbfakerate/trees/"),
    configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/bbbfakerate/config/"),
    trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/bbbfakerate/weights/"),
    outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/bbbfakerate/output/"),
    puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
    #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
    # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/")
    ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew_2/")



    # # inputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/samples/CMSSW_5_3_7_patch4/fakerate/"),
    # # trainingTreesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/trees/"),
    # # configArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakerate/config/"),
    # # trainingOutputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/"),
    # # outputArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/fakescrosscheck/out/"),
    # # puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_9/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_70300_100bins.root"),
    # # #puFileName = cms.string("/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/data/pileup/MyDataPileupHistogram_RunAB_73500.root"),
    # # # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_noTopPtRew/")
    # # ntuplesArea = cms.string("/lustre/ncg.ingrid.pt/cmslocal/vischia/store/tau/Jan13ReReco/dataRedo/test_TopPtRew/")
    

    )
