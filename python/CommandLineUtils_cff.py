import FWCore.ParameterSet.Config as cms
import os,sys
import getopt
import commands

"""
wrapper to read the configuration from command line
args: input_file output_file
"""
def configureSourceFromCommandLine() :
    inputList=cms.untracked.vstring()
    outputFile='output.root'
    ffile=0
    step=-1
    trigpath=''
    try:
        if(len(sys.argv)>2 ):
            if(sys.argv[2].find('.root')>0) :
               inputList.extend( [ 'file:'+sys.argv[2] ] )
               if(len(sys.argv)>3 ):
                   if(sys.argv[3].find('.root')>0):  outputFile=sys.argv[3]
                   if(len(sys.argv)>4 ):
                       trigpath=sys.argv[4]
    except:
        print '[CommandLineUtils_cff.py] Could not configure from command line, will return default values'
    return inputList, outputFile, trigpath
# TODO: implement reading file list from .txt ##, fillFromCastor(castorDir,ffile,step)
    
                        
                        

                                                                                                 
