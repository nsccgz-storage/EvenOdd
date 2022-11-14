#!/bin/bash

function buffer_test {
    file_size=$1
    prime=$2
    buffer_size=$3

    mkdir -p test_data
    if [ ! -e ./test_data/data_$file_size ]; then
        dd if=/dev/urandom of=./test_data/data_$file_size bs=$file_size count=1 iflag=fullblock
    fi
    echo "file size: $file_size $prime"
    ../build/time_check write ./test_data/data_$file_size $prime $buffer_size
    rm -rf disk_*
}

for buffer_size in 1 4 16 32 64 128 256 512 1024;
do
    for fs_ in 1 4 16 32 64 128 256 512 1024;
    do
        fs_size=`expr $fs_ \* 1024 \* 1024 \* 2`
        echo "buffer_size: $buffer_size MiB, fs_size: $fs_size Bytes , p: 11"
        buffer_test $fs_size 11 $buffer_size
    done
done


# 2>&1 | tee