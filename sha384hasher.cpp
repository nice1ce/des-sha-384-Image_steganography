#include "sha384hasher.h"
#include <QDebug>

SHA384Hasher::SHA384Hasher(QObject *parent) : QObject(parent)
{
}

QByteArray SHA384Hasher::hash(const QByteArray &data)
{
    if (data.isEmpty()) {
        m_lastError = "Пустые данные для хеширования.";
        return QByteArray();
    }

    // Вычисляем хеш SHA-384
    uint8_t hash[48]; // SHA-384 дает 384 бита = 48 байт
    m_sha384.hash(reinterpret_cast<const uint8_t*>(data.constData()), data.size(), hash);
    
    // Преобразуем результат в QByteArray
    return QByteArray(reinterpret_cast<const char*>(hash), 48);
}

QByteArray SHA384Hasher::hashText(const QString &text)
{
    return hash(text.toUtf8());
}

QByteArray SHA384Hasher::hashFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_lastError = QString("Не удалось открыть файл: %1").arg(file.errorString());
        return QByteArray();
    }

    // Для больших файлов читаем и хешируем по частям
    SHA384 sha;
    const qint64 bufferSize = 8192; // 8 КБ
    QByteArray buffer;
    
    while (!file.atEnd()) {
        buffer = file.read(bufferSize);
        sha.update(reinterpret_cast<const uint8_t*>(buffer.constData()), buffer.size());
    }
    
    uint8_t hash[48]; // SHA-384 дает 384 бита = 48 байт
    sha.finalize(hash);
    
    file.close();
    
    // Преобразуем результат в QByteArray
    return QByteArray(reinterpret_cast<const char*>(hash), 48);
}

QString SHA384Hasher::hashToHex(const QByteArray &data)
{
    QByteArray hashData = hash(data);
    if (hashData.isEmpty()) {
        return QString();
    }
    
    return hashData.toHex();
}

QString SHA384Hasher::hashTextToHex(const QString &text)
{
    return hashToHex(text.toUtf8());
}

QString SHA384Hasher::hashFileToHex(const QString &filePath)
{
    QByteArray hashData = hashFile(filePath);
    if (hashData.isEmpty()) {
        return QString();
    }
    
    return hashData.toHex();
}

QString SHA384Hasher::hashToBase64(const QByteArray &data)
{
    QByteArray hashData = hash(data);
    if (hashData.isEmpty()) {
        return QString();
    }
    
    return hashData.toBase64();
}

QString SHA384Hasher::hashTextToBase64(const QString &text)
{
    return hashToBase64(text.toUtf8());
}

QString SHA384Hasher::hashFileToBase64(const QString &filePath)
{
    QByteArray hashData = hashFile(filePath);
    if (hashData.isEmpty()) {
        return QString();
    }
    
    return hashData.toBase64();
}

bool SHA384Hasher::verify(const QByteArray &data, const QByteArray &expectedHash)
{
    QByteArray computedHash = hash(data);
    if (computedHash.isEmpty()) {
        return false;
    }
    
    return computedHash == expectedHash;
}

bool SHA384Hasher::verifyText(const QString &text, const QString &expectedHash)
{
    // Предполагаем, что expectedHash в шестнадцатеричном формате
    QByteArray expectedHashData = QByteArray::fromHex(expectedHash.toLatin1());
    return verify(text.toUtf8(), expectedHashData);
}

bool SHA384Hasher::verifyFile(const QString &filePath, const QString &expectedHash)
{
    // Предполагаем, что expectedHash в шестнадцатеричном формате
    QByteArray expectedHashData = QByteArray::fromHex(expectedHash.toLatin1());
    QByteArray fileHash = hashFile(filePath);
    
    if (fileHash.isEmpty()) {
        return false;
    }
    
    return fileHash == expectedHashData;
}

QString SHA384Hasher::lastError() const
{
    return m_lastError;
}

