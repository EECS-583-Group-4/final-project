#!/bin/bash
# e.g. sh run.sh testcase

# ACTION NEEDED: If the path is different, please update it here.
PATH2MASBASED="./../MASBasedPass/build/MASBasedPass/MASBasedPass.so"        # Specify your build directory in the project
PATH2NAIVE="./../NaivePass/build/NaivePass/NaivePass.so"
# ACTION NEEDED: Choose the correct pass when running.
PASS1=masbased-pass
PASS2=naive-pass                   

# Delete outputs from previous runs. Update this when you want to retain some files.
rm -f default.profraw *_prof *_optim *.bc *.profdata *_output *.ll *_masbased-pass *_naive-pass

# Convert source code to bitcode (IR).
clang -emit-llvm -c ${1}.c -Xclang -disable-O0-optnone -o ${1}.bc

opt -passes='function(mem2reg,loop-simplify)' ${1}.bc -o ${1}.ls.bc

# We now use the profile augmented bc file as input to your pass.
opt -load-pass-plugin="${PATH2MASBASED}" -passes="${PASS1}" ${1}.bc -o ${1}.${PASS1}.bc > /dev/null

# We now use the profile augmented bc file as input to your pass.
opt -load-pass-plugin="${PATH2NAIVE}" -passes="${PASS2}" ${1}.bc -o ${1}.${PASS2}.bc > /dev/null

# Generate binary excutable before optim: Unoptimzed code
clang ${1}.ls.bc -o ${1}_no_optim 
# Generate binary executable after optim: Optimized code
clang ${1}.${PASS1}.bc -o ${1}_${PASS1}
# Generate binary executable after optim: Optimized code
clang ${1}.${PASS2}.bc -o ${1}_${PASS2}

# Produce output from binary to check correctness
#./${1}_${PASS1} > ${PASS1}_output
# Produce output from binary to check correctness
#./${1}_${PASS2} > ${PASS2}_output

#echo -e "\n=== Program Correctness Validation ==="
#if [ "$(diff ${PASS1}_output ${PASS2}_output)" != "" ]; then
    #echo -e ">> Outputs do not match\n"
#else
    #echo -e ">> Outputs match\n"
    # Measure performance
    echo "=== Program Performance ==="
    echo -e "1. Performance of unoptim"
    time ./${1}_no_optim > /dev/null
    echo -e "\n\n"
    echo -e "2. Performance of MAS Based code"
    time ./${1}_${PASS1} > /dev/null
    echo -e "\n\n"
    echo -e "3. Performance of naive code"
    time ./${1}_${PASS2} > /dev/null
    echo -e "\n\n"
    echo "=== Program Size ==="
    echo -e "1. Size of unoptim"
    unoptim_size=$(stat -c %s ${1}_no_optim)
    echo -e "${unoptim_size} bytes"
    echo -e "\n\n"
    echo -e "2. Size of MAS Based code"
    masbased_size=$(stat -c %s ${1}_${PASS1})
    echo -e "${masbased_size} bytes"
    echo -e "\n\n"
    echo -e "3. Size of naive code"
    naive_size=$(stat -c %s ${1}_${PASS2})
    echo -e "${naive_size} bytes"
    echo -e "\n\n"

#fi

# Cleanup: Remove this if you want to retain the created files. And you do need to.
#rm -f default.profraw *_prof *_optim *.bc *.profdata *_output *.ll *_masbased-pass *_naive-pass