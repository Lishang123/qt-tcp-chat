#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QDebug>
#include <QThread>

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

private slots:
    void clientDisconnected();
    void broadcast(const QByteArray& data);

protected:
    void incomingConnection(qintptr handle) override;

private:
    void sendMessage(Client* client, const QByteArray& message);

    QString m_welcome_msg;
    QList<Client*> m_clients;
};

#endif //SERVER_H
