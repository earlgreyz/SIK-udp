cmake_minimum_required(VERSION 3.6)
project(SIK_udp)

set(CMAKE_CXX_STANDARD 14)

find_package(Boost)

set(SOURCE_FILES error.h protocol.h parse.h)
set(TEST_FILES private/tests.cc private/test_parse.cc )

if (Boost_FOUND)
endif()

add_executable(client client.cc ${SOURCE_FILES})
add_executable(server server.cc ${SOURCE_FILES})
add_executable(tests ${TEST_FILES})