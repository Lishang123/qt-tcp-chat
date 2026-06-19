#include "Application.hpp"

Application::Application(QObject *parent) {
    init();
}

void Application::init() {

    //log in
    connect(&m_chatRoom, &ChatRoom::userAdded, &m_server, &Server::handleLoginSuccess);
    connect(&m_chatRoom, &ChatRoom::loginFailed, &m_server, &Server::handleLoginFailed);
    connect(&m_server, &Server::clientDisconnected, &m_chatRoom, &ChatRoom::removeUser);

    // receive packet
    connect(&m_server, &Server::messageReceived, &m_chatRoom, &ChatRoom::handleMessage);
    // qInfo() << "app connect ok:" << ok;

    // send chat message
    connect(&m_chatRoom, &ChatRoom::sendMessageToClient, &m_server, &Server::sendMessageToClient);
    connect(&m_chatRoom, &ChatRoom::broadcast, &m_server, &Server::broadcast);

    // connect(&m_server, &Server::messageReceived,
    //     this, [] {
    //         qInfo() << "Application sees Server::messageReceived";
    //     });
};
