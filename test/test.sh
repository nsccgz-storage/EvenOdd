#!/bin/bash
j=`expr 1024 \* 1024 \* 4`
for((i=1; i<=j; i*=2))
do
file_size=`expr $i \* 1024`

./encode_test.sh $file_size 11
done

