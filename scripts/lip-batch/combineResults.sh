#!/bin/bash

# usage: sh combineResults.sh outputArea [hadd|clean] [AB|ABC|ABCD]

if [ "${2}" = "hadd" ]; then

    DATAPLACE="${1}/nomt-2012-V1-data-MU-20GeV/"
    
     #DATAPLACE="/lustre/data3/cmslocal/vischia/tau_dilepton/outputFiles444_3/mt-2011-V1-data-MU-20GeV/"

    cd $DATAPLACE

#    if [ "${3}" = "AB" ]; then
#    	hadd -f out-data.root out-Muon_RunA.root out-Muon_RunA_06AugRecover.root out-Muon_RunB_*.root
#    elif ["${3}" = "ABC" ]; then
#        hadd -f out-data.root out-Muon_RunA.root out-Muon_RunA_06AugRecover.root out-Muon_RunB_*.root out-Muon_RunC1.root out-Muon_RunC2_*.root
#    elif ["${3}" = "ABCD" ]; then
    hadd -f out-data.root out-Muon_RunA_*.root out-Muon_RunB_*.root out-Muon_RunC_1_*.root out-Muon_RunC_2_*.root out-Muon_RunD_1_*.root out-Muon_RunD_2_*.root &
#    fi



    DATAPLACE="${1}/nomt-2012-V1-mc-MU-20GeV/"
    
    cd $DATAPLACE

    
    # QCD
    hadd -f $DATAPLACE/out-qcd.root $DATAPLACE/out-qcd_*root $DATAPLACE/out-qcdmu15*.root $DATAPLACE/out-PhotonJets*root

    # zjets
    hadd -f $DATAPLACE/out-zjets.root $DATAPLACE/out-dy_from50_*root $DATAPLACE/out-dy_10to50_*.root

    # single top
    hadd -f $DATAPLACE/out-singletop.root $DATAPLACE/out-stop*.root
    
    # diboson
    hadd -f $DATAPLACE/out-dibosons.root $DATAPLACE/out-ww_*.root $DATAPLACE/out-wz_*.root $DATAPLACE/out-zz_*.root

    # wjets
    hadd -f $DATAPLACE/out-wjets.root $DATAPLACE/out-wjets_*.root

    hadd -f $DATAPLACE/out-ttbar.root $DATAPLACE/out-ttbar_*.root
    hadd -f $DATAPLACE/out-ttbar-ddbkg.root $DATAPLACE/out-ttbar-ddbkg_*.root
    hadd -f $DATAPLACE/out-ttbar-mcbkg.root $DATAPLACE/out-ttbar-mcbkg_*.root
    hadd -f $DATAPLACE/out-ttbar-mutau.root $DATAPLACE/out-ttbar-mutau_*.root
    hadd -f $DATAPLACE/out-ttbar-mumu.root $DATAPLACE/out-ttbar-mumu_*.root
    hadd -f $DATAPLACE/out-ttbar-emu.root $DATAPLACE/out-ttbar-emu_*.root

    
    # mc total
    hadd -f $DATAPLACE/out-mc.root $DATAPLACE/out-qcd.root $DATAPLACE/out-wjets.root $DATAPLACE/out-zjets.root $DATAPLACE/out-dibosons.root $DATAPLACE/out-ttbar.root $DATAPLACE/out-singletop.root &
    
    # data driven bkg, using mc
    hadd -f $DATAPLACE/out-ddbkg.root $DATAPLACE/out-qcd.root $DATAPLACE/out-wjets.root $DATAPLACE/out-ttbar-ddbkg.root

    ## no ##    # ttbar mc for shapes?
    ## no ##    hadd -f $DATAPLACE/out-ttbar_mcbkg_shapes.root $DATAPLACE/out-ttbar_mcbkg.root $DATAPLACE/out-ttbar_emu.root $DATAPLACE/out-ttbar_mumu.root

    # mc bkg (w.r.t. top xsec measurement: ttbar is signal for this)
    hadd -f $DATAPLACE/out-mcbkg.root $DATAPLACE/out-zjets.root $DATAPLACE/out-singletop.root $DATAPLACE/out-dibosons.root $DATAPLACE/out-ttbar-mcbkg.root
    ## no ## $DATAPLACE/out-ttbar_emu.root $DATAPLACE/out-ttbar_mumu.root

    # other cont
    hadd -f $DATAPLACE/out-ttbar-other.root  $DATAPLACE/out-ttbar-ddbkg.root $DATAPLACE/out-ttbar-mcbkg.root &

    # allmcmkg (w.r.t. charged higgs search: ttbar is includes as irreducible background)
    hadd -f $DATAPLACE/out-allmcbkg.root $DATAPLACE/out-ttbar.root $DATAPLACE/out-mcbkg.root &


    hadd -f $DATAPLACE/out-htb-pythia-m180.root $DATAPLACE/out-htb-pythia-m180_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m200.root $DATAPLACE/out-htb-pythia-m200_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m220.root $DATAPLACE/out-htb-pythia-m220_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m240.root $DATAPLACE/out-htb-pythia-m240_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m250.root $DATAPLACE/out-htb-pythia-m250_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m260.root $DATAPLACE/out-htb-pythia-m260_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m280.root $DATAPLACE/out-htb-pythia-m280_*.root 
    hadd -f $DATAPLACE/out-htb-pythia-m300.root $DATAPLACE/out-htb-pythia-m300_*.root 


    # higgs  mass points
    hadd -f $DATAPLACE/out-heavyHiggs-pythia-m180.root $DATAPLACE/out-tbh-pythia-m180.root $DATAPLACE/out-htb-pythia-m180.root &
    cp  $DATAPLACE/out-tbh-pythia-m190.root $DATAPLACE/out-heavyHiggs-pythia-m190.root                                         & 
    hadd -f $DATAPLACE/out-heavyHiggs-pythia-m200.root $DATAPLACE/out-tbh-pythia-m200.root $DATAPLACE/out-htb-pythia-m200.root &
    hadd -f $DATAPLACE/out-heavyHiggs-pythia-m220.root $DATAPLACE/out-tbh-pythia-m220.root $DATAPLACE/out-htb-pythia-m220.root &
    cp  $DATAPLACE/out-htb-pythia-m240.root $DATAPLACE/out-heavyHiggs-pythia-m240.root                                         &  
    hadd -f $DATAPLACE/out-heavyHiggs-pythia-m250.root $DATAPLACE/out-tbh-pythia-m250.root $DATAPLACE/out-htb-pythia-m250.root &
    cp  $DATAPLACE/out-htb-pythia-m260.root $DATAPLACE/out-heavyHiggs-pythia-m260.root                                         &
    cp  $DATAPLACE/out-htb-pythia-m280.root $DATAPLACE/out-heavyHiggs-pythia-m280.root                                         &
    hadd -f $DATAPLACE/out-heavyHiggs-pythia-m300.root $DATAPLACE/out-tbh-pythia-m300.root $DATAPLACE/out-htb-pythia-m300.root &
    

elif [ "${2}" = "clean" ]; then
  # Clean processed files
    DATAPLACE="${1}/nomt-2012-V1-mc-MU-20GeV/"
    rm $DATAPLACE/out-*.root
    rm $DATAPLACE/out-*.txt
    DATAPLACE="${1}/nomt-2012-V1-data-MU-20GeV/"
    rm $DATAPLACE/out-*.root
    rm $DATAPLACE/out-*.txt
    #rm -rf $DATAPLACE/out-qcd.root
    #rm -rf $DATAPLACE/out-zjets.root
    #rm -rf $DATAPLACE/out-singletop.root
    #rm -rf $DATAPLACE/out-dibosons.root
    #rm -rf $DATAPLACE/out-mc.root
    #DATAPLACE="/lip-sw/cmssw/users/nalmeida/new/CMSSW_5_2_5/src/lipcms/Physics/TopTauDileptons2012/mt-2011-V1-data-MU-20GeV/"
    #rm -rf $DATAPLACE/out-data.root 

fi