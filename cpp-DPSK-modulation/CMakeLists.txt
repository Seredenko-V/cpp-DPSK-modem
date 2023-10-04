cmake_minimum_required(VERSION 3.5)

project(cpp-DPSK-modulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAY_CODE gray_code.h gray_code.cpp)

add_executable(cpp-DPSK-modulation main.cpp ${GRAY_CODE} tests.cpp)
