#!/bin/bash

LIPCMS_BASE=/exper-sw/cmst3/cmssw/users/olek/CMSSW_5_3_9/src/LIP/TopTaus/
myFolder=${LIPCMS_BASE}/scripts/lip-batch/

cd $myFolder;

if [ "${1}" = "1" ]; then
    echo "Submitting 1800 jobs..." 
    for i in $(seq 0 49); do
        # 1400 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar.sh ${i}
#exit
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mutau.sh ${i}    
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mumu.sh ${i}    
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-emu.sh ${i}    
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-mcbkg.sh ${i}    
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ww.sh ${i} 
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wz.sh ${i} 
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-zz.sh ${i} 
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-ttbar-ddbkg.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunA.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunB.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC1.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunC2.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD1.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-RunD2.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 3 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 4 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 5 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 6 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 7 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 8 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 15 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 16 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 17 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 18 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 19 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 20 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-singletop.sh ${i}
    done
    
    for i in $(seq 0 399); do
    # 800 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wjets.sh ${i}
    done
    echo "1800 jobs submitted"
    
elif [ "${1}" = "2" ]; then
### #####qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-ttbar-unc.sh

    for i in $(seq 0 9); do
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-MissingRunB.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-MissingRunC1.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-MissingRunC2.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-MissingRunD1.sh ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-data-MissingRunD2.sh ${i}
    done

    for i in $(seq 0 99); do
    # 700 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 1 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 2 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 9 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 11 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 12 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 13 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-zjets_10to50.sh ${i}
    done
    
    for i in $(seq 0 29); do
    # 510 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 10 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 180 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 200 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 220 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 240 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 250 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 260 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 280 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 300 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 180 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 200 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 220 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 240 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 250 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 260 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 280 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 300 ${i}
    done
    
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-qcd.sh 14 0
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wh-higgs-bychannel.sh
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-wh-higgs.sh
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hh-higgs-bychannel.sh
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hh-higgs.sh
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-tbh-higgs.sh
    qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-tbh-higgs-bychannel.sh
    

### #qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-trigger-mht.sh
### #qsub -v LIPCMS_BASE${LIPCMS_BASE}/    ./job-trigger-mhtjets.sh

    for i in $(seq 0 199); do
    # 200 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-zjets_from50.sh ${i}
    done
    
    for i in $(seq 0 30); do
    # 300 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 350 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 400 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 500 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 600 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs.sh 700 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 350 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 400 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 500 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 600 ${i}
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-htb-higgs-bychannel.sh 700 ${i}
    done
    
    for i in $(seq 0 19); do 
    # 60 jobs
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hhhtautaubb-higgs.sh 260 0
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hhhtautaubb-higgs.sh 300 0
	qsub -v LIPCMS_BASE=${LIPCMS_BASE}    ./job-hhhtautaubb-higgs.sh 350 0
    done
    

fi

exit 0
