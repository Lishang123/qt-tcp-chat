#ifndef TCP_CHAT_CLIENT_CHATROOM_HPP
#define TCP_CHAT_CLIENT_CHATROOM_HPP
#include <iostream>
#include <ranges>
#include <qobject.h>
#include "../common/Packet.hpp"
#include "../common/ChatMessagePacket.hpp"
#include "client.h"

struct User {
    User(const QUuid &user_id, const QString &username)
        : user_id(user_id),
          username(username){}

    std::string toString() {
        std::ostringstream os;
        os <<  "UUID: " <<  user_id.data1 << std::endl;
        os <<  "username: " << username.toStdString() << std::endl;
        return os.str();
    }

    QUuid user_id;
    QString username;
};

struct Users : public QMap<QUuid,std::shared_ptr<User>>{
    QMap<QUuid, QString> getUserInfos() {
        QMap<QUuid, QString> userInfos;
        foreach(const auto& user, values()) {
            userInfos.insert(user->user_id, user->username);
        }
        return userInfos;
    }
};


class ChatRoom {

public:
    explicit ChatRoom(RoomType roomType, QUuid roomId, const QString& roomName);

    size_t getClientsCount();

    void addUser(QUuid clientId, std::shared_ptr<User> user);
    bool isEmpty() const {
        return m_users.isEmpty();
    }

    [[nodiscard]] const QString & getRoomName() const {
        return m_roomName;
    }

    [[nodiscard]] const QUuid & getRoomId() const {
        return m_roomID;
    }

    [[nodiscard]] const RoomType & getRoomType() const {
        return m_roomType;
    }

    void setWelcomeMsg(const QString &newWelcomeMsg);
    RoomInfo getRoomInfo();
    void removeUser(QUuid userId);

private:

public:
    [[nodiscard]] const Users& getRoomUsers() const {
        return m_users;
    }

private:
    RoomType m_roomType;
    QString m_welcome_msg;
    QString m_roomName;
    QUuid m_roomID;
    Users m_users;
};


#endif //TCP_CHAT_CLIENT_CHATROOM_HPP
