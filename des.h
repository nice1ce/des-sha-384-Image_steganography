#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>

/**
 * @brief Класс DES реализует алгоритм Data Encryption Standard (режим ECB)
 */
class DES : public QObject {
    Q_OBJECT

public:
    explicit DES(QObject *parent = nullptr);

    bool setKey(const QByteArray &key); // ключ должен быть 8 байт
    void encryptBlock(const quint8* input, quint8* output);
    void decryptBlock(const quint8* input, quint8* output);
    QString lastError() const;

private:
    QByteArray m_key;
    QString m_lastError;

    void xorBlock(const quint8* in, quint8* out); // временная XOR-заглушка
};
