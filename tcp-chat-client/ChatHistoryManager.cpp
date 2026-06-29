#include "ChatHistoryManager.hpp"

#include "../common/ChatMessagePacket.hpp"


ChatHistoryManager::ChatHistoryManager(QUuid userId): m_userId(userId) {
    // create history folder
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_historyPath = dataPath + "/history/" + m_userId.toString() + "/";
    if (!QDir().mkpath(m_historyPath)) {
        qCritical() << Q_FUNC_INFO << "Unable to create history directory";
    }
}

bool ChatHistoryManager::saveHistory(ChatRoom& chatRoom) {
    QString filename = m_historyPath +  chatRoom.getRoomId().toString() + ".dat";

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << Q_FUNC_INFO << file.errorString();
        return false;
    }

    QDataStream stream(&file);

    qInfo() << Q_FUNC_INFO <<  "Writing history";
    stream << chatRoom;;
    if(!file.flush())
    {
        qInfo() << file.errorString();
        file.close();
        return false;
    }
    qInfo() << Q_FUNC_INFO <<  "Chat history written in " << filename;

    return true;
}

bool ChatHistoryManager::loadHistory(ChatRoom &chatRoom) {
    QString filename = m_historyPath +  chatRoom.getRoomId().toString() + ".dat";
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << Q_FUNC_INFO << file.errorString() << ", filename: " << filename;
        return false;
    }
    QDataStream stream(&file);
    qInfo() << Q_FUNC_INFO <<  "Reading local chat history";
    stream >> chatRoom;;
    file.close();
    qInfo() << Q_FUNC_INFO <<  "Chat history loaded";
    return true;
}

bool ChatHistoryManager::exportHistoryJSON(ChatRoom &chatRoom, const QString &filepath) {
    qInfo() << Q_FUNC_INFO;
    // Construct JSON document.
    QJsonObject root;
    // if (chatRoom.getRoomType() == RoomType::Self)
    root["room name"] = chatRoom.getRoomName();
    QJsonArray messages;
    std::ranges::for_each(chatRoom.getChatMessages(), [&messages](const ChatMessage &chatMessage) {
        QJsonObject msg;
        msg.insert("sender name", chatMessage.senderName);
        msg.insert("timestamp", chatMessage.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
        msg.insert("message", chatMessage.text);
        messages.append(msg);
    });
    root["messages"] = messages;

    //Save to file
    QJsonDocument document(root);
    QFile file(filepath);
    if(!file.open(QIODevice::WriteOnly)) {
        qCritical() << file.errorString();
        return false;
    }
    file.write(document.toJson());
    file.close();
    qInfo() << Q_FUNC_INFO << "Chat history exported";
    return true;
}

bool ChatHistoryManager::exportHistoryTXT(ChatRoom &chatRoom, const QString &filepath) {
    qInfo() << Q_FUNC_INFO;
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << Q_FUNC_INFO << file.errorString();
    }
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << "room name: " << chatRoom.getRoomName() << "\n\n";
    stream << "messages:" << "\n";

    std::ranges::for_each(chatRoom.getChatMessages(), [&stream](const ChatMessage &chatMessage) {
        stream << chatMessage.getMessage() << "\n";
    });

    file.close();
    qInfo() << Q_FUNC_INFO <<  "Chat history exported";
    return true;
}

bool ChatHistoryManager::exportHistoryHTML(ChatRoom &chatRoom, const QString &filepath) {
    qInfo() << Q_FUNC_INFO;
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly)) {
        qCritical() << file.errorString();
    }
    QTextStream stream(&file);
    stream << getHTML(chatRoom);
    qInfo() << Q_FUNC_INFO << "Chat history exported";
    return true;
}

bool ChatHistoryManager::exportHistoryPDF(ChatRoom &chatRoom, const QString &filepath) {
    qInfo() << Q_FUNC_INFO;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(filepath);

    QTextDocument doc;
    doc.setHtml(getHTML(chatRoom));
    doc.print(&printer);
    qInfo() << Q_FUNC_INFO << "Chat history exported";
    return true;
}

QString ChatHistoryManager::getHTML(ChatRoom &chatRoom) {
    /*
     *
       <!DOCTYPE html>
       <html>
       <head>
       <style>
       body {
       font-family: sans-serif;
       }

       .time {
       color: gray;
       font-size: 80%;
       }

       .sender {
       font-weight: bold;
       }
       </style>
       </head>
       <body>

       <p>
       <span class="sender">Alice</span>
       <span class="time">2026-06-27 20:31</span><br>
       Hello!
       </p>

       </body>
       </html>
   */
    QString out;
    QTextStream stream(&out);
    stream << "<html>";
    stream << "<body>\n";
    for (const auto &chatMessage : chatRoom.getChatMessages()) {
        stream << "<p>";
        stream << "<b>" << chatMessage.senderName.toHtmlEscaped() << "</b> ";
        stream << "(" << chatMessage.timestamp.toString("yyyy-MM-dd HH:mm:ss").toHtmlEscaped() << ")<br>";
        stream << chatMessage.text.toHtmlEscaped();
        stream << "</p>\n";
    }
    stream << "</body>\n";
    stream << "</html>";
    return out;
}
