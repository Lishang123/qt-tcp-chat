#include "Application.hpp"


Application::Application(QObject *parent) : QObject(parent) {
    m_ChatModel.setStringList(m_list);
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
    m_ChatModel.setStringList(m_list);
}

void Application::sendMessage(const QString &message) {
    m_client.sendMessage(message, m_publicRoomId);
}

void Application::updateRooms() {
    //This line keeps the program running forever!!
    //m_roomListModel.appendRow(m_roomListModel.invisibleRootItem());

    // print the public room
    foreach(auto roomInfo, m_roomInfos) {
        if(roomInfo.roomId == m_publicRoomId) {
            m_roomListModel.appendRow(new QStandardItem(roomInfo.roomName));
            break;
        }
    }
    // print the top room if not public
    if (m_publicRoomId != m_currentRoomId) {
        foreach(auto roomInfo, m_roomInfos) {
            if(roomInfo.roomId == m_currentRoomId) {
                m_roomListModel.appendRow(new QStandardItem(roomInfo.roomName));
                break;
            }
        }
    }

    foreach(auto roomInfo, m_roomInfos) {
        if(roomInfo.roomId == m_publicRoomId || roomInfo.roomId == m_currentRoomId) continue;
        m_roomListModel.appendRow(new QStandardItem(roomInfo.roomName));
    }
}

void Application::disconnectFromServer() {
    m_client.disconnectFromHost();
}

//
//
// void Application::addFile(QStandardItem *root, QFileInfo info)
// {
//     QStandardItem *item = new QStandardItem(info.fileName());
//     root->appendRow(item);
//
//     item->setChild(0,0, new QStandardItem("Size"));
//     item->setChild(0,1, new QStandardItem(QString::number(info.size())));
//
//     item->setChild(1,0, new QStandardItem("Created"));
//     item->setChild(1,1, new QStandardItem(info.birthTime().toString()));
//
//     item->setChild(2,0, new QStandardItem("Modified"));
//     item->setChild(2,1, new QStandardItem(info.lastModified().toString()));
//
//     item->setChild(3,0, new QStandardItem("Accessed"));
//     item->setChild(3,1, new QStandardItem(info.lastRead().toString()));
//
//     item->setChild(4,0, new QStandardItem("Readable"));
//     item->setChild(4,1, new QStandardItem(info.isReadable() ? "Yes" : "No"));
//
//     item->setChild(5,0, new QStandardItem("Writable"));
//     item->setChild(5,1, new QStandardItem(info.isWritable() ? "Yes" : "No"));
//
//     item->setChild(6,0, new QStandardItem("Path"));
//     item->setChild(6,1, new QStandardItem(info.path()));
// }
//



