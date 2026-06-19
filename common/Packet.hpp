#ifndef TCP_CHAT_SERVER_MESSAGE_HPP
#define TCP_CHAT_SERVER_MESSAGE_HPP
#include <QUuid>

enum PacketType : quint8 {
    LoginRequest = 1,
    LoginSuccess = 11,
    LoginFail = 12,

    LogoutRequest = 2,

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
    QString username;
    quint8 roomID;
    QString welcomeMsg;
};

inline QDataStream& operator<<(QDataStream& stream, const LoginSuccessPacket& loginPacket) {
    stream << loginPacket.username << loginPacket.roomID << loginPacket.welcomeMsg;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, LoginSuccessPacket& loginPacket) {
    stream >> loginPacket.username >> loginPacket.roomID >> loginPacket.welcomeMsg;
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

struct ChatMessagePacket {
    QString senderName;
    QString recipientName;
    QString message;
};

inline QDataStream& operator<<(QDataStream& stream, const ChatMessagePacket& messagePacket) {
    stream  << messagePacket.senderName << messagePacket.recipientName << messagePacket.message;
    return stream;
}
inline QDataStream& operator>>(QDataStream& stream, ChatMessagePacket& messagePacket) {
    stream >> messagePacket.senderName >> messagePacket.recipientName >> messagePacket.message;
    return stream;
}



#endif //TCP_CHAT_SERVER_MESSAGE_HPP
