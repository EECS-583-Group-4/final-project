#!/bin/bash

# Usage: sh run.sh <test_name>
# Get the input file name from the command line argument
test_name=$1

PASS=naive-pass

# Create IR file for LLVM to take as input
clang -emit-llvm -S ${test_name}.c -Xclang -disable-O0-optnone -o ${test_name}.ll

# Run the pass
opt -load-pass-plugin=./../build/NaivePass/NaivePass.so -passes="${PASS}" ${test_name}.ll -o ${test_name}.ll

# Generate binary executable
clang ${test_name}.ll -o ${test_name}_NaivePass

# Get the size of the executable
executable_size=$(stat -c %s ${test_name}_NaivePass)

# Run the executable
# start=$(date +%s%3N)
# ./${test_name}_NaivePass > ${test_name}_NaivePass_output
# end=$(date +%s%3N)
# execution_time=$((end - start))

execution_time=$({ time -p ./${test_name}_NaivePass > ${test_name}_NaivePass_output; } 2>&1 | grep real | awk '{print $2}')


# Print out the output
echo "=== Program Output ==="
cat ${test_name}_NaivePass_output

# Print out executable size
echo "@@ Size of executable: ${executable_size} bytes"

# Print out execution time
echo "@@@ Execution time: ${execution_time} milliseconds"