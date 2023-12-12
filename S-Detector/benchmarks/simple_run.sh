#!/bin/bash

# Usage: sh run.sh <test_name>
# Get the input file name from the command line argument
test_name=$1

PASS=masbased-pass

# Create IR file for LLVM to take as input
clang -emit-llvm -S ${test_name}.c -Xclang -disable-O0-optnone -o ${test_name}.ll

# Run the pass
opt -load-pass-plugin=./../MASBasedPass/build/MASBasedPass/MASBasedPass.so -passes="${PASS}" ${test_name}.ll -o ${test_name}.ll

# Generate binary executable
clang ${test_name}.ll -o ${test_name}_MASBasedPass

# # Run the executable
./${test_name}_MASBasedPass > ${test_name}_MASBasedPass_output

# Print out the output
echo "=== Program Output ==="
cat ${test_name}_MASBasedPass_output