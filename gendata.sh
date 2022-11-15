#!/bin/bash
# usage: ./gendata.sh <filebytes> <filename>

if [ $# != 2 ]; then
    echo "usage: bash gendata.sh <filebytes> <filename>"
    exit 1
fi

filebytes=$1
filename=$2

dd if=/dev/urandom of=$filename bs=$filebytes count=1 iflag=fullblock


