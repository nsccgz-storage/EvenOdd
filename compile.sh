#!/bin/bash
rm -rf build
mkdir -p build
cp ./Makefile ./build
cd build
make
cp evenodd ../
