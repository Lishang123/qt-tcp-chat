#ifndef QT_TCP_CHAT_CHATHISTORYSAVER_HPP
#define QT_TCP_CHAT_CHATHISTORYSAVER_HPP
#include <QUuid>
#include <QFile>
#include <QStandardPaths>
#include <QDir>

#include "ChatRoom.hpp"

class ChatRoom;

class ChatHistoryManager  {

public:
    explicit ChatHistoryManager(QUuid userId);
    bool saveHistory(ChatRoom& chatRoom);

private:
    QUuid m_userId;
    QString m_historyPath;
};



#endif //QT_TCP_CHAT_CHATHISTORYSAVER_HPP
