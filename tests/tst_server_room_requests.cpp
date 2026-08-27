#include <QtTest/QtTest>

#include "tcp-chat-server/RoomManager.hpp"

class ServerRoomRequestTest : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void chatGroupCreationNotifiesCreatorAndMembers();
    void chatGroupDeletionNotifiesCreatorAndMembers();

private:
    QByteArray makeRoomRequestPayload(const RoomRequestPacket &packet) const;
    QByteArray makeRoomDeleteRequestPayload(const RoomDeleteRequestPacket &packet) const;
};

void ServerRoomRequestTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

QByteArray ServerRoomRequestTest::makeRoomRequestPayload(const RoomRequestPacket &packet) const
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << PacketType::RoomRequest << packet;
    return payload;
}

QByteArray ServerRoomRequestTest::makeRoomDeleteRequestPayload(const RoomDeleteRequestPacket &packet) const
{
    QByteArray payload;
    QDataStream stream(&payload, QIODevice::WriteOnly);
    stream << PacketType::RoomDeleteRequest << packet;
    return payload;
}

void ServerRoomRequestTest::chatGroupCreationNotifiesCreatorAndMembers()
{
    RoomManager roomManager;
    const QUuid creatorId = QUuid::createUuid();
    const QUuid memberId = QUuid::createUuid();

    roomManager.createUser(creatorId, "alice", true);
    roomManager.createUser(memberId, "bob", true);

    QList<QUuid> notifiedUserIds;
    QList<RoomInfo> emittedRoomInfos;
    connect(&roomManager, &RoomManager::roomCreated, this, [&](const QUuid &userId, const RoomInfo &roomInfo) {
        notifiedUserIds.append(userId);
        emittedRoomInfos.append(roomInfo);
    });

    const RoomRequestPacket request{RoomType::Chatgroup, {memberId}, "Project"};

    roomManager.handleMessage(creatorId, makeRoomRequestPayload(request));

    QCOMPARE(notifiedUserIds.size(), 2);
    QVERIFY(notifiedUserIds.contains(creatorId));
    QVERIFY(notifiedUserIds.contains(memberId));
    QCOMPARE(emittedRoomInfos.size(), 2);
    for (const auto &roomInfo : emittedRoomInfos) {
        QCOMPARE(roomInfo.roomType, RoomType::Chatgroup);
        QCOMPARE(roomInfo.roomName, QString("Project"));
        QCOMPARE(roomInfo.creatorId, creatorId);
        QVERIFY(roomInfo.userInfos.contains(creatorId));
        QVERIFY(roomInfo.userInfos.contains(memberId));
    }
}

void ServerRoomRequestTest::chatGroupDeletionNotifiesCreatorAndMembers()
{
    RoomManager roomManager;
    const QUuid creatorId = QUuid::createUuid();
    const QUuid memberId = QUuid::createUuid();
    QUuid groupId;

    roomManager.createUser(creatorId, "alice", true);
    roomManager.createUser(memberId, "bob", true);

    connect(&roomManager, &RoomManager::roomCreated, this, [&](const QUuid &, const RoomInfo &roomInfo) {
        groupId = roomInfo.roomId;
    });
    const RoomRequestPacket createRequest{RoomType::Chatgroup, {memberId}, "Project"};
    roomManager.handleMessage(creatorId, makeRoomRequestPayload(createRequest));
    QVERIFY(!groupId.isNull());

    QList<QUuid> notifiedUserIds;
    QList<RoomDeletedPacket> emittedPackets;
    connect(&roomManager, &RoomManager::roomDeleted, this, [&](const QUuid &userId, const RoomDeletedPacket &packet) {
        notifiedUserIds.append(userId);
        emittedPackets.append(packet);
    });

    const RoomDeleteRequestPacket deleteRequest{groupId};
    roomManager.handleMessage(creatorId, makeRoomDeleteRequestPayload(deleteRequest));

    QCOMPARE(notifiedUserIds.size(), 2);
    QVERIFY(notifiedUserIds.contains(creatorId));
    QVERIFY(notifiedUserIds.contains(memberId));
    QCOMPARE(emittedPackets.size(), 2);
    for (const auto &packet : emittedPackets) {
        QCOMPARE(packet.roomId, groupId);
    }
}

QTEST_MAIN(ServerRoomRequestTest)

#include "tst_server_room_requests.moc"
