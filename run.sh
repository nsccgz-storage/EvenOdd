#!/bin/bash
rm -rf test
mkdir test
cd test
mkdir result

INPUT='lsmtree.pdf'

rm -r ../bin/*
g++ -o ../bin/evenodd ../evenodd.cpp
../bin/evenodd write ../data/$INPUT 5

## test reading 
../bin/evenodd read $INPUT result/testread_0
diff ../data/$INPUT result/testread_0

## test one file failed
echo "============================"
mv disk_3/$INPUT disk_3/deleted_file
../bin/evenodd read $INPUT result/testread_11
diff ../data/$INPUT result/testread_11

echo "============================"
mv disk_3/deleted_file disk_3/$INPUT
mv disk_6/$INPUT disk_6/deleted_file
../bin/evenodd read $INPUT result/testread_12
mv disk_6/deleted_file disk_6/$INPUT
diff ../data/$INPUT result/testread_12

## test two files failed
### case 1:
echo "============================"
rm -rf disk_*
../bin/evenodd write ../data/$INPUT 5
rm disk_[5,6]/*
../bin/evenodd read $INPUT result/testread_21
diff ../data/$INPUT result/testread_21

### case 2:
echo "============================"
rm -rf disk_*
../bin/evenodd write ../data/$INPUT 5
rm disk_[3,6]/*

../bin/evenodd read $INPUT result/testread_22
diff ../data/$INPUT result/testread_22

### case 3:
echo "============================"
rm -rf disk_*
../bin/evenodd write ../data/$INPUT 5
rm disk_[4,5]/*

../bin/evenodd read $INPUT result/testread_23
diff ../data/$INPUT result/testread_23

### case 4:
echo "============================"
rm -rf disk_*
../bin/evenodd write ../data/$INPUT 5
rm -r disk_[0,2]

../bin/evenodd read $INPUT result/testread_24
diff ../data/$INPUT result/testread_24

## test more disks failed
echo "============================"
rm -rf disk_*
../bin/evenodd write ../data/$INPUT 5
rm disk_[0-2]/*
../bin/evenodd read $INPUT result/testread_3




