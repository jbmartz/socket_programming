cmake_minimum_required(VERSION 3.14)
project(tcp_p2p_project)

set(CMAKE_CXX_STANDARD 14)

add_executable(tcp_p2p_project tcp_client.cpp tcp_server.cpp P2P.h main.cpp main.h tcp_server.h)