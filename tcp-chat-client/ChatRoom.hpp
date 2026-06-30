#ifndef QT_TCP_CHAT_CHATROOM_HPP
#define QT_TCP_CHAT_CHATROOM_HPP
#include <QStandardItemModel>
#include <QUuid>
#include <QTimer>
#include <QDataStream>

#include "ChatModel.hpp"
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

    [[nodiscard]] ChatModel* getChatModel() {
        return &m_chatModel;
    }

    [[nodiscard]] const ChatModel* getChatModel() const {
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

    const QList<ChatMessage>& getChatMessages() const {
        return m_chatModel.getMessages();
    }

private slots:
    void saveHistory();

private:
    void init();

    RoomType m_roomType;
    QUuid m_roomId;
    QString m_roomName;
    ChatModel m_chatModel;
    u_int16_t m_unreadCount = 0;

    std::shared_ptr<ChatHistoryManager> m_chatHistoryManager;
    QTimer m_timer;
    bool m_historyModifed = false;
};


inline QDataStream& operator<<(QDataStream& stream, const ChatRoom& chatRoom) {
    stream << chatRoom.getRoomId();
    stream << chatRoom.getRoomName();
    stream << chatRoom.getUnreadCount();
    auto chatHistory = chatRoom.getChatModel()->getChatItems();
    stream << chatHistory;
    return stream;
}

inline QDataStream& operator>>(QDataStream& stream, ChatRoom& chatRoom) {
    QUuid roomId;
    QString roomName;
    uint16_t unreadCount;
    QList<ChatItem> chatHistory;
    stream >> roomId;
    stream >> roomName;
    stream >> unreadCount;
    stream >> chatHistory;
    chatRoom.setRoomId(roomId);
    chatRoom.setRoomName(roomName);
    chatRoom.setUnreadCount(unreadCount);
    auto chatModel = chatRoom.getChatModel();
    chatModel->setChatItems(std::move(chatHistory));
    return stream;
}

#endif //QT_TCP_CHAT_CHATROOM_HPP
