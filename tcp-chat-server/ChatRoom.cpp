#include "ChatRoom.hpp"

ChatRoom::ChatRoom(QObject *parent) : QObject(parent) {
}

void ChatRoom::setWelcomeMsg(const QString &newWelcomeMsg) {
    m_welcome_msg = newWelcomeMsg;
}

size_t ChatRoom::getClientsCount() {
    return m_users.count();
}

void ChatRoom::handleMessage(const QByteArray & data) {
    qInfo() << Q_FUNC_INFO << data;
    QString msg = QString::fromUtf8(data);
    emit broadcast(msg);
}

void ChatRoom::removeUser(QUuid userId) {
    m_users.remove(userId);
}

void ChatRoom::addUser(QUuid userId) {
    User user(userId, "username", 1);
    std::cout << user.toString();
    m_users.insert(userId, user);
}
