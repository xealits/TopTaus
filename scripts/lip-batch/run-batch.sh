#!/bin/bash

# Set LIPCMS_BASE to your local area !
LIPCMS_BASE=/soft/lip-sw/cmssw/users/vischia/tau_dilepton/code/devel/CMSSW_5_3_7_patch4/src/LIP/TopTaus/

archbin=x86_64
SCRAM_ARCH=slc5_amd64_gcc462
# 2011: SCRAM_ARCH=slc5_amd64_gcc434
GLOBUS_LOCATION=/opt/globus/
EDG_WL_LOCATION=/opt/edg/
VO_CMS_SW_DIR=/cvmfs/cms.cern.ch/

# set yout LIPCMS path 

SCRIPTS=$LIPCMS_BASE/scripts/lip-batch/

source $VO_CMS_SW_DIR/cmsset_default.sh

cmsenv

cd $LIPCMS_BASE;

