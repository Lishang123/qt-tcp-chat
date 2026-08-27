cmake_minimum_required(VERSION 3.14)

project(qt-tcp-chat-tests LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Network PrintSupport Sql Test Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Network PrintSupport Sql Test Widgets)

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

add_executable(tst_client_packets
        tst_client_packets.cpp
        ${CMAKE_SOURCE_DIR}/tcp-chat-client/Client.cpp
)

target_link_libraries(tst_client_packets
        PRIVATE
        common
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(tst_client_packets
        PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/tcp-chat-client
)

add_test(NAME client_packets COMMAND tst_client_packets)

add_executable(tst_chat_model
    tst_chat_model.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatModel.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatRoom.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatHistoryManager.cpp
)

target_link_libraries(tst_chat_model
    PRIVATE
        common
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(tst_chat_model
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/tcp-chat-client
)

add_test(NAME chat_model COMMAND tst_chat_model)

add_executable(tst_group_creation
    tst_group_creation.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/Application.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/Client.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatModel.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatRoom.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/ChatHistoryManager.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-client/CreateGroupDialog.cpp
)

target_link_libraries(tst_group_creation
    PRIVATE
        common
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Widgets
)

target_include_directories(tst_group_creation
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/tcp-chat-client
)

add_test(NAME group_creation COMMAND tst_group_creation)

add_executable(tst_server_room_requests
    tst_server_room_requests.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-server/RoomManager.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-server/ChatRoom.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-server/DatabaseManager.cpp
    ${CMAKE_SOURCE_DIR}/tcp-chat-server/Resources.qrc
)

target_link_libraries(tst_server_room_requests
    PRIVATE
        common
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Sql
        Qt${QT_VERSION_MAJOR}::Test
)

target_include_directories(tst_server_room_requests
    PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/tcp-chat-server
)

add_test(NAME server_room_requests COMMAND tst_server_room_requests)
