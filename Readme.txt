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
setenv SCRAM_ARCH slc5_amd64_gcc462
scramv1 project CMSSW CMSSW_5_3_7
cd CMSSW_5_3_7/src/
cvs co -p UserCode/LIP/TopTaus/TAGS_2012.txt | sh

setenv SCRAM_ARCH slc5_amd64_gcc462
scramv1 project CMSSW CMSSW_5_3_7_patch4
cd CMSSW_5_3_7_patch4/src/
cvs co -p UserCode/LIP/TopTaus/TAGS_2012.txt | sh


# 444
setenv SCRAM_ARCH slc5_amd64_gcc434
scramv1 project CMSSW CMSSW_4_4_4
cd CMSSW_4_4_4/src
cvs co -p UserCode/LIP/TopTaus/TAGS_2011.txt | sh

Local test run (local data file at LIP)
cmsRun LIP/TopTaus/test/createDataPattuple_cfg.py /lustre/data3/cmslocal/samples/CMSSW_5_2_5/test/Run2012B_SingleMu_AOD_PromptReco-v1_000_193_998_0C7DCC80-4E9D-E111-B22A-001D09F25267.root pattuple.root inclusive_mu


---------------------------------
Instructions for running analysis
---------------------------------

Finding HLT trigger on a per-run-range basis, together with the prescale
------------------------------------------------------------------------
ssh lxplus.cern.ch # needed for access to database
cvs co HLTrigger/Tools/python
scram b
cd HLTrigger/Tools/python
source $CMSSW_BASE/LIP/TopTaus/scripts/getTriggers.sh
This will fetch the list of triggers, and depending on what you comment will print or not the prescales table. (see TODO list)


Multiple likelihood fit for improving tau fakes uncertainty
-----------------------------------------------------------
runTauDileptonPDFBuilderFitter tauDileptonAnalysisParSets_cfg.py

Relevant parameters: straightforward cfg file. Must write doc here, though

Produce shapes:
---------------
produceLandSShapes tauDileptonAnalysisParSets_cfg.py

Relevant parameters: straightforward cfg file.
- produceOnly: (necessary because in plots HH and WH are rescaled, in rootfile not)
	       -True: just produces rootfile with all the shapes, no plot.
	       -False: just produces plots, no rootfile.



Changelog for major updates:
2013-02-13: updated with 2012 cross sections - must check sample-specific ones
	    jet pt, btag, mlj added for leadingjet/nlj/nnlj
2013-02-11: updated JEC uncertainty sources
	    code is running on samples
	    lip-batch submission implemented
2013-02-08: ended base porting of the code. Now must move stuff to config file.
2013-02-07: added scripts/getTriggers.sh for easy trigger and prescales fetching
	    (will be crucial in particular for etau channel, later on)
2013-01-23: Likelihood fitter for tau fakes has RooDataset which will take into account correlations
            when performing the combined fit. Must just switch likelihoods to use it and to
	    product of likelihoods
2012-12-17: 2011 tagged (V12-12-17), start 2012 heavy developing and code porting
2012-09-23: memory problem solved. Now both lands and mlf are usable with multiple variables
2012-09-21: shapes producer for lands and multiple likelihood fit for fake rates are ready to use.

TODO:
- btagmulticorrected splitting by leadjet/nlj/nnlj

- check sample-specific 2012 cross-sections

- split JEC uncertainty sources into the 16 components.

- scripts/getTriggers.hs: implement running through sh getTriggers.sh PARS, in order to allow to choose
  at runtime whether to print the prescales table or just the triggers

- Make the unbinned fit work, caralho

- Apply single RooDataset per sample to LandSShapesProducer too

- Rework of LandSShapesProducer and PDFBuilderFitter
  --> New structure: -> single class for building datasets and models
                     -> one inheriting class for shapes production, storing and plotting (LandSShapesProducer)
		     -> one inheriting class for fitting, saving and plotting (PDFBuilderFitter)
		     -> perhaps one single executable with switch via ParSet in config file
- Pattuple producer 
  --> Acquire and improve version from 2013-01-23 - must upload from lipcms svn - in order to use
      multicrab in the future

- Ntuples producer --> Convert plugin

- Analysis code:
                 --> move hardcoded parameters to configuration file via edm::ParameterSet (minimize the need for recompiling)
		 --> import batch submission and modify process in order to run on a given subset of events.
		     (add s.th like void processEvents(uint firstEv, uint lastEv, string dataset){ if(string...) CutflowAnalyzer::process_ttbar(.. process(firstEv, lastEv) ..)  }

- MVA code --> port to binary and commit

- Tau energy scale for main analysis code. 

- Split fake contribution into e and mu
  --> Evan/Monica 2013-01-23: "there are different scale factors for e and mu fakes to be applied,
      so you better split the two components in the cutflow yields table"
      