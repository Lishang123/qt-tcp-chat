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

    // Construct JSON document.
    QJsonObject root;
    // if (chatRoom.getRoomType() == RoomType::Self)
    root["room name"] = chatRoom.getRoomName();
    QJsonArray messages;
    std::ranges::for_each(chatRoom.getChatMessages(), [&messages](const ChatMessagePacket &chatMessagePacket) {
        QJsonObject msg;
        msg["sender ID"] = chatMessagePacket.senderId.toString();
        msg.insert("sender name", chatMessagePacket.senderName);
        msg.insert("timestamp", chatMessagePacket.timestamp.toString("yyyy-MM-dd HH:mm:ss"));
        msg.insert("message", chatMessagePacket.text);
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
    return true;
}

bool ChatHistoryManager::exportHistoryTXT(ChatRoom &chatRoom, const QString &filepath) {
    return true;
}

bool ChatHistoryManager::exportHistoryHTML(ChatRoom &chatRoom, const QString &filepath) {
    return true;
}

bool ChatHistoryManager::exportHistoryPDF(ChatRoom &chatRoom, const QString &filepath) {
    return true;
}
