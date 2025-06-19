#pragma once

#include <QTcpSocket>
#include <QObject>

/**
 * @class MyClient
 * @brief Класс TCP клиента для работы в GUI приложении
 *
 * @details Реализует:
 * - Подключение к серверу
 * - Отправку команд
 * - Чтение ответа и генерацию сигнала в интерфейс
 * - Реализацию паттерна Singleton
 */
class MyClient : public QObject {
    Q_OBJECT

signals:
    /**
     * @brief Сигнал получения ответа от сервера
     * @param message Текст ответа от сервера
     */
    void responseReceived(const QString& message);

public:
    static MyClient* getInstance();

    void connectToServer();
    void sendCommand(const QString& command);
    QTcpSocket* getSocket();

    // Новый метод: получить последний ответ от сервера
    QString getLastResponse() const;

private slots:
    void onConnected();
    void onReadyRead();
    void onDisconnected();

private:
    explicit MyClient(QObject* parent = nullptr);

    QTcpSocket* socket;
    static MyClient* instance;

    QString lastResponse;  // 🆕 переменная для хранения последнего ответа
};
