import FWCore.ParameterSet.Config as cms

##
## adds trigger sequence
##
def addTriggerSequence(process,
                       egmuTriggerFilter='',
                       egtriglist=['HLT_Ele10_LW_L1R'],
                       mutriggerlist=['HLT_Mu9'],
                       trigMenu='HLT'
                       ) :
    
    # single lepton inclusive triggers
    from HLTrigger.HLTfilters.hltHighLevel_cfi import hltHighLevel as trigbit_selector
    trigbit_selector.throw = cms.bool(False)
    process.eg_selector = trigbit_selector.clone()    
    process.eg_selector.TriggerResultsTag = cms.InputTag('TriggerResults', '', trigMenu)
    process.eg_selector.andOr=cms.bool(True)
    process.eg_selector.HLTPaths = cms.vstring(egtriglist)
    process.mu_selector = process.eg_selector.clone()
    process.mu_selector.andOr=cms.bool(True)
    process.mu_selector.HLTPaths = cms.vstring(mutriggerlist)
    process.egmu_selector = process.eg_selector.clone()
    process.egmu_selector.HLTPaths.extend( process.mu_selector.HLTPaths)
    process.egmu_selector.andOr=cms.bool(True)
    
    #trigger sequences 
    if(egmuTriggerFilter=="inclusive_eg") :
        process.inclusive_eg = cms.Sequence( process.eg_selector )        
        process.triggerselection = cms.Sequence( process.inclusive_eg*process.triggerselection )
    elif(egmuTriggerFilter=="exclusive_eg") :
        process.exclusive_eg = cms.Sequence( ~process.mu_selector*process.eg_selector )        
        process.triggerselection = cms.Sequence( process.exclusive_eg*process.triggerselection )
    elif(egmuTriggerFilter=="inclusive_mu") :
        process.inclusive_mu = cms.Sequence( process.mu_selector )
        process.triggerselection = cms.Sequence( process.inclusive_mu*process.triggerselection )
    elif(egmuTriggerFilter=="exclusive_mu") :
        process.exclusive_mu = cms.Sequence( ~process.eg_selector*process.mu_selector )
        process.triggerselection = cms.Sequence( process.exclusive_mu*process.triggerselection )
    elif(egmuTriggerFilter=="inclusive_egmu") :
        process.inclusive_egmu = cms.Sequence( process.egmu_selector )
        process.triggerselection = cms.Sequence( process.inclusive_egmu*process.triggerselection )
        if(len(egmuTriggerFilter)>0) :
        print '   EG/Mu trigger filter defined as: ' + egmuTriggerFilter
        
    print "   Trigger sequence is: " + str(process.triggerselection)
    print "   Note: process.triggerselection has to be added to your main sequence"
   
