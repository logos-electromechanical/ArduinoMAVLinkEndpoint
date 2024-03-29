
cmake_minimum_required(VERSION 3.12)

project(mavlink_arduino_test LANGUAGES CXX)
enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Time for flags
# This flag is required to prevent compiler whining about mavlink
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-address-of-packed-member -fsanitize=address")

set(SOURCE_HOME_DIR ${mavlink_arduino_test_SOURCE_DIR}/../src)

# add_compile_definitions(LONG_TEST=1)

find_package(GTest REQUIRED)
include(GoogleTest)

set (SRCS 
    ${mavlink_arduino_test_SOURCE_DIR}/WString.cpp
    ${mavlink_arduino_test_SOURCE_DIR}/Mocks.cpp
    ${SOURCE_HOME_DIR}/MAVLinkEndpoint.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkDefaultDeactivateRX.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkHeartbeatRX.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkRXCallback.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkServoOutputRawRX.cpp
    ${SOURCE_HOME_DIR}/transmitters/MAVLinkHeartbeatTX.cpp
    ${SOURCE_HOME_DIR}/transmitters/MAVLinkTXCallback.cpp
    ${SOURCE_HOME_DIR}/params/MAVLinkParamListener.cpp)

add_executable(mavlink_arduino_test 
    mavlink_constructor_tests.cpp 
    mavlink_process_tests.cpp
    ${SRCS})

include_directories(AFTER ${SOURCE_HOME_DIR} ${mavlink_arduino_test_SOURCE_DIR})

target_link_libraries(mavlink_arduino_test pthread gtest gmock gtest_main)

gtest_add_tests(TARGET mavlink_arduino_test SOURCES ${SRCS})

add_custom_target(mavlink_arduino_tests_exec ALL 
                COMMAND mavlink_arduino_test)