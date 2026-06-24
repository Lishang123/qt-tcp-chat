#include "ChatRoom.hpp"


ChatRoom::ChatRoom(RoomType roomType, QUuid roomId, const QString &roomName):
    m_roomType(roomType), m_roomID(roomId), m_roomName(roomName) { }

void ChatRoom::setWelcomeMsg(const QString &newWelcomeMsg) {
    m_welcome_msg = newWelcomeMsg;
}

RoomInfo ChatRoom::getRoomInfo() {
    qInfo() << Q_FUNC_INFO;
    //auto usernames = m_users.usernames();
    return RoomInfo{m_roomType, m_roomID, m_roomName, m_users.getUserInfos()};
}

size_t ChatRoom::getClientsCount() {
    return m_users.count();
}

void ChatRoom::addUser(QUuid clientId, std::shared_ptr<User> user) {
    m_users.insert(clientId, user);
}

void ChatRoom::removeUser(QUuid userId) {
    m_users.remove(userId);
}

