import FWCore.ParameterSet.Config as cms

process = cms.Process("PAT")

# global options
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True),
                                      SkipEvent = cms.untracked.vstring('ProductNotFound')
                                      )

# event source
process.source = cms.Source("PoolSource",fileNames = cms.untracked.vstring())

# TODO: implement list in CommandLineUtils_cff.py for txt list of files# process.source.fileNames=inputList
process.source.fileNames=inList

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

# global tag
process.load("Configuration.StandardSequences.Geometry_cff")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.autoCond import autoCond
##if(runOnMC) : process.GlobalTag.globaltag=cms.string(autoCond.get('startup',autoCond['mc']))
##else        : process.GlobalTag.globaltag=cms.string(autoCond['com10'])
if ( runOnMC ): process.GlobalTag.globaltag = 'START52_V9::All'
else          : process.GlobalTag.globaltag = 'GR_R_52_V7::All'
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')

## Output Module Configuration
from LIP.TopTaus.OutputConfiguration_cff import configureOutput
process.out = cms.OutputModule("PoolOutputModule",
                               fileName = cms.untracked.string('patTuple.root'),
                               SelectEvents   = cms.untracked.PSet( SelectEvents = cms.vstring('p') ),
                               outputCommands = cms.untracked.vstring('keep *'))
process.out.fileName = cms.untracked.string(outFile)

if(runOnMC) :
    from LIP.TopTaus.GeneratorLevelSequences_cff import defineGeneratorLevelSequence
    addGeneratorLevelSequence(process)


################
# PRESELECTION #
################
from LIP.TopTaus.PreselectionSequences_cff import addPreselectionSequences, addLumifilter
if(not runOnMC ):
    addPreselectionSequences(process)
    addLumifilter(process,'lipcms/EventSelection/test/grid/8TeV/json/Cert_190456-195016_8TeV_PromptReco_Collisions12_JSON_v3.txt')
    
# trigger filter
from LIP.TopTaus.TriggerSequences_cff import addTriggerSequence, getTriggerPaths
if(not runOnMC):
    mutriglist, egtriglist, jettriglist = getTriggerPaths(version=2012)
    addTriggerSequence(process,trigpath,egtriglist,mutriglist)


jecLevels=['L1FastJet','L2Relative','L3Absolute']
if(not runOnMC) : jecLevels.append( 'L2L3Residual' )
 
#############################################
#  DEFINITION OF THE PFBRECO+PAT SEQUENCES  #
#############################################
# load the PAT config
process.load("PhysicsTools.PatAlgos.patSequences_cff")
from PhysicsTools.PatAlgos.tools.pfTools import *

# adding vbtf,CIC, HEEP electron IDs to both electron collections
from LIP.TopTaus.ElectronID_cff import addElectronID
#from SHarper.HEEPAnalyzer.HEEPSelectionCuts_cfi import *

# defining sequence for taus
process.reprocessTaus = cms.Sequence()
#adding custom detector based iso deposit
#from RecoLocalCalo.EcalRecAlgos.EcalSeverityLevelESProducer_cfi import *
#from CMGTools.Common.PAT.addLeptCustomIsoDeposit_cff import addMuonCustomIsoDeposit
#from CMGTools.Common.PAT.addLeptCustomIsoDeposit_cff import addElectronCustomIsoDeposit

# rho for JEC and isolation
from RecoJets.JetProducers.kt4PFJets_cfi import kt4PFJets
process.kt6PFJets = kt4PFJets.clone( rParam = cms.double(0.6),
                                     doAreaFastjet = cms.bool(True),
                                     doRhoFastjet = cms.bool(True),
                                     )
process.kt6PFJetsForIso = process.kt6PFJets.clone( Rho_EtaMax = cms.double(2.5),
                                                   Ghost_EtaMax = cms.double(2.5) )
process.pfOnlyNeutrals = cms.EDFilter("PdgIdPFCandidateSelector",
                                      src = cms.InputTag("particleFlow"),
                                      pdgId = cms.vint32(22,111,130,310,2112)
                                      )
process.kt6PFJetsCentralNeutral = process.kt6PFJets.clone( src = cms.InputTag("pfOnlyNeutrals"),
                                                           Ghost_EtaMax = cms.double(3.1),
                                                           Rho_EtaMax = cms.double(2.5),
                                                           inputEtMin = cms.double(0.5)
                                                           )

# postfixes      = ['PFlow',    'PFlowNoPuSub']
# doPFNoPU       = [True,       False]
# jetAlgo        = ['AK5',      'AK5']     
# jetAlgoPayLoad = ['AK5PFchs', 'AK5PF'] 

postfixes      = ['PFlow','PFlowNoGsf']
doPFNoPU       = [True, True]
jetAlgo        = ['AK5', 'AK5']     
jetAlgoPayLoad = ['AK5PFchs', 'AK5PFchs']
eleFromGsfElectrons = [True, False]

