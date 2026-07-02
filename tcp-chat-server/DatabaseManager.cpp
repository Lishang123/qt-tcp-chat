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

bool DatabaseManager::addUser() {
    return true;
}

bool DatabaseManager::removeUser() {
    return true;
}
