#!/bin/bash

DATAPLACE="/lustre/data3/cmslocal/samples/CMSSW_5_3_7_patch4/HEREPUTxsecORchiggs/nomt-2012-V1-mc-MU-20GeV/"

cd $DATAPLACE

hadd -f $DATAPLACE/out-wjets.root $DATAPLACE/out-wjets_*root
hadd -f $DATAPLACE/out-stop_t.root $DATAPLACE/out-stop_t_*root
hadd -f $DATAPLACE/out-ttbar.root $DATAPLACE/out-ttbar_v1.root $DATAPLACE/out-ttbar_v2.root
hadd -f $DATAPLACE/out-ttbar_mutau.root $DATAPLACE/out-ttbar_v1_mutau.root $DATAPLACE/out-ttbar_v2_mutau.root
hadd -f $DATAPLACE/out-ttbar_ddbkg.root $DATAPLACE/out-ttbar_v1_ddbkg.root $DATAPLACE/out-ttbar_v2_ddbkg.root
hadd -f $DATAPLACE/out-ttbar_mcbkg.root $DATAPLACE/out-ttbar_v1_mcbkg.root $DATAPLACE/out-ttbar_v2_mcbkg.root

# QCD
hadd -f $DATAPLACE/out-qcd.root $DATAPLACE/out-qcd*root $DATAPLACE/out-PhotonJets*root

# zjets
hadd -f $DATAPLACE/out-zjets.root $DATAPLACE/out-dy*root 

# single top
hadd -f $DATAPLACE/out-singletop.root $DATAPLACE/out-stop*.root

# diboson
hadd -f $DATAPLACE/out-dibosons.root $DATAPLACE/out-ww.root $DATAPLACE/out-wz.root $DATAPLACE/out-zz.root

## mutau
#hadd -f $DATAPLACE/out-ttbar_mutau.root $DATAPLACE/out-ttbar_mutau_*.root
## normal
#hadd -f $DATAPLACE/out-ttbar.root $DATAPLACE/ttbar/out-ttbar_*.root


# mc total
hadd -f $DATAPLACE/out-mc.root $DATAPLACE/out-qcd.root $DATAPLACE/out-wjets.root $DATAPLACE/out-zjets.root $DATAPLACE/out-dibosons.root $DATAPLACE/out-ttbar.root $DATAPLACE/out-singletop.root

# data driven bkg
hadd -f $DATAPLACE/out-ddbkg.root $DATAPLACE/out-qcd.root $DATAPLACE/out-wjets.root $DATAPLACE/out-ttbar_ddbkg.root
#hadd -f $DATAPLACE/out-ddbkg.root $DATAPLACE/out-qcd.root $DATAPLACE/out-wjets.root $DATAPLACE/out-ttbar_ddbkg_*.root

# mc bkg (w.r.t. top xsec measurement: ttbar is signal for this)
hadd -f $DATAPLACE/out-mcbkg.root $DATAPLACE/out-zjets.root $DATAPLACE/out-singletop.root $DATAPLACE/out-dibosons.root $DATAPLACE/out-ttbar_mcbkg.root
#hadd -f $DATAPLACE/out-mcbkg.root $DATAPLACE/out-zjets.root $DATAPLACE/out-singletop.root $DATAPLACE/out-dibosons.root $DATAPLACE/out-ttbar_mcbkg_*.root

# other cont
hadd -f $DATAPLACE/out-ttbar_other.root  $DATAPLACE/out-ttbar_ddbkg.root $DATAPLACE/out-ttbar_mcbkg.root

# allmcmkg (w.r.t. charged higgs search: ttbar is includes as irreducible background)
hadd -f $DATAPLACE/out-allmcbkg.root $DATAPLACE/out-ttbar.root $DATAPLACE/out-mcbkg.root


