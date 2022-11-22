#!/bin/bash

for ((i=0; i<12 ;i++))
do
    for((j=0; j<4; j++))
    do
        diff disk_$i/data_10G.$j debug_test/disk_$i/data_10G.$j
    done
done