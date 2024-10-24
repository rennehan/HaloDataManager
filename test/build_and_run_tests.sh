#!/bin/bash

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
            else
                echo "Compilation failed for $cpp_file"
            fi
        done

        cd ..
    fi
done

