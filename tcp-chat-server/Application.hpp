#ifndef TCP_CHAT_CLIENT_APPLICATION_HPP
#define TCP_CHAT_CLIENT_APPLICATION_HPP

#include "server.h"

class Application : public QObject{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);

private:
    void init();
    Server m_server;
    ChatRoom m_chatRoom;

public:
    [[nodiscard]] Server & getServer() {
        return m_server;
    }

    [[nodiscard]] ChatRoom & getChatRoom() {
        return m_chatRoom;
    }

};



#endif //TCP_CHAT_CLIENT_APPLICATION_HPP
