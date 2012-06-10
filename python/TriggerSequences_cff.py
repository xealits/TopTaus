import FWCore.ParameterSet.Config as cms

###
### trigger paths of interest
### Trigger evolution in 2011 cf. http://fwyzard.web.cern.ch/fwyzard/hlt/summary
###
def getTriggerPaths(version=2012) :

    #################
    # 2012 triggers #
    #################
    if(version==2012) :
        mcTrigs = []
        
        mutriglist = ['HLT_IsoMu24_v15']
        egtriglist = ['HLT_Ele27_WP80_PFMET_MT50_v4+HLT_Ele27_WP80_v10']
        jettriglist = ['HLT_Jet30U_v3']
        
        #################
        # 2011 triggers #
        #################
    else:
        
        mcTrigs = ['HLT_IsoMu17_v',
                   'HLT_DoubleMu7_v',
                   'HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v',
                   'HLT_Mu8_Ele17_CaloIdL_v',
                   'HLT_Mu10_Ele10_CaloIdL_v',
                   'HLT_Mu17_Ele8_CaloIdL_v']
        
        mutriglist = ['HLT_Ele17_CaloIdL_CaloIsoVL_Ele8_CaloIdL_CaloIsoVL_v',
                          'HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v']
        
        egtriglist = ['HLT_DoubleMu7_v',
                    'HLT_Mu13_Mu8_v',
                    'HLT_Mu17_Mu8_v',
                    'HLT_Mu17_TkMu8_v']
        
        jettriglist = ['HLT_Mu17_Ele8_CaloIdL_v',
                'HLT_Mu17_Ele8_CaloIdT_CaloIsoVL_v',
                'HLT_Mu8_Ele17_CaloIdL_v',
                'HLT_Mu8_Ele17_CaloIdT_CaloIsoVL_v']
        

    return mutriglist, egtriglist, jettriglist
    
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

    process.triggerselection = cms.Sequence()
    #trigger sequences 
    if(egmuTriggerFilter=="inclusive_eg") :
        process.inclusive_eg = cms.Sequence( process.eg_selector )        
        process.triggerselection = cms.Sequence( process.inclusive_eg*process.triggerselection )
    elif(egmuTriggerFilter=="exclusive_eg") :
        process.exclusive_eg = cms.Sequence( ~process.mu_selector*process.eg_selector )        
        process.triggerselection= cms.Sequence( process.exclusive_eg*process.triggerselection )
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
   
