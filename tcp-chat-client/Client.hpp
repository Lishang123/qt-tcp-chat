#ifndef QT_TCP_CHAT_CLIENT_HPP
#define QT_TCP_CHAT_CLIENT_HPP
#include <QObject>
#include <QTcpSocket>
#include <QMessageBox>
#include "../common/Packet.hpp"
#include "../common/ChatMessagePacket.hpp"
#include "../common/TcpFraming.hpp"

struct ChatMessagePacket;

/**
 * @brief Client-side TCP connection that serializes and dispatches chat packets.
 */
class Client : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a disconnected client socket wrapper.
     */
    explicit Client(QObject *parent = nullptr);

    /**
     * @brief Sends login credentials and requested room information to the server.
     */
    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);

    /**
     * @brief Sends a request to create or acquire a chat room.
     */
    void sendRoomRequest(const RoomRequestPacket& roomRequestPacket);

    /**
     * @brief Sends a request to delete a chat group.
     */
    void sendRoomDeleteRequest(const RoomDeleteRequestPacket& roomDeleteRequestPacket);

    /**
     * @brief Disconnects from the server.
     */
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
    void roomAcquired(const RoomInfoPacket& roomAcquiredPacket);
    void roomDeleted(const RoomDeletedPacket& roomDeletedPacket);

public slots:
    /**
     * @brief Opens a TCP connection to the server.
     */
    void connectToServer(const QString& address, quint16 port);

    /**
     * @brief Sends a text message to the specified room.
     */
    void sendMessage(const QString& message, QUuid roomId);

private slots:

    /**
     * @brief Emits the public connected signal after socket connection.
     */
    void onConnected();

    /**
     * @brief Emits the public disconnected signal after socket disconnection.
     */
    void onDisconnected();

    /**
     * @brief Reads available bytes and processes complete framed packets.
     */
    void OnReadyRead();

    /**
     * @brief Converts socket errors into user-facing error messages.
     */
    void error(QAbstractSocket::SocketError error);

    /**
     * @brief Dispatches one serialized application packet by packet type.
     */
    void processPacket(const QByteArray &data);

    /**
     * @brief Writes one framed application payload to the socket.
     */
    bool writePacket(const QByteArray &data);

public:
    /**
     * @brief Returns the underlying TCP socket.
     */
    [[nodiscard]] QTcpSocket & getSocket() {
        return m_socket;
    }

    /**
     * @brief Returns the local user name.
     */
    [[nodiscard]] const QString & getUserName() {
        return m_name;
    }

    /**
     * @brief Stores the authenticated user id assigned by the server.
     */
    void setClientId(QUuid clientId) {
        m_clientId = clientId;
    }

    /**
     * @brief Returns the authenticated user id.
     */
    QUuid getClientId() const {
        return m_clientId;
    }

private:
    QUuid m_clientId;
    QString m_name;
    QTcpSocket m_socket;
    QByteArray m_receiveBuffer;

};



#endif //QT_TCP_CHAT_CLIENT_HPP
