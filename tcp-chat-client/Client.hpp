#ifndef QT_TCP_CHAT_CLIENT_HPP
#define QT_TCP_CHAT_CLIENT_HPP
#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include "../common/Packet.hpp"


class Client : public QObject {
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);
    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);
    void disconnectFromHost();

signals:
    void connected();
    void disconnected();
    void loggedIn(const LoginSuccessPacket& loginSuccessPacket);
    void notifyUserLogin(const LoginNotificationPacket& loginNotificationPacket);
    void notifyUserLogout(const LogoutNotificationPacket& logoutNotificationPacket);
    void loggedOut();
    void messageReceived(const ChatMessagePacket& chatMessagePacket);
    void errorOccured(const QString& errorMessage);

public slots:
    void connectToServer(const QString& address, quint16 port);
    void sendMessage(const QString& message, QUuid roomId);

private slots:

    void onConnected();

    void onDisconnected();

    void OnReadyRead();

    void error(QAbstractSocket::SocketError error);

public:
    [[nodiscard]] QTcpSocket & getSocket() {
        return m_socket;
    }

    [[nodiscard]] const QString & getUserName() {
        return m_name;
    }

private:

    QString m_name;
    QTcpSocket m_socket;

};



#endif //QT_TCP_CHAT_CLIENT_HPP
