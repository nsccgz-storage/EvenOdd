#!/bin/bash
#!/bin/bash

#!/bin/bash  
if [ $# != 2 ]; then
    echo "usage: bash correct.sh <file_size> <prime>"
    exit 1
fi

file_size=$1
index_1=0
index_2=0
prime=$2

#生成测试文件
mkdir ./test_data
dd if=/dev/urandom of=./test_data/data bs=$file_size count=1 iflag=fullblock

## test two files failed
### case 1:
echo "============================"
./evenodd write ./test_data/data $prime

let "index_1=prime"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read ./test_data/data ./test_data/data_read
result=`diff ./test_data/data ./test_data/data_read`
if [ -n "$result" ]
then
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read
echo ====================================
# ### case 2:
let "index_1=prime-2"
let "index_2=prime"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read ./test_data/data ./test_data/data_read
result=`diff ./test_data/data ./test_data/data_read`
if [ -n "$result" ]
then
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "==============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read
echo ====================================
# ### case 3:
let "index_1=prime-1"
let "index_2=prime+1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read ./test_data/data ./test_data/data_read
result=`diff ./test_data/data ./test_data/data_read`
if [ -n "$result" ]
then
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read
echo ====================================

# ### case 4:
let "index_1=prime-2"
let "index_2=prime-1"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read ./test_data/data ./test_data/data_read
result=`diff ./test_data/data ./test_data/data_read`
if [ -n "$result" ]
then
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read
echo ====================================

# ### case 5:
let "index_1=prime-2"
let "index_2=prime-3"
mv disk_$index_1 _disk_$index_1
mv disk_$index_2 _disk_$index_2
./evenodd read ./test_data/data ./test_data/data_read
result=`diff ./test_data/data ./test_data/data_read`
if [ -n "$result" ]
then
    echo "删除掉disk_${index_1}和disk_${index_2} read失败" >> error_log.txt
    echo "此时素数取值为:$prime  文件大小为: ${file_size}B" >> error_log.txt
    echo "$result" >> error_log.txt
    echo "===============================================" >> error_log.txt

fi
mv _disk_$index_1 disk_$index_1
mv _disk_$index_2 disk_$index_2
rm -rf ./test_data/data_read
echo ====================================

rm -rf disk*
rm -rf test*

# for ((i=1;i<=test_number;i++))
# do

#     #生成测试文件
#     let "file_size = skip * 1024"
#     mkdir ./test_data_"$skip"_KB
#     dd if=/dev/urandom of=./test_data_"$skip"_KB/data bs=$file_size count=1 iflag=fullblock
    
#     ## test two files failed
#     ### case 1:
#     echo "============================"
#     ./evenodd write ./test_data_"$skip"_KB/data $prime

#     let "index_1=prime"
#     let "index_2=prime+1"
#     mv disk_$index_1 _disk_$index_1
#     mv disk_$index_2 _disk_$index_2
#     ./evenodd read ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read
#     result=`diff ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read`
#     if [ -n "$result" ]
#     then
#         echo "素数取值为:$prime  文件大小为: ${skip}KB"
#         echo "修复失败"
#         #exit 1
#     fi
#     mv _disk_$index_1 disk_$index_1
#     mv _disk_$index_2 disk_$index_2
#     #rm -rf ./test_data_"$skip"_KB/data_read
#     echo ====================================
    
#     # # ### case 2:
#     # # echo "============================"
#     # mv disk_3 _disk_3
#     # mv disk_54 _disk_54
#     # ./evenodd read ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read
#     # result=`diff ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read`
#     # if [ -n "$result" ]
#     # then
#     #     echo "修复失败"
#     #     exit 1
#     # fi
#     # mv _disk_3 disk_3
#     # mv _disk_54 disk_54
#     # rm -rf ./test_data_"$skip"_KB/data_read

#     # # ### case 3:
#     # # echo "============================"
#     # mv disk_4 _disk_4
#     # mv disk_53 _disk_53
#     # ./evenodd read ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read
#     # result=`diff ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read`
#     # if [ -n "$result" ]
#     # then
#     #     echo "修复失败"
#     #     exit 1
#     # fi
#     # mv _disk_4 disk_4
#     # mv _disk_53 disk_53
#     # rm -rf ./test_data_"$skip"_KB/data_read


#     # # ### case 4:
#     # # echo "============================"

#     # mv disk_0 _disk_0
#     # mv disk_6 _disk_6
#     # ./evenodd read ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read
#     # result=`diff ./test_data_"$skip"_KB/data ./test_data_"$skip"_KB/data_read`
#     # if [ -n "$result" ]
#     # then
#     #     echo "修复失败"
#     #     exit 1
#     # fi
#     # mv _disk_0 disk_0
#     # mv _disk_6 disk_6
#     # rm -rf ./test_data_"$skip"_KB/data_read

#     # #     ./evenodd write ./test_data_"$skip"_KB/data 5
#     # #     echo test_file_name:./test_data_"$skip"_KB/data
#     # #     echo test_file_size:$file_size
#     # #     echo test_prime:7
#     let "skip = skip * 8"
#     #rm -rf test_data*
#     #rm -rf disk*
# done


