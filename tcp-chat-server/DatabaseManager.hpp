#ifndef QT_TCP_CHAT_DATABASEMANAGER_HPP
#define QT_TCP_CHAT_DATABASEMANAGER_HPP
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QFile>

class DatabaseManager {
public:
    DatabaseManager();

    bool createTables();

    bool addUser();
    bool removeUser();

private:

};



#endif //QT_TCP_CHAT_DATABASEMANAGER_HPP
