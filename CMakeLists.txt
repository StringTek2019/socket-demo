cmake_minimum_required(VERSION 3.15)
project(socket_demo)

set(CMAKE_CXX_STANDARD 11)

add_executable(server_epoll server_epoll.cpp)
add_executable(client_epoll client_epoll.cpp)
add_executable(server_epoll_with_fork server_epoll_with_fork.cpp)