cmake_minimum_required(VERSION 3.10)
project(ToyCCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(your_compiler
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
)

target_include_directories(your_compiler PRIVATE src)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


