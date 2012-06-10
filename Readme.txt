-----------------------------------
Repository for tauDilepton analysis
-----------------------------------

* Will work for ttbar cross section measurement as well as charged higgs search


* This started as a porting to a compiled binary approach like LIP/Top, in order
to remove levels of messiness in the original code to be compiled every time together with
its libraries

Installation
# 55x
export SCRAM_ARCH=slc5_amd64_gcc462
scramv1 project CMSSW CMSSW_5_2_3_patch2
cd CMSSW_5_2_3_patch2/src/
cvs co -p UserCode/LIP/TopTaus/TAGS.txt | sh



TODO:

- Pattuple producer --> Must finish editing the createPattuple_cfg.py (arrived ad Trigger selection level)

- Ntuples producer --> Convert plugin

- Analysis code --> Convert Nuno's code