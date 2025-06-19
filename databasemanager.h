#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager* getInstance();
    QSqlDatabase getDatabase();

private:
    static DatabaseManager* instance;
    QSqlDatabase db;

    DatabaseManager();  // Приватный конструктор
};

#endif // DATABASEMANAGER_H
