#ifndef QT_TCP_CHAT_CHATMESSAGE_HPP
#define QT_TCP_CHAT_CHATMESSAGE_HPP

#include <QUuid>
#include <QDateTime>

struct ChatMessagePacket {
    QUuid roomId;
    QUuid senderId;
    QUuid messageId;
    QDateTime timestamp;
    QString senderName;
    QString text;

    QString getMessage() const {
        return strName() + strTimeStamp() + strMessage();
    }
// Houcai [2026.1.3 2:30pm] : Hello!
private:

    QString strName() const {
        return senderName + ' ';
    }

    QString strTimeStamp() const {
        return '[' + timestamp.toString("yyyy-MM-dd HH:mm:ss") + ']' + ' ';
    }

    QString strMessage() const {
        return text;
    }
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
