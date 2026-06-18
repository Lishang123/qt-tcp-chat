#include "server.h"

#include "Client.h"


Server::Server(QObject *parent) : QTcpServer(parent)
{

}

void Server::closeServer()
{
    foreach (Client* client, m_clients)
    {
        client->getSocket()->close();
    }
    qDeleteAll(m_clients);
    m_clients.clear();

    // update GUI
    emit clientChanged();
    QTcpServer::close();
}


void Server::clientDisconnected() {
    auto* client = qobject_cast<Client*>(sender());
    if(!client) return;

    m_clients.removeAll(client);
    disconnect(client, &Client::disconnected, this, &Server::clientDisconnected);
    disconnect(client, &Client::dataReceived, this, &Server::broadcast);
    client->deleteLater();

    // update GUI
    emit clientChanged();
}

void Server::sendMessage(Client* client, const QByteArray& message) {
    client->getSocket()->write(message);
}

void Server::broadcast(const QByteArray& data)
{
    //Print the message for each connected client
    for (Client* a_client : m_clients)
    {
        sendMessage(a_client, data);
    }
}

/*
     * incomingConnection() is only called AFTER the TCP
     * connection has already been accepted by the OS.
     * So the socket is already connected.
     * So QTcpSocket::connected() signal is usually not useful
     * on the server side in this pattern.
    */

void Server::incomingConnection(qintptr handle)
{
    // create the client in another thread
    auto client = new Client(nullptr, handle);

    client->start();

    // add the client socket to the client list.
    m_clients.append(client);

    connect(client, &Client::disconnected, this, &Server::clientDisconnected);
    connect(client,&Client::dataReceived, this, &Server::broadcast);

    // update GUI
    emit clientChanged();

    sendMessage(client,m_welcome_msg.toUtf8());
}

void Server::setWelcome_msg(const QString &newWelcome_msg)
{
    m_welcome_msg = newWelcome_msg;
}

size_t Server::getClientsCount()
{
    return m_clients.count();
}

