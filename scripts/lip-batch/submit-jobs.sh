#!/bin/bash

LIPCMS_BASE=/soft/lip-sw/cmssw/users/vischia/tau_dilepton/code/devel/CMSSW_5_3_7_patch4/src/LIP/TopTaus/
myFolder=${LIPCMS_BASE}/scripts/lip-batch/

cd $myFolder;

qsub ./job-data-RunA.sh
qsub ./job-data-RunAr.sh
qsub ./job-data-RunB.sh
qsub ./job-data-RunC1.sh
qsub ./job-data-RunC2.sh
qsub ./job-data-RunD.sh
qsub ./job-dibosons.sh
qsub ./job-hh-higgs-bychannel.sh
qsub ./job-hh-higgs.sh
qsub ./job-qcd.sh
qsub ./job-singletop.sh
#qsub ./job-trigger-mht.sh
#qsub ./job-trigger-mhtjets.sh
qsub ./job-ttbar-bychannel.sh
#qsub ./job-ttbar-unc.sh
qsub ./job-ttbar.sh
qsub ./job-wh-higgs-bychannel.sh
qsub ./job-wh-higgs.sh
qsub ./job-wjets.sh
qsub ./job-zjets.sh
qsub ./job-tbh-higgs.sh
qsub ./job-tbh-higgs-bychannel.sh
