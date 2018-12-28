cmake_minimum_required(VERSION 3.13)
project(MyC89Compiler)

set(CMAKE_CXX_STANDARD 11)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(c89c src/syntax/parser.y ${CMAKE_SOURCE_DIR}/src/syntax/parser.cpp)
FLEX_TARGET(c89c src/syntax/scanner.l ${CMAKE_SOURCE_DIR}/src/syntax/scanner.cpp)

add_executable(c89c
        src/main.cpp
        ${BISON_c89c_OUTPUTS}
        ${FLEX_c89c_OUTPUTS}
        src/syntax/scanner.h)