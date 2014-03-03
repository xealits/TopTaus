#!/bin/bash


# "grep olek" selects only the lines corresponding to a job. It might as well be "grep job-" or stuff like that.
while true ;
 do
  echo "running: " `qstat -u olek | grep olek | grep " r " | wc -l` "    total: " `qstat -u olek | grep olek | wc -l` 
done
