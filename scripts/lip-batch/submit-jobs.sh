#!/bin/bash

LIPCMS_BASE=/soft/lip-sw/cmssw/users/vischia/tau_dilepton/code/devel/CMSSW_5_3_7_patch4/src/LIP/TopTaus/
myFolder=${LIPCMS_BASE}/scripts/lip-batch/

cd $myFolder;

qsub ./job-data-RunA.sh
qsub ./job-data-RunAr.sh
qsub ./job-data-RunB_1.sh
qsub ./job-data-RunB_2.sh
qsub ./job-data-RunB_3.sh
qsub ./job-data-RunB_4.sh
qsub ./job-data-RunB_5.sh
qsub ./job-data-RunB_6.sh
qsub ./job-data-RunB_7.sh
qsub ./job-data-RunB_8.sh
qsub ./job-data-RunB_9.sh
qsub ./job-data-RunC1.sh
qsub ./job-data-RunC2_1.sh
qsub ./job-data-RunC2_2.sh
qsub ./job-data-RunC2_3.sh
qsub ./job-data-RunC2_4.sh
qsub ./job-data-RunC2_5.sh
qsub ./job-data-RunC2_6.sh
qsub ./job-data-RunC2_7.sh
qsub ./job-data-RunC2_8.sh
qsub ./job-data-RunC2_9.sh
qsub ./job-data-RunD_1.sh
qsub ./job-data-RunD_2.sh
qsub ./job-data-RunD_3.sh
qsub ./job-data-RunD_4.sh
qsub ./job-data-RunD_5.sh
qsub ./job-data-RunD_6.sh
qsub ./job-data-RunD_7.sh
qsub ./job-data-RunD_8.sh
qsub ./job-data-RunD_9.sh
qsub ./job-dibosons.sh
qsub ./job-hh-higgs-bychannel.sh
qsub ./job-hh-higgs.sh
qsub ./job-qcd.sh
qsub ./job-singletop.sh
#qsub ./job-trigger-mht.sh
#qsub ./job-trigger-mhtjets.sh
qsub ./job-ttbar-mutau.sh
qsub ./job-ttbar-mumu.sh
qsub ./job-ttbar-emu.sh
qsub ./job-ttbar-ddbkg.sh
qsub ./job-ttbar-mcbkg.sh
#qsub ./job-ttbar-unc.sh
qsub ./job-ttbar.sh
qsub ./job-wh-higgs-bychannel.sh
qsub ./job-wh-higgs.sh
qsub ./job-wjets.sh
qsub ./job-zjets.sh
qsub ./job-tbh-higgs.sh
qsub ./job-tbh-higgs-bychannel.sh
