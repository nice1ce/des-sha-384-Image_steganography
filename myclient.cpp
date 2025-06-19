#include "myclient.h"
#include <QDebug>

MyClient* MyClient::instance = nullptr;

MyClient::MyClient(QObject* parent) : QObject(parent) {
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &MyClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &MyClient::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &MyClient::onDisconnected);
}

MyClient* MyClient::getInstance() {
    if (!instance) {
        instance = new MyClient();
    }
    return instance;
}

void MyClient::connectToServer() {
    qDebug() << "🔌 Подключение к серверу...";
    socket->connectToHost("127.0.0.1", 33333);
}

void MyClient::sendCommand(const QString& command) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(command.toUtf8() + "\r\n");
        qDebug() << "📤 Команда отправлена:" << command;
    } else {
        qDebug() << "❌ Не удалось отправить команду. Нет соединения.";
    }
}

void MyClient::onConnected() {
    qDebug() << "✅ Подключено к серверу";
}

void MyClient::onReadyRead() {
    QByteArray response = socket->readAll();
    QString text = QString::fromUtf8(response).trimmed().replace("\r", "");

    lastResponse = text;  // 🆕 сохранить последний ответ
    emit responseReceived(text);
}

void MyClient::onDisconnected() {
    qDebug() << "❌ Отключено от сервера";
}

QTcpSocket* MyClient::getSocket() {
    return socket;
}

QString MyClient::getLastResponse() const {
    return lastResponse;
}
