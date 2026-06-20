#ifndef QT_TCP_CHAT_CHATROOMS_HPP
#define QT_TCP_CHAT_CHATROOMS_HPP
#include <unordered_map>

#include "ChatRoom.hpp"


class RoomManager: public QObject {
    Q_OBJECT

public:
    explicit RoomManager(QObject *parent = nullptr);

    ChatRoom* createRoom(QUuid roomId, const QString & roomName);
    bool removeRoom(QUuid roomId);

signals:
    void clientChanged();

    void loginSuccess(QUuid userId, const QString& username, QList<RoomInfo>& roomInfos);
    void loginFailed(QUuid userId, const QString& errorMsg);
    void sendMessageToRoom(ChatRoom& chatRoom, const ChatMessagePacket &packet);
    //void userRemoved(QUuid userId);
    void broadcast(const ChatMessagePacket &packet);

public slots:
    void handleMessage(QUuid senderId, const QByteArray & data);
    bool handleLoginRequest(QUuid clientId, LoginRequestPacket &packet);
    void removeUser(QUuid userId);

private:
    std::shared_ptr<User> findUserByName(QString &username);
    std::shared_ptr<User> addUser(QUuid clientId, const QString & username);
    bool handleChatMessage(QUuid senderId, ChatMessagePacket& packet);

    std::map<QUuid, ChatRoom*> m_rooms;
    QUuid m_publicRoomId;
    Users m_users;
};



#endif //QT_TCP_CHAT_CHATROOMS_HPP
