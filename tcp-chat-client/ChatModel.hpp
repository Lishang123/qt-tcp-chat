#ifndef QT_TCP_CHAT_CHATMODEL_HPP
#define QT_TCP_CHAT_CHATMODEL_HPP
#include <QAbstractListModel>
#include "../common/ChatMessagePacket.hpp"


class ChatModel : public QAbstractListModel {
    Q_OBJECT

    enum Roles
    {
        SenderRole = Qt::UserRole + 1, // sender name
        TextRole, // whole message
        TimestampRole, // timestamp
    };

public:

    explicit ChatModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    void addMessage(const ChatMessagePacket &chatMsg);

    const ChatMessage& getMessage(int row);

    const QList<ChatMessage>& getMessages() const {
        return m_messages;
    }

    void setMessages(QList<ChatMessage> messages);

private:
    QList<ChatMessage> m_messages;
};



#endif //QT_TCP_CHAT_CHATMODEL_HPP
