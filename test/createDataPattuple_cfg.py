import os,sys
runOnMC=False
cfgFile=os.path.expandvars('${CMSSW_BASE}/src/LIP/TopTaus/test/createPattuple_cfg.py')
from LIP.TopTaus.CommandLineUtils_cff import configureSourceFromCommandLine
inList, outFile, trigpath = configureSourceFromCommandLine()
execfile(cfgFile)


