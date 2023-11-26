#!/bin/bash 

PATH2LIB="./MASPass/MASPass.so"
PASS=MASPass

BENCH=${1}.c

rm -f default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll *.in *.in.Z

clang -emit-llvm -c ${BENCH} -Xclang -disable-O0-optnone -o ${1}.bc

# These are instrument profiler passes, probably don't need or want this
opt -passes='pgo-instr-gen,instrprof' ${1}.bc -o ${1}.prof.bc

clang -fprofile-instr-generate ${1}.prof.bc -o ${1}_prof

./${1}_prof > /dev/null

llvm-profdata merge -o ${1}.profdata default.profraw

opt -passes="pgo-instr-use" -o ${1}.profdata.bc -pgo-test-profile-file=${1}.profdata < ${1}.bc

opt --disable-output -load-pass-plugin="${PATH2LIB}" -passes="${PASS}" ${1}.profdata.bc

rm -f *.in *.in.Z default.profraw *_prof *_fplicm *.bc *.profdata *_output *.ll words