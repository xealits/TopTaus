-----------------------------------
Repository for tauDilepton analysis
-----------------------------------
* This is the manual. Read the fucking manual.

* Will work for ttbar cross section measurement as well as charged higgs search

* This started as a porting to a compiled binary approach like LIP/Top, in order
to remove levels of messiness in the original code to be compiled every time together with
its libraries

------------
Installation
------------

# 537
export SCRAM_ARCH=slc5_amd64_gcc462
scramv1 project CMSSW CMSSW_5_3_7
cd CMSSW_5_3_7/src/
cvs co -p UserCode/LIP/TopTaus/TAGS_2012.txt | sh

# 444
export SCRAM_ARCH=slc5_amd64_gcc434
scramv1 project CMSSW CMSSW_4_4_4
cd CMSSW_4_4_4/src
cvs co -p UserCode/LIP/TopTaus/TAGS_2011.txt | sh

Local test run (local data file at LIP)
cmsRun LIP/TopTaus/test/createDataPattuple_cfg.py /lustre/data3/cmslocal/samples/CMSSW_5_2_5/test/Run2012B_SingleMu_AOD_PromptReco-v1_000_193_998_0C7DCC80-4E9D-E111-B22A-001D09F25267.root pattuple.root inclusive_mu


---------------------------------
Instructions for running analysis
---------------------------------

Multiple likelihood fit for improving tau fakes uncertainty
-----------------------------------------------------------
runTauDileptonPDFBuilderFitter tauDileptonAnalysisParSets_cfg.py


Produce shapes:
---------------
produceLandSShapes tauDileptonAnalysisParSets_cfg.py





Changelog:
2012-09-21: shapes producer for lands and multiple likelihood fit for fake rates are ready to use.
2012-09-23: memory problem solved. Now both lands and mlf are usable with multiple variables


TODO:

- Pattuple producer 
  --> Check tauID discriminators. Still no answer from TauID HN. Sent another email soliciting clarifications.
  --> Check gsfElectrons/pfElectrons (wrote to Sadia and Tae Jeong)
  --> Remove PF2PAT from everything, and make sure HEEP ID is not used (bugs described in https://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=177218 )


- Ntuples producer --> Convert plugin

- Analysis code --> Convert Nuno's code

- MVA code --> port to binary and commit