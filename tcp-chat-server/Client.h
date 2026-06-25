#ifndef TCP_CHAT_SERVER_CLIENT_HPP
#define TCP_CHAT_SERVER_CLIENT_HPP
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QRunnable>
#include <QList>
#include <QUuid>

class Client : public QObject{

    Q_OBJECT

public:

    explicit Client(QObject *parent = nullptr, qintptr socketDescriptor = 0);

    void start();

    void setClientId(QUuid clientId) {
        m_clientId = clientId;
    }

    QUuid getClientId() {
        return m_clientId;
    }


signals:
    void disconnected();
    void dataReceived(const QByteArray & data);
    void finished();

private slots:

    void readyRead();
    void sendMessage(const QByteArray& message);

public:
    [[nodiscard]] QTcpSocket * getSocket() const {
        return m_socket;
    }

private:
    QUuid m_clientId;
    QTcpSocket* m_socket;
    qintptr m_socketDescriptor;

};



#endif //TCP_CHAT_SERVER_CLIENT_HPP
