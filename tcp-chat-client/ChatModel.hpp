#ifndef QT_TCP_CHAT_CHATMODEL_HPP
#define QT_TCP_CHAT_CHATMODEL_HPP
#include <QAbstractListModel>
#include "../common/ChatMessagePacket.hpp"


struct DateSeparator {
    QDate date;
};

struct UnreadSeparator {};

enum ChatItemType {
    Message,
    DateSep,
    UnreadSep,
};
using ChatItem = std::variant<ChatMessage, DateSeparator, UnreadSeparator>;

inline QDataStream &operator<<(QDataStream &out, const ChatItem &item)
{
    if (const auto *msg = std::get_if<ChatMessage>(&item))
    {
        out << ChatItemType::Message;
        out << *msg;
    }
    else if (const auto *date = std::get_if<DateSeparator>(&item))
    {
        out << ChatItemType::DateSep;
        out << date->date;
    }
    else if (std::get_if<UnreadSeparator>(&item))
    {
        out << ChatItemType::UnreadSep;
    }
    return out;
}

inline QDataStream &operator>>(QDataStream &in, ChatItem &item)
{
    ChatItemType type;
    in >> type;

    switch (type)
    {
        case ChatItemType::Message:
        {
            ChatMessage msg;
            in >> msg;
            item = std::move(msg);
            break;
        }
        case ChatItemType::DateSep:
        {
            DateSeparator date;
            in >> date.date;
            item = std::move(date);
            break;
        }
        case ChatItemType::UnreadSep:
        {
            item = UnreadSeparator{};
            break;
        }
    }
    return in;
}


class ChatModel : public QAbstractListModel {
    Q_OBJECT

public:

    enum Roles
    {
        ItemTypeRole = Qt::UserRole + 1, // sender name
        SenderRole,
        MsgRole, // only the message
        TextRole, // whole string representation
        TimestampRole, // timestamp
        OutGoingRole,
    };

    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const ChatMessagePacket &chatMsg);

    const ChatItem& getChatItem(int row);

    const QList<ChatMessage> getMessages() const {
        QList<ChatMessage> chatMessages;
        foreach (const ChatItem& chatItem, m_items) {
            if (std::holds_alternative<ChatMessage>(chatItem)) {
                chatMessages.append(std::get<ChatMessage>(chatItem));
            }
        }
        return chatMessages;
    }

    const QList<ChatItem>& getChatItems() const {
        return m_items;
    }

    void setChatItems(QList<ChatItem> messages);

private:
    void appendDateSeparatorIfNeeded(QDateTime date);
    void appendDateSeparator(QDateTime date);
    std::optional<ChatMessage> getLastMessage();
    QList<ChatItem> m_items;
};



#endif //QT_TCP_CHAT_CHATMODEL_HPP
