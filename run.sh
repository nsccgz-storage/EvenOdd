#!/bin/bash

./compile.sh

rm -rf test
mkdir test
cd test
mkdir result

INPUT='lsmtree.pdf'

../evenodd write ../data/$INPUT 5

## test reading 
../evenodd read $INPUT result/testread_0
diff ../data/$INPUT result/testread_0

## test one file failed
echo "============================"
mv disk_3 _disk_3
../evenodd read $INPUT result/testread_11
diff ../data/$INPUT result/testread_11

echo "============================"
mv _disk_3 disk_3
mv disk_6 _disk_6
../evenodd read $INPUT result/testread_12
mv _disk_6 disk_6
diff ../data/$INPUT result/testread_12

## test two files failed
### case 1:
echo "============================"
rm -rf disk_*
../evenodd write ../data/$INPUT 5
rm disk_[5,6]/*
../evenodd read $INPUT result/testread_21
diff ../data/$INPUT result/testread_21

### case 2:
echo "============================"
rm -rf disk_*
../evenodd write ../data/$INPUT 5
rm disk_[3,6]/*

../evenodd read $INPUT result/testread_22
diff ../data/$INPUT result/testread_22

### case 3:
echo "============================"
rm -rf disk_*
../evenodd write ../data/$INPUT 5
rm disk_[4,5]/*

../evenodd read $INPUT result/testread_23
diff ../data/$INPUT result/testread_23

### case 4:
echo "============================"
rm -rf disk_*
../evenodd write ../data/$INPUT 5
rm -r disk_[0,2]

../evenodd read $INPUT result/testread_24
diff ../data/$INPUT result/testread_24

## test more disks failed
echo "============================"
rm -rf disk_*
../evenodd write ../data/$INPUT 5
rm disk_[0-2]/*
../evenodd read $INPUT result/testread_3




