#include <QtTest/QtTest>

#include <QDialogButtonBox>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

#include "tcp-chat-client/Application.hpp"
#include "tcp-chat-client/CreateGroupDialog.hpp"

class GroupCreationTest : public QObject {
    Q_OBJECT

private slots:
    void createChatGroupRequestRejectsMissingInput();
    void createChatGroupRequestBuildsTrimmedGroupRequest();
    void getContactsReturnsOnlineAndOfflineUsers();
    void dialogRequiresNameAndSelectedContact();
    void removeChatGroupRemovesGroupItem();
    void getRoomMembersReturnsChatGroupUsers();
    void addChatGroupFromRoomInfoStoresMembers();
    void setRoomIdOnUserWorksAfterUserLogsOut();
};

void GroupCreationTest::createChatGroupRequestRejectsMissingInput()
{
    const QList<QUuid> members{ QUuid::createUuid() };

    QVERIFY(!Application::createChatGroupRequest("", members).has_value());
    QVERIFY(!Application::createChatGroupRequest("   ", members).has_value());
    QVERIFY(!Application::createChatGroupRequest("Project", {}).has_value());
}

void GroupCreationTest::createChatGroupRequestBuildsTrimmedGroupRequest()
{
    const QUuid firstUserId = QUuid::createUuid();
    const QUuid secondUserId = QUuid::createUuid();
    const QList<QUuid> members{ firstUserId, secondUserId };

    const auto request = Application::createChatGroupRequest("  Project Team  ", members);

    QVERIFY(request.has_value());
    QCOMPARE(request->roomType, RoomType::Chatgroup);
    QCOMPARE(request->roomName, QString("Project Team"));
    QCOMPARE(request->memberIds, members);
}

void GroupCreationTest::getContactsReturnsOnlineAndOfflineUsers()
{
    Application application;
    const QUuid currentUserId = QUuid::createUuid();
    const QUuid onlineUserId = QUuid::createUuid();
    const QUuid offlineUserId = QUuid::createUuid();

    application.setUserId(currentUserId);
    application.addRoomItem(currentUserId, currentUserId, RoomType::Self, UserInfo{ "me", true });
    application.addRoomItem(QUuid(), onlineUserId, RoomType::DirectChat, UserInfo{ "alice", true });
    application.addRoomItem(QUuid(), offlineUserId, RoomType::DirectChat, UserInfo{ "bob", false });

    const QList<Application::Contact> contacts = application.getContacts();

    QCOMPARE(contacts.size(), 2);
    QCOMPARE(contacts.at(0).userId, onlineUserId);
    QCOMPARE(contacts.at(0).username, QString("alice"));
    QVERIFY(contacts.at(0).isOnline);
    QCOMPARE(contacts.at(1).userId, offlineUserId);
    QCOMPARE(contacts.at(1).username, QString("bob"));
    QVERIFY(!contacts.at(1).isOnline);
}

void GroupCreationTest::dialogRequiresNameAndSelectedContact()
{
    const QUuid userId = QUuid::createUuid();
    const QList<Application::Contact> contacts{
        Application::Contact{ userId, "alice", true }
    };
    CreateGroupDialog dialog(contacts);

    auto *nameEdit = dialog.findChild<QLineEdit *>("groupNameEdit");
    auto *contactList = dialog.findChild<QListWidget *>("contactList");
    auto *buttonBox = dialog.findChild<QDialogButtonBox *>();
    QVERIFY(nameEdit != nullptr);
    QVERIFY(contactList != nullptr);
    QVERIFY(buttonBox != nullptr);

    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    QVERIFY(okButton != nullptr);
    QVERIFY(!okButton->isEnabled());

    nameEdit->setText("Project");
    QVERIFY(!okButton->isEnabled());

    contactList->item(0)->setCheckState(Qt::Checked);
    QVERIFY(okButton->isEnabled());
    QCOMPARE(dialog.groupName(), QString("Project"));
    QCOMPARE(dialog.selectedUserIds(), QList<QUuid>{ userId });
}

void GroupCreationTest::removeChatGroupRemovesGroupItem()
{
    Application application;
    const QUuid groupId = QUuid::createUuid();
    application.addChatGroup(groupId, "Project");

    QStandardItem *groups = application.getRoomListModel().item(0);
    QCOMPARE(groups->rowCount(), 1);

    QVERIFY(application.removeChatGroup(groupId));
    QCOMPARE(groups->rowCount(), 0);
    QVERIFY(!application.removeChatGroup(groupId));
}

void GroupCreationTest::getRoomMembersReturnsChatGroupUsers()
{
    Application application;
    const QUuid groupId = QUuid::createUuid();
    const QUuid firstUserId = QUuid::createUuid();
    const QUuid secondUserId = QUuid::createUuid();

    LoginSuccessPacket loginPacket;
    loginPacket.userId = QUuid::createUuid();

    RoomInfo selfRoom;
    selfRoom.roomType = RoomType::Self;
    selfRoom.roomId = loginPacket.userId;
    selfRoom.roomName = "me";
    selfRoom.userInfos.insert(loginPacket.userId, UserInfo{ "me", true });
    loginPacket.roomInfos.append(selfRoom);

    RoomInfo groupRoom;
    groupRoom.roomType = RoomType::Chatgroup;
    groupRoom.roomId = groupId;
    groupRoom.roomName = "Project";
    groupRoom.userInfos.insert(firstUserId, UserInfo{ "alice", true });
    groupRoom.userInfos.insert(secondUserId, UserInfo{ "bob", false });
    loginPacket.roomInfos.append(groupRoom);

    application.initRooms(loginPacket);

    const auto members = application.getRoomMembers(groupId);
    QCOMPARE(members.size(), 2);
    QCOMPARE(members.value(firstUserId).username, QString("alice"));
    QVERIFY(members.value(firstUserId).isOnline);
    QCOMPARE(members.value(secondUserId).username, QString("bob"));
    QVERIFY(!members.value(secondUserId).isOnline);
}

void GroupCreationTest::addChatGroupFromRoomInfoStoresMembers()
{
    Application application;
    const QUuid groupId = QUuid::createUuid();
    const QUuid userId = QUuid::createUuid();

    RoomInfo groupRoom;
    groupRoom.roomType = RoomType::Chatgroup;
    groupRoom.roomId = groupId;
    groupRoom.roomName = "Project";
    groupRoom.userInfos.insert(userId, UserInfo{ "alice", true });

    QStandardItem *item = application.addChatGroupFromRoomInfo(groupRoom);

    QVERIFY(item != nullptr);
    QCOMPARE(item->text(), QString("Project"));
    QCOMPARE(application.getRoomMembers(groupId).size(), 1);
    QCOMPARE(application.getRoomMembers(groupId).value(userId).username, QString("alice"));
}

void GroupCreationTest::setRoomIdOnUserWorksAfterUserLogsOut()
{
    Application application;
    const QUuid userId = QUuid::createUuid();
    const QUuid directRoomId = QUuid::createUuid();

    application.addRoomItem(QUuid(), userId, RoomType::DirectChat, UserInfo{ "alice", true });
    QVERIFY(application.disableUser(LogoutNotificationPacket{ userId }) != nullptr);

    QVERIFY(application.setRoomIdOnUser(directRoomId, userId, false));
    QCOMPARE(application.getUserItem(userId)->data(RoomIdRole).toUuid(), directRoomId);
}

QTEST_MAIN(GroupCreationTest)

#include "tst_group_creation.moc"
