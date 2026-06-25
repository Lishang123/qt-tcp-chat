#include "ChatHistoryManager.hpp"


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
        qCritical() << Q_FUNC_INFO << file.errorString();
        return false;
    }
    QDataStream stream(&file);
    qInfo() << Q_FUNC_INFO <<  "Reading local chat history";
    stream >> chatRoom;;
    file.close();
    qInfo() << Q_FUNC_INFO <<  "Chat history loaded";
    return true;
}
