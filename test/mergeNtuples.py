#!/usr/bin/env python

import os,sys
import json
import optparse
import commands

usage = 'usage: %prog [options]'
parser = optparse.OptionParser(usage)
parser.add_option('-i', '--inDir'      ,    dest='inDir'              , help='input directory containing the crab output'           , default='')
parser.add_option('-o', '--outDir'     ,    dest='outDir'             , help='output directory where the merged file will be moved' , default='')
parser.add_option('-n', '--outName'     ,    dest='outName'             , help='base nome for the merged files' , default='')
parser.add_option('-s', '--split'     ,    dest='split'             , help='split' , default='10', type=int)
(opt, args) = parser.parse_args()


os.system("mkdir -p " + opt.outDir) #make sure that the output dir exist

lscommand="ls -d -1 " + opt.inDir +"/*"
filenames = commands.getstatusoutput(lscommand)[1].split()

nfiles=len(filenames)
NFilesToMerge = nfiles//opt.split
NFilesToMergeRemains = nfiles%opt.split 
startFile = 0
endFile = 0

print "split "
print opt.split
print "files to merge "
print NFilesToMerge
print "remain "
print NFilesToMergeRemains

for segment in range(0,opt.split) :
    startFile = endFile 
    endFile   = endFile + NFilesToMerge
    #    print "-----"
    #    print "start "
    #    print startFile
    #    print "end "
    #    print endFile
    #    print "remain "
    #    print NFilesToMergeRemains
    appendString = ""
 
    if(NFilesToMergeRemains>0):
        endFile+=1
        NFilesToMergeRemains-=1

    for appendRange in range(startFile,endFile) :
        appendString+=" "
        appendString+=filenames[appendRange]

    mergedFileName = opt.outName
    if(opt.split>1): mergedFileName+='_' + str(segment)

    mergedFileName+= '.root'
    mergedFilePath = opt.outDir + "/" + mergedFileName
    
    os.system("hadd -f " + mergedFilePath + " " + appendString )
    os.system("rm -f " + appendString)
