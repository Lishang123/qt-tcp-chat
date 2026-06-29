#ifndef QT_TCP_CHAT_CHATMESSAGE_HPP
#define QT_TCP_CHAT_CHATMESSAGE_HPP

#include <QUuid>
#include <QDateTime>

struct ChatMessage {
    QUuid senderId;
    QUuid messageId;
    QDateTime timestamp;
    QString senderName;
    QString text;
    bool outgoing;

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


inline QDataStream& operator<<(QDataStream& stream, const ChatMessage& chatMsg) {
    stream  << chatMsg.senderId
            << chatMsg.messageId
            << chatMsg.timestamp
            << chatMsg.senderName
            << chatMsg.text
            << chatMsg.outgoing;

    return stream;
}
inline QDataStream& operator>>(QDataStream& stream, ChatMessage& chatMsg) {
    stream  >> chatMsg.senderId
            >> chatMsg.messageId
            >> chatMsg.timestamp
            >> chatMsg.senderName
            >> chatMsg.text
            >> chatMsg.outgoing;
    return stream;
}


struct ChatMessagePacket : ChatMessage {
    QUuid roomId;
};

inline QDataStream& operator<<(QDataStream& stream, const ChatMessagePacket& messagePacket) {
    stream  << messagePacket.roomId
            << messagePacket.senderId
            << messagePacket.messageId
            << messagePacket.timestamp
            << messagePacket.senderName
            << messagePacket.text
            << messagePacket.outgoing;
    return stream;
}
inline QDataStream& operator>>(QDataStream& stream, ChatMessagePacket& messagePacket) {
    stream  >> messagePacket.roomId
            >> messagePacket.senderId
            >> messagePacket.messageId
            >> messagePacket.timestamp
            >> messagePacket.senderName
            >> messagePacket.text
            >> messagePacket.outgoing;
    return stream;
}



#endif //QT_TCP_CHAT_CHATMESSAGE_HPP
