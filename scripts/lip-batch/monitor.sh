#!/bin/bash


# "grep vischia" selects only the lines corresponding to a job. It might as well be "grep job-" or stuff like that.
while true ;
 do
  echo "running: " `qstat -u vischia | grep vischia | grep " r " | wc -l` "    total: " `qstat -u vischia | grep vischia | wc -l` 
done