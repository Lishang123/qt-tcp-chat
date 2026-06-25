#include "ChatRoom.hpp"

#include "ChatHistoryManager.hpp"

ChatRoom::ChatRoom(QObject *parent): QObject (parent) {
    init();
}

ChatRoom::ChatRoom(QUuid id, QString roomName, uint16_t unreadCount, std::shared_ptr<ChatHistoryManager> chatHistoryManager)
        : m_roomId(id), m_roomName(roomName), m_unreadCount(unreadCount), m_chatHistoryManager(chatHistoryManager) {
    init();
}

void ChatRoom::addMessage(const QString& message) {
    m_chatModel.appendRow(new QStandardItem(message));
    m_historyModifed = true;
    m_timer.start(5000);

}

bool ChatRoom::loadHistory() {
    m_chatHistoryManager->loadHistory(*this);
}

void ChatRoom::saveHistory() {
    if (!m_historyModifed) return;
    m_chatHistoryManager->saveHistory(*this);
    m_historyModifed = false;
}

void ChatRoom::init() {
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, &ChatRoom::saveHistory);
}
