#!/bin/bash


if [ $# != 2 ]; then
    echo "usage: bash correct.sh <file_name> <prime>"
    exit 1
fi

file_name=`realpath $1`
index_1=0
index_2=0
prime=$2

#生成测试文件
cd ..
rm -rf build
# ./my_compile.sh
mkdir build
./compile.sh
cp evenodd build/
cp time_check build/
cd build

rm -rf disk_*
rm -rf test*

mkdir ./test_data

#编译时间测试程序



#测试evenodd正确性
#测试write时间
ls -la $file_name
./time_check write $file_name $prime
echo ====================================

#测试read时间

# ## test no filed failed
./time_check read $file_name ./test_data/data_read_20

diff $file_name ./test_data/data_read_20
rm -rf ./test_data/data_read_0
echo ====================================

## test two failed
### case1 行校验列和对角线校验列丢失
let "index_1=prime"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_21
diff $file_name ./test_data/data_read_21
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_21
echo ====================================
### case2 行校验列和数据列丢失
let "index_1=prime-1"
let "index_2=prime"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_22
diff $file_name ./test_data/data_read_22

mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_22
echo ====================================

### case3 对角线校验列和数据列丢失
let "index_1=prime-2"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_23
diff $file_name ./test_data/data_read_23

mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_23
echo ====================================

### case4 两列数据列丢失
let "index_1=prime-4"
let "index_2=prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_24
diff $file_name ./test_data/data_read_24

mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_26
echo ====================================

### case5 两列数据列丢失
let "index_1=prime-2"
let "index_2=prime-3"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read $file_name ./test_data/data_read_25
diff $file_name ./test_data/data_read_25

mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_25
echo ====================================

## test one failed
### case6 l列数据列丢失
let "index_1=prime-2"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_26
diff $file_name ./test_data/data_read_26

mv _disk_$index_1 disk_$index_1
rm -rf ./test_data/data_read_26
echo ====================================
### case7 行校验列丢失
let "index_1=prime"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_27
diff $file_name ./test_data/data_read_27

mv _disk_$index_1 disk_$index_1
rm -rf ./test_data/data_read_27
echo ====================================

### case8 对角线校验列丢失
let "index_1=prime+1"
mv disk_$index_1 _disk_$index_1
./time_check read $file_name ./test_data/data_read_28
diff $file_name ./test_data/data_read_28

mv _disk_$index_1 disk_$index_1
rm -rf ./test_data/data_read_28
echo ====================================

rm -rf disk_*