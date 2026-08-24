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
    m_receiveBuffer.append(m_socket->readAll());

    QByteArray data;
    while (true) {
        const auto result = TcpFraming::takeNextFrame(m_receiveBuffer, data);
        if (result == TcpFraming::ReadResult::Incomplete) {
            return;
        }
        if (result == TcpFraming::ReadResult::Invalid) {
            qWarning() << Q_FUNC_INFO << "Invalid or oversized frame; disconnecting client";
            m_socket->disconnectFromHost();
            return;
        }

        emit dataReceived(data);
    }
}

void Client::sendMessage(const QByteArray& message) {
    const QByteArray framedMessage = TcpFraming::frame(message);
    if (framedMessage.isEmpty() && !message.isEmpty()) {
        qWarning() << Q_FUNC_INFO << "Refusing to send oversized frame";
        return;
    }
    m_socket->write(framedMessage);
}
