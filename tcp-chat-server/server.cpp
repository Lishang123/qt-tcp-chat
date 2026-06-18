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
    if (!client) return;

    m_clients.remove(client->getClientId());
    disconnect(client, &Client::disconnected, this, &Server::handleClientDisconnected);
    disconnect(client, &Client::dataReceived, this, &Server::broadcast);
    client->deleteLater();

    // update GUI
    emit clientDisconnected(client->getClientId());
    emit clientChanged();
}

void Server::sendMessage(Client *client, const QString &message) {
    client->getSocket()->write(message.toUtf8());
}

void Server::sendMessageToClient(QUuid clientId, QString &message) {
    sendMessage(m_clients[clientId], message);
}


void Server::broadcast(const QString &message) {
    //Print the message for each connected client
    qInfo() << Q_FUNC_INFO << message;
    for (Client *a_client: m_clients.values()) {
        sendMessage(a_client, message);
    }
    emit clientChanged();
}

// void Server::messageReceived(const QByteArray &message) {
//     emit messageReceived(message);
// }

/*
     * incomingConnection() is only called AFTER the TCP
     * connection has already been accepted by the OS.
     * So the socket is already connected.
     * So QTcpSocket::connected() signal is usually not useful
     * on the server side in this pattern.
    */

void Server::incomingConnection(qintptr handle) {
    // create the client in another thread
    auto client = new Client(nullptr, handle);
    client->start();

    // add the client socket to the client list.
    client->setClientId(QUuid::createUuid());
    m_clients.insert(client->getClientId(), client);

    connect(client, &Client::disconnected, this, &Server::handleClientDisconnected);
    bool ok = connect(client, &Client::dataReceived, this, &Server::messageReceived);
    // qInfo() << "forward connect ok:" << ok;
    // connect(client, &Client::dataReceived,
    //     this, [] {
    //         qInfo() << "Server received Client::dataReceived";
    //     });

    // update GUI
    emit clientConnected(client->getClientId());
    emit clientChanged();

    sendMessage(client, m_welcome_msg.toUtf8());
}

void Server::setWelcome_msg(const QString &newWelcome_msg) {
    m_welcome_msg = newWelcome_msg;
}

size_t Server::getClientsCount() {
    return m_clients.count();
}
