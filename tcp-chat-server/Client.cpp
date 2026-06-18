#include "Client.h"

Client::Client(QObject *parent, qintptr socketDescriptor)
    :QObject(parent), m_socketDescriptor(socketDescriptor)
{
}

void Client::start() {

    m_socket = new QTcpSocket(this);
    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        m_socket -> deleteLater();
        qInfo() << Q_FUNC_INFO << ": Set socket setSocketDescriptor failed";
        return;
    }
    connect(m_socket, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::readyRead);

}

void Client::readyRead() {
    QByteArray data = m_socket->readAll();
    qInfo() << Q_FUNC_INFO << data;
    emit dataReceived(data);
}

void Client::sendMessage(const QByteArray& message) {
    m_socket->write(message);
}
