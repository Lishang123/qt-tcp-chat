#include "DatabaseManager.hpp"

#include <QDir>
#include <QStandardPaths>


DatabaseManager::DatabaseManager() {
    //initialize database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir().mkpath(dbDir)) {
        qDebug() << Q_FUNC_INFO << ": cannot create directory " << dbDir;
        return;
    }
    QString dbPath = dbDir + "/chat-server.db";
    db.setDatabaseName(dbPath);
    // QSqlDatabase::open doesn't create the parent folder for you if it doesn't exist!
    if (!db.open()) {
        qDebug() << db.lastError() << Q_FUNC_INFO;
        return;
    }
    createTables();
}

bool DatabaseManager::createTables() {
    QSqlQuery query;
    QFile file(":/sql/schema.sql");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << file.errorString();
        return false;
    }

    // Don't do this, it's hard to debug...
    // if (!query.exec(file.readAll())) {
    //     qDebug() << query.lastError() << Q_FUNC_INFO;
    //     return false;
    // }

    QString sql = file.readAll();
    QStringList statements = sql.split(';', Qt::SkipEmptyParts);
    for (QString statement : statements) {
        statement = statement.trimmed();
        if (statement.isEmpty())
            continue;
        qDebug().noquote() << "Executing:\n" << statement;
        if (!query.exec(statement)) {
            qDebug() << query.lastError();
            break;
        }
    }
    return true;
}

bool DatabaseManager::addUser(const User& user) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO user (id, username, password)
        VALUES (:id, :username, :password)
    )");
    query.bindValue(":id", user.user_id);
    query.bindValue(":username", user.username);
    query.bindValue(":password", QVariant());
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::removeUser(const QUuid& userId) {
    QSqlQuery query;
    query.prepare(R"(
        DELETE FROM user WHERE id = ?
    )");
    query.addBindValue(userId.toString());
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::addRoom(const ChatRoom &room) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO room (id, room_name, room_type)
        VALUES (:id, :room_name, :room_type)
    )");
    query.bindValue(":id", room.getRoomId().toString());
    query.bindValue(":room_name", room.getRoomName());
    query.bindValue(":room_type", static_cast<int>(room.getRoomType()));
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::removeRoom(const QUuid &roomId) {
    QSqlQuery query;
    query.prepare(R"(
        DELETE FROM room where id = ?
    )");
    query.addBindValue(roomId);
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::addRoomMember(const QUuid &roomId, const QUuid &userId) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO room_member (room_id, user_id)
        VALUES (:room_id, :user_id)
    )");
    query.bindValue(":room_id", roomId);
    query.bindValue(":user_id", userId);
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::removeRoomMember(const QUuid &roomId, const QUuid &userId) {
    QSqlQuery query;
    query.prepare(R"(
        DELETE FROM room_member WHERE room_id = ? AND user_id = ?
    )");
    query.addBindValue(roomId);
    query.addBindValue(userId);
    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}

bool DatabaseManager::addMessage(const QUuid &messageId, const QUuid &senderId, const QUuid &roomId,
                                 const QString &content, const QDateTime &sentAt) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO message (id, sender_id, room_id, content, sent_at)
        VALUES (:id, :sender_id, :room_id, :content, :sent_at)
    )");
    query.bindValue(":id", messageId);
    query.bindValue(":sender_id", senderId);
    query.bindValue(":room_id", roomId);
    query.bindValue(":content", content);
    query.bindValue(":sent_at", sentAt.toSecsSinceEpoch());

    if (!query.exec()) {
        qDebug() << query.lastError() << Q_FUNC_INFO;
        return false;
    }
    return true;
}
