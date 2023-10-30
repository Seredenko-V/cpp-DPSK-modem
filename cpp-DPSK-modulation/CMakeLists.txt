cmake_minimum_required(VERSION 3.5)
project(cpp-DPSK-modulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(MATH_OPERATIONS math_operations.h math_operations.cpp)
set(GRAY_CODE gray_code.h gray_code.cpp)
set(DPSK_MODULATION dpsk_modulation.h dpsk_modulation.cpp)

add_executable(cpp-DPSK-modulation main.cpp ${GRAY_CODE} ${DPSK_MODULATION} ${MATH_OPERATIONS} tests.cpp)

