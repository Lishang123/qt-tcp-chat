#ifndef QT_TCP_CHAT_CHATMODEL_HPP
#define QT_TCP_CHAT_CHATMODEL_HPP
#include <QAbstractListModel>
#include "../common/ChatMessagePacket.hpp"


/**
 * @brief Marker item used to separate messages from different dates.
 */
struct DateSeparator {
    QDate date;
};

/**
 * @brief Marker item inserted before unread messages.
 */
struct UnreadSeparator {};

/**
 * @brief Serialized discriminator for ChatItem variants.
 */
enum ChatItemType {
    Message,
    DateSep,
    UnreadSep,
};
using ChatItem = std::variant<ChatMessage, DateSeparator, UnreadSeparator>;

/**
 * @brief Serializes a chat model item variant.
 */
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

/**
 * @brief Deserializes a chat model item variant.
 */
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


/**
 * @brief List model that exposes chat messages and separator rows to Qt views.
 */
class ChatModel : public QAbstractListModel {
    Q_OBJECT

public:

    /**
     * @brief Custom Qt roles consumed by chat delegates.
     */
    enum Roles
    {
        ItemTypeRole = Qt::UserRole + 1, // sender name
        SenderRole,
        MsgRole, // only the message
        TextRole, // whole string representation
        TimestampRole, // timestamp
        OutGoingRole,
    };

    /**
     * @brief Constructs an empty chat model.
     */
    explicit ChatModel(QObject *parent = nullptr);

    /**
     * @brief Returns the number of chat items in the model.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * @brief Returns display data for the requested index and role.
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief Returns role names for Qt model consumers.
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Appends a message and inserts date separators when needed.
     */
    void addMessage(const ChatMessagePacket &chatMsg);

    /**
     * @brief Returns the item at the given row.
     */
    const ChatItem& getChatItem(int row);

    /**
     * @brief Returns only chat messages, excluding separator items.
     */
    const QList<ChatMessage> getMessages() const {
        QList<ChatMessage> chatMessages;
        foreach (const ChatItem& chatItem, m_items) {
            if (std::holds_alternative<ChatMessage>(chatItem)) {
                chatMessages.append(std::get<ChatMessage>(chatItem));
            }
        }
        return chatMessages;
    }

    /**
     * @brief Returns all model items, including separators.
     */
    const QList<ChatItem>& getChatItems() const {
        return m_items;
    }

    /**
     * @brief Replaces all chat model items.
     */
    void setChatItems(QList<ChatItem> messages);

private:
    /**
     * @brief Appends a date separator if the given date differs from the last message.
     */
    void appendDateSeparatorIfNeeded(QDateTime date);

    /**
     * @brief Appends a date separator for the given date.
     */
    void appendDateSeparator(QDateTime date);

    /**
     * @brief Returns the last message item if one exists.
     */
    std::optional<ChatMessage> getLastMessage();
    QList<ChatItem> m_items;
};



#endif //QT_TCP_CHAT_CHATMODEL_HPP
