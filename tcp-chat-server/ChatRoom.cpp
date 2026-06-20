#include "ChatRoom.hpp"


ChatRoom::ChatRoom(QUuid roomId, const QString &roomName, QObject* parent ):
    m_roomID(roomId), m_roomName(roomName), QObject(parent) {
}

void ChatRoom::setWelcomeMsg(const QString &newWelcomeMsg) {
    m_welcome_msg = newWelcomeMsg;
}

RoomInfo ChatRoom::getRoomInfo() {
    qInfo() << Q_FUNC_INFO;
    auto usernames = m_users.usernames();
    return RoomInfo{m_roomID, m_roomName, usernames};
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

