#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDebug>
#include <QThread>

#include "ChatRoom.hpp"
#include "Client.h"

/**
 * @brief TCP server that owns connected clients and routes serialized packets.
 */
class Server : public QTcpServer
{
    Q_OBJECT
public:
    /**
     * @brief Constructs the chat TCP server.
     * @param parent Optional Qt parent object.
     */
    explicit Server(QObject *parent = nullptr);

    /**
     * @brief Sets the message sent to clients after successful login.
     * @param newWelcome_msg Welcome message text.
     */
    void setWelcome_msg(const QString &newWelcome_msg);

    /**
     * @brief Returns the number of currently connected client sockets.
     */
    size_t getClientsCount();

    /**
     * @brief Closes all client sockets and stops accepting connections.
     */
    void closeServer();

signals:
    void clientChanged();
    void clientConnected(QUuid clientId);
    void clientDisconnected(QUuid clientId);
    void messageReceived(QUuid clientId, const QByteArray & data);

public slots:
    /**
     * @brief Handles cleanup and notifications when a client socket disconnects.
     */
    void handleClientDisconnected();

    /**
     * @brief Sends a successful login response and notifies other clients.
     */
    void handleLoginSuccess(QUuid userId, const QString& username, const QMap<QUuid, UserInfo>& users, QList<RoomInfo>& roomInfos);

    /**
     * @brief Sends a failed login response and disconnects the client.
     */
    void handleLoginFailed(QUuid userId, const QString& errorMsg);
    // void handleUserRemoved(QUuid clientId);

    /**
     * @brief Sends a chat message to all online users in a room.
     */
    void sendMessageToRoom(ChatRoom& chatRoom, ChatMessagePacket &packet);

    /**
     * @brief Sends a chat message packet to every connected client.
     */
    void broadcast(ChatMessagePacket& packet);

    /**
     * @brief Emits received application payloads with the sending client id.
     */
    void onDataReceived(const QByteArray & data);

    /**
     * @brief Sends room metadata to one connected user.
     */
    void sendRoomInfo(QUuid userId, const RoomInfo& roomInfo);

    /**
     * @brief Sends a room deletion notification to one connected user.
     */
    void sendRoomDeleted(QUuid userId, const RoomDeletedPacket& roomDeletedPacket);

    /**
     * @brief Replaces a temporary connection id with a registered user id.
     */
    void changeClientId(QUuid clientId, QUuid newClientId);

protected:
    /**
     * @brief Accepts an incoming TCP connection and wraps it in a Client object.
     */
    void incomingConnection(qintptr handle) override;

private:
    /**
     * @brief Sends a serialized payload to every connected client.
     */
    void sendData(const QByteArray &data);

    /**
     * @brief Sends a serialized payload to one client.
     */
    void sendData(Client* client, const QByteArray &data);

    /**
     * @brief Removes a client from the connection map and schedules deletion.
     */
    void removeClient(Client* client);


    QString m_welcome_msg;
    QMap<QUuid, Client*> m_clients;
};
#endif //SERVER_H
