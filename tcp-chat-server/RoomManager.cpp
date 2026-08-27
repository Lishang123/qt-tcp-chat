#include "RoomManager.hpp"


RoomManager::RoomManager(QObject *parent) : QObject(parent) {
    //load from database
    m_dbManager.initFromDB(*this);

    if (m_publicRoomId.isNull()) {
        //create public room
        auto publicRoom = createRoom(RoomType::Public, QUuid::createUuid(), "public", false);
        m_publicRoomId = publicRoom->getRoomId();
    }
}


void RoomManager::handleMessage(QUuid senderId, const QByteArray &data) {
    qInfo() << Q_FUNC_INFO;
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
        case PacketType::ChatMessagePkt: {
            ChatMessagePacket chatMessagePacket;
            ds >> chatMessagePacket;
            handleChatMessage(senderId, chatMessagePacket);
            break;
        }
        case PacketType::RoomRequest: {
            RoomRequestPacket roomRequestPacket;
            ds >> roomRequestPacket;
            handleRoomRequest(senderId, roomRequestPacket);
            break;
        }
        case PacketType::RoomDeleteRequest: {
            RoomDeleteRequestPacket roomDeleteRequestPacket;
            ds >> roomDeleteRequestPacket;
            handleRoomDeleteRequest(senderId, roomDeleteRequestPacket);
            break;
        }
        // case PacketType::LogoutRequest: {
        //     LogoutRequestPacket logoutPacket;
        //     ds >> logoutPacket;
        //     removeUser(clientId);
        //     break;
        // }
        default:
            qCritical() << Q_FUNC_INFO << "Unknown packet type: " << type ;

    }
}


bool RoomManager::handleLoginRequest(QUuid clientId, LoginRequestPacket & packet) {
    //TODO: Authentication
    qInfo() << Q_FUNC_INFO;

    bool registered = false;
    QUuid userId = clientId;
    // Check if the user is registered.
    auto user = findUserByName(packet.username);
    if (user != nullptr) {
        // Check if user already logged in.
        if ( user->isOnline()) {
            QString errorMsg = "User already logged in!";
            emit loginFailed(clientId, errorMsg);
            qInfo() << Q_FUNC_INFO << errorMsg;
            return false;
        }
        // User re-login
        userId = user->getUserId();
        user->setOnlineStatus(true);
        registered = true;
        // clientId will be discarded if user is already registered.
        emit changeClientId(clientId, userId);
    }

    if (!registered) {
        // User Registration
        // Add user to user list
        user = createUser(userId, packet.username, false);
        if (!user) {
            qCritical() << "Registering user failed: This should not happen.";
            return false;
        }

        //  Create the private room for user
        auto room = createRoom(RoomType::Self, userId, packet.username, false);
        addRoomMember(room, user, false);

        //  Add user to public group
        addRoomMember(m_rooms[m_publicRoomId], user, false);
        //m_rooms[m_publicRoomId]->addUser(userId, user);
    }

    // Construct roomInfo
    QList<RoomInfo> roomInfos;
    auto userInfos = m_users.getUserInfos();
    for (const auto& [_, chatRoom]: m_rooms) {
        // qInfo() << Q_FUNC_INFO << chatRoom->getRoomInfo().roomId;
        RoomType roomType = chatRoom->getRoomType();
        switch (roomType) {
            case RoomType::Public: {
                roomInfos.append(chatRoom->getRoomInfo());
                break;
            }
            case RoomType::Self: {
                // add monologue to the room info
                if (chatRoom->getRoomUsers().contains(userId)) {
                    roomInfos.append(chatRoom->getRoomInfo());
                    // Remove self from userInfos so it's not printed twice on the client side
                    userInfos.remove(chatRoom->getRoomId());
                }
                break;
            }
            case RoomType::Chatgroup: {
                if (chatRoom->getRoomUsers().contains(userId)) {
                    roomInfos.append(chatRoom->getRoomInfo());
                }
                break;
            }
            case RoomType::DirectChat: {
                if (chatRoom->getRoomUsers().contains(userId)) {
                    auto users = chatRoom->getRoomUsers();
                    auto it = std::ranges::find_if(
                        users,
                        [&](const auto &participant)
                        {
                            return participant->user_id != userId;
                        });
                    if (it != users.end()) {
                        auto otherUser = *it;
                        //update the name of roomInfo to the other user
                        auto roomInfo = chatRoom->getRoomInfo();
                        // Remove the user who is getting it, leaving only the id of the other participant.
                        roomInfo.userInfos.remove(userId);
                        roomInfo.roomName = otherUser->username;
                        roomInfos.append(roomInfo);
                        // Remove the other participant from user list
                        userInfos.remove(otherUser->user_id);
                    }
                    else {
                        qCritical() << Q_FUNC_INFO << " This should not happen: The other participant is not found!";
                    }
                }
                break;
            }
            default:
                qCritical() << Q_FUNC_INFO << "Unknown room type!";
        }
    }
    qInfo() << Q_FUNC_INFO << "emitting loginSuccess";
    emit loginSuccess(userId, packet.username, userInfos, roomInfos);
    return true;
}

