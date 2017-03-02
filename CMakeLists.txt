cmake_minimum_required(VERSION 2.8.9)
project(server)

set(CMAKE_CXX_COMPLIER clang++)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++14)

include_directories(zmq)

add_executable(server server.cpp zmq/zmq_functions.cpp)
add_executable(client client.cpp zmq/zmq_functions.cpp)

target_link_libraries(server zmq)
target_link_libraries(client zmq)
target_link_libraries(server pthread)
target_link_libraries(client pthread)
