#ifndef QT_TCP_CHAT_CHATROOM_HPP
#define QT_TCP_CHAT_CHATROOM_HPP
#include <QStandardItemModel>
#include <QUuid>
#include <QTimer>
#include <QDataStream>

#include "../common/ChatRoomInfo.hpp"
#include "../common/ChatMessagePacket.hpp"

struct ChatMessagePacket;
class ChatHistoryManager;

class ChatRoom : public QObject {
Q_OBJECT

public:
    explicit ChatRoom(QObject* parent = nullptr);;
    ChatRoom(QUuid id, QString roomName, uint16_t unreadCount, std::shared_ptr<ChatHistoryManager> chatHistoryManager);

    void addMessage(const ChatMessagePacket &chatMsg);

    [[nodiscard]] QUuid getRoomId() const {
        return m_roomId;
    }

    [[nodiscard]] QString getRoomName() const {
        return m_roomName;
    }

    [[nodiscard]] QStandardItemModel* getChatModel() {
        return &m_chatModel;
    }

    [[nodiscard]] const QStandardItemModel* getChatModel() const {
        return &m_chatModel;
    }

    [[nodiscard]] u_int16_t getUnreadCount() const {
        return m_unreadCount;
    }

    void incrementUnreadCount() {
        m_unreadCount++;
    }

    void clearUnread() {
        m_unreadCount = 0;
    }

    void setRoomId(const QUuid& m_room_id) {
        m_roomId = m_room_id;
    }

    void setRoomName(const QString& m_room_name) {
        m_roomName = m_room_name;
    }

    void setUnreadCount(u_int16_t m_unread_count) {
        m_unreadCount = m_unread_count;
    }

    void setHistorySaver(std::shared_ptr<ChatHistoryManager> chatHistorySaver) {
        m_chatHistoryManager = chatHistorySaver;
    }

    bool loadHistory();

    bool exportHistoryJSON(const QString &filepath);
    bool exportHistoryTXT(const QString &filepath);
    bool exportHistoryHTML(const QString &filepath);
    bool exportHistoryPDF(const QString &filepath);

    RoomType getRoomType() {
        return m_roomType;
    };

    void setChatMessages(const QList<ChatMessage>& chatMessages) {
        m_messages = chatMessages;
    }

    const QList<ChatMessage>& getChatMessages() const {
        return m_messages;
    }

private slots:
    void saveHistory();

private:
    void init();

    RoomType m_roomType;
    QUuid m_roomId;
    QString m_roomName;
    QList<ChatMessage> m_messages;
    QStandardItemModel m_chatModel;
    u_int16_t m_unreadCount = 0;

    std::shared_ptr<ChatHistoryManager> m_chatHistoryManager;
    QTimer m_timer;
    bool m_historyModifed = false;
};


inline QDataStream& operator<<(QDataStream& stream, const ChatRoom& chatRoom) {
    stream << chatRoom.getRoomId();
    stream << chatRoom.getRoomName();
    stream << chatRoom.getUnreadCount();
    stream << chatRoom.getChatMessages();
    auto chatModel = chatRoom.getChatModel();
    for (int rowIdx = 0; rowIdx < chatModel->rowCount(); rowIdx++) {
        QStandardItem* item = chatModel->item(rowIdx);
        stream << item->text();
    }
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, ChatRoom& chatRoom) {
    QUuid roomId;
    QString roomName;
    uint16_t unreadCount;
    QList<ChatMessage> messages;
    stream >> roomId;
    stream >> roomName;
    stream >> unreadCount;
    stream >> messages;
    chatRoom.setRoomId(roomId);
    chatRoom.setRoomName(roomName);
    chatRoom.setUnreadCount(unreadCount);
    chatRoom.setChatMessages(messages);
    auto chatModel = chatRoom.getChatModel();
    while (!stream.atEnd()) {
        QString text;
        stream >> text;
        chatModel->appendRow(new QStandardItem(text));
    }
    return stream;
}

#endif //QT_TCP_CHAT_CHATROOM_HPP
