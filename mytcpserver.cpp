#include "mytcpserver.h"
#include <QDebug>
#include <QCoreApplication>
#include <QString>

#include "databasemanager.h"
#include <QSqlQuery>
#include <QSqlError>

MyTcpServer::~MyTcpServer()
{
    mTcpServer->close();
    //server_status=0;
}

MyTcpServer::MyTcpServer(QObject *parent) : QObject(parent){
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection,
            this, &MyTcpServer::slotNewConnection);

    if(!mTcpServer->listen(QHostAddress::Any, 33333)){
        qDebug() << "server is not started";
    } else {
        //server_status=1;
        qDebug() << "server is started";
    }
}

void MyTcpServer::slotNewConnection(){
    QTcpSocket *clientSocket = mTcpServer->nextPendingConnection();
    clients.append(clientSocket);

    clientSocket->write("Hello, World!!! I am echo server!\r\n");

    connect(clientSocket, &QTcpSocket::readyRead, this, [=](){
        this->slotServerRead(clientSocket);
    });

    connect(clientSocket, &QTcpSocket::disconnected, this, [=](){
        this->slotClientDisconnected(clientSocket);
    });
}

void MyTcpServer::slotServerRead(QTcpSocket* socket){
    QByteArray array = socket->readAll();
    QString input = QString::fromUtf8(array).trimmed();

    qDebug() << "Получена команда:" << input;

    if (input.startsWith("reg ")) {
        QStringList parts = input.mid(4).split(":");
        if (parts.size() != 2) {
            socket->write("Неверный формат. Используй: reg логин:пароль\r\n");
            return;
        }

        QString login = parts[0].trimmed();
        QString password = parts[1].trimmed();
        QString role = "user";
        if (login == "superadmin" && password == "SuperSecure234") {
            role = "admin";
        }

        QSqlDatabase db = DatabaseManager::getInstance()->getDatabase();
        QSqlQuery query;

        query.prepare("SELECT id FROM users WHERE login = :login");
        query.bindValue(":login", login);
        if (query.exec() && query.next()) {
            socket->write("Ошибка: пользователь уже существует\r\n");
            return;
        }

        query.prepare("INSERT INTO users (login, password, role) VALUES (:login, :password, :role)");
        query.bindValue(":login", login);
        query.bindValue(":password", password);
        query.bindValue(":role", role);

        if (query.exec()) {
            socket->write(QString("Регистрация успешна. Роль: %1\r\n").arg(role).toUtf8());
        } else {
            socket->write("Ошибка при регистрации\r\n");
        }

        return;
    }

    else if (input == "list_users") {
        QSqlDatabase db = DatabaseManager::getInstance()->getDatabase();
        QSqlQuery query("SELECT login, role FROM users");

        QStringList users;
        while (query.next()) {
            QString login = query.value(0).toString();
            QString role = query.value(1).toString();
            users << QString("%1 (%2)").arg(login, role);
        }

        QString response;
        if (!users.isEmpty()) {
            response = "Список пользователей:\n" + users.join("\n") + "\r\n";
        } else {
            response = "Нет зарегистрированных пользователей\r\n";
        }

        socket->write(response.toUtf8());
        return;
    }

    if (input.startsWith("auth ")) {
        QStringList parts = input.mid(5).split(":");
        if (parts.size() != 2) {
            socket->write("Неверный формат. Используй: auth логин:пароль\r\n");
            return;
        }

        QString login = parts[0].trimmed();
        QString password = parts[1].trimmed();

        QSqlDatabase db = DatabaseManager::getInstance()->getDatabase();
        QSqlQuery query;

        query.prepare("SELECT password, role FROM users WHERE login = :login");
        query.bindValue(":login", login);
        if (query.exec() && query.next()) {
            QString storedPassword = query.value(0).toString();
            QString role = query.value(1).toString();
            if (storedPassword == password) {
                socket->write(QString("Авторизация успешна. Роль: %1\r\n").arg(role).toUtf8());
            } else {
                socket->write("Ошибка: неверный пароль\r\n");
            }
        } else {
            socket->write("Ошибка: пользователь не найден\r\n");
        }

        return;
    }

    if (input.startsWith("enc")) {
        QSqlDatabase db = DatabaseManager::getInstance()->getDatabase();
        QSqlQuery query;
        query.prepare("INSERT INTO messages (message, aes_key, sha_hash, image_path) "
                      "VALUES (:msg, :key, :hash, :img)");
        query.bindValue(":msg", input);               // Пока сохраняем просто введённый текст
        query.bindValue(":key", "demo_key");          // Заглушка для AES-ключа
        query.bindValue(":hash", "demo_hash");        // Заглушка для SHA-384
        query.bindValue(":img", "demo_path.png");     // Путь к изображению (заглушка)
        if (query.exec()) {
            socket->write("ENC: Сообщение сохранено в БД\r\n");
        } else {
            socket->write("Ошибка при сохранении в БД\r\n");
            qDebug() << "Ошибка БД:" << query.lastError().text();
        }
    }

    else if (input.startsWith("hash")) {
        socket->write("HASH: заглушка. Хэш пока не реализован.\r\n");
    }
    else if (input.startsWith("solve")) {
        socket->write("SOLVE: заглушка. Решатель ещё не готов.\r\n");
    }
    else if (input.startsWith("hide")) {
        socket->write("HIDE: заглушка. Пока не встраиваем в картинки.\r\n");
    }
    else {
        socket->write("Неизвестная команда\r\n");
    }
}

void MyTcpServer::slotClientDisconnected(QTcpSocket* socket){
    clients.removeAll(socket);
    socket->deleteLater();
}
