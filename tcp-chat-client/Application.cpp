#include "Application.hpp"


Application::Application(QObject *parent) : QObject(parent), m_ChatModel(nullptr) {
    auto groupCategory = new QStandardItem("groups");
    groupCategory->setData(Category, Qt::UserRole);
    auto onlineCategory = new QStandardItem("online");
    onlineCategory->setData(Category, Qt::UserRole);
    auto offlineCategory = new QStandardItem("offline");
    offlineCategory->setData(Category, Qt::UserRole);

    m_roomListModel.appendRow(groupCategory);
    m_roomListModel.appendRow(onlineCategory);
    m_roomListModel.appendRow(offlineCategory);
}

void Application::sendLoginRequest(const LoginRequestPacket &loginRequestPacket) {
    qInfo() << Q_FUNC_INFO;
    m_client.sendLoginRequest(loginRequestPacket);
}

void Application::connectToServer(const QString &address, quint16 port) {
    m_client.connectToServer(address, port);
}

void Application::disconnectFromHost() {
    m_client.disconnectFromHost();
}

void Application::addChatMessage(const QUuid &targetRoomId, const ChatMessagePacket chatMsg) {
    m_rooms[targetRoomId]->addMessage(chatMsg);
    if (targetRoomId != m_currentRoomId) {
        //update the unread status and the chat model in that room
        m_rooms[targetRoomId]->incrementUnreadCount();
        setUnreadBadge(targetRoomId, true);
    }
    else if(m_rooms[targetRoomId]->getUnreadCount()) {
        setUnreadBadge(targetRoomId, false);
    }
}

void Application::sendMessage(const QString &message) {
    m_client.sendMessage(message, m_currentRoomId);
}

bool Application::sendChatGroupRequest(const QString &groupName, const QList<QUuid> &memberIds) {
    const auto request = createChatGroupRequest(groupName, memberIds);
    if (!request) {
        return false;
    }
    m_client.sendRoomRequest(*request);
    return true;
}

bool Application::sendDeleteGroupRequest(const QUuid &roomId) {
    if (roomId.isNull()) {
        return false;
    }
    auto *item = getRoomItem(roomId);
    if (!item || static_cast<RoomType>(item->data(RoomTypeRole).toInt()) != RoomType::Chatgroup) {
        return false;
    }
    m_client.sendRoomDeleteRequest(RoomDeleteRequestPacket{roomId});
    return true;
}

std::optional<RoomRequestPacket> Application::createChatGroupRequest(const QString &groupName, const QList<QUuid> &memberIds) {
    const QString trimmedName = groupName.trimmed();
    if (trimmedName.isEmpty() || memberIds.isEmpty()) {
        return std::nullopt;
    }
    return RoomRequestPacket{RoomType::Chatgroup, memberIds, trimmedName};
}

void Application::initRooms(const LoginSuccessPacket & loginSuccessPacket) {
    //This line keeps the program running forever!!
    //m_roomListModel.appendRow(m_roomListModel.invisibleRootItem());

    // add rooms:
    QUuid userId = loginSuccessPacket.userId;
    auto roomInfos = loginSuccessPacket.roomInfos;
    // add the public room to the sidebar
    addChatGroup(m_publicRoomId, "public", RoomType::Public);
    createRoom(m_publicRoomId, "public", RoomType::Public);

    //add the rest of the rooms
    foreach(auto roomInfo, roomInfos) {
        if(roomInfo.roomId == m_publicRoomId || roomInfo.roomId == m_currentRoomId) continue;
        if (roomInfo.roomType == RoomType::Self) {
            assert(roomInfo.userInfos.count() == 1);
            assert(roomInfo.roomId == userId);
            addRoomItem(roomInfo.roomId, roomInfo.roomId, roomInfo.roomType, roomInfo.userInfos.first());
        }
        if (roomInfo.roomType == RoomType::DirectChat) {
            assert(roomInfo.userInfos.count() == 1);
            //get the userId
            addRoomItem(roomInfo.roomId, roomInfo.userInfos.firstKey() , roomInfo.roomType, roomInfo.userInfos.first());
        }
        if (roomInfo.roomType == RoomType::Chatgroup) {
            addChatGroup(roomInfo.roomId, roomInfo.roomName);
        }
        // create ChatRoom object and add it to the room list
        createRoom(roomInfo.roomId,
            roomInfo.roomType == RoomType::Chatgroup ? roomInfo.roomName : roomInfo.userInfos.first().username,
            roomInfo.roomType,
            roomInfo.userInfos);
    }

    // add users without rooms
    for (const auto& [userId, userInfo]: loginSuccessPacket.contacts.asKeyValueRange()) {
        // qInfo() << chatRoom->getRoomId() << chatRoom->getRoomName();
        // qInfo() << Q_FUNC_INFO << chatRoom->getRoomInfo().roomId;
        addRoomItem(QUuid(), userId, RoomType::DirectChat, userInfo);
    }
}