from CommonTools.ParticleFlow.Tools.enablePileUpCorrection import enablePileUpCorrection
for i in xrange(0,len(postfixes) ):

    i_jetAlgo        = jetAlgo[i]
    i_jetAlgoPayLoad = jetAlgoPayLoad[i]
    i_postfix        = postfixes[i]
    i_doPFNoPU       = doPFNoPU[i]
    i_eleFromGsfElectrons = eleFromGsfElectrons[i]
    
    print ' ******* Setting up PF2PAT sequence: ' + i_jetAlgo
    print '         PFnoPU will run? ' + str(i_doPFNoPU)
    print '         ' + i_jetAlgo + ' jet algo payload is: ' + i_jetAlgoPayLoad

    usePF2PAT(process,runPF2PAT=True, jetAlgo=i_jetAlgo, runOnMC=runOnMC, postfix=i_postfix, jetCorrections=(i_jetAlgoPayLoad, jecLevels))

    #    uselessModules=["produceCaloMETCorrections","pfCandsNotInJet","pfJetMETcorr","pfCandMETcorr",
    #                    "pfchsMETcorr","pfType1CorrectedMet","pfType1p2CorrectedMet"]
    if( i_eleFromGsfElectrons ) :
        useGsfElectrons(process,i_postfix)
        getattr(process,'patDefaultSequence'+i_postfix).replace( getattr(process,"patElectrons"+i_postfix),
                                                               process.pfParticleSelectionSequence +
                                                               process.eleIsoSequence +
                                                               getattr(process,"patElectrons"+i_postfix) )

    uselessModules=[]
    for uMod in uselessModules :
        getattr(process,"patDefaultSequence"+i_postfix).remove( getattr(process, uMod+i_postfix) )

    # configure muons
    #    getattr(process,"patMuons"+i_postfix).embedCaloMETMuonCorrs = False 
    #    getattr(process,"patMuons"+i_postfix).embedTcMETMuonCorrs = False
    getattr(process,"patMuons"+i_postfix).embedTrack = True
    #    getattr(process,"pfMuonsFromVertex"+i_postfix).dzCut = 99
    #    getattr(process,"pfMuonsFromVertex"+i_postfix).d0Cut = 99
    getattr(process,"pfSelectedMuons"+i_postfix).cut="pt()>3"

    # configure electrons
    getattr(process,"patElectrons"+i_postfix).embedTrack = True
    #    getattr(process,"pfElectronsFromVertex"+i_postfix).dzCut = 99
    #    getattr(process,"pfElectronsFromVertex"+i_postfix).d0Cut = 99
    getattr(process,"pfSelectedElectrons"+i_postfix).cut="pt()>5"
    #    from LIP.TopTaus.ElectronID_cff import addElectronID
    addElectronID(process, 'patDefaultSequence', i_postfix)
    # addPATElectronID( process, 'patDefaultSequence', i_postfix)    

    # configure jets
    enablePileUpCorrection( process, postfix=i_postfix)
    getattr(process,"patJetCorrFactors"+i_postfix).payload = i_jetAlgoPayLoad 
    getattr(process,"patJets"+i_postfix).embedPFCandidates = cms.bool(True)
    getattr(process,"patJets"+i_postfix).embedCaloTowers   = cms.bool(True)
    
    # use non pileup substracted rho as in the Jan2012 JEC set
#    getattr(process,"patJetCorrFactors"+i_postfix).rho = cms.InputTag("kt6PFJets","rho")

    #configure Top projectors
    getattr(process,"pfNoPileUp"+i_postfix).enable   = i_doPFNoPU
    getattr(process,"pfNoMuon"+i_postfix).enable     = False 
    getattr(process,"pfNoElectron"+i_postfix).enable = False 
    getattr(process,"pfNoTau"+i_postfix).enable      = False 
    getattr(process,"pfNoJet"+i_postfix).enable      = True
    getattr(process,"pfIsolatedMuons"+i_postfix).isolationCut = 999999
    getattr(process,"pfIsolatedElectrons"+i_postfix).isolationCut = 999999

    #    # insert the PFMET significance calculation
    #    from CMGTools.Common.PAT.addMETSignificance_cff import addMETSig
    #    addMETSig( process, i_postfix )

    #tau
    from LIP.TopTaus.TausTools_cff import configureTauProduction, removeHPSTauIsolation, embedPFCandidatesInTaus
    #  adaptSelectedPFJetForHpSTau
    configureTauProduction(process, i_postfix, runOnMC)
    removeHPSTauIsolation(process, i_postfix)
    #adaptSelectedPFJetForHPSTau(process,jetSelection="pt()>15.0",postfix=i_postfix)
    embedPFCandidatesInTaus( process, postfix=i_postfix, enable=True )
    #    embedPFCandidatesInTaus( process, postfix=i_postfix, enable=False)
    

    # cure photon matching in PAT
    from LIP.TopTaus.PhotonsTools_cff import removePhotonMatching
    removePhotonMatching( process, i_postfix )
    
    
