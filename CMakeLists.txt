cmake_minimum_required(VERSION 3.17)
project(_Lang)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror -fdeclspec)


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(src)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
message(STATUS "Include Directory ${LLVM_INCLUDE_DIRS}")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(_Lang ${SOURCES})
llvm_map_components_to_libnames(LLVM_LIBS core support executionengine passes interpreter orcjit x86asmparser x86disassembler x86codegen target)
target_link_libraries(_Lang ${LLVM_LIBS})