QStandardItem *Application::addRoomItem(const QUuid &roomId, const QUuid &userId, const RoomType type, const UserInfo &userInfo) {
    // TODO: replace it with subclass of QAbstractItem
    qInfo() << Q_FUNC_INFO << "add new user " << roomId << " " << userId << " " << userInfo.username;
    QStandardItem* item = new QStandardItem(userInfo.username);
    item->setData(static_cast<int>(type), RoomTypeRole);
    item->setData(roomId, RoomIdRole);
    item->setData(userId, UserIdRole);
    item->setData(!userInfo.isOnline, OfflineRole);
    item->setIcon(QIcon(":/icons/icons/mann-avatar.png"));
    if (userInfo.isOnline) {
        m_roomListModel.item(1)->appendRow(item);
    }
    else {
        m_roomListModel.item(2)->appendRow(item);
    }
    return item;
}

QStandardItem *Application::enableUser(const LoginNotificationPacket &loginNotificationPacket) {
    auto userItem = getUserItem(loginNotificationPacket.userId);
    if (userItem) {
        qInfo() << Q_FUNC_INFO << "enable user " << userItem->text();
        setUserOnlineStatus(loginNotificationPacket.userId, true);
        return userItem;
    }
    qInfo() << Q_FUNC_INFO << "add new user item " << loginNotificationPacket.username;
    return addRoomItem(QUuid(), loginNotificationPacket.userId, RoomType::DirectChat, {loginNotificationPacket.username, true});
}


QStandardItem *Application::disableUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    return setUserOnlineStatus(logoutNotificationPacket.userId, false);
}

QStandardItem *Application::addChatGroup(const QUuid &roomId, const QString &groupName, RoomType roomType) {
    QStandardItem* item = new QStandardItem(groupName);
    item->setData(static_cast<int>(roomType), RoomTypeRole);
    item->setData(roomId, RoomIdRole);
    item->setData(QUuid(), UserIdRole);
    if (roomType == RoomType::Chatgroup) {
        item->setIcon(QIcon(":/icons/icons/group.png"));
    }
    else {
        item->setIcon(QIcon(":/icons/icons/globe.png"));
    }
    m_roomListModel.item(0)->appendRow(item);
    return item;
}

QStandardItem *Application::addChatGroupFromRoomInfo(const RoomInfo &roomInfo) {
    if (roomInfo.roomType != RoomType::Chatgroup || roomInfo.roomId.isNull()) {
        return nullptr;
    }

    auto *item = addChatGroup(roomInfo.roomId, roomInfo.roomName);

    createRoom(roomInfo.roomId, roomInfo.roomName, roomInfo.roomType, roomInfo.userInfos);
    return item;
}

QList<Application::Contact> Application::getContacts() const {
    QList<Contact> contacts;
    for (int categoryRow = 1; categoryRow < m_roomListModel.rowCount(); ++categoryRow) {
        const QStandardItem *categoryItem = m_roomListModel.item(categoryRow);
        if (!categoryItem) {
            continue;
        }
        for (int itemRow = 0; itemRow < categoryItem->rowCount(); ++itemRow) {
            const QStandardItem *item = categoryItem->child(itemRow);
            if (!item) {
                continue;
            }
            const QUuid userId = item->data(UserIdRole).toUuid();
            if (userId.isNull() || userId == getUserId()) {
                continue;
            }
            contacts.append(Contact{
                userId,
                item->text(),
                !item->data(OfflineRole).toBool()
            });
        }
    }
    return contacts;
}

bool Application::removeChatGroup(const QUuid &roomId) {
    auto *item = getRoomItem(roomId);
    if (!item || static_cast<RoomType>(item->data(RoomTypeRole).toInt()) != RoomType::Chatgroup) {
        return false;
    }
    if (m_currentRoomId == roomId) {
        m_currentRoomId = {};
        m_ChatModel = nullptr;
    }
    m_rooms.remove(roomId);
    item->parent()->removeRow(item->row());
    emit roomStatusChanged();
    return true;
}

QMap<QUuid, UserInfo> Application::getRoomMembers(const QUuid &roomId) const {
    if (!m_rooms.contains(roomId)) {
        return {};
    }
    return m_rooms[roomId]->getUserInfos();
}