void RoomManager::removeUser(QUuid userId) {
    qInfo() << Q_FUNC_INFO;

    QList<QUuid> roomsToRemove;

    // remove the user from all user groups
    for (const auto & [roomId, room]: m_rooms) {
        qInfo()  << room->getRoomName();
        room->removeUser(userId);
        if (room->getRoomName() != "public" && room->isEmpty())
            roomsToRemove.append(roomId);
    }
    // remove all empty rooms
    foreach (auto roomId, roomsToRemove) {
        removeRoom(roomId);
    }
    // remove the user
    m_users.remove(userId);
    m_dbManager.removeUser(userId);
}

void RoomManager::logoutUser(QUuid userId) {
    qInfo() << Q_FUNC_INFO;

    if (!m_users.contains(userId)) {
        qCritical() << Q_FUNC_INFO << " User " << userId << " not found!";
        return;
    }
    m_users[userId]->setOnlineStatus(false);
    qInfo() << Q_FUNC_INFO << userId << "Logged out";
}


bool RoomManager::handleChatMessage(QUuid senderId, ChatMessagePacket &packet) {
    qInfo() << Q_FUNC_INFO;
    //generate a new message id
    packet.messageId = QUuid::createUuid();
    // add message to database
    m_dbManager.addMessage(packet.messageId, senderId, packet.roomId, packet.text, packet.timestamp);
    emit sendMessageToRoom(*m_rooms[packet.roomId], packet);
    return true;
}

bool RoomManager::handleRoomRequest(QUuid senderId, RoomRequestPacket &packet) {
    auto room = createRoom(packet.roomType, QUuid::createUuid(), packet.roomName, false);
    addRoomMember(room, m_users[senderId], false);// first, add the user who requests it
    //room->addUser(senderId, m_users[senderId]);
    std::ranges::for_each(packet.memberIds, [&](const auto &memberId) {
        addRoomMember(room, m_users[memberId], false);
        //room->addUser(memberId, m_users[memberId]);
    });
    auto roomInfo = room->getRoomInfo();
    roomInfo.creatorId = senderId;
    QList<QUuid> notificationTargets{senderId};
    notificationTargets.append(packet.memberIds);
    for (const auto &userId : notificationTargets) {
        RoomInfo userRoomInfo = roomInfo;
        if (packet.roomType == RoomType::DirectChat) {
            // remove the user requesting for convenience.
            userRoomInfo.userInfos.remove(userId);
        }
        emit roomCreated(userId, userRoomInfo);
    }
    return true;
}

bool RoomManager::handleRoomDeleteRequest(QUuid senderId, RoomDeleteRequestPacket &packet) {
    if (!m_rooms.contains(packet.roomId)) {
        qCritical() << Q_FUNC_INFO << "room not found:" << packet.roomId;
        return false;
    }
    auto room = m_rooms[packet.roomId];
    if (room->getRoomType() != RoomType::Chatgroup || !room->getRoomUsers().contains(senderId)) {
        qCritical() << Q_FUNC_INFO << "invalid delete request:" << packet.roomId << senderId;
        return false;
    }

    const QList<QUuid> memberIds = room->getRoomUsers().keys();
    const RoomDeletedPacket roomDeletedPacket{packet.roomId};
    removeRoom(packet.roomId);
    for (const auto &memberId : memberIds) {
        emit roomDeleted(memberId, roomDeletedPacket);
    }
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

std::shared_ptr<User> RoomManager::createUser(QUuid clientId, const QString & username, bool fromDB) {
    qInfo() << Q_FUNC_INFO;
    auto user = std::make_shared<User>(clientId, username);
    m_users.insert(clientId, user);
    if (!fromDB) {
        // user registration
        user->setOnlineStatus(true);
        m_dbManager.addUser(*user);
    }
    return user;
}

void RoomManager::addRoomMember(std::shared_ptr<ChatRoom> room, std::shared_ptr<User> user, bool fromDB) {
    room->addUser(user->user_id, user);
    if (!fromDB) m_dbManager.addRoomMember(room->getRoomId(), user->user_id);
}

bool RoomManager::addRoomMember(const QUuid &roomId, const QUuid &userId) {
    if (!m_rooms.contains(roomId)) {
        qDebug() << "roomId not found!" << Q_FUNC_INFO;
        return false;
    }
    if (!m_users.contains(userId)) {
        qDebug() << "userId not found!" << Q_FUNC_INFO;
        return false;
    }
    auto room = m_rooms[roomId];
    auto user = m_users[userId];
    addRoomMember(room,user, true);
    return true;
}


std::shared_ptr<ChatRoom> RoomManager::createRoom(RoomType roomType, QUuid roomId, const QString &roomName, bool fromDB) {
    qDebug() << Q_FUNC_INFO;
    auto room = std::make_shared<ChatRoom>(roomType, roomId, roomName);
    m_rooms[roomId] = room;
    if (!fromDB) m_dbManager.addRoom(*room);
    return room;
}

bool RoomManager::removeRoom(QUuid roomId) {
    m_rooms.erase(roomId);
    m_dbManager.removeRoom(roomId);
    return true;
}
