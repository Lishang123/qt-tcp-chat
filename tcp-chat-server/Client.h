#ifndef TCP_CHAT_SERVER_CLIENT_HPP
#define TCP_CHAT_SERVER_CLIENT_HPP
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QRunnable>
#include <QList>
#include <QUuid>
#include "../common/TcpFraming.hpp"

/**
 * @brief Server-side wrapper around a connected TCP socket.
 */
class Client : public QObject{

    Q_OBJECT

public:

    /**
     * @brief Creates a client for an accepted socket descriptor.
     */
    explicit Client(QObject *parent = nullptr, qintptr socketDescriptor = 0);

    /**
     * @brief Initializes the QTcpSocket and connects socket signals.
     */
    void start();

    /**
     * @brief Assigns the logical chat user id for this connection.
     */
    void setClientId(QUuid clientId) {
        m_clientId = clientId;
    }

    /**
     * @brief Returns the logical chat user id for this connection.
     */
    QUuid getClientId() {
        return m_clientId;
    }

    /**
     * @brief Sends one framed application payload to the socket.
     */
    void sendMessage(const QByteArray& message);


signals:
    void disconnected();
    void dataReceived(const QByteArray & data);
    void finished();

private slots:

    /**
     * @brief Reads available bytes and emits complete framed payloads.
     */
    void readyRead();

public:
    /**
     * @brief Returns the underlying TCP socket.
     */
    [[nodiscard]] QTcpSocket * getSocket() const {
        return m_socket;
    }

private:
    QUuid m_clientId;
    QTcpSocket* m_socket;
    qintptr m_socketDescriptor;
    QByteArray m_receiveBuffer;

};



#endif //TCP_CHAT_SERVER_CLIENT_HPP
