#!/bin/bash
g++ -c decoding.cpp -o decoding.o 
g++ -c encoding.cpp -o encoding.o
g++ -o evenodd evenodd.cpp decoding.o encoding.o

rm -rf *.o