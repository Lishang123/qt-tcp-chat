#include "Application.hpp"


Application::Application(QObject *parent) : QObject(parent), m_ChatModel(nullptr) {
    auto groupCategory = new QStandardItem("groups");
    groupCategory->setData(Category, Qt::UserRole);
    auto onlineCategory = new QStandardItem("online");
    onlineCategory->setData(Category, Qt::UserRole);
    auto offlineCategory = new QStandardItem("offline");
    offlineCategory->setData(Category, Qt::UserRole);

    m_roomListModel.appendRow(groupCategory);
    m_roomListModel.appendRow(onlineCategory);
    m_roomListModel.appendRow(offlineCategory);
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

void Application::addChatMessage(const QUuid &targetRoomId, const QString &message) {
    m_rooms[targetRoomId]->m_chat_model()->appendRow(new QStandardItem(message));
    if (targetRoomId != m_currentRoomId) {
        //update the unread status and the chat model in that room
        m_rooms[targetRoomId]->incrementUnreadCount();
        //TODO: update UI: icon...
    }
}

void Application::sendMessage(const QString &message) {
    m_client.sendMessage(message, m_currentRoomId);
}

void Application::updateRooms(const LoginSuccessPacket & loginSuccessPacket) {
    //This line keeps the program running forever!!
    //m_roomListModel.appendRow(m_roomListModel.invisibleRootItem());

    // add rooms:
    auto roomInfos = loginSuccessPacket.roomInfos;
    // add the public room to the sidebar
    addChatGroup(m_publicRoomId, "public");

    //add the rest of the rooms
    foreach(auto roomInfo, roomInfos) {
        if(roomInfo.roomId == m_publicRoomId || roomInfo.roomId == m_currentRoomId) continue;
        if (roomInfo.roomType == RoomType::Self) {
            addUser(roomInfo.roomId, roomInfo.roomId, roomInfo.roomName);
        }
        if (roomInfo.roomType == RoomType::DirectChat) {
            addUser(roomInfo.roomId, QUuid() , roomInfo.roomName);
        }
    }

    // add users without rooms
    for (const auto& [userId, userName]: loginSuccessPacket.contacts.asKeyValueRange()) {
        // qInfo() << chatRoom->getRoomId() << chatRoom->getRoomName();
        // qInfo() << Q_FUNC_INFO << chatRoom->getRoomInfo().roomId;
        addUser(QUuid(), userId, userName);
    }
}

void Application::addUser(const QUuid &roomId, const QUuid &userId, const QString &userName) {
    // TODO: replace it with subclass of QAbstractItem
    QStandardItem* item = new QStandardItem(userName);
    item->setData(roomId, RoomIdRole);
    item->setData(userId, UserIdRole);
    item->setIcon(QIcon(":/icons/icons/mann-avatar.png"));
    m_roomListModel.item(1)->appendRow(item);
}

void Application::addChatGroup(const QUuid &roomId, const QString &groupName) {
    QStandardItem* item = new QStandardItem(groupName);
    item->setData(roomId, RoomIdRole);
    item->setData(QUuid(), UserIdRole);
    item->setIcon(QIcon(":/icons/icons/globe.png"));
    m_roomListModel.item(0)->appendRow(item);
}


void Application::removeUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    auto userCategoryItem = m_roomListModel.item(1);
    for (int row = 0; row < userCategoryItem->rowCount(); ++row) {
        auto item = userCategoryItem->child(row);
        if (item->data(UserIdRole) == logoutNotificationPacket.userId) {
            userCategoryItem->removeRow(row);
            break;
        }
    }
}

bool Application::setRoomIdOnUser(const QUuid &roomId, const QUuid &userId, bool switchRoomLater) {
    qInfo() << Q_FUNC_INFO;
    auto userCategoryItem = m_roomListModel.item(1);
    for (int row = 0; row < userCategoryItem->rowCount(); ++row) {
        auto item = userCategoryItem->child(row);
        if (item->data(UserIdRole) == userId) {
            item->setData(roomId, RoomIdRole);
            if (switchRoomLater)
                return switchRoom(item->index()) != nullptr;
            return true;
        }
    }
    return false;
}

void Application::processMessage(const ChatMessagePacket &chatMessagePacket) {
    qInfo() << Q_FUNC_INFO;
    QUuid targetRoomId = chatMessagePacket.roomId;
    // If room is not created, then create the room
    if (!m_rooms.contains(targetRoomId)) {
        qInfo() << Q_FUNC_INFO << "add new RoomId " << chatMessagePacket.roomId;
        QString roomName = (targetRoomId == m_publicRoomId ? "public" : chatMessagePacket.senderName);

        if (roomName != "public") {
            setRoomIdOnUser(targetRoomId, chatMessagePacket.senderId, false);
        }

        m_rooms.insert(targetRoomId,
                std::make_shared<ChatRoom>(targetRoomId, roomName, 0));
    }
    // if the message is sent to the current room, print the message here
    addChatMessage(targetRoomId, chatMessagePacket.getMessage());
}

std::shared_ptr<ChatRoom> Application::switchRoom(const QModelIndex &index) {
    // check entry category
    if (index.data(Qt::UserRole) == Category) {
        qInfo() << Q_FUNC_INFO << "category item clicked";
        return nullptr;
    }
    // check current chatroom
    QUuid targetRoomId = index.data(RoomIdRole).toUuid();
    if (targetRoomId.isNull()) { // request a new direct chat room
        qInfo() << Q_FUNC_INFO << ": requesting a new roomId";
        QUuid userId = index.data(UserIdRole).toUuid();
        if (userId.isNull()) {
            qCritical() << Q_FUNC_INFO << " :This should not happen!";
        }
        RoomRequestPacket roomRequestPacket{RoomType::DirectChat,{userId}, ""};
        m_client.sendRoomRequest(roomRequestPacket);
        return nullptr;
    }

    if ( targetRoomId != m_currentRoomId) {
        qInfo() << Q_FUNC_INFO << ": setting the current roomId " << targetRoomId;
        m_currentRoomId = targetRoomId;
        // switch the chat room, retrieve the chat history
        if (!m_rooms.contains(targetRoomId)) { // the chatroom hasn't been created yet
            // create a new room
            m_rooms.insert(targetRoomId,
                std::make_shared<ChatRoom>(targetRoomId, index.data(Qt::DisplayRole).toString(), 0));
        }
        m_ChatModel = m_rooms[targetRoomId]->m_chat_model();
        return m_rooms[targetRoomId];
    }

    qInfo() << Q_FUNC_INFO << "you are already in the room you selected!";
    return nullptr;
}





void Application::disconnectFromServer() {
    m_client.disconnectFromHost();
}
//
// void Application::onRoomAcquired(const RoomInfoPacket &roomInfoPacket) {
//
// }


