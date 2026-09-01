#ifndef TCP_CHAT_CLIENT_CHATROOM_HPP
#define TCP_CHAT_CLIENT_CHATROOM_HPP
#include <iostream>
#include <ranges>
#include <qobject.h>
#include "../common/Packet.hpp"
#include "../common/ChatMessagePacket.hpp"
#include "Client.h"

/**
 * @brief Server-side user record tracked by the room manager.
 */
struct User {
    /**
     * @brief Creates a user with a stable id and display name.
     */
    User(const QUuid &user_id, const QString &username)
        : user_id(user_id),
          username(username){}

    /**
     * @brief Formats the user for diagnostic output.
     */
    std::string toString() {
        std::ostringstream os;
        os <<  "UUID: " <<  user_id.data1 << std::endl;
        os <<  "username: " << username.toStdString() << std::endl;
        return os.str();
    }

    /**
     * @brief Returns the display name.
     */
    QString getUsername() const {
        return username;
    }

    /**
     * @brief Returns the stable user id.
     */
    QUuid getUserId() {
        return user_id;
    }

    /**
     * @brief Returns whether the user currently has an active session.
     */
    bool isOnline() {
        return onlineStatus;
    }

    /**
     * @brief Updates the user's online status.
     */
    void setOnlineStatus(bool status) {
        onlineStatus = status;
    }

    bool onlineStatus = false;
    QUuid user_id;
    QString username;
};

/**
 * @brief Map of users keyed by id with conversion helpers for packets.
 */
struct Users : public QMap<QUuid,std::shared_ptr<User>>{
    /**
     * @brief Converts server user objects into transferable user metadata.
     */
    QMap<QUuid, UserInfo> getUserInfos() {
        QMap<QUuid, UserInfo> userInfos;
        foreach(const auto& user, values()) {
            userInfos.insert(user->user_id, {user->getUsername(), user->isOnline()});
        }
        return userInfos;
    }
};


/**
 * @brief Server-side room containing users and room metadata.
 */
class ChatRoom {

public:
    /**
     * @brief Creates a room with the supplied type, id, and name.
     */
    explicit ChatRoom(RoomType roomType, QUuid roomId, const QString& roomName);

    /**
     * @brief Returns the number of users currently assigned to the room.
     */
    size_t getClientsCount();

    /**
     * @brief Adds a user to the room.
     */
    void addUser(QUuid clientId, std::shared_ptr<User> user);

    /**
     * @brief Returns whether the room has no users.
     */
    bool isEmpty() const {
        return m_users.isEmpty();
    }

    /**
     * @brief Returns the room display name.
     */
    [[nodiscard]] const QString & getRoomName() const {
        return m_roomName;
    }

    /**
     * @brief Returns the room id.
     */
    [[nodiscard]] const QUuid & getRoomId() const {
        return m_roomID;
    }

    /**
     * @brief Returns the room type.
     */
    [[nodiscard]] const RoomType & getRoomType() const {
        return m_roomType;
    }

    /**
     * @brief Sets the room welcome message.
     */
    void setWelcomeMsg(const QString &newWelcomeMsg);

    /**
     * @brief Builds transferable room metadata for clients.
     */
    RoomInfo getRoomInfo();

    /**
     * @brief Removes a user from the room.
     */
    void removeUser(QUuid userId);

private:

public:
    /**
     * @brief Returns the users assigned to the room.
     */
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
