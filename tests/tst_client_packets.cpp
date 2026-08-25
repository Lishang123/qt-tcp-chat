#include <QtTest/QtTest>

#include "tcp-chat-client/Client.hpp"

class ClientPacketTest : public QObject {
    Q_OBJECT

private slots:
    void loginFailEmitsErrorMessage();
    void loginSuccessUpdatesNameAndEmitsPacket();
    void notifyLoginEmitsPacket();
    void notifyLogoutEmitsPacket();
    void chatMessageEmitsPacket();
    void roomAcquiredEmitsPacket();

private:
    template <typename Packet>
    QByteArray makePayload(PacketType packetType, const Packet &packet) const;

    bool processPacket(Client &client, const QByteArray &payload) const;
};

template <typename Packet>
QByteArray ClientPacketTest::makePayload(PacketType packetType, const Packet &packet) const
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << packetType << packet;
    return payload;
}

bool ClientPacketTest::processPacket(Client &client, const QByteArray &payload) const
{
    // Qt's meta-object system can invoke a slot even though it is private
    // return value: a bool indicating whether Qt successfully invoked the method.
    return QMetaObject::invokeMethod(
        &client,
        "processPacket",
        Qt::DirectConnection,
        Q_ARG(QByteArray, payload));
}

void ClientPacketTest::loginFailEmitsErrorMessage()
{
    Client client;
    QString emittedError;
    int emitCount = 0;
    connect(&client, &Client::errorOccured, this, [&](const QString &message) {
        emittedError = message;
        ++emitCount;
    });

    const LoginFailedPacket packet{ "Invalid username or password" };

    QVERIFY(processPacket(client, makePayload(PacketType::LoginFail, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(emittedError, packet.errorMsg);
}

void ClientPacketTest::loginSuccessUpdatesNameAndEmitsPacket()
{
    Client client;
    LoginSuccessPacket emittedPacket;
    int emitCount = 0;
    connect(&client, &Client::loggedIn, this, [&](const LoginSuccessPacket &packet) {
        emittedPacket = packet;
        ++emitCount;
    });

    LoginSuccessPacket packet;
    packet.userId = QUuid::createUuid();
    packet.username = "alice";
    packet.welcomeMsg = "Welcome";

    RoomInfo room;
    room.roomType = RoomType::Public;
    room.roomId = QUuid::createUuid();
    room.roomName = "Lobby";
    room.userInfos.insert(packet.userId, UserInfo{ packet.username, true });
    packet.roomInfos.append(room);
    packet.contacts.insert(QUuid::createUuid(), UserInfo{ "bob", false });

    QVERIFY(processPacket(client, makePayload(PacketType::LoginSuccess, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(client.getUserName(), packet.username);
    QCOMPARE(emittedPacket.userId, packet.userId);
    QCOMPARE(emittedPacket.username, packet.username);
    QCOMPARE(emittedPacket.welcomeMsg, packet.welcomeMsg);
    QCOMPARE(emittedPacket.roomInfos.size(), 1);
    QCOMPARE(emittedPacket.roomInfos.first().roomId, room.roomId);
    QCOMPARE(emittedPacket.contacts.size(), 1);
}

void ClientPacketTest::notifyLoginEmitsPacket()
{
    Client client;
    LoginNotificationPacket emittedPacket;
    int emitCount = 0;
    connect(&client, &Client::notifyUserLogin, this, [&](const LoginNotificationPacket &packet) {
        emittedPacket = packet;
        ++emitCount;
    });

    const LoginNotificationPacket packet{ QUuid::createUuid(), "carol" };

    QVERIFY(processPacket(client, makePayload(PacketType::NotifyLogin, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(emittedPacket.userId, packet.userId);
    QCOMPARE(emittedPacket.username, packet.username);
}

void ClientPacketTest::notifyLogoutEmitsPacket()
{
    Client client;
    LogoutNotificationPacket emittedPacket;
    int emitCount = 0;
    connect(&client, &Client::notifyUserLogout, this, [&](const LogoutNotificationPacket &packet) {
        emittedPacket = packet;
        ++emitCount;
    });

    const LogoutNotificationPacket packet{ QUuid::createUuid() };

    QVERIFY(processPacket(client, makePayload(PacketType::NotifyLogout, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(emittedPacket.userId, packet.userId);
}

void ClientPacketTest::chatMessageEmitsPacket()
{
    Client client;
    ChatMessagePacket emittedPacket;
    int emitCount = 0;
    connect(&client, &Client::messageReceived, this, [&](const ChatMessagePacket &packet) {
        emittedPacket = packet;
        ++emitCount;
    });

    ChatMessagePacket packet;
    packet.roomId = QUuid::createUuid();
    packet.senderId = QUuid::createUuid();
    packet.messageId = QUuid::createUuid();
    packet.timestamp = QDateTime(QDate(2026, 1, 15), QTime(12, 30, 0));
    packet.senderName = "dave";
    packet.text = "hello";
    packet.outgoing = false;

    QVERIFY(processPacket(client, makePayload(PacketType::ChatMessagePkt, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(emittedPacket.roomId, packet.roomId);
    QCOMPARE(emittedPacket.senderId, packet.senderId);
    QCOMPARE(emittedPacket.messageId, packet.messageId);
    QCOMPARE(emittedPacket.timestamp, packet.timestamp);
    QCOMPARE(emittedPacket.senderName, packet.senderName);
    QCOMPARE(emittedPacket.text, packet.text);
    QCOMPARE(emittedPacket.outgoing, packet.outgoing);
}

void ClientPacketTest::roomAcquiredEmitsPacket()
{
    Client client;
    RoomInfoPacket emittedPacket;
    int emitCount = 0;
    connect(&client, &Client::roomAcquired, this, [&](const RoomInfoPacket &packet) {
        emittedPacket = packet;
        ++emitCount;
    });

    RoomInfoPacket packet;
    packet.roomInfo.roomType = RoomType::Chatgroup;
    packet.roomInfo.roomId = QUuid::createUuid();
    packet.roomInfo.roomName = "Project";
    packet.roomInfo.userInfos.insert(QUuid::createUuid(), UserInfo{ "erin", true });
    packet.roomInfo.userInfos.insert(QUuid::createUuid(), UserInfo{ "frank", false });

    QVERIFY(processPacket(client, makePayload(PacketType::RoomAcquired, packet)));
    QCOMPARE(emitCount, 1);
    QCOMPARE(emittedPacket.roomInfo.roomType, packet.roomInfo.roomType);
    QCOMPARE(emittedPacket.roomInfo.roomId, packet.roomInfo.roomId);
    QCOMPARE(emittedPacket.roomInfo.roomName, packet.roomInfo.roomName);
    QCOMPARE(emittedPacket.roomInfo.userInfos.size(), packet.roomInfo.userInfos.size());
}

QTEST_MAIN(ClientPacketTest)

#include "tst_client_packets.moc"
