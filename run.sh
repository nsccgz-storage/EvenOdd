#!/bin/bash

rm -r bin/*
gcc -o bin/evenodd evenodd.c
./bin/evenodd write data/temp.txt 5