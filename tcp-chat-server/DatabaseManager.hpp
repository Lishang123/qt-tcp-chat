#ifndef QT_TCP_CHAT_DATABASEMANAGER_HPP
#define QT_TCP_CHAT_DATABASEMANAGER_HPP
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFile>
#include "ChatRoom.hpp"

class RoomManager;

class DatabaseManager {
public:
    DatabaseManager();

    bool createTables();

    bool addUser(const User& user);
    bool removeUser(const QUuid& userId);

    bool addRoom(const ChatRoom& room);
    bool removeRoom(const QUuid& roomId);

    bool addRoomMember(const QUuid& roomId, const QUuid& userId);
    bool removeRoomMember(const QUuid& roomId, const QUuid& userId);

    bool addMessage(const QUuid& messageId, const QUuid& senderId,
        const QUuid& roomId, const QString& content, const QDateTime& sentAt);

    bool initFromDB(RoomManager& roomManager);

private:
    bool loadAllUsers(RoomManager& roomManager);
    bool loadAllRooms(RoomManager& roomManager);
    bool loadMembershipInfo(RoomManager& roomManager);
};



#endif //QT_TCP_CHAT_DATABASEMANAGER_HPP
