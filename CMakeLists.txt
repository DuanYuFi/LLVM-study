cmake_minimum_required(VERSION 3.27.1)
project(MyPass)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LLVM_VERSION
    "13"
    CACHE STRING "LLVM Version")

set(CMAKE_CXX_STANDARD 17)

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/llvm-${LLVM_VERSION}/lib/cmake/llvm/")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

include_directories(${LLVM_INCLUDE_DIRS})
# link_directories("/usr/lib/llvm-${LLVM_VERSION}/lib")

add_library(MyPass MODULE MyAnalysisPass.cpp)