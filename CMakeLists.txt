cmake_minimum_required(VERSION 3.6)
project(SIK_udp)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Boost)

set(SOURCE_FILES error.h protocol.h parse.h)
set(TEST_FILES private/tests.cc private/test_protocol.cc private/test_parse.cc private/test_buffer.cc)

add_executable(client client.cc ${SOURCE_FILES})
add_executable(server server.cc ${SOURCE_FILES})
add_executable(tests ${SOURCE_FILES} ${TEST_FILES})