cmake_minimum_required(VERSION 3.27)
project(Desmos_Compiler)

set(CMAKE_CXX_STANDARD 20)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test.des ${CMAKE_CURRENT_BINARY_DIR}/test.des COPYONLY)

add_executable(Desmos_Compiler main.cpp
        frontend.h
        frontend.cpp
        lexer.cpp
        parser.cpp
        compiler.h
        backend.cpp
        semantic_analyzer.cpp
        ast.h
)
