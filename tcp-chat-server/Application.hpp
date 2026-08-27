#ifndef TCP_CHAT_CLIENT_APPLICATION_HPP
#define TCP_CHAT_CLIENT_APPLICATION_HPP

#include "RoomManager.hpp"
#include "server.h"

/**
 * @brief Server application object that wires the TCP server to room management.
 */
class Application : public QObject{
    Q_OBJECT

public:
    /**
     * @brief Constructs and initializes the server application.
     */
    explicit Application(QObject *parent = nullptr);

    /**
     * @brief Returns the TCP server instance.
     */
    [[nodiscard]] Server & getServer() {
        return m_server;
    }

    /**
     * @brief Returns the room manager instance.
     */
    [[nodiscard]] RoomManager & getChatRooms() {
        return m_roomManager;
    }


private:
    /**
     * @brief Connects server and room-manager signals and slots.
     */
    void init();
    Server m_server;
    RoomManager m_roomManager;
};



#endif //TCP_CHAT_CLIENT_APPLICATION_HPP
