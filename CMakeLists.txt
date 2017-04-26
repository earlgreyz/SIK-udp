cmake_minimum_required(VERSION 3.6)
project(SIK_udp)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES )
add_executable(client client.cc ${SOURCE_FILES})
add_executable(server server.cc ${SOURCE_FILES})