#ifndef QT_TCP_CHAT_CHATROOMS_HPP
#define QT_TCP_CHAT_CHATROOMS_HPP
#include <unordered_map>

#include "ChatRoom.hpp"
#include "DatabaseManager.hpp"

struct ChatMessagePacket;

// TODO: refactor this class to split responsibilities.
// RoomManager should only manage rooms! Move UserManager, Authentication, DatabaseManager etc. up to Application.
class RoomManager: public QObject {
    Q_OBJECT

public:
    explicit RoomManager(QObject *parent = nullptr);

signals:
    void clientChanged();
    void changeClientId(QUuid clientId, QUuid newClientId);
    void loginSuccess(QUuid userId, const QString& username, const QMap<QUuid, UserInfo>& users, QList<RoomInfo>& roomInfos);
    void loginFailed(QUuid userId, const QString& errorMsg);
    void sendMessageToRoom(ChatRoom& chatRoom, ChatMessagePacket &packet);
    //void userRemoved(QUuid userId);
    void roomCreated(QUuid userId, const RoomInfo& roomInfo);
    void broadcast(ChatMessagePacket &packet);

public slots:
    void handleMessage(QUuid senderId, const QByteArray & data);
    bool handleLoginRequest(QUuid clientId, LoginRequestPacket &packet);
    void removeUser(QUuid userId);
    void logoutUser(QUuid userId);

private:
    std::shared_ptr<User> findUserByName(QString &username);
    std::shared_ptr<User> addUser(QUuid clientId, const QString & username);
    bool handleChatMessage(QUuid senderId, ChatMessagePacket& packet);
    bool handleRoomRequest(QUuid senderId, RoomRequestPacket &packet);
    std::shared_ptr<ChatRoom> createRoom(RoomType roomType,  QUuid roomId, const QString & roomName);
    bool removeRoom(QUuid roomId);

    std::map<QUuid, std::shared_ptr<ChatRoom>> m_rooms;
    //std::map<QUuid, QUuid> m_userIdToRoomId;
    QUuid m_publicRoomId;
    Users m_users;
    DatabaseManager m_dbManager;
};



#endif //QT_TCP_CHAT_CHATROOMS_HPP
