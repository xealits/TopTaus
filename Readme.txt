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

Local test run (local data file at LIP)
cmsRun LIP/TopTaus/test/createDataPattuple_cfg.py /lustre/data3/cmslocal/samples/CMSSW_5_2_5/test/Run2012B_SingleMu_AOD_PromptReco-v1_000_193_998_0C7DCC80-4E9D-E111-B22A-001D09F25267.root pattuple.root inclusive_mu

TODO:

- Pattuple producer 
  --> Check tauID discriminators. Still no answer from TauID HN. Sent another email soliciting clarifications.
  --> Check gsfElectrons/pfElectrons (wrote to Sadia and Tae Jeong)
  --> Fix second PF2PAT in order to keep just the pfElectrons (or add the pfElectrons to the first PF2PAT)

- Ntuples producer --> Convert plugin

- Analysis code --> Convert Nuno's code