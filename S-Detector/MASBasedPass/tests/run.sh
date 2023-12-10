#!/bin/bash
# e.g. sh run.sh testcase

# ACTION NEEDED: If the path is different, please update it here.
PATH2LIB="./../build/MASBasedPass/MASBasedPass.so"        # Specify your build directory in the project

# ACTION NEEDED: Choose the correct pass when running.
PASS=masbased-pass                

# Delete outputs from previous runs. Update this when you want to retain some files.
rm -f default.profraw *_prof *_optim *.bc *.profdata *_output *.ll

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

# We now use the profile augmented bc file as input to your pass.
opt -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.bc -o ${1}.optim.bc > /dev/null

# Generate binary excutable before optim: Unoptimzed code
clang ${1}.bc -o ${1}_no_optim 
# Generate binary executable after optim: Optimized code
clang ${1}.optim.bc -o ${1}_optim

# Produce output from binary to check correctness
./${1}_no_optim > correct_output
# Produce output from binary to check correctness
./${1}_optim > optim_output

echo -e "\n=== Program Correctness Validation ==="
if [ "$(diff correct_output optim_output)" != "" ]; then
    echo -e ">> Outputs do not match\n"
else
    echo -e ">> Outputs match\n"
    # Measure performance
    echo -e "1. Performance of unoptimized code"
    time ./${1}_no_optim > /dev/null
    echo -e "\n\n"
    echo -e "2. Performance of optimized code"
    time ./${1}_optim > /dev/null
    echo -e "\n\n"
fi

# Cleanup: Remove this if you want to retain the created files. And you do need to.
#rm -f default.profraw *_prof *_optim *.bc *.profdata *_output *.ll