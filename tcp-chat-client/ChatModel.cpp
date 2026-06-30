#include "ChatModel.hpp"

ChatModel::ChatModel(QObject *parent): QAbstractListModel(parent) {
}

int ChatModel::rowCount(const QModelIndex &parent) const {
    return m_items.count();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return {};
    const auto& chatItem = m_items[index.row()];
    if (const auto *msg = std::get_if<ChatMessage>(&chatItem)) {
        switch (role) {
            case ItemTypeRole:
                return static_cast<int>(ChatItemType::Message);
            case Qt::DisplayRole:
            case TextRole:
                return msg->getMessage();
            case MsgRole:
                return msg->text;
            case SenderRole:
                return msg->senderName;
            case TimestampRole:
                return msg->timestamp;
            case OutGoingRole:
                return msg->outgoing;
            default:
                return {};
        }
    }
    if (const auto *msg = std::get_if<DateSeparator>(&chatItem)) {
        switch (role) {
            case ItemTypeRole:
                return static_cast<int>(ChatItemType::DateSep);
            case Qt::DisplayRole:
            case TextRole:
                return msg->date.toString("yyyy-MM-dd");
            default:
                return {};
        }
    }
    if (const auto *msg = std::get_if<UnreadSeparator>(&chatItem)) {
        switch (role) {
            case ItemTypeRole:
                return static_cast<int>(ChatItemType::UnreadSep);
            default:
                return {};
        }
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
    appendDateSeparatorIfNeeded(chatMsg.timestamp);
    //first: index of the first new row
    //last: index of the last new row
    beginInsertRows({}, m_items.count(), m_items.count());
    m_items.push_back({static_cast<ChatMessage>(chatMsg)});
    endInsertRows();
}

const ChatItem& ChatModel::getChatItem(int row) {
    return m_items[row];
}

void ChatModel::setChatItems(QList<ChatItem>messages) {
    beginResetModel();
    m_items = std::move(messages);
    endResetModel();
}

void ChatModel::appendDateSeparatorIfNeeded(QDateTime date) {
    if (auto lastMessage = getLastMessage()) {
        if (lastMessage->timestamp.date() != date.date()) {
            //append the date separator
            appendDateSeparator(date);
        }
        return;
    }
    appendDateSeparator(date);
}

void ChatModel::appendDateSeparator(QDateTime date) {
    beginInsertRows({}, m_items.count(), m_items.count());
    m_items.push_back({static_cast<DateSeparator>(date.date())});
    endInsertRows();
}

std::optional<ChatMessage> ChatModel::getLastMessage() {
    for (int i = m_items.count() - 1; i >= 0; --i) {
        if (const auto *msg = std::get_if<ChatMessage>(&m_items[i])) {
            return *msg;
        }
    }
    return {};
}
