import FWCore.ParameterSet.Config as cms

def addElectronID(process, sequenceName, postfix) :
    """
    Adds electron ID working points to PAT electrons
    """
    getattr(process, "patElectrons" + postfix).addElectronID = cms.bool(True)
    
    electronIDs = cms.PSet(
        #SimpleCutsBasedEleID
        simpleEleId95relIso= cms.InputTag("simpleEleId95relIso"),
        simpleEleId90relIso= cms.InputTag("simpleEleId90relIso"),
        simpleEleId85relIso= cms.InputTag("simpleEleId85relIso"),
        simpleEleId80relIso= cms.InputTag("simpleEleId80relIso"),
        simpleEleId70relIso= cms.InputTag("simpleEleId70relIso"),
        simpleEleId60relIso= cms.InputTag("simpleEleId60relIso"),
        #Cuts in Categories (CiC) -non exhaustive (5 flavors here, up to 9 available)
        eidVeryLoose = cms.InputTag("eidVeryLoose"),
        eidLoose = cms.InputTag("eidLoose"),
        eidMedium = cms.InputTag("eidMedium"),
        eidTight = cms.InputTag("eidTight"),
        eidSuperTight = cms.InputTag("eidSuperTight"),
        #MVA
        mvaTrigV0 = cms.InputTag("mvaTrigV0"),
        mvaNonTrigV0 = cms.InputTag("mvaNonTrigV0"),
        )

    getattr(process, "patElectrons" + postfix).electronIDSources  = electronIDs.clone()

    # Define sequences
    process.load('LIP.TopTaus.ElectronIDSequences_cff')

    getattr(process, sequenceName + postfix).replace(
        getattr(process, "patElectrons" + postfix),
        process.patElectronIDSequence  + getattr(process, "patElectrons" + postfix)
        )
    
    
    
    # match the trigger information for selected pat leptons
    ##process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff")
    ##process.triggerProducerPF = process.patTrigger.clone()
    ##setattr( process, 'patTrigger' + postfix, process.triggerProducerPF )
    ##process.muTriggerMatchPF = cms.EDProducer( "PATTriggerMatcherDRDPtLessByR"
    ##                                           , src     = cms.InputTag( "patMuons" )
    ##                                           , matched = cms.InputTag( "patTrigger"+postfix )
    ##                                           , matchedCuts = cms.string( 'path( "HLT_*" )' )
    ##                                           , maxDPtRel = cms.double( 0.5 )
    ##                                           , maxDeltaR = cms.double( 0.5 )
    ##                                           , resolveAmbiguities    = cms.bool( True )
    ##                                           , resolveByMatchQuality = cms.bool( True )
    ##                                           )
    ##setattr( process, 'muTriggerMatch' + postfix, process.muTriggerMatchPF )
    ##process.eleTriggerMatchPF = cms.EDProducer( "PATTriggerMatcherDRDPtLessByR"
    ##                                    , src     = cms.InputTag( "patElectrons" )
    ##                                    , matched = cms.InputTag( "patTrigger"+postfix )
    ##                                    , matchedCuts = cms.string( 'path( "HLT_*" )' )
    ##                                    , maxDPtRel = cms.double( 0.5 )
    ##                                    , maxDeltaR = cms.double( 0.5 )
    ##                                    , resolveAmbiguities    = cms.bool( True )
    ##                                    , resolveByMatchQuality = cms.bool( True )
    ##                                    )
    ##setattr( process, 'eleTriggerMatch' + postfix, process.eleTriggerMatchPF )
    ##switchOnTriggerMatching( process,
    ##                         triggerProducer = 'patTrigger' + postfix,
    ##                         triggerMatchers = [ 'muTriggerMatch'+postfix, 'eleTriggerMatch' + postfix ],
    ##                         sequence        = 'patPF2PATSequence' + postfix,
    ##                         postfix         = postfix )
    ##removeCleaningFromTriggerMatching( process, sequence = 'patPF2PATSequence' + postfix )


