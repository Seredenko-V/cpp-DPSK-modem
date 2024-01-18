cmake_minimum_required(VERSION 3.5)
project(DPSK-modem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

set(MATH_OPERATIONS math_operations.h math_operations.cpp)
set(GRAY_CODE gray_code.h gray_code.cpp)

set(SIGNAL_PARAMS signal_parameters.h signal_parameters.cpp)
set(DPSK_MODULATION dpsk_modulator.h dpsk_modulator.cpp)
set(DPSK_DEMODULATOR dpsk_demodulator.h dpsk_demodulator.cpp)
set(PHASE_SYNCHRONIZATOR phase_synchronizator.h phase_synchronizator.cpp)

add_library(DPSKModem STATIC ${GRAY_CODE} ${SIGNAL_PARAMS} ${MATH_OPERATIONS}
    ${DPSK_MODULATION} ${DPSK_DEMODULATOR} ${PHASE_SYNCHRONIZATOR} matrix.h tests.cpp)
