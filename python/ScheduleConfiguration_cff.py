import FWCore.ParameterSet.Config as cms

# adds/removes the outpath
def checkProcessSchedule(process, storeOutPath=True, debug=True) :
    
    if( storeOutPath ) :
        process.schedule.append( process.outpath )
    else :
        del process.outpath
    if(debug): print process.schedule
