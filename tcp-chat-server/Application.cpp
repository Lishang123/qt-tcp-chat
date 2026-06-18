#include "Application.hpp"

Application::Application(QObject *parent) {
    init();
}

void Application::init() {

    connect(&m_server, &Server::clientConnected, &m_chatRoom, &ChatRoom::addUser);
    connect(&m_server, &Server::clientDisconnected, &m_chatRoom, &ChatRoom::removeUser);

    bool ok = connect(&m_server, &Server::messageReceived, &m_chatRoom, &ChatRoom::handleMessage);
    // qInfo() << "app connect ok:" << ok;

    connect(&m_chatRoom, &ChatRoom::sendMessageToClient, &m_server, &Server::sendMessageToClient);

    connect(&m_chatRoom, &ChatRoom::broadcast, &m_server, &Server::broadcast);

    // connect(&m_server, &Server::messageReceived,
    //     this, [] {
    //         qInfo() << "Application sees Server::messageReceived";
    //     });
};
