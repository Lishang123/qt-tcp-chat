#include "server.h"

#include "Client.h"


Server::Server(QObject *parent) : QTcpServer(parent) {
}

void Server::closeServer() {
    foreach(Client* client, m_clients) {
        client->getSocket()->close();
    }
    qDeleteAll(m_clients);
    m_clients.clear();

    // update GUI
    emit clientChanged();
    QTcpServer::close();
}


void Server::handleClientDisconnected() {
    auto *client = qobject_cast<Client *>(sender());
    removeClient(client);

    // Notify other users that someone has logged out.
    if (m_clients.count() > 0) {
        QByteArray data;
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << PacketType::NotifyLogout;
        stream << LogoutNotificationPacket{client->getClientId()};
        foreach(QUuid clientId, m_clients.keys()) {
            if (clientId == client->getClientId()) continue;
            sendData(m_clients[clientId], data);
        }
    }

    emit clientChanged();
    emit clientDisconnected(client->getClientId());
}

void Server::handleLoginSuccess(QUuid userId, const QString& username, const QMap<QUuid, UserInfo>& users, QList<RoomInfo>& roomInfos) {
    qInfo() << Q_FUNC_INFO;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << PacketType::LoginSuccess;
    stream << LoginSuccessPacket{userId, username, roomInfos, users, m_welcome_msg};
    sendData(m_clients[userId], data);
    if (m_clients.count() > 1) {
        QByteArray notifyData;
        QDataStream notifyStream(&notifyData, QIODevice::WriteOnly);
        notifyStream << PacketType::NotifyLogin;
        notifyStream << LoginNotificationPacket{userId, username };
        foreach(QUuid clientId, m_clients.keys()) {
            if (clientId == userId) continue;
            sendData(m_clients[clientId], notifyData);
        }
    }

}

void Server::handleLoginFailed(QUuid userId, const QString& errorMsg) {
    auto *client = m_clients[userId];
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << PacketType::LoginFail;
    stream << LoginFailedPacket{ errorMsg};
    sendData(m_clients[userId], data);

    client->getSocket()->disconnectFromHost();
}


void Server::removeClient(Client *client) {
    if (!client) return;

    m_clients.remove(client->getClientId());
    disconnect(client, &Client::disconnected, this, &Server::handleClientDisconnected);
    disconnect(client, &Client::dataReceived, this, &Server::onDataReceived);
    client->deleteLater();
    emit clientChanged();
}

void Server::changeClientId(QUuid clientId , QUuid newClientId) {
    qInfo() << Q_FUNC_INFO << "reusing the clientId " << newClientId << ", remove the generated clientId" << clientId;
    auto it = m_clients.find(clientId);
    if (it != m_clients.end()) {
        auto client = it.value();
        client->setClientId(newClientId);
        m_clients.erase(it);
        m_clients.insert(newClientId, client);
    }
    else {
        qCritical() << Q_FUNC_INFO << " client not found";
    }
}

void Server::sendMessageToRoom( ChatRoom& chatRoom, ChatMessagePacket &packet) {
    qInfo() << Q_FUNC_INFO;
    //generate a new message id
    packet.messageId = QUuid::createUuid();
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << PacketType::ChatMessage;
    stream << packet;
    for (auto user: chatRoom.getRoomUsers()) {
        if (user->isOnline()) sendData(m_clients[user->user_id], data);
    }
}


void Server::broadcast(ChatMessagePacket& packet) {
    //Print the message for each connected client
    qInfo() << Q_FUNC_INFO;
    packet.messageId = QUuid::createUuid();
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << PacketType::ChatMessage;
    stream << packet;
    for (Client *a_client: m_clients.values()) {
        a_client->getSocket()->write(data);
    }
    emit clientChanged();
}

/*
     * incomingConnection() is only called AFTER the TCP
     * connection has already been accepted by the OS.
     * So the socket is already connected.
     * So QTcpSocket::connected() signal is usually not useful
     * on the server side in this pattern.
    */

void Server::incomingConnection(qintptr handle) {
    // create the client
    auto client = new Client(nullptr, handle);
    client->start();

    // add the client socket to the client list.
    client->setClientId(QUuid::createUuid());
    m_clients.insert(client->getClientId(), client);

    connect(client, &Client::disconnected, this, &Server::handleClientDisconnected);
    connect(client, &Client::dataReceived, this, &Server::onDataReceived);

    // update GUI
    emit clientChanged();
}


void Server::onDataReceived(const QByteArray & data) {
    auto client = qobject_cast<Client *>(sender());
    if (!client) {
        qInfo() << Q_FUNC_INFO << " sender is not a client " << data;
        return;
    };
    emit messageReceived(client->getClientId(), data);
}

void Server::sendRoomInfo(const QUuid userId, const RoomInfo &roomInfo) {
    RoomInfoPacket roomInfoPacket{roomInfo};
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << PacketType::RoomAcquired;
    stream << roomInfoPacket;
    sendData(m_clients[userId], data);
}

void Server::setWelcome_msg(const QString &newWelcome_msg) {
    m_welcome_msg = newWelcome_msg;
}

size_t Server::getClientsCount() {
    return m_clients.count();
}


void Server::sendData(Client *client, const QByteArray &data) {
    client->getSocket()->write(data);
}


void Server::sendData(const QByteArray &data) {
    for (Client *a_client: m_clients.values()) {
        a_client->getSocket()->write(data);
    }
}