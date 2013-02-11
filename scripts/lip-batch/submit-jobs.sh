#!/bin/bash

myFolder=${LIPCMS_BASE}/scripts/lip-batch/

cd $myFolder;

qsub ./job-data-RunA.sh
qsub ./job-data-RunB.sh
qsub ./job-dibosons.sh
qsub ./job-wjets.sh
qsub ./job-zjets.sh 
qsub ./job-qcd.sh
qsub ./job-singletop.sh
qsub ./job-ttbar.sh
qsub ./job-ttbar-bychannel.sh
qsub ./job-wh-higgs.sh
qsub ./job-wh-higgs-bychannel.sh
qsub ./job-hh-higgs.sh
qsub ./job-hh-higgs-bychannel.sh
qsub ./job-tbh-higgs.sh
qsub ./job-tbh-higgs-bychannel.sh

qsub ./job-data-muonA.sh
qsub ./job-data-muonAr.sh
qsub ./job-data-muonB.sh
qsub ./job-data-muonC1.sh
qsub ./job-data-muonC2.sh
qsub ./job-data-muonD.sh
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
