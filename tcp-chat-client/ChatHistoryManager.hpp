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

/**
 * @brief Saves, loads, and exports per-user chat room history files.
 */
class ChatHistoryManager  {

public:
    /**
     * @brief Creates a history manager rooted under the user's app data folder.
     */
    explicit ChatHistoryManager(QUuid userId);

    /**
     * @brief Saves one room's history to the local binary history file.
     */
    bool saveHistory(ChatRoom& chatRoom);

    /**
     * @brief Loads one room's local binary history file.
     */
    bool loadHistory(ChatRoom& chatRoom);

    /**
     * @brief Exports room history as JSON.
     */
    static bool exportHistoryJSON(ChatRoom& chatRoom, const QString &filepath);

    /**
     * @brief Exports room history as plain text.
     */
    static bool exportHistoryTXT(ChatRoom& chatRoom, const QString &filepath);

    /**
     * @brief Exports room history as HTML.
     */
    static bool exportHistoryHTML(ChatRoom& chatRoom, const QString &filepath);

    /**
     * @brief Exports room history as PDF.
     */
    static bool exportHistoryPDF(ChatRoom& chatRoom, const QString &filepath);

private:
    /**
     * @brief Builds the HTML representation shared by HTML and PDF exports.
     */
    static QString getHTML(ChatRoom& chatRoom);

    QUuid m_userId;
    QString m_historyPath;
};



#endif //QT_TCP_CHAT_CHATHISTORYSAVER_HPP
