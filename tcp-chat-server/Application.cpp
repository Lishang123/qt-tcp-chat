#include "Application.hpp"

Application::Application(QObject *parent) {
    init();
}

void Application::init() {

    //log in
    connect(&m_roomManager, &RoomManager::loginSuccess, &m_server, &Server::handleLoginSuccess);
    connect(&m_roomManager, &RoomManager::changeClientId, &m_server, &Server::changeClientId);

    connect(&m_roomManager, &RoomManager::loginFailed, &m_server, &Server::handleLoginFailed);
    connect(&m_server, &Server::clientDisconnected, &m_roomManager, &RoomManager::logoutUser);

    // receive packet
    connect(&m_server, &Server::messageReceived, &m_roomManager, &RoomManager::handleMessage);
    // qInfo() << "app connect ok:" << ok;

    // send chat message
    connect(&m_roomManager, &RoomManager::sendMessageToRoom, &m_server, &Server::sendMessageToRoom);
    connect(&m_roomManager, &RoomManager::roomCreated, &m_server, &Server::sendRoomInfo);

    // connect(&m_server, &Server::messageReceived,
    //     this, [] {
    //         qInfo() << "Application sees Server::messageReceived";
    //     });
};
