#ifndef QT_TCP_CHAT_CHATROOM_HPP
#define QT_TCP_CHAT_CHATROOM_HPP
#include <QStandardItemModel>
#include <QUuid>
#include <QTimer>
#include <QDataStream>
#include <cstdint>

#include "ChatModel.hpp"
#include "../common/ChatRoomInfo.hpp"
#include "../common/ChatMessagePacket.hpp"

struct ChatMessagePacket;
class ChatHistoryManager;

/**
 * @brief Client-side room state, message model, and history persistence hooks.
 */
class ChatRoom : public QObject {
Q_OBJECT

public:
    /**
     * @brief Creates an empty room object.
     */
    explicit ChatRoom(QObject* parent = nullptr);;

    /**
     * @brief Creates a room with initial metadata and a history manager.
     */
    ChatRoom(QUuid id, QString roomName, RoomType roomType, uint16_t unreadCount, std::shared_ptr<ChatHistoryManager> chatHistoryManager);

    /**
     * @brief Adds a message to the room and schedules history saving.
     */
    void addMessage(const ChatMessagePacket &chatMsg);

    /**
     * @brief Returns the room id.
     */
    [[nodiscard]] QUuid getRoomId() const {
        return m_roomId;
    }

    /**
     * @brief Returns the room display name.
     */
    [[nodiscard]] QString getRoomName() const {
        return m_roomName;
    }

    /**
     * @brief Returns the mutable chat model for this room.
     */
    [[nodiscard]] ChatModel* getChatModel() {
        return &m_chatModel;
    }

    /**
     * @brief Returns the immutable chat model for this room.
     */
    [[nodiscard]] const ChatModel* getChatModel() const {
        return &m_chatModel;
    }

    /**
     * @brief Returns the unread message count.
     */
    [[nodiscard]] uint16_t getUnreadCount() const {
        return m_unreadCount;
    }

    /**
     * @brief Increments the unread message count.
     */
    void incrementUnreadCount() {
        m_unreadCount++;
    }

    /**
     * @brief Clears the unread message count.
     */
    void clearUnread() {
        m_unreadCount = 0;
    }

    /**
     * @brief Updates the room id.
     */
    void setRoomId(const QUuid& m_room_id) {
        m_roomId = m_room_id;
    }

    /**
     * @brief Updates the room display name.
     */
    void setRoomName(const QString& m_room_name) {
        m_roomName = m_room_name;
    }

    /**
     * @brief Sets the unread message count.
     */
    void setUnreadCount(uint16_t m_unread_count) {
        m_unreadCount = m_unread_count;
    }

    /**
     * @brief Assigns the history manager used for load/save/export.
     */
    void setHistorySaver(std::shared_ptr<ChatHistoryManager> chatHistorySaver) {
        m_chatHistoryManager = chatHistorySaver;
    }

    /**
     * @brief Loads persisted chat history into the room.
     */
    bool loadHistory();

    /**
     * @brief Exports the room history as JSON.
     */
    bool exportHistoryJSON(const QString &filepath);

    /**
     * @brief Exports the room history as plain text.
     */
    bool exportHistoryTXT(const QString &filepath);

    /**
     * @brief Exports the room history as HTML.
     */
    bool exportHistoryHTML(const QString &filepath);

    /**
     * @brief Exports the room history as PDF.
     */
    bool exportHistoryPDF(const QString &filepath);

    /**
     * @brief Returns the room type.
     */
    RoomType getRoomType() {
        return m_roomType;
    };

    /**
     * @brief Replaces the room member metadata.
     */
    void setUserInfos(QMap<QUuid, UserInfo> userInfos) {
        m_userInfos = std::move(userInfos);
    }

    /**
     * @brief Returns room member metadata.
     */
    const QMap<QUuid, UserInfo>& getUserInfos() const {
        return m_userInfos;
    }

    /**
     * @brief Updates one room member's online status.
     */
    bool setUserOnlineStatus(const QUuid &userId, bool online) {
        if (!m_userInfos.contains(userId)) {
            return false;
        }
        m_userInfos[userId].isOnline = online;
        return true;
    }

    /**
     * @brief Returns only message items, excluding separators.
     */
    const QList<ChatMessage> getChatMessages() const {
        return m_chatModel.getMessages();
    }

signals:
    void historySaved();

private slots:
    /**
     * @brief Saves modified history when the debounce timer expires.
     */
    void saveHistory();

private:
    /**
     * @brief Configures the history save timer.
     */
    void init();

    RoomType m_roomType;
    QUuid m_roomId;
    QString m_roomName;
    ChatModel m_chatModel;
    uint16_t m_unreadCount = 0;
    QMap<QUuid, UserInfo> m_userInfos;

    std::shared_ptr<ChatHistoryManager> m_chatHistoryManager;
    QTimer m_timer;
    bool m_historyModifed = false;
};


/**
 * @brief Serializes client-side room history and unread state.
 */
inline QDataStream& operator<<(QDataStream& stream, const ChatRoom& chatRoom) {
    stream << chatRoom.getRoomId();
    stream << chatRoom.getRoomName();
    stream << chatRoom.getUnreadCount();
    auto chatHistory = chatRoom.getChatModel()->getChatItems();
    stream << chatHistory;
    return stream;
}

/**
 * @brief Deserializes client-side room history and unread state.
 */
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