void Application::removeUser(const LogoutNotificationPacket &logoutNotificationPacket) {
    auto userItem = getUserItem(logoutNotificationPacket.userId);
    if (userItem) {
        qInfo() << Q_FUNC_INFO << "remove user item " << userItem->text();
        userItem->parent()->removeRow(userItem->row());
    }
}


bool Application::setRoomIdOnUser(const QUuid &roomId, const QUuid &userId, bool switchRoomLater) {
    qInfo() << Q_FUNC_INFO;
    for (int categoryRow = Online; categoryRow <= Offline; ++categoryRow) {
        auto userCategoryItem = m_roomListModel.item(categoryRow);
        if (!userCategoryItem) {
            continue;
        }

        for (int row = 0; row < userCategoryItem->rowCount(); ++row) {
            auto item = userCategoryItem->child(row);
            if (item->data(UserIdRole) == userId) {
                item->setData(roomId, RoomIdRole);
                if (switchRoomLater)
                    return switchRoom(item->index()) != nullptr;
                return true;
            }
        }
    }
    return false;
}

void Application::processMessage(const ChatMessagePacket &chatMessagePacket) {
    qInfo() << Q_FUNC_INFO;
    QUuid targetRoomId = chatMessagePacket.roomId;
    // If room is not created, then create the room
    if (!m_rooms.contains(targetRoomId)) {
        qInfo() << Q_FUNC_INFO << "add new RoomId " << chatMessagePacket.roomId;
        QString roomName = (targetRoomId == m_publicRoomId ? "public" : chatMessagePacket.senderName);

        if (roomName != "public") { // room name should not be public since public room should be added at login success
            setRoomIdOnUser(targetRoomId, chatMessagePacket.senderId, false);
        }
        //all received message without room should be from a friend: aka type direct chat
        createRoom(targetRoomId, roomName, RoomType::DirectChat);
    }
    //this is the only place where unread badges are set!
    addChatMessage(targetRoomId, chatMessagePacket);

    // move room upward under user
	// if targetRoomId is the current room, the user's own room, or the public room, do not move it
    if (targetRoomId == m_currentRoomId || targetRoomId == getUserId() || targetRoomId == m_publicRoomId) return;
    auto roomItem = getRoomItem(targetRoomId);
    if (!roomItem) {
        qCritical()<< Q_FUNC_INFO << " room id not found: " << targetRoomId;
        return;
    }
    auto parent = roomItem->parent();
    //TODO: really need a QAbstractItemModel for moving rows (considering pinned rooms).
    //const int destinationRow = qMin(1, parent->rowCount() - 1
    parent->insertRow(1, parent->takeRow(roomItem->row()));
}

std::shared_ptr<ChatRoom> Application::switchRoom(const QModelIndex &index) {
    // check entry category
    if (index.data(Qt::UserRole) == Category) {
        qInfo() << Q_FUNC_INFO << "category item clicked";
        return nullptr;
    }
    // check current chatroom
    QUuid targetRoomId = index.data(RoomIdRole).toUuid();

    if (targetRoomId.isNull()) {
        qInfo() << Q_FUNC_INFO << ": requesting a new roomId";
        QUuid userId = index.data(UserIdRole).toUuid();
        if (userId.isNull()) {
            qCritical() << Q_FUNC_INFO << " :This should not happen!";
        }
        RoomRequestPacket roomRequestPacket{RoomType::DirectChat,{userId}, ""};
        m_client.sendRoomRequest(roomRequestPacket);
        return nullptr;
    }

    if ( targetRoomId != m_currentRoomId) {
        qInfo() << Q_FUNC_INFO << ": setting the current roomId " << targetRoomId;
        m_currentRoomId = targetRoomId;
        // switch the chat room, retrieve the chat history
        if (!m_rooms.contains(targetRoomId)) { // the chatroom hasn't been created yet
            createRoom(targetRoomId,
                index.data(Qt::DisplayRole).toString(),
                static_cast<RoomType>(index.data(RoomTypeRole).toInt()));
        }

        //the room is created, unset its unread badge
        setUnreadBadge(targetRoomId, false);
        auto room = m_rooms[targetRoomId];
        room->clearUnread();
        m_ChatModel = room->getChatModel();
        return room;
    }

    qInfo() << Q_FUNC_INFO << "you are already in the room you selected!";
    return m_rooms[m_currentRoomId];
}


