#ifndef TCP_CHAT_SERVER_MESSAGE_HPP
#define TCP_CHAT_SERVER_MESSAGE_HPP
#include <QUuid>
#include <QMap>

#include "ChatRoomInfo.hpp"

/**
 * @brief Application-level packet identifiers sent before each payload.
 */
enum PacketType : quint8 {
    LoginRequest = 1,
    LoginSuccess = 11,
    LoginFail = 12,
    NotifyLogin = 13,
    NotifyLogout = 14,

    RoomRequest = 2,
    RoomAcquired = 21,
    RoomDeleteRequest = 22,
    RoomDeleted = 23,

    ChatMessagePkt = 3,
};

/**
 * @brief Login request sent by a client to register or resume a user session.
 */
struct LoginRequestPacket {
    QString username;
    quint8 roomID;
    QString password;
};

/**
 * @brief Serializes a login request packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LoginRequestPacket& loginPacket) {
    stream << loginPacket.username << loginPacket.roomID << loginPacket.password;
    return stream;
}

/**
 * @brief Deserializes a login request packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LoginRequestPacket& loginPacket) {
    stream >> loginPacket.username >> loginPacket.roomID >> loginPacket.password;
    return stream;
}

/**
 * @brief Login response containing the assigned user id and initial room state.
 */
struct LoginSuccessPacket {
    QUuid userId;
    QString username;
    QList<RoomInfo> roomInfos; // chat rooms
    QMap<QUuid, UserInfo> contacts; // users without chat rooms
    QString welcomeMsg;
};

/**
 * @brief Serializes a successful login response packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LoginSuccessPacket& loginPacket) {
    stream << loginPacket.userId << loginPacket.username << loginPacket.roomInfos  << loginPacket.contacts << loginPacket.welcomeMsg ;
    return stream;
}

/**
 * @brief Deserializes a successful login response packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LoginSuccessPacket& loginPacket) {
    stream >> loginPacket.userId >> loginPacket.username >> loginPacket.roomInfos >> loginPacket.contacts >> loginPacket.welcomeMsg ;
    return stream;
}




/**
 * @brief Login failure response with a user-facing error message.
 */
struct LoginFailedPacket {
    QString errorMsg;
};

/**
 * @brief Serializes a login failure packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LoginFailedPacket& loginPacket) {
    stream  << loginPacket.errorMsg;
    return stream;
}

/**
 * @brief Deserializes a login failure packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LoginFailedPacket& loginPacket) {
    stream  >> loginPacket.errorMsg;
    return stream;
}


/**
 * @brief Notification broadcast when a user comes online.
 */
struct LoginNotificationPacket {
    QUuid userId;
    QString username;
};

/**
 * @brief Serializes a login notification packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LoginNotificationPacket& loginPacket) {
    stream  << loginPacket.userId << loginPacket.username;
    return stream;
}

/**
 * @brief Deserializes a login notification packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LoginNotificationPacket& loginPacket) {
    stream  >> loginPacket.userId >> loginPacket.username;
    return stream;
}


/**
 * @brief Notification broadcast when a user goes offline.
 */
struct LogoutNotificationPacket {
    QUuid userId;
};

/**
 * @brief Serializes a logout notification packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LogoutNotificationPacket& loginPacket) {
    stream  << loginPacket.userId;
    return stream;
}

/**
 * @brief Deserializes a logout notification packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LogoutNotificationPacket& loginPacket) {
    stream  >> loginPacket.userId;;
    return stream;
}


/**
 * @brief Request to create or acquire a room with the specified members.
 */
struct RoomRequestPacket {
    RoomType roomType;
    QList<QUuid> memberIds;
    QString roomName;
};

/**
 * @brief Serializes a room request packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const RoomRequestPacket& roomPacket) {
    stream  << roomPacket.roomType << roomPacket.memberIds << roomPacket.roomName;
    return stream;
}

/**
 * @brief Deserializes a room request packet.
 */
inline QDataStream& operator>>(QDataStream& stream, RoomRequestPacket& roomPacket) {
    stream  >> roomPacket.roomType >> roomPacket.memberIds >> roomPacket.roomName;
    return stream;
}

/**
 * @brief Response containing room metadata after a room is acquired or created.
 */
struct RoomInfoPacket {
    RoomInfo roomInfo;
};

/**
 * @brief Serializes a room info packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const RoomInfoPacket& roomInfo) {
    stream  << roomInfo.roomInfo;
    return stream;
}

/**
 * @brief Deserializes a room info packet.
 */
inline QDataStream& operator>>(QDataStream& stream, RoomInfoPacket& roomInfo) {
    stream  >> roomInfo.roomInfo;
    return stream;
}

/**
 * @brief Request to delete a chat group room.
 */
struct RoomDeleteRequestPacket {
    QUuid roomId;
};

/**
 * @brief Serializes a room delete request packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const RoomDeleteRequestPacket& roomDeletePacket) {
    stream << roomDeletePacket.roomId;
    return stream;
}

/**
 * @brief Deserializes a room delete request packet.
 */
inline QDataStream& operator>>(QDataStream& stream, RoomDeleteRequestPacket& roomDeletePacket) {
    stream >> roomDeletePacket.roomId;
    return stream;
}

/**
 * @brief Notification that a room was deleted.
 */
struct RoomDeletedPacket {
    QUuid roomId;
};

/**
 * @brief Serializes a room deleted notification packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const RoomDeletedPacket& roomDeletedPacket) {
    stream << roomDeletedPacket.roomId;
    return stream;
}

/**
 * @brief Deserializes a room deleted notification packet.
 */
inline QDataStream& operator>>(QDataStream& stream, RoomDeletedPacket& roomDeletedPacket) {
    stream >> roomDeletedPacket.roomId;
    return stream;
}



/**
 * @brief Logout request carrying the user name of the disconnecting client.
 */
struct LogoutRequestPacket {
    QString username;
};

/**
 * @brief Serializes a logout request packet.
 */
inline QDataStream& operator<<(QDataStream& stream, const LogoutRequestPacket& logoutPacket) {
    stream << logoutPacket.username;
    return stream;
}

/**
 * @brief Deserializes a logout request packet.
 */
inline QDataStream& operator>>(QDataStream& stream, LogoutRequestPacket& logoutPacket) {
    stream >> logoutPacket.username;
    return stream;
}




#endif //TCP_CHAT_SERVER_MESSAGE_HPP
