#!/bin/bash

RUN=./run.sh

for num in 16 32 64 128 256 512 1024 2048;
do
    file_size=`expr $num \* 1024 \*  1024`
    $RUN $file_size 11 > decoding_$num.log
done