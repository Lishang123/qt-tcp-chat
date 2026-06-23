#ifndef QT_TCP_CHAT_APPLICATION_HPP
#define QT_TCP_CHAT_APPLICATION_HPP
#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>

#include "ChatRoom.hpp"
#include "Client.hpp"

class Application : public QObject{
    Q_OBJECT

    enum Roles {
        UserIdRole = Qt::UserRole + 1,
        RoomIdRole = Qt::UserRole,
        StatusRole = Qt::UserRole
    };

public:

    explicit Application(QObject *parent = nullptr);

    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);

    void connectToServer(const QString &address, quint16 port);
    void disconnectFromHost();

    void addChatMessage(const QUuid &targetRoomId, const QString &message);

    void sendMessage(const QString& message);

    void updateRooms(const LoginSuccessPacket & loginSuccessPacket);

    void addUser(const QUuid &roomId, const QUuid &userId, const QString &userName);
    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

    void processMessage(const ChatMessagePacket& chatMessagePacket);

    bool setRoomIdOnUser(const QUuid &roomId, const QUuid &userId, bool switchRoomLater);

    std::shared_ptr<ChatRoom> switchRoom(const QModelIndex &index);

    [[nodiscard]] Client& getClient() {
        return m_client;
    }

    [[nodiscard]] QStandardItemModel* getChatModel() {
        return m_ChatModel;
    }

    [[nodiscard]] QStandardItemModel& getRoomListModel() {
        return m_roomListModel;
    }

    [[nodiscard]] QUuid getPublicRoomId()  {
        return m_publicRoomId;
    }

    void setPublicRoomId(QUuid m_room_id) {
        this->m_publicRoomId = m_room_id;
    }

    [[nodiscard]] QUuid getCurrentRoomId()  {
        return m_currentRoomId;
    }

    void setCurrentRoomId(QUuid m_room_id) {
        this->m_currentRoomId = m_room_id;
    }

    void setUserId(QUuid userId) {
        m_client.setClientId(userId);
    }

signals:
    // void roomSwitched(const QModelIndex &index, ChatRoom &chatRoom);

public slots:
    void disconnectFromServer();

private:

    Client m_client;
    //QStringList m_list;
    QStandardItemModel* m_ChatModel;
    QUuid m_publicRoomId;
    QUuid m_currentRoomId;
    QMap<QUuid, std::shared_ptr<ChatRoom>> m_rooms;
    QStandardItemModel m_roomListModel;
};

#endif //QT_TCP_CHAT_APPLICATION_HPP
