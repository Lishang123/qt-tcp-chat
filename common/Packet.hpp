#ifndef TCP_CHAT_SERVER_MESSAGE_HPP
#define TCP_CHAT_SERVER_MESSAGE_HPP
#include <QUuid>
#include <QMap>

#include "ChatRoomInfo.hpp"

enum PacketType : quint8 {
    LoginRequest = 1,
    LoginSuccess = 11,
    LoginFail = 12,
    NotifyLogin = 13,
    NotifyLogout = 14,

    RoomRequest = 2,
    RoomAcquired = 21,

    ChatMessage = 3,
};

struct LoginRequestPacket {
    QString username;
    quint8 roomID;
    QString password;
};

inline QDataStream& operator<<(QDataStream& stream, const LoginRequestPacket& loginPacket) {
    stream << loginPacket.username << loginPacket.roomID << loginPacket.password;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LoginRequestPacket& loginPacket) {
    stream >> loginPacket.username >> loginPacket.roomID >> loginPacket.password;
    return stream;
}

struct LoginSuccessPacket {
    QUuid userId;
    QString username;
    QList<RoomInfo> roomInfos; // chat rooms
    QMap<QUuid, UserInfo> contacts; // users without chat rooms
    QString welcomeMsg;
};

inline QDataStream& operator<<(QDataStream& stream, const LoginSuccessPacket& loginPacket) {
    stream << loginPacket.userId << loginPacket.username << loginPacket.roomInfos  << loginPacket.contacts << loginPacket.welcomeMsg ;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LoginSuccessPacket& loginPacket) {
    stream >> loginPacket.userId >> loginPacket.username >> loginPacket.roomInfos >> loginPacket.contacts >> loginPacket.welcomeMsg ;
    return stream;
}




struct LoginFailedPacket {
    QString errorMsg;
};

inline QDataStream& operator<<(QDataStream& stream, const LoginFailedPacket& loginPacket) {
    stream  << loginPacket.errorMsg;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LoginFailedPacket& loginPacket) {
    stream  >> loginPacket.errorMsg;
    return stream;
}


struct LoginNotificationPacket {
    QUuid userId;
    QString username;
};

inline QDataStream& operator<<(QDataStream& stream, const LoginNotificationPacket& loginPacket) {
    stream  << loginPacket.userId << loginPacket.username;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LoginNotificationPacket& loginPacket) {
    stream  >> loginPacket.userId >> loginPacket.username;
    return stream;
}


struct LogoutNotificationPacket {
    QUuid userId;
};

inline QDataStream& operator<<(QDataStream& stream, const LogoutNotificationPacket& loginPacket) {
    stream  << loginPacket.userId;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LogoutNotificationPacket& loginPacket) {
    stream  >> loginPacket.userId;;
    return stream;
}


struct RoomRequestPacket {
    RoomType roomType;
    QList<QUuid> memberIds;
    QString roomName;
};

inline QDataStream& operator<<(QDataStream& stream, const RoomRequestPacket& roomPacket) {
    stream  << roomPacket.roomType << roomPacket.memberIds << roomPacket.roomName;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, RoomRequestPacket& roomPacket) {
    stream  >> roomPacket.roomType >> roomPacket.memberIds >> roomPacket.roomName;
    return stream;
}

struct RoomInfoPacket {
    RoomInfo roomInfo;
};

inline QDataStream& operator<<(QDataStream& stream, const RoomInfoPacket& roomInfo) {
    stream  << roomInfo.roomInfo;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, RoomInfoPacket& roomInfo) {
    stream  >> roomInfo.roomInfo;
    return stream;
}



struct LogoutRequestPacket {
    QString username;
};

inline QDataStream& operator<<(QDataStream& stream, const LogoutRequestPacket& logoutPacket) {
    stream << logoutPacket.username;
    return stream;
}
inline QDataStream& operator>>(QDataStream& stream, LogoutRequestPacket& logoutPacket) {
    stream >> logoutPacket.username;
    return stream;
}




#endif //TCP_CHAT_SERVER_MESSAGE_HPP
