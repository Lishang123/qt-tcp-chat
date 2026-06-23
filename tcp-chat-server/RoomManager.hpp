#ifndef QT_TCP_CHAT_CHATROOMS_HPP
#define QT_TCP_CHAT_CHATROOMS_HPP
#include <unordered_map>

#include "ChatRoom.hpp"


struct ChatMessagePacket;

class RoomManager: public QObject {
    Q_OBJECT

public:
    explicit RoomManager(QObject *parent = nullptr);

signals:
    void clientChanged();

    void loginSuccess(QUuid userId, const QString& username, const QMap<QUuid, QString>& users, QList<RoomInfo>& roomInfos);
    void loginFailed(QUuid userId, const QString& errorMsg);
    void sendMessageToRoom(ChatRoom& chatRoom, const ChatMessagePacket &packet);
    //void userRemoved(QUuid userId);
    void roomCreated(QUuid userId, const RoomInfo& roomInfo);
    void broadcast(const ChatMessagePacket &packet);

public slots:
    void handleMessage(QUuid senderId, const QByteArray & data);
    bool handleLoginRequest(QUuid clientId, LoginRequestPacket &packet);
    void removeUser(QUuid userId);

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
};



#endif //QT_TCP_CHAT_CHATROOMS_HPP
