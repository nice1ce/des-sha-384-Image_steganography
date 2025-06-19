#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DatabaseManager* DatabaseManager::instance = nullptr;

DatabaseManager::DatabaseManager() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("project_data.db");

    if (!db.open()) {
        qDebug() << "Не удалось открыть БД:" << db.lastError().text();
    } else {
        qDebug() << "База данных успешно открыта.";

        QSqlQuery query;
        query.exec("CREATE TABLE IF NOT EXISTS messages ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "message TEXT, "
                   "aes_key TEXT, "
                   "sha_hash TEXT, "
                   "image_path TEXT)");
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "login TEXT UNIQUE, "
                   "password TEXT, "
                   "role TEXT)");

    }
}

DatabaseManager* DatabaseManager::getInstance() {
    if (!instance)
        instance = new DatabaseManager();
    return instance;
}

QSqlDatabase DatabaseManager::getDatabase() {
    return db;
}
