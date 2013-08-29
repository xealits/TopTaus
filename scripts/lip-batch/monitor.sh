#!/bin/bash


while true ;
 do
  echo "running: " `qstat -u vischia | grep " r " | wc -l` "    total: " `qstat -u vischia | wc -l` 
done