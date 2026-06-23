#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDebug>
#include <QThread>

#include "ChatRoom.hpp"
#include "Client.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    void setWelcome_msg(const QString &newWelcome_msg);

    size_t getClientsCount();

    void closeServer();

signals:
    void clientChanged();
    void clientConnected(QUuid clientId);
    void clientDisconnected(QUuid clientId);
    void messageReceived(QUuid clientId, const QByteArray & data);

public slots:
    void handleClientDisconnected();
    void handleLoginSuccess(QUuid userId, const QString& username, const QMap<QUuid, QString>& users, QList<RoomInfo>& roomInfos);
    void handleLoginFailed(QUuid userId, const QString& errorMsg);
    // void handleUserRemoved(QUuid clientId);
    void sendMessageToRoom(ChatRoom& chatRoom, const ChatMessagePacket &packet);
    void broadcast(const ChatMessagePacket& packet);
    void onDataReceived(const QByteArray & data);
    void sendRoomInfo(QUuid userId, const RoomInfo& roomInfo);

protected:
    void incomingConnection(qintptr handle) override;

private:
    void sendData(const QByteArray &data);
    void sendData(Client* client, const QByteArray &data);
    void removeClient(Client* client);


    QString m_welcome_msg;
    QMap<QUuid, Client*> m_clients;
};
#endif //SERVER_H
