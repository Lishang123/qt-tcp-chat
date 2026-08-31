#ifndef QT_TCP_CHAT_APPLICATION_HPP
#define QT_TCP_CHAT_APPLICATION_HPP
#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>
#include <optional>
#include "ChatRoom.hpp"
#include "Client.hpp"
#include "ChatRoomDelegate.hpp"

/**
 * @brief Client application facade for networking, rooms, contacts, and history.
 */
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
    /**
     * @brief Contact metadata used by group creation UI.
     */
    struct Contact {
        QUuid userId;
        QString username;
        bool isOnline;
    };

    /**
     * @brief Supported chat history export formats.
     */
    enum ExportFormat {
        HTML,
        TXT,
        JSON,
        PDF,
        UNKNOWN
    };

    /**
     * @brief Constructs the application state and room-list categories.
     */
    explicit Application(QObject *parent = nullptr);

    /**
     * @brief Forwards a login request to the network client.
     */
    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);

    /**
     * @brief Connects the network client to a server endpoint.
     */
    void connectToServer(const QString &address, quint16 port);

    /**
     * @brief Disconnects the network client from the server.
     */
    void disconnectFromHost();

    /**
     * @brief Adds a message to a room and updates unread state when needed.
     */
    void addChatMessage(const QUuid &targetRoomId, ChatMessagePacket chatMsg);

    /**
     * @brief Sends a text message to the currently selected room.
     */
    void sendMessage(const QString& message);

    /**
     * @brief Validates and sends a chat group creation request.
     */
    bool sendChatGroupRequest(const QString &groupName, const QList<QUuid> &memberIds);

    /**
     * @brief Sends a chat group deletion request when the room is deletable.
     */
    bool sendDeleteGroupRequest(const QUuid &roomId);

    /**
     * @brief Builds a valid chat group request from UI input.
     */
    static std::optional<RoomRequestPacket> createChatGroupRequest(const QString &groupName, const QList<QUuid> &memberIds);

    /**
     * @brief Initializes rooms and contacts from the login response.
     */
    void initRooms(const LoginSuccessPacket & loginSuccessPacket);

    /**
     * @brief Adds a direct/self user entry to the room list.
     */
    QStandardItem *addRoomItem(const QUuid &roomId, const QUuid &userId, RoomType type, const UserInfo &userInfo);

    /**
     * @brief Marks a user online or adds the user to the contact list.
     */
    QStandardItem *enableUser(const LoginNotificationPacket &logoutNotificationPacket);

    /**
     * @brief Adds a public room or group entry to the room list.
     */
    QStandardItem *addChatGroup(const QUuid &roomId, const QString &groupName, RoomType roomType = RoomType::Chatgroup);

    /**
     * @brief Adds a chat group from server-provided room metadata.
     */
    QStandardItem *addChatGroupFromRoomInfo(const RoomInfo &roomInfo);

    /**
     * @brief Removes a chat group from local state and the room list.
     */
    bool removeChatGroup(const QUuid &roomId);

    /**
     * @brief Returns contacts that can be selected for group creation.
     */
    QList<Contact> getContacts() const;

    /**
     * @brief Returns member metadata for a room.
     */
    QMap<QUuid, UserInfo> getRoomMembers(const QUuid &roomId) const;

    /**
     * @brief Removes a user entry from the room list.
     */
    void removeUser(const LogoutNotificationPacket &logoutNotificationPacket);

    /**
     * @brief Marks a user offline in rooms and the room list.
     */
    QStandardItem *disableUser(const LogoutNotificationPacket &logoutNotificationPacket);

    /**
     * @brief Processes an incoming message and creates missing direct rooms.
     */
    void processMessage(const ChatMessagePacket& chatMessagePacket);

    /**
     * @brief Assigns a room id to a user entry after a room is acquired.
     */
    bool setRoomIdOnUser(const QUuid &roomId, const QUuid &userId, bool switchRoomLater);

    /**
     * @brief Switches the active room for a clicked room-list index.
     */
    std::shared_ptr<ChatRoom> switchRoom(const QModelIndex &index);

    /**
     * @brief Returns the network client.
     */
    [[nodiscard]] Client& getClient() {
        return m_client;
    }

    /**
     * @brief Returns the current room's chat model, or null when no room is active.
     */
    [[nodiscard]] ChatModel* getChatModel() {
        return m_ChatModel;
    }

    /**
     * @brief Returns the room-list model used by the sidebar.
     */
    [[nodiscard]] QStandardItemModel& getRoomListModel() {
        return m_roomListModel;
    }

    /**
     * @brief Returns the public room id.
     */
    [[nodiscard]] QUuid getPublicRoomId()  {
        return m_publicRoomId;
    }

    /**
     * @brief Stores the public room id.
     */
    void setPublicRoomId(QUuid m_room_id) {
        this->m_publicRoomId = m_room_id;
    }

    /**
     * @brief Returns the active room id.
     */
    [[nodiscard]] QUuid getCurrentRoomId()  {
        return m_currentRoomId;
    }

    /**
     * @brief Returns the active room, or null when no active room exists.
     */
    std::shared_ptr<ChatRoom> getCurrentRoom() {
        if (m_rooms.contains(m_currentRoomId)) {
            return m_rooms[m_currentRoomId];
        }
        return nullptr;
    }

    /**
     * @brief Sets the active room id.
     */
    void setCurrentRoomId(QUuid m_room_id) {
        this->m_currentRoomId = m_room_id;
    }

    /**
     * @brief Stores the authenticated user id on the network client.
     */
    void setUserId(QUuid userId) {
        m_client.setClientId(userId);
    }

    /**
     * @brief Returns the authenticated user id.
     */
    QUuid getUserId() const {
        return m_client.getClientId();
    }

    /**
     * @brief Sets the icon size used by room-list decorations.
     */
    void setIconSize(QSize size) {
        m_iconSize = size;
    }

    /**
     * @brief Assigns the history manager used by created rooms.
     */
    void setHistoryManager(std::shared_ptr<ChatHistoryManager> historyManager) {
        m_chatHistoryManager = historyManager;
    }

    /**
     * @brief Exports the active room history in the requested format.
     */
    bool exportHistory(const QString &fileName, ExportFormat format);

    /**
     * @brief Finds a room-list item by user id.
     */
    QStandardItem * getUserItem(const QUuid &userId);

