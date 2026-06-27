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
#include <QPrinter>
#include <QTextDocument>

#include "ChatRoom.hpp"

class ChatRoom;

class ChatHistoryManager  {

public:
    explicit ChatHistoryManager(QUuid userId);
    bool saveHistory(ChatRoom& chatRoom);
    bool loadHistory(ChatRoom& chatRoom);

    static bool exportHistoryJSON(ChatRoom& chatRoom, const QString &filepath);
    static bool exportHistoryTXT(ChatRoom& chatRoom, const QString &filepath);
    static bool exportHistoryHTML(ChatRoom& chatRoom, const QString &filepath);
    static bool exportHistoryPDF(ChatRoom& chatRoom, const QString &filepath);

private:
    static QString getHTML(ChatRoom& chatRoom);

    QUuid m_userId;
    QString m_historyPath;
};



#endif //QT_TCP_CHAT_CHATHISTORYSAVER_HPP
