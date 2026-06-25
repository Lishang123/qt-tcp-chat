#ifndef QT_TCP_CHAT_CHATROOM_HPP
#define QT_TCP_CHAT_CHATROOM_HPP
#include <QStandardItemModel>
#include <QUuid>


class ChatRoom {

public:
    ChatRoom(QUuid id, QString roomName, uint16_t unreadCount);

    [[nodiscard]] QUuid getRoomId() const {
        return m_roomId;
    }

    [[nodiscard]] QString getRoomName() const {
        return m_roomName;
    }

    [[nodiscard]] QStandardItemModel* getChatModel() {
        return &m_chatModel;
    }

    [[nodiscard]] u_int16_t getUnreadCount() const {
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
