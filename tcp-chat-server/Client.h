#ifndef TCP_CHAT_SERVER_CLIENT_HPP
#define TCP_CHAT_SERVER_CLIENT_HPP
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QRunnable>
#include <QList>


class Client : public QObject{

    Q_OBJECT

public:

    explicit Client(QObject *parent = nullptr, qintptr socketDescriptor = 0);

    void start();

signals:
    void disconnected();
    void dataReceived(const QByteArray & data);
    void finished();

private slots:

    //void disconnected();
    void readyRead();
    void sendMessage(const QByteArray& message);

public:
    [[nodiscard]] QTcpSocket * getSocket() const {
        return m_socket;
    }

private:
    QTcpSocket* m_socket;
    qintptr m_socketDescriptor;

};



#endif //TCP_CHAT_SERVER_CLIENT_HPP
