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
    void messageReceived(const QByteArray & data);

public slots:
    void handleClientDisconnected();
    void sendMessageToClient(QUuid clientId, QString& message);
    void broadcast(const QString& message);


protected:
    void incomingConnection(qintptr handle) override;

private:
    void sendMessage(Client* client, const QString& message);


    QString m_welcome_msg;
    QMap<QUuid, Client*> m_clients;
};
#endif //SERVER_H
