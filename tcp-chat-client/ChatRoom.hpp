#ifndef QT_TCP_CHAT_CHATROOM_HPP
#define QT_TCP_CHAT_CHATROOM_HPP
#include <QStandardItemModel>
#include <QUuid>


class ChatRoom {

public:
    ChatRoom(QUuid id, QString roomName, uint16_t unreadCount);

    [[nodiscard]] QUuid m_room_id() const {
        return m_roomId;
    }

    [[nodiscard]] QString m_room_name() const {
        return m_roomName;
    }

    [[nodiscard]] QStandardItemModel* m_chat_model() {
        return &m_chatModel;
    }

    [[nodiscard]] u_int16_t m_unread_count() const {
        return m_unreadCount;
    }

    void incrementUnreadCount() {
        m_unreadCount++;
    }


private:
    QUuid m_roomId;
    QString m_roomName;
    QStandardItemModel m_chatModel;
    u_int16_t m_unreadCount;
};



#endif //QT_TCP_CHAT_CHATROOM_HPP
