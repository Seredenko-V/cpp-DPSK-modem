cmake_minimum_required(VERSION 3.5)
project(user-DPSK-modem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if (CMAKE_SYSTEM_NAME MATCHES "^MINGW")
    set(SYSTEM_LIBS -lstdc++)
else()
    set(SYSTEM_LIBS)
endif()

add_subdirectory(../DPSK-modem build-DPSK-modem)

add_executable(UserDPSKModem main.cpp)

target_include_directories(UserDPSKModem PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../DPSK-modem")
target_link_libraries(UserDPSKModem DPSKModem ${SYSTEM_LIBS})

