#!/bin/bash

if [ "${2}" = "1" ]; then
    qsub batchHadd.sh ${1} data
    qsub batchHadd.sh ${1} mc1_1
    qsub batchHadd.sh ${1} mc1_2
    qsub batchHadd.sh ${1} mc1_3
    qsub batchHadd.sh ${1} mc1_4
    qsub batchHadd.sh ${1} mc1_5
    qsub batchHadd.sh ${1} mc1_6
    qsub batchHadd.sh ${1} mc1_7
    qsub batchHadd.sh ${1} mc1_8
    qsub batchHadd.sh ${1} mc1_9
    qsub batchHadd.sh ${1} mc1_10
    qsub batchHadd.sh ${1} mc1_11
    qsub batchHadd.sh ${1} mc1_12
    qsub batchHadd.sh ${1} mc1_13
    qsub batchHadd.sh ${1} mc1_14
elif [ "${2}" = "1" ]; then
    qsub batchHadd.sh ${1} mc2_1
    qsub batchHadd.sh ${1} mc2_2
    qsub batchHadd.sh ${1} mc2_3
    qsub batchHadd.sh ${1} mc2_4
    qsub batchHadd.sh ${1} mc2_5
    qsub batchHadd.sh ${1} mc2_6
elif [ "${2}" = "1" ]; then
    qsub batchHadd.sh ${1} mc3_1
fi