#include "descipher.h"

DESCipher::DESCipher(QObject *parent) : QObject(parent) {}

bool DESCipher::setKey(const QByteArray &key) {
    if (key.size() != 8) {
        m_lastError = "Ключ должен быть 8 байт.";
        return false;
    }
    m_key = key;
    if (!m_des.setKey(key)) {
        m_lastError = m_des.lastError();
        return false;
    }
    return true;
}

QByteArray DESCipher::padData(const QByteArray &data) {
    int padLen = 8 - (data.size() % 8);
    QByteArray padded = data;
    padded.append(QByteArray(padLen, static_cast<char>(padLen)));
    return padded;
}

QByteArray DESCipher::unpadData(const QByteArray &data) {
    if (data.isEmpty()) return {};
    int padLen = static_cast<int>(data[data.size() - 1]);
    if (padLen <= 0 || padLen > 8) {
        m_lastError = "Неверное дополнение.";
        return {};
    }
    for (int i = data.size() - padLen; i < data.size(); ++i) {
        if (static_cast<int>(data[i]) != padLen) {
            m_lastError = "Неверное дополнение.";
            return {};
        }
    }
    return data.left(data.size() - padLen);
}

QByteArray DESCipher::encrypt(const QByteArray &data) {
    if (m_key.size() != 8) {
        m_lastError = "Ключ не установлен.";
        return {};
    }

    QByteArray input = padData(data);
    QByteArray output(input.size(), 0);

    for (int i = 0; i < input.size(); i += 8) {
        m_des.encryptBlock(
            reinterpret_cast<const quint8*>(input.constData() + i),
            reinterpret_cast<quint8*>(output.data() + i)
            );
    }

    return output;
}

QByteArray DESCipher::decrypt(const QByteArray &data) {
    if (m_key.size() != 8 || data.size() % 8 != 0) {
        m_lastError = "Неверный ключ или данные.";
        return {};
    }

    QByteArray output(data.size(), 0);
    for (int i = 0; i < data.size(); i += 8) {
        m_des.decryptBlock(
            reinterpret_cast<const quint8*>(data.constData() + i),
            reinterpret_cast<quint8*>(output.data() + i)
            );
    }

    return unpadData(output);
}

QString DESCipher::encryptToBase64(const QString &text) {
    return encrypt(text.toUtf8()).toBase64();
}

QString DESCipher::decryptFromBase64(const QString &base64) {
    return QString::fromUtf8(decrypt(QByteArray::fromBase64(base64.toLatin1())));
}

QString DESCipher::encryptToHex(const QString &text) {
    return encrypt(text.toUtf8()).toHex();
}

QString DESCipher::decryptFromHex(const QString &hex) {
    return QString::fromUtf8(decrypt(QByteArray::fromHex(hex.toLatin1())));
}

QString DESCipher::lastError() const {
    return m_lastError;
}
