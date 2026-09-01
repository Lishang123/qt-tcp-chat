#ifndef QT_TCP_CHAT_CHATROOMS_HPP
#define QT_TCP_CHAT_CHATROOMS_HPP
#include <unordered_map>

#include "ChatRoom.hpp"
#include "DatabaseManager.hpp"

struct ChatMessagePacket;

// TODO: refactor this class to split responsibilities.
// RoomManager should only manage rooms! Move UserManager, Authentication, DatabaseManager etc. up to Application.
/**
 * @brief Coordinates users, rooms, persistence, and server packet handling.
 */
class RoomManager: public QObject {
    Q_OBJECT

public:
    /**
     * @brief Loads persisted state and ensures the public room exists.
     */
    explicit RoomManager(QObject *parent = nullptr);

    /**
     * @brief Creates a user and optionally persists it.
     */
    std::shared_ptr<User> createUser(QUuid clientId, const QString & username, bool fromDB);

    /**
     * @brief Creates a room and optionally persists it.
     */
    std::shared_ptr<ChatRoom> createRoom(RoomType roomType, QUuid roomId, const QString & roomName, bool fromDB);

    /**
     * @brief Removes a room from memory and persistent storage.
     */
    bool removeRoom(QUuid roomId);

    /**
     * @brief Adds a user object to a room and optionally persists membership.
     */
    void addRoomMember(std::shared_ptr<ChatRoom> room, std::shared_ptr<User> user, bool fromDB);

    /**
     * @brief Adds an existing user to an existing room by id.
     */
    bool addRoomMember(const QUuid& roomId, const QUuid& userId);

    /**
     * @brief Records the id of the public room loaded from the database.
     */
    void setPublicRoomId(const QUuid& roomId) {
        m_publicRoomId = roomId;
    }
signals:
    void clientChanged();
    void changeClientId(QUuid clientId, QUuid newClientId);
    void loginSuccess(QUuid userId, const QString& username, const QMap<QUuid, UserInfo>& users, QList<RoomInfo>& roomInfos);
    void loginFailed(QUuid userId, const QString& errorMsg);
    void sendMessageToRoom(ChatRoom& chatRoom, ChatMessagePacket &packet);
    //void userRemoved(QUuid userId);
    void roomCreated(QUuid userId, const RoomInfo& roomInfo);
    void roomDeleted(QUuid userId, const RoomDeletedPacket& roomDeletedPacket);
    void broadcast(ChatMessagePacket &packet);

public slots:
    /**
     * @brief Dispatches an incoming serialized packet from a client.
     */
    void handleMessage(QUuid senderId, const QByteArray & data);

    /**
     * @brief Registers or logs in a user and emits the login result.
     */
    bool handleLoginRequest(QUuid clientId, LoginRequestPacket &packet);

    /**
     * @brief Removes a user, empty rooms, and persisted user data.
     */
    void removeUser(QUuid userId);

    /**
     * @brief Marks a user offline without deleting persisted data.
     */
    void logoutUser(QUuid userId);

private:
    /**
     * @brief Finds a user by display name.
     */
    std::shared_ptr<User> findUserByName(QString &username);

    /**
     * @brief Persists and routes a chat message to its room.
     */
    bool handleChatMessage(QUuid senderId, ChatMessagePacket& packet);

    /**
     * @brief Creates a direct chat or chat group from a client request.
     */
    bool handleRoomRequest(QUuid senderId, RoomRequestPacket &packet);

    /**
     * @brief Deletes a chat group after validating the requester.
     */
    bool handleRoomDeleteRequest(QUuid senderId, RoomDeleteRequestPacket &packet);


    std::map<QUuid, std::shared_ptr<ChatRoom>> m_rooms;
    //std::map<QUuid, QUuid> m_userIdToRoomId;
    QUuid m_publicRoomId;
    Users m_users;
    DatabaseManager m_dbManager;
};



#endif //QT_TCP_CHAT_CHATROOMS_HPP
