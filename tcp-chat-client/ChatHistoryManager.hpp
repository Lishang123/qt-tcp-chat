#ifndef QT_TCP_CHAT_CHATHISTORYSAVER_HPP
#define QT_TCP_CHAT_CHATHISTORYSAVER_HPP
#include <QUuid>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonArray>
#include "ChatRoom.hpp"

class ChatRoom;

class ChatHistoryManager  {

public:
    explicit ChatHistoryManager(QUuid userId);
    bool saveHistory(ChatRoom& chatRoom);
    bool loadHistory(ChatRoom& chatRoom);
    bool exportHistoryJSON(ChatRoom& chatRoom, const QString &filepath);
    bool exportHistoryTXT(ChatRoom& chatRoom, const QString &filepath);
    bool exportHistoryHTML(ChatRoom& chatRoom, const QString &filepath);
    bool exportHistoryPDF(ChatRoom& chatRoom, const QString &filepath);

private:
    QUuid m_userId;
    QString m_historyPath;
};



#endif //QT_TCP_CHAT_CHATHISTORYSAVER_HPP
