#include "des.h"

DES::DES(QObject *parent) : QObject(parent) {}

bool DES::setKey(const QByteArray &key) {
    if (key.size() != 8) {
        m_lastError = "Ключ должен быть ровно 8 байт.";
        return false;
    }
    m_key = key;
    return true;
}

void DES::xorBlock(const quint8* in, quint8* out) {
    for (int i = 0; i < 8; ++i)
        out[i] = in[i] ^ static_cast<quint8>(m_key[i]);
}

void DES::encryptBlock(const quint8* input, quint8* output) {
    if (m_key.size() != 8) {
        m_lastError = "Ключ не установлен.";
        return;
    }
    xorBlock(input, output);
}

void DES::decryptBlock(const quint8* input, quint8* output) {
    if (m_key.size() != 8) {
        m_lastError = "Ключ не установлен.";
        return;
    }
    xorBlock(input, output);
}

QString DES::lastError() const {
    return m_lastError;
}
