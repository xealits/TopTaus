import FWCore.ParameterSet.Config as cms
import os,sys
import getopt
import commands

"""
wrapper to read the configuration from command line
args: input_file output_file
"""
def configureSourceFromCommandLine() :
    inputFile='/store/cmst3/group/cmgtools/CMG/WWJetsTo2L2Nu_TuneZ2_7TeV-madgraph-tauola/Fall11-PU_S6-START44_V5-v1/AODSIM/V3'
    outputFile='output.root'
    ffile=0
    step=-1
    try:
        if(len(sys.argv)>2 ):
            if(sys.argv[2].find('.root')>=0: inputFile=sys.argv[2]
                if(len(sys.argv)>3 ):
                    if(sys.argv[3].find('.root')>0):  outputFile=sys.argv[3]
                    if(len(sys.argv)>4 ):
                        if(sys.argv[4].isdigit()) : step=int(sys.argv[4])
    except:
        print '[CommandLineUtils_cff.py] Could not configure from command line, will return default values'
    return inputFile, outputFile
# TODO: implement reading file list from .txt ##, fillFromCastor(castorDir,ffile,step)
    
                        
                        

                                                                                                 