# higgs  mass points
#hadd -f $DATAPLACE/out-higgs-pythia-m80.root $DATAPLACE/out-hh-pythia-m80.root $DATAPLACE/out-wh-pythia-m80.root
#hadd -f $DATAPLACE/out-higgs-pythia-m80_mutau.root $DATAPLACE/out-hh-pythia-m80_mutau.root $DATAPLACE/out-wh-pythia-m80_mutau.root

#hadd -f $DATAPLACE/out-higgs-pythia-m100.root $DATAPLACE/out-hh-pythia-m100.root $DATAPLACE/out-wh-pythia-m100.root
#hadd -f $DATAPLACE/out-higgs-pythia-m100_mutau.root $DATAPLACE/out-hh-pythia-m100_mutau.root $DATAPLACE/out-wh-pythia-m100_mutau.root

hadd -f $DATAPLACE/out-higgs-pythia-m120.root $DATAPLACE/out-hh-pythia-m120.root $DATAPLACE/out-wh-pythia-m120.root
hadd -f $DATAPLACE/out-higgs-pythia-m120_mutau.root $DATAPLACE/out-hh-pythia-m120_mutau.root $DATAPLACE/out-wh-pythia-m120_mutau.root

#hadd -f $DATAPLACE/out-higgs-pythia-m140.root $DATAPLACE/out-hh-pythia-m140.root $DATAPLACE/out-wh-pythia-m140.root
#hadd -f $DATAPLACE/out-higgs-pythia-m140_mutau.root $DATAPLACE/out-hh-pythia-m140_mutau.root $DATAPLACE/out-wh-pythia-m140_mutau.root

#hadd -f $DATAPLACE/out-higgs-pythia-m150.root $DATAPLACE/out-hh-pythia-m150.root $DATAPLACE/out-wh-pythia-m150.root
#hadd -f $DATAPLACE/out-higgs-pythia-m150_mutau.root $DATAPLACE/out-hh-pythia-m150_mutau.root $DATAPLACE/out-wh-pythia-m150_mutau.root

#hadd -f $DATAPLACE/out-higgs-pythia-m155.root $DATAPLACE/out-hh-pythia-m155.root $DATAPLACE/out-wh-pythia-m155.root
#hadd -f $DATAPLACE/out-higgs-pythia-m155_mutau.root $DATAPLACE/out-hh-pythia-m155_mutau.root $DATAPLACE/out-wh-pythia-m155_mutau.root

#hadd -f $DATAPLACE/out-higgs-pythia-m160.root $DATAPLACE/out-hh-pythia-m160.root $DATAPLACE/out-wh-pythia-m160.root
#hadd -f $DATAPLACE/out-higgs-pythia-m160_mutau.root $DATAPLACE/out-hh-pythia-m160_mutau.root $DATAPLACE/out-wh-pythia-m160_mutau.root

DATAPLACE="/lustre/data3/cmslocal/samples/CMSSW_5_3_7_patch4/HEREPUTTxsecORchiggs/nomt-2012-V1-data-MU-20GeV/"
#DATAPLACE="/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-data-MU-20GeV/"

cd $DATAPLACE

hadd -f out-data.root out-Muon_RunA.root out-Muon_RunA_06AugRecover.root out-Muon_RunB.root out-Muon_RunC1.root out-Muon_RunC2.root out-Muon_RunD.root



# To remove processed files uncoment lines below here ===============================================================
#DATAPLACE="/lip-sw/cmssw/users/nalmeida/new/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/mt-2011-V1-mc-MU-20GeV/"
#rm -rf $DATAPLACE/out-qcd.root
#rm -rf $DATAPLACE/out-zjets.root
#rm -rf $DATAPLACE/out-singletop.root
#rm -rf $DATAPLACE/out-dibosons.root
#rm -rf $DATAPLACE/out-mc.root
#DATAPLACE="/lip-sw/cmssw/users/nalmeida/new/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/mt-2011-V1-data-MU-20GeV/"
#rm -rf $DATAPLACE/out-data.root 

