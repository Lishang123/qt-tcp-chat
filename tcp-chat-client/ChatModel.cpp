#include "ChatModel.hpp"

ChatModel::ChatModel(QObject *parent): QAbstractListModel(parent) {
}

int ChatModel::rowCount(const QModelIndex &parent) const {
    return m_messages.count();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};
    const auto& msg = m_messages[index.row()];
    switch (role) {
        case Qt::DisplayRole:
        case TextRole:
            return msg.getMessage();
        case SenderRole:
            return msg.senderName;
        case TimestampRole:
            return msg.timestamp;
        default:
            return {};
    }
}

QHash<int, QByteArray> ChatModel::roleNames() const {
    return {
        {SenderRole, "sender"},
        {TextRole, "text"},
        {TimestampRole, "timestamp"}
    };
}

void ChatModel::addMessage(const ChatMessagePacket &chatMsg) {
    //first: index of the first new row
    //last: index of the last new row
    beginInsertRows({}, m_messages.count(), m_messages.count());
    m_messages.push_back(chatMsg);
    endInsertRows();
}

const ChatMessage& ChatModel::getMessage(int row) {
    return m_messages[row];
}

void ChatModel::setMessages(QList<ChatMessage>messages) {
    beginResetModel();
    m_messages = std::move(messages);
    endResetModel();
}
