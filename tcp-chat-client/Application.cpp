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

void Application::updateRooms(const QList<RoomInfo>& room_infos) {
    //This line keeps the program running forever!!
    //m_roomListModel.appendRow(m_roomListModel.invisibleRootItem());

    // print the public room
    foreach(auto roomInfo, room_infos) {
        if(roomInfo.roomId == m_publicRoomId) {
            addUser(roomInfo.roomId, roomInfo.roomName);
            break;
        }
    }
    // // print the top room if not public
    // if (m_publicRoomId != m_currentRoomId) {
    //     foreach(auto roomInfo, room_infos) {
    //         if(roomInfo.roomId == m_currentRoomId) {
    //             addUser(roomInfo.roomId, roomInfo.roomName);
    //             break;
    //         }
    //     }
    // }
    //print the rest
    foreach(auto roomInfo, room_infos) {
        if(roomInfo.roomId == m_publicRoomId || roomInfo.roomId == m_currentRoomId) continue;
        addUser(roomInfo.roomId, roomInfo.roomName);
    }
}

void Application::addUser(const QUuid &roomId, const QString &userName) {
    QStandardItem* item = new QStandardItem(userName);
    item->setData(roomId, Qt::UserRole + 1);
    m_roomListModel.appendRow(item);
}


void Application::removeUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    for (int row = 0; row < m_roomListModel.rowCount(); ++row) {
        auto item = m_roomListModel.item(row);
        if (item->data(Qt::UserRole + 1) == logoutNotificationPacket.userId) {
            m_roomListModel.removeRow(row);
            break;
        }
    }
}

void Application::disconnectFromServer() {
    m_client.disconnectFromHost();
}


