cmake_minimum_required(VERSION 3.20)
project(qt6-advanced-tutorial)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(tcp-chat-client)
add_subdirectory(tcp-chat-server)
add_subdirectory(common)