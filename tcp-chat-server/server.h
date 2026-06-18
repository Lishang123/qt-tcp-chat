#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);

    void setWelcome_msg(const QString &newWelcome_msg);

    int getClientsCount();

    void close();

signals:
    void clientChanged();

private slots:
    void disconnected();
    void readyRead();

protected:
    void incomingConnection(qintptr handle) override;

private:
    QString m_welcome_msg;
    QList<QTcpSocket*> m_sockets;
};

#endif //SERVER_H
