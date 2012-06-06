import FWCore.ParameterSet.Config as cms
import FWCore.PythonUtilities.LumiList as LumiList
import FWCore.ParameterSet.Types as CfgTypes

##
## standard pre-selection sequences
##
def addPreselectionSequences(process,
                             filterHBHEnoise=True
                             ) :

    # require scraping filter
    # https://twiki.cern.ch/twiki/bin/view/CMS/TKPOG2010Collisions
    # cut on monster events (mainly low quality tracks)
    process.scrapingVeto = cms.EDFilter("FilterOutScraping",
                                        applyfilter = cms.untracked.bool(True),
                                        debugOn = cms.untracked.bool(False), ## Or 'True' to get some per-event info
                                        numtrack = cms.untracked.uint32(10),
                                        thresh = cms.untracked.double(0.25)
                                        )
    
    # filter primary vertex
    # https://twiki.cern.ch/twiki/bin/view/CMS/TWikiTopRefEventSel#PAT_Configuration
    process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                               vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                               minimumNDOF = cms.uint32(4) ,
                                               # minimumNDOF = cms.uint32(7) ,
                                               maxAbsZ = cms.double(24),
                                               maxd0 = cms.double(2)
                                               )
    
    # HB/HE noise filter
    # https://twiki.cern.ch/twiki/bin/view/CMS/MissingETOptionalFilters
    if(filterHBHEnoise) :
        process.load('CommonTools/RecoAlgos/HBHENoiseFilter_cfi')
        process.HBHENoiseFilter.minIsolatedNoiseSumE = cms.double(999999.)
        process.HBHENoiseFilter.minNumIsolatedNoiseChannels = cms.int32(999999)
        process.HBHENoiseFilter.minIsolatedNoiseSumEt = cms.double(999999.)
        print "    Adding HB/HE noise filter"      
        process.basePreSel = cms.Sequence( process.basePreSel*process.HBHENoiseFilter )
        
    # filter counters
    process.preSelectionCounter = cms.EDProducer("EventCountProducer")
    process.noScrapCounter = process.preSelectionCounter.clone()
    process.goodVertexCounter = process.preSelectionCounter.clone()
    process.noHBHEnoiseCounter = process.preSelectionCounter.clone()
    
    # define a preselection sequence
    process.preselection = cms.Sequence(
        process.preSelectionCounter*
        process.scrapingVeto*
        process.noScrapCounter*
        process.primaryVertexFilter*
        process.goodVertexCounter*
        process.HBHENoiseFilter*
        process.noHBHEnoiseCounter
        )

    ###    # correct for EE misalignment -------------------------------------------
    ###    if(correctForEEmisalignment) :
    ###        process.load('RecoEgamma.EgammaTools.correctedElectronsProducer_cfi')
    ###        print '   GSF electrons will be reprocessed : add process.baseReReco to your standard sequence'
    ###        process.baseReReco = cms.Sequence( process.gsfElectrons )
    ###    # BSC activity ---------------------------------------------------------------
    ###    if(useTechTriggerBits) :
    ###        process.load('L1TriggerConfig.L1GtConfigProducers.L1GtTriggerMaskTechTrigConfig_cff')
    ###        process.load('HLTrigger/HLTfilters/hltLevel1GTSeed_cfi')
    ###        process.hltLevel1GTSeed.L1TechTriggerSeeding = cms.bool(True)
    ###        # process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('(0 AND 9) AND (40 OR 41) AND NOT (36 OR 37 OR 38 OR 39)')
    ###        #process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0 AND NOT (36 OR 37 OR 38 OR 39)')
    ###        process.hltLevel1GTSeed.L1SeedsLogicalExpression = cms.string('0')
    ###        print "   Adding technical trigger bits: " + str(process.hltLevel1GTSeed.L1SeedsLogicalExpression)
    ###        process.basePreSel = cms.Sequence( process.basePreSel*process.hltLevel1GTSeed )
    ###        

    print " *** Event preselection defined: " + str(process.preselection)
    
    """
    """
    def addLumifilter(process,fname='') :
        if(len(fname)>0):
            myLumis = LumiList.LumiList(filename = fname).getCMSSWString().split(',')
            process.source.lumisToProcess = CfgTypes.untracked(CfgTypes.VLuminosityBlockRange())
            process.source.lumisToProcess.extend(myLumis)
            print 'Lumi sections will be filtered with: ' + fname
            
            
            
            
            

