cmake_minimum_required(VERSION 3.15)
project(socket_demo)

set(CMAKE_CXX_STANDARD 11)

add_executable(server_select server_select.cpp)
add_executable(client_select client_select.cpp)