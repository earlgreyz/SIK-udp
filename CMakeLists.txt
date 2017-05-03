cmake_minimum_required(VERSION 3.6)
project(SIK_udp)

set(CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

find_package(Boost)

set(SOURCE_FILES error.h protocol.h parse.h)
set(TEST_FILES private/tests.cc private/test_protocol.cc private/test_parse.cc private/test_buffer.cc)

add_executable(client client.h client.cc ${SOURCE_FILES} memory.h)
add_executable(server buffer.h poll.h server.h server.cc ${SOURCE_FILES})
add_executable(tests ${SOURCE_FILES} ${TEST_FILES})