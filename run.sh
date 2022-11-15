#!/bin/bash

rm -rf build

./compile.sh

if [ $# != 2 ]; then
    echo "usage: bash correct.sh <file_size> <prime>"
    exit 1
fi

file_size=$1
index_1=0
index_2=0
prime=$2

#生成测试文件
cd build

mkdir ./test_data
dd if=/dev/urandom of=./test_data/data bs=$file_size count=1 iflag=fullblock

#测试encode正确性
./time_check write ./test_data/data $prime


## test no filed failed
echo ====================================
./time_check read ./test_data/data ./test_data/data_read_0

result=`diff ./test_data/data ./test_data/data_read_0`
if [ -n "$result" ]
then
    echo "test no file failed" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt
fi
rm -rf ./test_data/data_read_0
echo ====================================

## test two files failed
### case 1:

let "index_1=prime"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data ./test_data/data_read_21
result=`diff ./test_data/data ./test_data/data_read_21`
if [ -n "$result" ]
then
    echo "case 1" >> error_log.txt
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_21
echo ====================================
# ### case 2:
let "index_1=prime-2"
let "index_2=prime"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data ./test_data/data_read_22
result=`diff ./test_data/data ./test_data/data_read_22`
if [ -n "$result" ]
then
    echo "case 2" >> error_log.txt
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "==============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_22
echo ====================================
# ### case 3:
let "index_1=prime-1"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data ./test_data/data_read_23

result=`diff ./test_data/data ./test_data/data_read_23`
if [ -n "$result" ]
then
    echo "case 3" >> error_log.txt
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_23
echo ====================================

# ### case 4:
let "index_1=prime-2"
let "index_2=prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data ./test_data/data_read_24

result=`diff ./test_data/data ./test_data/data_read_24`
if [ -n "$result" ]
then
    echo "case 4" >> error_log.txt
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_24
echo ====================================

# ### case 5:
let "index_1=prime-2"
let "index_2=prime-3"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./time_check read ./test_data/data ./test_data/data_read_25

result=`diff ./test_data/data ./test_data/data_read_25`
if [ -n "$result" ]
then
    echo "case 5" >> error_log.txt
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt


fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read_25
echo ====================================

rm -rf disk_*
rm -rf test_data




# #测试encode正确性
# mkdir ../perf图
# sudo perf record -o out.data -a -g ./time_check write ./test_data/data $prime
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/write.svg
# rm -rf out.*


# ## test no filed failed
# echo ============================
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_0
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_0.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_0`
# if [ -n "$result" ]
# then
#     echo "test no file failed" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "===============================================" >> error_log.txt
# fi
# rm -rf ./test_data/data_read_0
# echo ============================

# ## test two files failed
# ### case 1:
# echo "============================"
# let "index_1=prime"
# let "index_2=prime+1"
# mv disk_$index_1 _disk_$index_1
# mv disk_$index_2 _disk_$index_2
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_21
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_21.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_21`
# if [ -n "$result" ]
# then
#     echo "case 1" >> error_log.txt
#     echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "===============================================" >> error_log.txt

# fi
# mv _disk_$index_1 disk_$index_1
# mv _disk_$index_2 disk_$index_2
# rm -rf ./test_data/data_read_21
# echo ====================================
# # ### case 2:
# let "index_1=prime-2"
# let "index_2=prime"
# mv disk_$index_1 _disk_$index_1
# mv disk_$index_2 _disk_$index_2
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_22
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_22.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_22`
# if [ -n "$result" ]
# then
#     echo "case 2" >> error_log.txt
#     echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "==============================================" >> error_log.txt

# fi
# mv _disk_$index_1 disk_$index_1
# mv _disk_$index_2 disk_$index_2
# rm -rf ./test_data/data_read_22
# echo ====================================
# # ### case 3:
# let "index_1=prime-1"
# let "index_2=prime+1"
# mv disk_$index_1 _disk_$index_1
# mv disk_$index_2 _disk_$index_2
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_23
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_23.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_23`
# if [ -n "$result" ]
# then
#     echo "case 3" >> error_log.txt
#     echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "===============================================" >> error_log.txt

# fi
# mv _disk_$index_1 disk_$index_1
# mv _disk_$index_2 disk_$index_2
# rm -rf ./test_data/data_read_23
# echo ====================================

# # ### case 4:
# let "index_1=prime-2"
# let "index_2=prime-1"
# mv disk_$index_1 _disk_$index_1
# mv disk_$index_2 _disk_$index_2
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_24
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_24.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_24`
# if [ -n "$result" ]
# then
#     echo "case 4" >> error_log.txt
#     echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "===============================================" >> error_log.txt

# fi

# mv _disk_$index_1 disk_$index_1
# mv _disk_$index_2 disk_$index_2
# rm -rf ./test_data/data_read_24
# echo ====================================

# # ### case 5:
# let "index_1=prime-2"
# let "index_2=prime-3"
# mv disk_$index_1 _disk_$index_1
# mv disk_$index_2 _disk_$index_2
# sudo perf record -o out.data -a -g ./time_check read ./test_data/data ./test_data/data_read_25
# sudo perf script -i out.data > out.perf
# ../FlameGraph/stackcollapse-perf.pl out.perf > out.floded
# ../FlameGraph/flamegraph.pl out.floded > ../perf图/read_25.svg
# rm -rf out.*
# result=`diff ./test_data/data ./test_data/data_read_25`
# if [ -n "$result" ]
# then
#     echo "case 5" >> error_log.txt
#     echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
#     echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
#     echo "$result" >> error_log.txt
#     echo "===============================================" >> error_log.txt


# fi
# mv _disk_$index_1 disk_$index_1
# mv _disk_$index_2 disk_$index_2
# rm -rf ./test_data/data_read_25
# echo ====================================

# rm -rf disk_*
# rm -rf test_data
