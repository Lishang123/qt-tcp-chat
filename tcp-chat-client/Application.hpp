#ifndef QT_TCP_CHAT_APPLICATION_HPP
#define QT_TCP_CHAT_APPLICATION_HPP
#include <QObject>
#include <QStringListModel>

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

    [[nodiscard]] Client& getClient() {
        return m_client;
    }

    [[nodiscard]] QStringList& getList() {
        return m_list;
    }

    [[nodiscard]] QStringListModel& getModel() {
        return m_model;
    }

    [[nodiscard]] quint8 getRoomId()  {
        return m_room_id;
    }

private:
    void init();
    Client m_client;
    QStringList m_list;
    QStringListModel m_model;
    quint8 m_room_id = 0;

    // ChatModel m_chatModel;
    // UserListModel m_userListModel;
};

#endif //QT_TCP_CHAT_APPLICATION_HPP
