#include <QtTest/QtTest>

#include "tcp-chat-client/ChatModel.hpp"
#include "tcp-chat-client/ChatRoom.hpp"

class ChatModelTest : public QObject {
    Q_OBJECT

private slots:
    void newModelIsEmpty();
    void invalidIndexesReturnNoData();
    void addFirstMessageInsertsDateSeparatorAndMessage();
    void addMessageOnSameDateDoesNotInsertAnotherDateSeparator();
    void addMessageOnNewDateInsertsNewDateSeparator();
    void setChatItemsResetsModelAndKeepsOnlyMessagesInGetMessages();
    void roleNamesExposeQmlRoles();
    void chatRoomStoresMetadataAndUnreadCount();

private:
    ChatMessagePacket makeMessage(const QString &senderName, const QString &text, const QDate &date, bool outgoing = false) const;
};

ChatMessagePacket ChatModelTest::makeMessage(const QString &senderName, const QString &text, const QDate &date, bool outgoing) const
{
    ChatMessagePacket message;
    message.roomId = QUuid::createUuid();
    message.senderId = QUuid::createUuid();
    message.messageId = QUuid::createUuid();
    message.timestamp = QDateTime(date, QTime(9, 30, 0));
    message.senderName = senderName;
    message.text = text;
    message.outgoing = outgoing;
    return message;
}

void ChatModelTest::newModelIsEmpty()
{
    const ChatModel model;

    QCOMPARE(model.rowCount(), 0);
    QCOMPARE(model.rowCount(model.index(0, 0)), 0);
    QVERIFY(model.getChatItems().isEmpty());
    QVERIFY(model.getMessages().isEmpty());
}

void ChatModelTest::invalidIndexesReturnNoData()
{
    ChatModel model;
    model.addMessage(makeMessage("alice", "hello", QDate(2026, 8, 25)));

    QVERIFY(!model.data(QModelIndex(), ChatModel::TextRole).isValid());
    QVERIFY(!model.data(model.index(model.rowCount()), ChatModel::TextRole).isValid());
}

void ChatModelTest::addFirstMessageInsertsDateSeparatorAndMessage()
{
    ChatModel model;
    QSignalSpy rowsInserted(&model, &QAbstractItemModel::rowsInserted);
    const ChatMessagePacket message = makeMessage("alice", "hello", QDate(2026, 8, 25), true);

    model.addMessage(message);

    QCOMPARE(rowsInserted.count(), 2);
    QCOMPARE(model.rowCount(), 2);

    const QModelIndex dateIndex = model.index(0);
    QCOMPARE(model.data(dateIndex, ChatModel::ItemTypeRole).toInt(), static_cast<int>(ChatItemType::DateSep));
    QCOMPARE(model.data(dateIndex, ChatModel::TextRole).toString(), QString("2026-08-25"));

    const QModelIndex messageIndex = model.index(1);
    QCOMPARE(model.data(messageIndex, ChatModel::ItemTypeRole).toInt(), static_cast<int>(ChatItemType::Message));
    QCOMPARE(model.data(messageIndex, ChatModel::SenderRole).toString(), message.senderName);
    QCOMPARE(model.data(messageIndex, ChatModel::MsgRole).toString(), message.text);
    QCOMPARE(model.data(messageIndex, ChatModel::TimestampRole).toDateTime(), message.timestamp);
    QCOMPARE(model.data(messageIndex, ChatModel::OutGoingRole).toBool(), message.outgoing);
    QVERIFY(model.data(messageIndex, ChatModel::TextRole).toString().contains(message.senderName));
    QVERIFY(model.data(messageIndex, ChatModel::TextRole).toString().contains(message.text));
}

void ChatModelTest::addMessageOnSameDateDoesNotInsertAnotherDateSeparator()
{
    ChatModel model;
    model.addMessage(makeMessage("alice", "first", QDate(2026, 8, 25)));
    QSignalSpy rowsInserted(&model, &QAbstractItemModel::rowsInserted);

    model.addMessage(makeMessage("bob", "second", QDate(2026, 8, 25)));

    QCOMPARE(rowsInserted.count(), 1);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.data(model.index(2, 0), ChatModel::ItemTypeRole).toInt(), static_cast<int>(ChatItemType::Message));
    QCOMPARE(model.getMessages().size(), 2);
}

void ChatModelTest::addMessageOnNewDateInsertsNewDateSeparator()
{
    ChatModel model;
    model.addMessage(makeMessage("alice", "first", QDate(2026, 8, 25)));
    QSignalSpy rowsInserted(&model, &QAbstractItemModel::rowsInserted);

    model.addMessage(makeMessage("bob", "next day", QDate(2026, 8, 26)));

    QCOMPARE(rowsInserted.count(), 2);
    QCOMPARE(model.rowCount(), 4);
    QCOMPARE(model.data(model.index(2, 0), ChatModel::ItemTypeRole).toInt(), static_cast<int>(ChatItemType::DateSep));
    QCOMPARE(model.data(model.index(2, 0), ChatModel::TextRole).toString(), QString("2026-08-26"));
    QCOMPARE(model.data(model.index(3, 0), ChatModel::MsgRole).toString(), QString("next day"));
}

void ChatModelTest::setChatItemsResetsModelAndKeepsOnlyMessagesInGetMessages()
{
    ChatModel model;
    QSignalSpy modelReset(&model, &QAbstractItemModel::modelReset);
    const ChatMessagePacket message = makeMessage("alice", "stored", QDate(2026, 8, 25));

    QList<ChatItem> items;
    items.append(DateSeparator{ QDate(2026, 8, 25) });
    items.append(static_cast<ChatMessage>(message));
    items.append(UnreadSeparator{});

    model.setChatItems(items);

    QCOMPARE(modelReset.count(), 1);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.getMessages().size(), 1);
    QCOMPARE(model.getMessages().first().text, message.text);
    QCOMPARE(model.data(model.index(2, 0), ChatModel::ItemTypeRole).toInt(), static_cast<int>(ChatItemType::UnreadSep));
    QVERIFY(!model.data(model.index(2, 0), ChatModel::TextRole).isValid());
}

void ChatModelTest::roleNamesExposeQmlRoles()
{
    const ChatModel model;
    const QHash<int, QByteArray> roleNames = model.roleNames();

    QCOMPARE(roleNames.value(ChatModel::SenderRole), QByteArray("sender"));
    QCOMPARE(roleNames.value(ChatModel::TextRole), QByteArray("text"));
    QCOMPARE(roleNames.value(ChatModel::TimestampRole), QByteArray("timestamp"));
}

void ChatModelTest::chatRoomStoresMetadataAndUnreadCount()
{
    const QUuid roomId = QUuid::createUuid();
    ChatRoom room(roomId, "Lobby", RoomType::Public, 2, {});

    QCOMPARE(room.getRoomId(), roomId);
    QCOMPARE(room.getRoomName(), QString("Lobby"));
    QCOMPARE(room.getRoomType(), RoomType::Public);
    QCOMPARE(room.getUnreadCount(), static_cast<uint16_t>(2));
    QVERIFY(room.getChatModel() != nullptr);

    room.incrementUnreadCount();
    QCOMPARE(room.getUnreadCount(), static_cast<uint16_t>(3));

    room.clearUnread();
    QCOMPARE(room.getUnreadCount(), static_cast<uint16_t>(0));

    room.setRoomName("General");
    QCOMPARE(room.getRoomName(), QString("General"));
}

QTEST_MAIN(ChatModelTest)

#include "tst_chat_model.moc"
