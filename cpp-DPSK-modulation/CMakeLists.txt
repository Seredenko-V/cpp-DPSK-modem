cmake_minimum_required(VERSION 3.5)
project(cpp-DPSK-modulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(MATH_OPERATIONS math_operations.h math_operations.cpp)
set(GRAY_CODE gray_code.h gray_code.cpp)

set(SIGNAL_PARAMS signal_parameters.h signal_parameters.cpp)
set(DPSK_MODULATION dpsk_modulation.h dpsk_modulation.cpp)
set(DPSK_DEMODULATOR dpsk_demodulator.h dpsk_demodulator.cpp)

add_executable(cpp-DPSK-modulation main.cpp ${GRAY_CODE} ${SIGNAL_PARAMS} ${DPSK_MODULATION} ${DPSK_DEMODULATOR} ${MATH_OPERATIONS} tests.cpp)

