#!/bin/bash
current=`date "+%Y-%m-%d-%H-%M-%S"`
mkdir -p logs
LOG=logs/$current.log

pushd .
cd ..
./compile.sh
popd

./run.sh | tee $LOG