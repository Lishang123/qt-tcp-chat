#include "Application.hpp"


Application::Application(QObject *parent) : QObject(parent) {
    m_chatModel.setStringList(m_list);
}

void Application::sendLoginRequest(const LoginRequestPacket &loginRequestPacket) {
    qInfo() << Q_FUNC_INFO;
    m_client.sendLoginRequest(loginRequestPacket);
}

void Application::connectToServer(const QString &address, quint16 port) {
    m_client.connectToServer(address, port);
}

void Application::disconnectFromHost() {
    m_client.disconnectFromHost();
}

void Application::addChatMessage(const QString &message) {
    m_list.append(message);
    m_chatModel.setStringList(m_list);
}

void Application::sendMessage(const QString &message) {
    m_client.sendMessage(message, m_currentRoomId);
}

void Application::updateRooms() {
    //m_roomListModel.appendRow(m_roomListModel.invisibleRootItem());
    foreach(auto& roomInfo, m_roomInfos) {
        m_roomListModel.appendRow(new QStandardItem(roomInfo.roomName));
    }
}