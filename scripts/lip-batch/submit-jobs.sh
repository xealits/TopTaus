#!/bin/bash

LIPCMS_BASE=/exper-sw/cmst3/cmssw/users/vischia/chiggs/CMSSW_5_3_7_patch4/src/LIP/TopTaus/
myFolder=${LIPCMS_BASE}/scripts/lip-batch/

cd $myFolder;

qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-dibosons.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-ddbkg.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunA.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunAr.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_1.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_2.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_3.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_4.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_5.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_6.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_7.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_8.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_9.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_10.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_11.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_12.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB_13.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC1.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_1.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_2.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_3.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_4.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_5.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_6.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_7.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_8.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_9.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_10.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_11.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_12.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2_13.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_1.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_2.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_3.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_4.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_5.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_6.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_7.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_8.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_9.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_10.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_11.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_12.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD_13.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wjets.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mutau.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mumu.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-emu.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mcbkg.sh
#qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-ttbar-unc.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hh-higgs-bychannel.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hh-higgs.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-singletop.sh
#qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-trigger-mht.sh
#qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-trigger-mhtjets.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wh-higgs-bychannel.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wh-higgs.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-zjets_from50.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-zjets_10to50.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-tbh-higgs.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-tbh-higgs-bychannel.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_180.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_200.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_220.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_240.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_250.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_260.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_280.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs_300.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_180.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_200.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_220.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_240.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_250.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_260.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_280.sh
qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel_300.sh


