#ifndef QT_TCP_CHAT_ROOMINFO_HPP
#define QT_TCP_CHAT_ROOMINFO_HPP

#include <QUuid>

enum class RoomType {
    Public = 0,
    Self = 1,
    DirectChat = 2,
    Chatgroup = 3,
};


/**
 * @brief Lightweight user metadata shared between server and client.
 */
struct UserInfo {
    QString username;
    bool isOnline;
};

/**
 * @brief Serializes user metadata into a Qt data stream.
 * @param stream Destination stream.
 * @param userInfo User information to serialize.
 * @return The destination stream.
 */
inline QDataStream& operator<<(QDataStream& stream, const UserInfo& userInfo) {
    stream  << userInfo.username << userInfo.isOnline;
    return stream;
}

/**
 * @brief Deserializes user metadata from a Qt data stream.
 * @param stream Source stream.
 * @param userInfo User information to populate.
 * @return The source stream.
 */
inline QDataStream& operator>>(QDataStream& stream, UserInfo& userInfo) {
    stream  >> userInfo.username >> userInfo.isOnline;
    return stream;
}


/**
 * @brief Public room metadata sent to clients.
 */
struct RoomInfo {
    RoomType roomType;
    QUuid roomId;
    QString roomName;
    QMap<QUuid,UserInfo> userInfos;
    QUuid creatorId;
    // uint16_t unreadCount;
    // QList<QString> usernames;
};

/**
 * @brief Serializes room metadata into a Qt data stream.
 * @param stream Destination stream.
 * @param roomInfo Room information to serialize.
 * @return The destination stream.
 */
inline QDataStream& operator<<(QDataStream& stream, const RoomInfo& roomInfo) {
    stream  << roomInfo.roomType << roomInfo.roomId << roomInfo.roomName << roomInfo.userInfos << roomInfo.creatorId; // << roomInfo.usernames;
    return stream;
}

/**
 * @brief Deserializes room metadata from a Qt data stream.
 * @param stream Source stream.
 * @param roomInfo Room information to populate.
 * @return The source stream.
 */
inline QDataStream& operator>>(QDataStream& stream, RoomInfo& roomInfo) {
    stream >> roomInfo.roomType  >> roomInfo.roomId >> roomInfo.roomName >> roomInfo.userInfos >> roomInfo.creatorId; // >> roomInfo.usernames;
    return stream;
}


#endif //QT_TCP_CHAT_ROOMINFO_HPP
