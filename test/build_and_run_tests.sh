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

mkdir -p ./bin
for dir in ./test_*/ ; do
    if [[ -d "$dir" ]]; then
        echo "Entering directory: $dir"
        cd "$dir" || exit

        for cpp_file in *.cpp; do
            # Extract the filename without the extension
            exe_file="${cpp_file%.cpp}"

            # Compile the cpp file
            g++ -Wall -Wextra -std=c++17 -O3 -o ../bin/$exe_file $cpp_file
            if [[ $? -eq 0 ]]; then
                echo "Succeeded compiling $cpp_file"
                ../bin/$exe_file
                if [ $? -ne 0 ]; then
                    echo "The test failed!"
                    break 2
                fi
            else
                echo "Compilation failed for $cpp_file"
            fi
        done

        cd ..
    fi
done

