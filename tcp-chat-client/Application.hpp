#ifndef QT_TCP_CHAT_APPLICATION_HPP
#define QT_TCP_CHAT_APPLICATION_HPP
#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>

#include "Client.hpp"

class Application : public QObject{
    Q_OBJECT

public:

    explicit Application(QObject *parent = nullptr);

    void sendLoginRequest(const LoginRequestPacket &loginRequestPacket);

    void connectToServer(const QString &address, quint16 port);
    void disconnectFromHost();

    void addChatMessage(const QString &message);

    void sendMessage(const QString& message);

    void updateRooms();

    [[nodiscard]] Client& getClient() {
        return m_client;
    }

    [[nodiscard]] QStringList& getList() {
        return m_list;
    }

    [[nodiscard]] QStringListModel& getChatModel() {
        return m_chatModel;
    }

    [[nodiscard]] QStandardItemModel& getRoomListModel() {
        return m_roomListModel;
    }

    [[nodiscard]] QUuid getRoomId()  {
        return m_currentRoomId;
    }

    void setRoomId(QUuid m_room_id) {
        this->m_currentRoomId = m_room_id;
    }

    void setRoomInfos(const QList<RoomInfo> &m_room_infos) {
        m_roomInfos = m_room_infos;
    }

    QList<RoomInfo>& getRoomInfos() {
        return m_roomInfos;
    }

private:
    Client m_client;
    QStringList m_list;
    QStringListModel m_chatModel;
    QUuid m_currentRoomId;
    QList<RoomInfo> m_roomInfos;
    // chatModel m_chatModel;
    QStandardItemModel m_roomListModel;
};

#endif //QT_TCP_CHAT_APPLICATION_HPP
