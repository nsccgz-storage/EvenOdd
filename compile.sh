#!/bin/bash

g++  -I. -g -o log.cpp.o -c log.cpp

g++  -I. -g -o decoding.cpp.o -c decoding.cpp

g++  -I. -g -o encoding.cpp.o -c encoding.cpp

g++  -I. -g -o repair.cpp.o -c repair.cpp

g++  -I. -g -o evenodd.cpp.o -c evenodd.cpp

g++  -I. -g -o time_check.cpp.o -c time_check.cpp

g++ -o evenodd evenodd.cpp.o decoding.cpp.o encoding.cpp.o log.cpp.o repair.cpp.o

g++ -o time_check time_check.cpp.o decoding.cpp.o encoding.cpp.o log.cpp.o repair.cpp.o

rm *.o
