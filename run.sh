#!/bin/bash

rm -r bin/*
gcc -o bin/evenodd evenodd.c
./bin/evenodd write data/temp 5

rm data/test*
./bin/evenodd read temp data/testread_0

# ## test one file failed
# mv disk/disk_2/temp_2 data/
# ./bin/evenodd read temp data/testread_11

# mv data/temp_2 disk/disk_2/
# mv disk/disk_6/temp_6 data/
# ./bin/evenodd read temp data/testread_12

## test two files failed
### case 1:
echo "============================"
./bin/evenodd write data/temp 5
rm disk/disk_[5,6]/*

./bin/evenodd read temp data/testread_21

### case 2:
echo "============================"
./bin/evenodd write data/temp 5
rm disk/disk_[3,6]/*

./bin/evenodd read temp data/testread_22

### case 3:
echo "============================"
./bin/evenodd write data/temp 5
rm disk/disk_[4,5]/*

./bin/evenodd read temp data/testread_23

### case 4:
echo "============================"
./bin/evenodd write data/temp 5
rm disk/disk_[0,2]/*

./bin/evenodd read temp data/testread_24

## test more disks failed
echo "============================"
./bin/evenodd write data/temp 5
rm disk/disk_[0-2]/*
./bin/evenodd read temp data/testread_3
