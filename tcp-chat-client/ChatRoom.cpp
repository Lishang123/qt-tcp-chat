#include "ChatRoom.hpp"

ChatRoom::ChatRoom(QUuid id, QString roomName, uint16_t unreadCount)
        : m_roomId(id), m_roomName(roomName), m_unreadCount(unreadCount) {
}
