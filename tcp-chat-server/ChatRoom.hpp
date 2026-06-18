#ifndef TCP_CHAT_CLIENT_CHATROOM_HPP
#define TCP_CHAT_CLIENT_CHATROOM_HPP
#include <iostream>
#include <qobject.h>

#include "client.h"

struct User {
    User(const QUuid &user_id, const QString &username, int room_id)
        : user_id(user_id),
          username(username),
          roomID(room_id) {
    }

    std::string toString() {
        std::ostringstream os;
        os <<  "UUID: " <<  user_id.data1 << std::endl;
        os <<  "username: " << username.toStdString() << std::endl;
        os <<  "roomID: " <<  roomID << std::endl;
        return os.str();
    }

    QUuid user_id;
    QString username;
    int roomID;
};

class ChatRoom : public QObject {
    Q_OBJECT

public:
    explicit ChatRoom(QObject *parent = nullptr);

    size_t getClientsCount();

    void setWelcomeMsg(const QString &newWelcomeMsg);

signals:
    void clientChanged();
    void sendMessageToClient(QUuid clientId, QString& message);
    void broadcast(QString& message);

public slots:
    void handleMessage(const QByteArray& message);
    void removeUser(QUuid userId);
    void addUser(QUuid userId);

private:

    QString m_welcome_msg;
    QMap<QUuid, User> m_users;

};



#endif //TCP_CHAT_CLIENT_CHATROOM_HPP
