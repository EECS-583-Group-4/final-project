#!/bin/bash

# Usage: sh run.sh <test_name>
# Get the input file name from the command line argument
test_name=$1

# Create IR file for LLVM to take as input
clang -emit-llvm -S ${test_name}.c -Xclang -disable-O0-optnone -o ${test_name}.ll

# Run the pass
opt -disable-output -load-pass-plugin=./build/NaivePass/NaivePass.so -passes="func-name" ${test_name}.ll