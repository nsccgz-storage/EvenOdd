#!/bin/bash

rm -rf build
./compile.sh
mkdir build
mv time_check build/
mv evenodd build/
cd build

file_size=104857600
prime=7

mkdir ./test_data
dd if=/dev/urandom of=./test_data/data bs=$file_size count=1 iflag=fullblock

./time_check write ./test_data/data $prime

#############################################################################
let "index_1=prime-2"
mv disk_$index_1 _disk_$index_1
./evenodd repair 1 $index_1
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing one data column ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
mv _disk_$index_1 disk_$index_1

#############################################################################
let "index_1=prime-1"
mv disk_$index_1 _disk_$index_1
./evenodd repair 1 $index_1
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing one data column(p-1) ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
mv _disk_$index_1 disk_$index_1

#############################################################################
let "index_1=prime"
mv disk_$index_1 _disk_$index_1
./evenodd repair 1 $index_1
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing row parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
mv _disk_$index_1 disk_$index_1

#############################################################################
let "index_1=prime+1"
mv disk_$index_1 _disk_$index_1
./evenodd repair 1 $index_1
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing diagonal parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
mv _disk_$index_1 disk_$index_1

#############################################################################
let "index_1=prime-1"
let "index_2=prime"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd repair 2 $index_1 $index_2
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing data column and row parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
result=`diff ./disk_$index_2/data.0 ./_disk_$index_2/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing data column and row parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
rm -rf disk_$index_2
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2

#############################################################################
let "index_1=prime-1"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd repair 2 $index_1 $index_2
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing data column and diagonal parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
result=`diff ./disk_$index_2/data.0 ./_disk_$index_2/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing data column and diagonal parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
rm -rf disk_$index_2
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2

# #############################################################################
let "index_1=prime"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd repair 2 $index_1 $index_2
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing row and diagonal parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
result=`diff ./disk_$index_2/data.0 ./_disk_$index_2/data.0`
if [ -n "$result" ]
then
    echo "======== test repairing row and diagonal parity ========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "=================================================" >> error_log.txt
fi
rm -rf disk_$index_1
rm -rf disk_$index_2
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2


# #############################################################################
let "index_1=prime-2"
let "index_2=prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd repair 2 $index_1 $index_2
result=`diff ./disk_$index_1/data.0 ./_disk_$index_1/data.0` #TODO: 测试不充分
if [ -n "$result" ]
then
    echo "========= test repairing two data columns =========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===================================================" >> error_log.txt
fi
result=`diff ./disk_$index_2/data.0 ./_disk_$index_2/data.0`
if [ -n "$result" ]
then
    echo "========= test repairing two data columns =========" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===================================================" >> error_log.txt
fi
rm -rf disk_$index_1
rm -rf disk_$index_2
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
