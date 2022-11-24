#!/bin/bash

for ((i=0; i<15 ;i++))
do
    for((j=0; j<2; j++))
    do
        echo disk_$i/data_4G.$j debug_test/disk_$i/data_4G.$j
        diff disk_$i/data_4G.$j debug_test/disk_$i/data_4G.$j
        echo "===================================="
    done
done