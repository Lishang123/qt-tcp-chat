#ifndef QT_TCP_CHAT_CHATMESSAGE_HPP
#define QT_TCP_CHAT_CHATMESSAGE_HPP

#include <QUuid>
#include <QDateTime>

/**
 * @brief Chat message data used by models, history storage, and transport.
 */
struct ChatMessage {
    QUuid senderId;
    QUuid messageId;
    QDateTime timestamp;
    QString senderName;
    QString text;
    bool outgoing;

    /**
     * @brief Builds the display string with sender, timestamp, and message text.
     * @return Human-readable message representation.
     */
    QString getMessage() const {
        return strSenderName() + strTimeStamp() + strMessage();
    }

    // Houcai [2026.1.3 2:30pm] : Hello!
private:
    /**
     * @brief Formats the sender name prefix for display.
     * @return Sender name followed by a space.
     */
    QString strSenderName() const {
        return senderName + ' ';
    }

    /**
     * @brief Formats the message timestamp for display.
     * @return Timestamp enclosed in square brackets and followed by a space.
     */
    QString strTimeStamp() const {
        return '[' + timestamp.toString("yyyy-MM-dd HH:mm:ss") + ']' + ' ';
    }

    /**
     * @brief Formats the message body for display.
     * @return Message text.
     */
    QString strMessage() const {
        return text;
    }
};


/**
 * @brief Serializes a chat message into a Qt data stream.
 * @param stream Destination stream.
 * @param chatMsg Message to serialize.
 * @return The destination stream.
 */
inline QDataStream& operator<<(QDataStream& stream, const ChatMessage& chatMsg) {
    stream  << chatMsg.senderId
            << chatMsg.messageId
            << chatMsg.timestamp
            << chatMsg.senderName
            << chatMsg.text
            << chatMsg.outgoing;

    return stream;
}

/**
 * @brief Deserializes a chat message from a Qt data stream.
 * @param stream Source stream.
 * @param chatMsg Message to populate.
 * @return The source stream.
 */
inline QDataStream& operator>>(QDataStream& stream, ChatMessage& chatMsg) {
    stream  >> chatMsg.senderId
            >> chatMsg.messageId
            >> chatMsg.timestamp
            >> chatMsg.senderName
            >> chatMsg.text
            >> chatMsg.outgoing;
    return stream;
}


/**
 * @brief Network packet carrying a chat message and its target room.
 */
struct ChatMessagePacket : ChatMessage {
    QUuid roomId;
};

/**
 * @brief Serializes a room-targeted chat message packet.
 * @param stream Destination stream.
 * @param messagePacket Packet to serialize.
 * @return The destination stream.
 */
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

/**
 * @brief Deserializes a room-targeted chat message packet.
 * @param stream Source stream.
 * @param messagePacket Packet to populate.
 * @return The source stream.
 */
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
