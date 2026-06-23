#include "RoomManager.hpp"


RoomManager::RoomManager(QObject *parent) : QObject(parent) {
    //create public room
    auto publicRoom = createRoom(RoomType::Public, QUuid::createUuid(), "public");
    m_publicRoomId = publicRoom->getRoomId();
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
        case PacketType::RoomRequest: {
            RoomRequestPacket roomRequestPacket;
            ds >> roomRequestPacket;
            handleRoomRequest(senderId, roomRequestPacket);
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


    //  Create the private room for user
    auto room = createRoom(RoomType::Self, clientId, packet.username);
    room->addUser(clientId, user);

    //  Add user to public group
    m_rooms[m_publicRoomId]->addUser(clientId, user);

    QList<RoomInfo> roomInfos;
    auto userInfos = m_users.getUserInfos();
    // Construct roomInfo here
    for (const auto& [_, chatRoom]: m_rooms) {
        // qInfo() << chatRoom->getRoomId() << chatRoom->getRoomName();
        // qInfo() << Q_FUNC_INFO << chatRoom->getRoomInfo().roomId;
        RoomType roomType = chatRoom->getRoomType();
        switch (roomType) {
            case RoomType::Public: {
                roomInfos.append(chatRoom->getRoomInfo());
                break;
            }
            case RoomType::Self: {
                // add monologue to the room info
                if (chatRoom->getRoomUsers().contains(clientId)) {
                    roomInfos.append(chatRoom->getRoomInfo());
                    //remove self from userInfos
                    userInfos.remove(chatRoom->getRoomId());
                }
                break;
            }
            case RoomType::Chatgroup: {
                if (chatRoom->getRoomUsers().contains(clientId)) {
                    roomInfos.append(chatRoom->getRoomInfo());
                }
                break;
            }
            case RoomType::DirectChat: {
                if (chatRoom->getRoomUsers().contains(clientId)) {
                    roomInfos.append(chatRoom->getRoomInfo());
                    // Remove the other participant from user list.
                    auto users = chatRoom->getRoomUsers();
                    auto it = std::ranges::find_if(
                        users,
                        [&](const auto &participant)
                        {
                            return participant->user_id != clientId;
                        });
                    if (it != users.end()) {
                        auto otherUser = *it;
                        userInfos.remove(otherUser->user_id);
                    }
                    else {
                        qCritical() << Q_FUNC_INFO << ": The other participant is not found!";
                    }
                    //userInfos.remove(chatRoom->getRoomUsers());
                }
                break;
            }
            default:
                qCritical() << Q_FUNC_INFO << "Unknown room type!";
        }
    }
    emit loginSuccess(clientId, packet.username, userInfos, roomInfos);
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
        removeRoom(roomId);
    }
    m_users.remove(userId);
}


bool RoomManager::handleChatMessage(QUuid senderId, ChatMessagePacket &packet) {
    qInfo() << Q_FUNC_INFO;
    emit sendMessageToRoom(*m_rooms[packet.roomId], packet);
    return true;
}

bool RoomManager::handleRoomRequest(QUuid senderId, RoomRequestPacket &packet) {
    auto room = createRoom(RoomType::DirectChat, QUuid::createUuid(), packet.roomName);
    room->addUser(senderId, m_users[senderId]); // first, add the user who requests it
    std::ranges::for_each(packet.memberIds, [&](const auto &memberId) {
        room->addUser(memberId, m_users[memberId]);
    });
    emit roomCreated(senderId, room->getRoomInfo());
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


std::shared_ptr<ChatRoom> RoomManager::createRoom(RoomType roomType, QUuid roomId, const QString &roomName) {
    auto room = std::make_shared<ChatRoom>(roomType, roomId, roomName);
    m_rooms[roomId] = room;
    return room;
}

bool RoomManager::removeRoom(QUuid roomId) {
    m_rooms.erase(roomId);
    return true;
}