cmake_minimum_required(VERSION 3.14)

project(qt-tcp-chat-tests LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Test)

add_executable(tst_tcp_framing
        tst_tcp_framing.cpp
)

target_link_libraries(tst_tcp_framing
        PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(tst_tcp_framing
        PRIVATE
        ${CMAKE_SOURCE_DIR}
)

add_test(NAME tcp_framing COMMAND tst_tcp_framing)
