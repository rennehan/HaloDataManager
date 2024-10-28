#!/bin/bash

mkdir -p ./bin/

g++ -c -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 ${1}.cpp
g++ -o ./bin/${1} -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 ${1}.o

rm ./${1}.o

./bin/${1} 160 10 1
