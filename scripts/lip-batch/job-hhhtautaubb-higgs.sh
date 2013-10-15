#!/bin/bash

. $LIPCMS_BASE/scripts/lip-batch/run-batch.sh

physicsAnalysis $LIPCMS_BASE/test/physicsAnalysisParSets_cfg.py hhhtautaubb_higgs_${1}_${2}