### # adding "standard muons and electons"
### print ' ******* Adding standard muons electrons and photons'
### process.patMuons.embedTcMETMuonCorrs = False
### process.patMuons.embedCaloMETMuonCorrs = False
### process.patMuons.embedTrack = True
### process.patElectrons.pfElectronSource = 'particleFlow'
### process.patElectrons.embedTrack = True
### process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons', 'PFIso')
### process.muIsoSequence = setupPFMuonIso(process, 'muons', 'PFIso')
### adaptPFIsoMuons( process, applyPostfix(process,"patMuons",""), 'PFIso')
### adaptPFIsoElectrons( process, applyPostfix(process,"patElectrons",""), 'PFIso')
### process.stdMuonSeq = cms.Sequence( process.pfParticleSelectionSequence +
###                                    process.muIsoSequence +
###                                    process.makePatMuons +
###                                    process.selectedPatMuons
###                                    )
### process.stdElectronSeq = cms.Sequence( process.pfParticleSelectionSequence +
###                                        process.eleIsoSequence +
###                                        process.makePatElectrons +
###                                        process.selectedPatElectrons
###                                        )
### if(runOnMC) :
###     process.stdPhotonSeq = cms.Sequence( process.makePatPhotons )
### else :
###     process.patPhotons.addGenMatch = cms.bool(False)
###     process.patPhotons.embedGenMatch = cms.bool(False)
###     process.stdPhotonSeq = cms.Sequence( process.patPhotons )
### 
### #tweak further standard lepton/photon sequences
### addPATElectronID( process, 'stdElectronSequence', '')
### getattr(process, "patElectrons" ).electronIDSources.eidHEEP = cms.InputTag("HEEPId")
### heepBarrelCuts.minEt = 5
### heepEndcapCuts.minEt = 5
### process.HEEPId = cms.EDProducer("HEEPIdValueMapProducer",
###                                 eleLabel = cms.InputTag("gsfElectrons"),
###                                 barrelCuts = cms.PSet(heepBarrelCuts),
###                                 endcapCuts = cms.PSet(heepEndcapCuts),
###                                 eleIsolEffectiveAreas = cms.PSet ( trackerBarrel = cms.double(0.),
###                                                                    trackerEndcap = cms.double(0.),
###                                                                    ecalBarrel = cms.double(0.101),
###                                                                    ecalEndcap = cms.double(0.046),
###                                                                    hcalBarrel = cms.double(0.021),
###                                                                    hcalEndcap = cms.double(0.040)
###                                                                    ),
###                                 eleRhoCorrLabel = cms.InputTag("kt6PFJets","rho"),
###                                 applyRhoCorrToEleIsol = cms.bool(False),
###                                 writeIdAsInt =cms.bool(False)
###                                 )
### if not runOnMC:
###     process.stdMuonSeq.remove( process.muonMatch )
###     process.stdElectronSeq.remove( process.electronMatch )
###     process.patMuons.embedGenMatch = False
###     process.patElectrons.embedGenMatch = False
### 
    
#trigger stuff
process.load("PhysicsTools.PatAlgos.triggerLayer1.triggerProducer_cff")
process.patTrigger.processName = cms.string('*')


#####################
#  PATH DEFINITION  #
#####################
print ' ******* Defining paths to be run'

# counters that can be used at analysis level to know the processed events
process.startCounter = cms.EDProducer("EventCountProducer")
process.endCounter = cms.EDProducer("EventCountProducer")

#pat sequence
process.patSequence = cms.Sequence( process.startCounter
#                                    + process.HEEPId
                                    + process.patTriggerDefaultSequence + process.kt6PFJets 
                                    + process.kt6PFJetsForIso + process.pfOnlyNeutrals + process.kt6PFJetsCentralNeutral
                                    + process.reprocessTaus
#                                    + getattr(process,"reprocessTaus"+postfixes[0])
#                                    + getattr(process,"reprocessTaus"+postfixes[1])
                                    + getattr(process,"patPF2PATSequence"+postfixes[0])
                                    + getattr(process,"patPF2PATSequence"+postfixes[1])
#                                    + process.stdMuonSeq + process.stdElectronSeq + process.stdPhotonSeq
                                    )

# define the paths
if(runOnMC):
    process.stdPath = cms.Path(process.startCounter * process.patSequence )
else:
    process.stdPath = cms.Path(process.startCounter * process.preselection * process.triggerselection * process.patSequence )
process.e = cms.EndPath( process.endCounter*process.out )
    

#######################################
# SCHEDULE THE EXECUTION OF THE PATHS #
#######################################
configureOutput(process,selPaths=['stdPath'],outFile=outFile)
if(runOnMC) : process.schedule = cms.Schedule( process.genLevelPath, process.stdPath, process.e )
else        : process.schedule = cms.Schedule( process.stdPath, process.e )


print '******************'
print process.out.fileName
print '*******************'
