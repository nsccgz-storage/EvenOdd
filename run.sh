#!/bin/bash

rm -r bin/*
gcc -o bin/evenodd evenodd.c
./bin/evenodd write data/temp 5

rm data/test*
./bin/evenodd read temp data/testread_0

# ## test one file failed
echo "============================"
mv disk/disk_2/temp data/temp_2
./bin/evenodd read temp data/testread_11

echo "============================"
mv data/temp_2 disk/disk_2/temp
mv disk/disk_6/temp data/temp_6
./bin/evenodd read temp data/testread_12

rm data/temp_6
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
