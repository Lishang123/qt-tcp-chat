#ifndef QT_TCP_CHAT_ROOMINFO_HPP
#define QT_TCP_CHAT_ROOMINFO_HPP

#include <QUuid>

enum class RoomType {
    Public = 0,
    Self = 1,
    DirectChat = 2,
    Chatgroup = 3,
};


struct UserInfo {
    QString username;
    bool isOnline;
};

inline QDataStream& operator<<(QDataStream& stream, const UserInfo& userInfo) {
    stream  << userInfo.username << userInfo.isOnline;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, UserInfo& userInfo) {
    stream  >> userInfo.username >> userInfo.isOnline;
    return stream;
}


struct RoomInfo {
    RoomType roomType;
    QUuid roomId;
    QString roomName;
    QMap<QUuid,UserInfo> userInfos;
    // uint16_t unreadCount;
    // QList<QString> usernames;
};

inline QDataStream& operator<<(QDataStream& stream, const RoomInfo& roomInfo) {
    stream  << roomInfo.roomType << roomInfo.roomId << roomInfo.roomName << roomInfo.userInfos; // << roomInfo.usernames;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, RoomInfo& roomInfo) {
    stream >> roomInfo.roomType  >> roomInfo.roomId >> roomInfo.roomName >> roomInfo.userInfos; // >> roomInfo.usernames;
    return stream;
}

struct MessageInfo {
    uint32_t messageId;
    QString text;
};


struct ChatHistoryInfo {

};

#endif //QT_TCP_CHAT_ROOMINFO_HPP
