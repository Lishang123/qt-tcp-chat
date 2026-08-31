#ifndef QT_TCP_CHAT_DATABASEMANAGER_HPP
#define QT_TCP_CHAT_DATABASEMANAGER_HPP
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFile>
#include "ChatRoom.hpp"

class RoomManager;

/**
 * @brief SQLite-backed persistence layer for users, rooms, memberships, and messages.
 */
class DatabaseManager {
public:
    /**
     * @brief Opens the configured database connection.
     */
    DatabaseManager();

    /**
     * @brief Creates the required tables when they do not already exist.
     */
    bool createTables();

    /**
     * @brief Inserts a user record.
     */
    bool addUser(const User& user);

    /**
     * @brief Deletes a user record by id.
     */
    bool removeUser(const QUuid& userId);

    /**
     * @brief Inserts a room record.
     */
    bool addRoom(const ChatRoom& room);

    /**
     * @brief Deletes a room record by id.
     */
    bool removeRoom(const QUuid& roomId);

    /**
     * @brief Persists a room membership.
     */
    bool addRoomMember(const QUuid& roomId, const QUuid& userId);

    /**
     * @brief Removes a persisted room membership.
     */
    bool removeRoomMember(const QUuid& roomId, const QUuid& userId);

    /**
     * @brief Persists a chat message.
     */
    bool addMessage(const QUuid& messageId, const QUuid& senderId,
        const QUuid& roomId, const QString& content, const QDateTime& sentAt);

    /**
     * @brief Loads persisted users, rooms, and memberships into a room manager.
     */
    bool initFromDB(RoomManager& roomManager);

private:
    /**
     * @brief Loads all persisted users.
     */
    bool loadAllUsers(RoomManager& roomManager);

    /**
     * @brief Loads all persisted rooms.
     */
    bool loadAllRooms(RoomManager& roomManager);

    /**
     * @brief Loads all persisted room memberships.
     */
    bool loadMembershipInfo(RoomManager& roomManager);
};



#endif //QT_TCP_CHAT_DATABASEMANAGER_HPP
