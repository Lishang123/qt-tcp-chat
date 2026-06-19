#ifndef TCP_CHAT_CLIENT_CHATROOM_HPP
#define TCP_CHAT_CLIENT_CHATROOM_HPP
#include <iostream>
#include <ranges>
#include <qobject.h>
#include "../common/Packet.hpp"
#include "client.h"

struct User {
    User(const QUuid &user_id, const QString &username, int room_id)
        : user_id(user_id),
          username(username),
          roomID(room_id) {
    }

    std::string toString() {
        std::ostringstream os;
        os <<  "UUID: " <<  user_id.data1 << std::endl;
        os <<  "username: " << username.toStdString() << std::endl;
        os <<  "roomID: " <<  roomID << std::endl;
        return os.str();
    }

    QUuid user_id;
    QString username;
    quint8 roomID;
};

class ChatRoom : public QObject {
    Q_OBJECT

public:
    explicit ChatRoom(QObject *parent = nullptr);

    size_t getClientsCount();

    void setWelcomeMsg(const QString &newWelcomeMsg);

signals:
    void clientChanged();
    void userAdded(QUuid userId, const QString& username, quint8 roomId);
    void loginFailed(QUuid userId, const QString& errorMsg);
    //void userRemoved(QUuid userId);
    void sendMessageToClient(QUuid senderId, QUuid recipientId, const ChatMessagePacket &packet);
    void broadcast(const ChatMessagePacket &packet);

public slots:
    void handleMessage(QUuid senderId, const QByteArray & data);
    void removeUser(QUuid userId);

private:
    bool handleLoginRequest(QUuid clientId, LoginRequestPacket& packet);
    bool handleChatMessage(QUuid senderId, ChatMessagePacket& packet);
    void addUser(QUuid clientId, const QString & username, quint8 roomId);
    User* findUserbyName(QString &username);

    QString m_welcome_msg;
    QMap<QUuid, User> m_users;

};



#endif //TCP_CHAT_CLIENT_CHATROOM_HPP
