#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include "des.h"

/**
 * @brief Класс DESCipher — Qt-обёртка для работы с DES
 */
class DESCipher : public QObject {
    Q_OBJECT

public:
    explicit DESCipher(QObject *parent = nullptr);

    bool setKey(const QByteArray &key); // 8 байт
    QByteArray encrypt(const QByteArray &data);
    QByteArray decrypt(const QByteArray &data);

    QString encryptToBase64(const QString &text);
    QString decryptFromBase64(const QString &base64);
    QString encryptToHex(const QString &text);
    QString decryptFromHex(const QString &hex);

    QString lastError() const;

private:
    DES m_des;
    QByteArray m_key;
    QString m_lastError;

    QByteArray padData(const QByteArray &data);
    QByteArray unpadData(const QByteArray &data);
};
