#ifndef TCP_CHAT_CLIENT_APPLICATION_HPP
#define TCP_CHAT_CLIENT_APPLICATION_HPP

#include "RoomManager.hpp"
#include "server.h"

class Application : public QObject{
    Q_OBJECT

public:
    explicit Application(QObject *parent = nullptr);

    [[nodiscard]] Server & getServer() {
        return m_server;
    }

    [[nodiscard]] RoomManager & getChatRooms() {
        return m_roomManager;
    }


private:
    void init();
    Server m_server;
    RoomManager m_roomManager;
};



#endif //TCP_CHAT_CLIENT_APPLICATION_HPP
