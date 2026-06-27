#include "ChatRoom.hpp"

#include "ChatHistoryManager.hpp"
#include "../common/ChatMessagePacket.hpp"

ChatRoom::ChatRoom(QObject *parent): QObject (parent) {
    init();
}

ChatRoom::ChatRoom(QUuid id, QString roomName, uint16_t unreadCount, std::shared_ptr<ChatHistoryManager> chatHistoryManager)
        : m_roomId(id), m_roomName(roomName), m_unreadCount(unreadCount), m_chatHistoryManager(chatHistoryManager) {
    init();
}

void ChatRoom::addMessage(const ChatMessagePacket &chatMsg) {
    m_messages.push_back(chatMsg);
    m_chatModel.appendRow(new QStandardItem(chatMsg.getMessage()));
    m_historyModifed = true;
    m_timer.start(3000); // save history after 3 secs
}

bool ChatRoom::loadHistory() {
    return m_chatHistoryManager->loadHistory(*this);
}

bool ChatRoom::exportHistoryJSON(const QString &filepath) {
    return m_chatHistoryManager->exportHistoryJSON(*this, filepath);
}

bool ChatRoom::exportHistoryTXT(const QString &filepath) {
    return m_chatHistoryManager->exportHistoryTXT(*this, filepath);
}

bool ChatRoom::exportHistoryHTML(const QString &filepath) {
    return m_chatHistoryManager->exportHistoryHTML(*this, filepath);
}

bool ChatRoom::exportHistoryPDF(const QString &filepath) {
    return m_chatHistoryManager->exportHistoryPDF(*this, filepath);
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
