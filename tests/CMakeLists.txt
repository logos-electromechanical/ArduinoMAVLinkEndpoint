
cmake_minimum_required(VERSION 3.10)

project(mavlink_arduino_test)
enable_testing()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(WARNING "${mavlink_arduino_test_SOURCE_DIR}")
set(SOURCE_HOME_DIR ${mavlink_arduino_test_SOURCE_DIR}/../src)
message(WARNING "${SOURCE_HOME_DIR}")

find_package(GTest REQUIRED)

set (SRCS 
    ${mavlink_arduino_test_SOURCE_DIR}/String.cpp
    ${SOURCE_HOME_DIR}/MAVLinkEndpoint.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkDefaultDeactivateRX.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkHeartbeatRX.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkRXCallback.cpp
    ${SOURCE_HOME_DIR}/receivers/MAVLinkServoOutputRawRX.cpp
    ${SOURCE_HOME_DIR}/transmitters/MAVLinkHeartbeatTX.cpp
    ${SOURCE_HOME_DIR}/transmitters/MAVLinkTXCallback.cpp
    ${SOURCE_HOME_DIR}/params/MAVLinkParamListener.cpp)

add_executable(mavlink_arduino_test mavlink_test.cpp ${SRCS})

include_directories(AFTER /usr/include ${SOURCE_HOME_DIR} ${mavlink_arduino_test_SOURCE_DIR})

message(WARNING "${INCLUDE_DIRECTORIES}")

target_link_libraries(mavlink_arduino_test gtest_main)

gtest_add_tests(TARGET mavlink_arduino_test SOURCES ${SRCS})