cmake_minimum_required(VERSION 3.20)
find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(MASBasedPass)

add_subdirectory(../../MAS ../../MAS/build)
# add_library(MASPass MODULE ../../MAS/MASPass/MASPass.cpp ../../MAS/MASPass/MAS.cpp)
# include_directories(../../MAS)
# add_subdirectory(../../MAS ../../MAS/build)

target_link_libraries(MASBasedPass MAS)