#ifndef QT_TCP_CHAT_APPLICATION_HPP
#define QT_TCP_CHAT_APPLICATION_HPP
#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>
#include "ChatRoom.hpp"
#include "Client.hpp"
#include "ChatRoomDelegate.hpp"

class Application : public QObject{
    Q_OBJECT

    enum ItemType {
        Category,
        Room,
    };

    enum CategoryType {
        Online = 1,
        Offline = 2,
    };

public:

    enum ExportFormat {
        HTML,
        TXT,
        JSON,
        PDF,
        UNKNOWN
    };

    explicit Application(QObject *parent = nullptr);

    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);

    void connectToServer(const QString &address, quint16 port);
    void disconnectFromHost();

    void addChatMessage(const QUuid &targetRoomId, ChatMessagePacket chatMsg);

    void sendMessage(const QString& message);

    void initRooms(const LoginSuccessPacket & loginSuccessPacket);

    QStandardItem *addRoomItem(const QUuid &roomId, const QUuid &userId, RoomType type, const UserInfo &userInfo);

    QStandardItem *enableUser(const LoginNotificationPacket &logoutNotificationPacket);

    void addChatGroup(const QUuid &roomId, const QString &groupName);

    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

    QStandardItem *disableUser(const LogoutNotificationPacket &logoutNotificationPacket);

    void processMessage(const ChatMessagePacket& chatMessagePacket);

    bool setRoomIdOnUser(const QUuid &roomId, const QUuid &userId, bool switchRoomLater);

    std::shared_ptr<ChatRoom> switchRoom(const QModelIndex &index);

    [[nodiscard]] Client& getClient() {
        return m_client;
    }

    [[nodiscard]] ChatModel* getChatModel() {
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

    std::shared_ptr<ChatRoom> getCurrentRoom() {
        if (m_rooms.contains(m_currentRoomId)) {
            return m_rooms[m_currentRoomId];
        }
        return nullptr;
    }

    void setCurrentRoomId(QUuid m_room_id) {
        this->m_currentRoomId = m_room_id;
    }

    void setUserId(QUuid userId) {
        m_client.setClientId(userId);
    }

    QUuid getUserId() {
        return m_client.getClientId();
    }

    void setIconSize(QSize size) {
        m_iconSize = size;
    }

    void setHistoryManager(std::shared_ptr<ChatHistoryManager> historyManager) {
        m_chatHistoryManager = historyManager;
    }

    bool exportHistory(const QString &fileName, ExportFormat format);

signals:
    // void roomSwitched(const QModelIndex &index, ChatRoom &chatRoom);
    void roomStatusChanged();
    void itemMoved(QStandardItem* userItem);

public slots:
    void disconnectFromServer();

private:

    void createRoom(const QUuid& roomId, const QString& roomName, RoomType roomType);

    bool setUnreadBadge(const QUuid &roomId, bool unread);
    void setUnreadBadge(QStandardItem* item, bool unread);

    QStandardItem *setUserOnlineStatus(const QUuid &userId, bool online);
    void moveUserToGroup(QStandardItem* userItem, CategoryType ctype);

    QStandardItem* getRoomItem(const QUuid &roomId);
    QStandardItem * getUserItem(const QUuid &userId);

    Client m_client;
    //QStringList m_list;
    ChatModel* m_ChatModel;
    QSize m_iconSize;
    QUuid m_publicRoomId;
    QUuid m_currentRoomId;

    QMap<QUuid, std::shared_ptr<UserInfo>> m_userInfo;

    QMap<QUuid, std::shared_ptr<ChatRoom>> m_rooms;
    QStandardItemModel m_roomListModel;
    std::shared_ptr<ChatHistoryManager> m_chatHistoryManager;
};

#endif //QT_TCP_CHAT_APPLICATION_HPP