bool Application::exportHistory(const QString &fileName, ExportFormat format) {
    qInfo() << Q_FUNC_INFO;
    switch (format) {
        case ExportFormat::HTML: {
            return m_rooms[m_currentRoomId]->exportHistoryHTML(fileName);
        }
        case ExportFormat::JSON: {
            return m_rooms[m_currentRoomId]->exportHistoryJSON(fileName);
        }
        case ExportFormat::TXT: {
            return m_rooms[m_currentRoomId]->exportHistoryTXT(fileName);
        }
        case ExportFormat::PDF: {
            return m_rooms[m_currentRoomId]->exportHistoryPDF(fileName);
        }
        default:
            return false;
    }
}

void Application::disconnectFromServer() {
    m_client.disconnectFromHost();
}

void Application::createRoom(const QUuid &roomId, const QString &roomName, RoomType roomType, const QMap<QUuid, UserInfo> &userInfos) {
    auto room = std::make_shared<ChatRoom>(roomId, roomName, roomType, 0, m_chatHistoryManager);
    room->setUserInfos(userInfos);
    connect(room.get(), &ChatRoom::historySaved, this, &Application::historySaved);
    room->loadHistory();
    m_rooms.insert(roomId,room);
}

bool Application::setUnreadBadge(const QUuid &roomId, bool unread) {
    qInfo() << Q_FUNC_INFO;
    auto item = getRoomItem(roomId);
    if (!item) {
        qCritical() << Q_FUNC_INFO << "room not found!";
        return false;
    }
    setUnreadBadge(item, unread);
    return true;
}

void Application::setUnreadBadge(QStandardItem *item, bool unread) {
    //item->setIcon(IconCreator::createBadgedIcon(item->icon(), m_iconSize));
    item->setData(unread, UnreadRole);
    emit roomStatusChanged();
}

QStandardItem *Application::setUserOnlineStatus(const QUuid &userId, bool online) {
    for (const auto &room : m_rooms) {
        room->setUserOnlineStatus(userId, online);
    }

    auto userItem = getUserItem(userId);
    if (userItem) {
        if (online) {
            qInfo() << Q_FUNC_INFO << "enable user item " << userItem->text();
            userItem->setData(false, OfflineRole);
            moveUserToGroup(userItem, CategoryType::Online);
        }
        else {
            qInfo() << Q_FUNC_INFO << "disable user item " << userItem->text();
            userItem->setData(true, OfflineRole);
            moveUserToGroup(userItem, CategoryType::Offline);
        }
        return userItem;
    }
    qCritical() << Q_FUNC_INFO << "user item not found!";
    return nullptr;
}

void Application::moveUserToGroup(QStandardItem *userItem, CategoryType ctype) {
    QStandardItem* movedItem = userItem;
    QStandardItem *oldParent = userItem->parent();
    QStandardItem *newParent = m_roomListModel.item(ctype);
    QList<QStandardItem *> row = oldParent->takeRow(userItem->row());
    if (ctype == CategoryType::Online) {
        newParent->appendRow(row);
    }
    else if (ctype == CategoryType::Offline) {
        // move the offline user to the first position
        newParent->insertRow(0, row);
    }
    // emit itemMoved(movedItem);
}

QStandardItem * Application::getRoomItem(const QUuid &roomId) {
    qInfo() << Q_FUNC_INFO;
    if (roomId.isNull()) {
        qCritical() << Q_FUNC_INFO << "given roomId is null";
        return nullptr;
    }
    for (int categoryRow = 0; categoryRow < m_roomListModel.rowCount(); ++categoryRow) {
        auto userCategoryItem = m_roomListModel.item(categoryRow);

        for (int itemRow = 0; itemRow < userCategoryItem->rowCount(); ++itemRow) {
            auto item = userCategoryItem->child(itemRow);
            if (item->data(RoomIdRole).toUuid() == roomId) {
                return item;
            }
        }
    }
    qCritical() << Q_FUNC_INFO << "roomId is not found!";
    return nullptr;
}

QStandardItem * Application::getUserItem(const QUuid &userId) {
    if (userId.isNull()) {
        qCritical() << Q_FUNC_INFO << "given userId is null";
        return nullptr;
    }
    for (int categoryRow = 1; categoryRow < m_roomListModel.rowCount(); ++categoryRow) {
        auto userCategoryItem = m_roomListModel.item(categoryRow);

        for (int itemRow = 0; itemRow < userCategoryItem->rowCount(); ++itemRow) {
            auto item = userCategoryItem->child(itemRow);
            qInfo() << Q_FUNC_INFO << "userId" << item->data(UserIdRole).toUuid();
            if (item->data(UserIdRole).toUuid() == userId) {
                return item;
            }
        }
    }
    qCritical() << Q_FUNC_INFO << "userId" << userId << " is not found!";
    return nullptr;
}
