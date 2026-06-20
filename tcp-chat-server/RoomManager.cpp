#include "RoomManager.hpp"


RoomManager::RoomManager(QObject *parent) : QObject(parent) {
    auto publicRoom = createRoom(QUuid::createUuid(), "public");
    m_publicRoomId = publicRoom->getRoomId();
}

ChatRoom* RoomManager::createRoom(QUuid roomId, const QString &roomName) {
    auto room = new ChatRoom(roomId, roomName, this);
    m_rooms[roomId] = room;
    return room;
}

bool RoomManager::removeRoom(QUuid roomId) {
    m_rooms.erase(roomId);
    return true;
}

void RoomManager::handleMessage(QUuid senderId, const QByteArray &data) {
    qInfo() << Q_FUNC_INFO << data;
    QDataStream ds(data);
    PacketType type;
    ds >> type;

    switch (type) {
        case PacketType::LoginRequest: {
            LoginRequestPacket loginPacket;
            ds >> loginPacket;
            handleLoginRequest(senderId, loginPacket);
            break;
        }
        case PacketType::ChatMessage: {
            ChatMessagePacket chatMessagePacket;
            ds >> chatMessagePacket;
            handleChatMessage(senderId, chatMessagePacket);
            break;
        }
        // case PacketType::LogoutRequest: {
        //     LogoutRequestPacket logoutPacket;
        //     ds >> logoutPacket;
        //     removeUser(clientId);
        //     break;
        // }
        default:
            qCritical() << Q_FUNC_INFO << "Unknown packet type: " << data ;

    }
}


bool RoomManager::handleLoginRequest(QUuid clientId, LoginRequestPacket & packet) {
    //TODO: Authentication
    qInfo() << Q_FUNC_INFO;

    // Check if user already logged in.
    auto user = findUserByName(packet.username);
    if (user != nullptr) {
        QString errorMsg = "User already logged in!";
        emit loginFailed(clientId, errorMsg);
        qInfo() << Q_FUNC_INFO << errorMsg;
        return false;
    }

    //  Add user to list
    user = addUser(clientId, packet.username);
    if (!user) {
        qCritical() << "This should not happen.";
        return false;
    }

    //  Create the room for user
    auto room = createRoom(QUuid::createUuid(), packet.username);
    room->addUser(clientId, user);

    //  Add user to public group
    m_rooms[m_publicRoomId]->addUser(clientId, user);

    // Construct roomInfo here
    QList<RoomInfo> rooms;
    for (const auto& [_, chatRoom]: m_rooms) {
        // qInfo() << chatRoom->getRoomId() << chatRoom->getRoomName();
        // qInfo() << Q_FUNC_INFO << chatRoom->getRoomInfo().roomId;
        rooms.append(chatRoom->getRoomInfo());
    }
    emit loginSuccess(clientId, packet.username, rooms);
    return true;
}

void RoomManager::removeUser(QUuid userId) {
    qInfo() << Q_FUNC_INFO;

    QList<QUuid> roomsToRemove;
    for (const auto & [roomId, room]: m_rooms) {
        qInfo()  << room->getRoomName();
        room->removeUser(userId);
        if (room->getRoomName() != "public" && room->isEmpty())
            roomsToRemove.append(roomId);
    }
    foreach (auto roomId, roomsToRemove) {
        m_rooms.erase(roomId);
    }
    m_users.remove(userId);
}


bool RoomManager::handleChatMessage(QUuid senderId, ChatMessagePacket &packet) {
    qInfo() << Q_FUNC_INFO;
    emit sendMessageToRoom(*m_rooms[packet.roomId], packet);
    return true;
}


std::shared_ptr<User> RoomManager::findUserByName(QString &username) {
    auto it = std::ranges::find_if(m_users, [&username](const auto user) {
        return username == user->username;
    });
    if (it != m_users.end()) {
        return std::shared_ptr(*it);
    }
    return nullptr;
}

std::shared_ptr<User> RoomManager::addUser(QUuid clientId, const QString & username) {
    qInfo() << Q_FUNC_INFO;
    auto user = std::make_shared<User>(clientId, username);
    m_users.insert(clientId, user);
    return user;
}
