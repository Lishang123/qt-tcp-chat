#include "server.h"



Server::Server(QObject *parent) : QTcpServer(parent)
{

}

void Server::close()
{
    foreach (QTcpSocket* socket, m_sockets)
    {
        socket->close();
    }
    qDeleteAll(m_sockets);
    m_sockets.clear();
    emit clientChanged();
    QTcpServer::close();
}


void Server::disconnected()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;

    m_sockets.removeAll(socket);
    disconnect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
    disconnect(socket, &QTcpSocket::readyRead, this, &Server::readyRead);
    socket->deleteLater();

    emit clientChanged();
}



void Server::readyRead()
{
    /*
     * TODO: how about we setting a message format so that we
     * distinguish user name, user message, etc.?
     * So we need to parse the message according to some
     * XML? format. And maybe react accordingly.
     */
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;

    QByteArray data = socket->readAll();

    //Print the message for each connected client
    foreach (QTcpSocket* socket, m_sockets)
    {
        socket->write(data);
    }
}



void Server::incomingConnection(qintptr handle)
{
    /*
     * incomingConnection() is only called AFTER the TCP
     * connection has already been accepted by the OS.
     * So the socket is already connected.
     *
     * QTcpSocket::connected() signal is usually not useful
     * on the server side in this pattern.
    */
    QTcpSocket *socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(handle))
    {
        delete socket;
        return;
    }
    // add the client socket to the client list.
    m_sockets.append(socket);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconnected);
    connect(socket,&QTcpSocket::readyRead, this, &Server::readyRead);

    emit clientChanged();

    socket->write(m_welcome_msg.toUtf8());
}

void Server::setWelcome_msg(const QString &newWelcome_msg)
{
    m_welcome_msg = newWelcome_msg;
}

int Server::getClientsCount()
{
    return m_sockets.count();
}

