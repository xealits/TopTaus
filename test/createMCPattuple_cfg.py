import os,sys
runOnMC=True
runStd=True
cfgFile=os.path.expandvars('${CMSSW_BASE}/src/LIP/TopTaus/test/createPattuple_cfg.py')
from LIP.TopTaus.CommandLineUtils_cff import configureSourceFromCommandLine
inFile, outFile = configureSourceFromCommandLine()
execfile(cfgFile)


