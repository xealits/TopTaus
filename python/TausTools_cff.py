import FWCore.ParameterSet.Config as cms
#import copy
#from PhysicsTools.PatAlgos.tools.tauTools import *
#from PhysicsTools.PatAlgos.tools.jetTools import *


def configureTauProduction(process, isMC=False) :
    #Re-process PFTau to give ak5pfjets as input to pftau, only for 36x MC data
    process.load("RecoTauTag.Configuration.RecoPFTauTag_cff")
    if(not isMC):
        removeMCMatching(process,['Taus'])
        

#removes isolation cut in PF2PAT and adds only the decayModeFinding (this is now compatible with PF2PAT in 5_2_3)
def removeHPSTauIsolation(process, postfix = ""):
    print "removing tau isolation discriminators and applying only the decayModeFinding "
    #note that isolation discriminators have an implicit decayModeFinding requirement
    getattr(process,"pfTausBaseSequence"+postfix).remove(applyPostfix(process,"pfTausBaseDiscriminationByLooseCombinedIsolationDBSumPtCorr",postfix))
    getattr(process,"pfTaus"+postfix).discriminators = [cms.PSet(discriminator = cms.InputTag("pfTausBaseDiscriminationByDecayModeFinding"+postfix),
                                                                 selectionCut = cms.double(0.5)
                                                                 )]
    
    
    
# insert a selected pfJet and apply it to the HPS Tau sequences (this is now compatible with PF2PAT in 5_2_3)
from CommonTools.ParticleFlow.ParticleSelectors.genericPFJetSelector_cfi import selectedPfJets
def adaptSelectedPFJetForHPSTau(process,
                                jetSelection = "",
                                postfix = ""):
    print "Preselecting the jets used to make taus : "+jetSelection
    process.pfJetsForHPSTau = selectedPfJets.clone()
    process.pfJetsForHPSTau.src = "pfJets"+postfix
    process.pfJetsForHPSTau.cut = cms.string(jetSelection)
    setattr(process,"pfJetsForHPSTau"+postfix, process.pfJetsForHPSTau)
    getattr(process,"pfTausPreSequence"+postfix).insert(0,getattr(process,"pfJetsForHPSTau"+postfix))
    applyPostfix(process,"pfTausBase",postfix).jetSrc = "pfJetsForHPSTau"+postfix
    # need to fix the tau presequence because it depends on the new jet collection
    applyPostfix(process,"pfJetTracksAssociatorAtVertex",postfix).jets = "pfJetsForHPSTau"+postfix
    applyPostfix(process,"pfTauPFJets08Region",postfix).src = "pfJetsForHPSTau"+postfix
    applyPostfix(process,"pfJetsPiZeros",postfix).jetSrc = "pfJetsForHPSTau"+postfix
    applyPostfix(process,"pfJetsLegacyTaNCPiZeros",postfix).jetSrc = "pfJetsForHPSTau"+postfix
    applyPostfix(process,"pfJetsLegacyHPSPiZeros",postfix).jetSrc = "pfJetsForHPSTau"+postfix
    # fix because the TopProjection in pfNoTau does not work because the taus come from the selected jets
    applyPostfix(process,"pfNoTau",postfix).bottomCollection = "pfJetsForHPSTau"+postfix
    # must use the pfJets for the patJets otherwise we will have the above selection on the pfJetsForHPSTau ( pfNoTau is a jet collection)
    print 'Warning!!!!!!!!!!!!!: switching patJet.jetSource from pfNoTau to pfJets because Tau cleaning on the jets cannot be applied currently'
    applyPostfix(process,"patJets",postfix).jetSource = "pfJets"+postfix
    
    
