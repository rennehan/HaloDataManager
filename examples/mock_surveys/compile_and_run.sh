#!/bin/bash
#####
# **
# * This file is part of HaloDataManager.
# * Copyright (c) 2024 Douglas Rennehan (douglas.rennehan@gmail.com)
# * 
# * This program is free software: you can redistribute it and/or modify it 
# * under the terms of the GNU General Public License as published by the 
# * Free Software Foundation, either version 3 of the License, or 
# * (at your option) any later version.
# * 
# * This program is distributed in the hope that it will be useful, 
# * but WITHOUT ANY WARRANTY; without even the implied warranty of 
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
# * See the GNU General Public License for more details.
# * 
# * You should have received a copy of the GNU General Public License 
# * along with HaloDataManager. If not, see <https://www.gnu.org/licenses/>.
# **
####

mkdir -p ./bin/

g++ -c ${2} -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 ${1}.cpp
g++ -o ./bin/${1} ${2} -std=c++17 -Wall -Wextra -lgsl -lgslcblas -lm -lhdf5_cpp -lhdf5 ${1}.o

rm ./${1}.o

./bin/${1} 160 10 1
