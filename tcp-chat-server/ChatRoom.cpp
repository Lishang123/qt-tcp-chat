#include "ChatRoom.hpp"

ChatRoom::ChatRoom(QObject *parent) : QObject(parent) {
}

void ChatRoom::setWelcomeMsg(const QString &newWelcomeMsg) {
    m_welcome_msg = newWelcomeMsg;
}

size_t ChatRoom::getClientsCount() {
    return m_users.count();
}

void ChatRoom::handleMessage(QUuid senderId, const QByteArray & data) {
    qInfo() << Q_FUNC_INFO << data;
    QDataStream ds(data);
    PacketType type;
    ds >> type;

    switch (type) {
        case PacketType::LoginRequest: {
            LoginRequestPacket loginPacket;
            ds >> loginPacket;
            if (!handleLoginRequest(senderId, loginPacket)) {
                return;
            }
            break;
        }
        // case PacketType::LogoutRequest: {
        //     LogoutRequestPacket logoutPacket;
        //     ds >> logoutPacket;
        //     removeUser(clientId);
        //     break;
        // }
        case PacketType::ChatMessage: {
            ChatMessagePacket chatMessagePacket;
            ds >> chatMessagePacket;
            if (!handleChatMessage(senderId, chatMessagePacket)) {}

        }
        default:
            qCritical() << Q_FUNC_INFO << "Unknown packet type";

    }

}

void ChatRoom::removeUser(QUuid userId) {
    m_users.remove(userId);
}

void ChatRoom::addUser(QUuid clientId, const QString & username, quint8 roomId) {
    qInfo() << Q_FUNC_INFO;
    m_users.insert(clientId, {clientId, username, roomId});
    emit userAdded(clientId, username, roomId);
}

User* ChatRoom::findUserbyName(QString &username) {
    auto it = std::ranges::find_if(m_users, [&username](const User& user) {
        return username == user.username;
    });
    if (it != m_users.end()) {
        return &(*it);
    }
    return nullptr;
}

bool ChatRoom::handleLoginRequest(QUuid clientId, LoginRequestPacket & packet) {
    //TODO: Authentication and Room
    qInfo() << Q_FUNC_INFO;
    User* user = findUserbyName(packet.username);
    if (user) {
        QString errorMsg = "User already logged in!";
        emit loginFailed(clientId, errorMsg);
        qInfo() << Q_FUNC_INFO << errorMsg;
        return false;
    }
    addUser(clientId, packet.username, 1);
    return true;
}

bool ChatRoom::handleChatMessage(QUuid senderId, ChatMessagePacket &packet) {
    qInfo() << Q_FUNC_INFO;
    if (packet.recipientName.isEmpty()) {
        emit broadcast(packet);
        return true;
    }

    auto recipient = findUserbyName(packet.recipientName);
    if (!recipient) {
        qCritical() << Q_FUNC_INFO << packet.recipientName << " doesn't exist!";
        return false;
    }
    emit sendMessageToClient(senderId, recipient->user_id, packet);
}
