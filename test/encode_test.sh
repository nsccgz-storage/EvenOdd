#!/bin/bash

# 测试 encode 
file_size=$1
prime=$2


mkdir -p test_data
# if [ ! -e ./test_data/data_$file_size ]; then
#     dd if=/dev/urandom of=./test_data/data_$file_size bs=$file_size count=1 iflag=fullblock
# fi
if [ -e ./test_data/data_$file_size ]; then
    echo "file size: $file_size $prime"
    ../build/time_check write ./test_data/data_$file_size $prime
fi

rm -rf disk_*

