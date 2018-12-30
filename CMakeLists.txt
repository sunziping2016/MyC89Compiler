cmake_minimum_required(VERSION 3.13)
project(MyC89Compiler C CXX ASM)

set(CMAKE_CXX_STANDARD 11)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(c89c src/syntax/parser.y ${CMAKE_SOURCE_DIR}/src/syntax/parser.cpp)
FLEX_TARGET(c89c src/syntax/scanner.l ${CMAKE_SOURCE_DIR}/src/syntax/scanner.cpp)

add_executable(c89c
        ${BISON_c89c_OUTPUTS}
        ${FLEX_c89c_OUTPUTS}
        src/main.cpp
        src/syntax/scanner.h
        src/ast/driver.h
        src/ast/driver.cpp)

find_package(LLVM REQUIRED CONFIG)

llvm_map_components_to_libnames(llvm_libs core)
target_link_libraries(c89c ${llvm_libs})

foreach(Test test)
    add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test/${Test}-llvm.ll
            COMMAND clang -S -emit-llvm -o ${Test}-llvm.ll ${Test}.c
            DEPENDS ${CMAKE_SOURCE_DIR}/test/${Test}.c
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMENT "Generating ${Test}-llvm.ll"
    )
    add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test/${Test}-llvm.s
            COMMAND llc -relocation-model=pic ${Test}-llvm.ll
            DEPENDS ${CMAKE_SOURCE_DIR}/test/${Test}-llvm.ll
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMENT "Generating ${Test}-llvm.s"
    )
    add_executable(${Test}-llvm ${CMAKE_SOURCE_DIR}/test/${Test}-llvm.s)
    add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.c
            COMMAND ${CMAKE_C_COMPILER} ${Test}.c -E -o ${Test}-c89c.c ${CMAKE_C_FLAGS}
            DEPENDS ${CMAKE_SOURCE_DIR}/test/${Test}.c
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMENT "Generating ${Test}-c89c.c"
    )
    add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.ll
            COMMAND $<TARGET_FILE:c89c> -o ${Test}-c89c.ll ${Test}-c89c.c
            DEPENDS ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.c
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMENT "Generating ${Test}-c89c.ll"
    )
    add_custom_command(
            OUTPUT ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.s
            COMMAND llc -relocation-model=pic ${Test}-c89c.ll
            DEPENDS ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.ll
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMENT "Generating ${Test}-c89c.s"
    )
    add_executable(${Test}-c89c ${CMAKE_SOURCE_DIR}/test/${Test}-c89c.s)
endforeach(Test)