signals:
    // void roomSwitched(const QModelIndex &index, ChatRoom &chatRoom);
    void roomStatusChanged();
    void historySaved();
    void itemMoved(QStandardItem* userItem);

public slots:
    /**
     * @brief Disconnects from the server.
     */
    void disconnectFromServer();

private:

    /**
     * @brief Creates local room state and loads saved history.
     */
    void createRoom(const QUuid& roomId, const QString& roomName, RoomType roomType, const QMap<QUuid, UserInfo> &userInfos = {});

    /**
     * @brief Sets a room's unread badge by room id.
     */
    bool setUnreadBadge(const QUuid &roomId, bool unread);

    /**
     * @brief Sets a room-list item's unread badge state.
     */
    void setUnreadBadge(QStandardItem* item, bool unread);

    /**
     * @brief Updates user online status in rooms and list categories.
     */
    QStandardItem *setUserOnlineStatus(const QUuid &userId, bool online);

    /**
     * @brief Moves a user item between online and offline categories.
     */
    void moveUserToGroup(QStandardItem* userItem, CategoryType ctype);

    /**
     * @brief Finds a room-list item by room id.
     */
    QStandardItem* getRoomItem(const QUuid &roomId);

    Client m_client;
    //QStringList m_list;
    ChatModel* m_ChatModel;
    QSize m_iconSize;
    QUuid m_publicRoomId;
    QUuid m_currentRoomId;

    // QMap<QUuid, std::shared_ptr<UserInfo>> m_userInfo;

    QMap<QUuid, std::shared_ptr<ChatRoom>> m_rooms;
    QStandardItemModel m_roomListModel;
    std::shared_ptr<ChatHistoryManager> m_chatHistoryManager;
};

#endif //QT_TCP_CHAT_APPLICATION_HPP
