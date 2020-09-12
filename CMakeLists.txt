cmake_minimum_required(VERSION 3.17)
project(_Lang)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Werror)

include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(_Lang ${SOURCES} )