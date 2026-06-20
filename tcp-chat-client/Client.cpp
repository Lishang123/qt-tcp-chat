#include "Client.hpp"

#include <QInputDialog>


Client::Client(QObject *parent): QObject(parent){
    connect(&m_socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &Client::OnReadyRead);
    //Qt5:
    //connect(&m_socket,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&Client::error);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &Client::error);
}

void Client::connectToServer(const QString &address, quint16 port) {
    //this should not happen
    if(m_socket.isOpen()) m_socket.close();

    //asynchronous call to connect to the host
    m_socket.connectToHost(address, port);

    // check if connected, force synchronous waiting.
    // unnecessary since the socket will emit error if connection failed.
    // if(!m_socket.waitForConnected(3000)){
    //     on_btnDisconnect_clicked();
    //     QMessageBox::critical(this, "Error", "Cannot connect to the server!");
    //     return;
    // }
}

void Client::onConnected()
{
    qInfo() << Q_FUNC_INFO << "Connected!";
    emit connected();
    qInfo() << Q_FUNC_INFO << "signal emitted!";

    // TODO: implement authentication!
}



void Client::onDisconnected()
{
    qInfo() << "Disconnected!";
    emit disconnected();
}



void Client::OnReadyRead()
{
    qInfo() << Q_FUNC_INFO;
    QByteArray data = m_socket.readAll();
    QDataStream stream(&data, QIODevice::ReadOnly);
    PacketType packetType;
    stream >> packetType;
    qInfo() << "packet type:" << packetType;
    switch (packetType) {
        case PacketType::LoginFail: {
            LoginFailedPacket loginFailedPacket;
            stream >> loginFailedPacket;
            emit errorOccured(loginFailedPacket.errorMsg);
            break;
        }
        case PacketType::LoginSuccess: {
            LoginSuccessPacket loginSuccessPacket;
            stream >> loginSuccessPacket;
            // set username
            m_name = loginSuccessPacket.username;
            emit loggedIn(loginSuccessPacket);
            break;
        }
        case PacketType::NotifyLogin: {
            LoginNotificationPacket loginNotificationPacket;
            stream >> loginNotificationPacket;
            emit notifyUserLogin(loginNotificationPacket);
            break;
        }
        case PacketType::NotifyLogout: {
            LogoutNotificationPacket logoutNotificationPacket;
            stream >> logoutNotificationPacket;
            emit notifyUserLogout(logoutNotificationPacket);
            break;
        }
        case PacketType::ChatMessage: {
            ChatMessagePacket messagePacket;
            stream >> messagePacket;
            emit messageReceived(messagePacket);
            break;
        }
        default:
            qCritical() << Q_FUNC_INFO << "\tUnknown packet type:" << packetType;
            break;
    }
}

void Client::error(QAbstractSocket::SocketError error)
{
    //Ignore this error since we already handled it.
    if (error == QAbstractSocket::RemoteHostClosedError)
        return;
    QString errorMsg {m_socket.errorString()};
    emit errorOccured(errorMsg);
}



void Client::sendLoginRequest(const LoginRequestPacket &loginRequestPacket) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << PacketType::LoginRequest;
    out << loginRequestPacket;
    if (!m_socket.write(data)) {
        qCritical() << Q_FUNC_INFO << "\tCannot send login reqeust:\t" << data << m_socket.errorString() ;
    }
}

void Client::disconnectFromHost() {
    m_socket.disconnectFromHost();
}


void Client::sendMessage(const QString &message, QUuid roomId) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << PacketType::ChatMessage;
    out << ChatMessagePacket{m_name, roomId, message};
    if (!m_socket.write(data)) {
        qCritical() << "cannot send message: " << data << m_socket.errorString();
    };
}


