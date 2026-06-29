#ifndef QT_TCP_CHAT_CHATMESSAGE_HPP
#define QT_TCP_CHAT_CHATMESSAGE_HPP

#include <QUuid>
#include <QDateTime>

struct ChatMessage {
    QUuid messageId;
    QDateTime timestamp;
    QString senderName;
    QString text;

    QString getMessage() const {
        return strSenderName() + strTimeStamp() + strMessage();
    }

    // Houcai [2026.1.3 2:30pm] : Hello!
private:
    QString strSenderName() const {
        return senderName + ' ';
    }

    QString strTimeStamp() const {
        return '[' + timestamp.toString("yyyy-MM-dd HH:mm:ss") + ']' + ' ';
    }

    QString strMessage() const {
        return text;
    }
};


struct ChatMessagePacket : ChatMessage {
    QUuid roomId;
    QUuid senderId;
};

inline QDataStream& operator<<(QDataStream& stream, const ChatMessagePacket& messagePacket) {
    stream  << messagePacket.roomId
            << messagePacket.senderId
            << messagePacket.messageId
            << messagePacket.timestamp
            << messagePacket.senderName
            << messagePacket.text;
    return stream;
}
inline QDataStream& operator>>(QDataStream& stream, ChatMessagePacket& messagePacket) {
    stream  >> messagePacket.roomId
            >> messagePacket.senderId
            >> messagePacket.messageId
            >> messagePacket.timestamp
            >> messagePacket.senderName
            >> messagePacket.text;
    return stream;
}



#endif //QT_TCP_CHAT_CHATMESSAGE_HPP
