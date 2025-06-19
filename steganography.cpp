/**
 * @file steganography.cpp
 * @brief Реализация класса Steganography для внедрения и извлечения сообщений из изображений
 * @date 2025
 */

#include "steganography.h"
#include <QRandomGenerator>
#include <QDateTime>

Steganography::Steganography(QObject *parent) : QObject(parent)
{
}

Steganography::~Steganography()
{
}

QImage Steganography::embedMessage(const QImage &image, const QString &message)
{
    // Проверка входных данных
    if (image.isNull()) {
        m_lastError = "Изображение пустое";
        return QImage();
    }
    
    if (message.isEmpty()) {
        m_lastError = "Сообщение пустое";
        return QImage();
    }
    
    // Проверка формата изображения
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        QImage convertedImage = image.convertToFormat(QImage::Format_ARGB32);
        return embedMessage(convertedImage, message);
    }
    
    // Проверка размера изображения
    int maxBytes = (image.width() * image.height() * 3) / 8 - HEADER_SIZE;
    QByteArray messageData = message.toUtf8();
    if (messageData.size() > maxBytes) {
        m_lastError = QString("Сообщение слишком большое для этого изображения. Максимальный размер: %1 байт").arg(maxBytes);
        return QImage();
    }
    
    // Создание копии изображения для внедрения
    QImage resultImage = image.copy();
    
    // Внедрение данных
    if (!embedData(resultImage, messageData)) {
        return QImage();
    }
    
    return resultImage;
}

QString Steganography::extractMessage(const QImage &image)
{
    // Проверка входных данных
    if (image.isNull()) {
        m_lastError = "Изображение пустое";
        return QString();
    }
    
    // Проверка формата изображения
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        QImage convertedImage = image.convertToFormat(QImage::Format_ARGB32);
        return extractMessage(convertedImage);
    }
    
    // Проверка наличия внедренного сообщения
    if (!hasEmbeddedMessage(image)) {
        m_lastError = "Изображение не содержит внедренного сообщения";
        return QString();
    }
    
    // Извлечение данных
    QByteArray data = extractData(image);
    if (data.isEmpty()) {
        return QString();
    }
    
    return QString::fromUtf8(data);
}

bool Steganography::hasEmbeddedMessage(const QImage &image)
{
    // Проверка входных данных
    if (image.isNull()) {
        m_lastError = "Изображение пустое";
        return false;
    }
    
    // Проверка формата изображения
    if (image.format() != QImage::Format_RGB32 && image.format() != QImage::Format_ARGB32) {
        QImage convertedImage = image.convertToFormat(QImage::Format_ARGB32);
        return hasEmbeddedMessage(convertedImage);
    }
    
    // Проверка размера изображения
    if (image.width() * image.height() < 50) {
        m_lastError = "Изображение слишком маленькое";
        return false;
    }
    
    // Извлечение сигнатуры
    int x = 0, y = 0;
    QByteArray signature;
    for (int i = 0; i < 5; i++) { // 5 байт для сигнатуры "STEGA"
        signature.append(extractByte(image, x, y));
    }
    
    return signature == SIGNATURE;
}

QString Steganography::lastError() const
{
    return m_lastError;
}

void Steganography::embedByte(QImage &image, int &x, int &y, unsigned char byte)
{
    for (int i = 0; i < 8; i++) {
        // Получение бита из байта
        int bit = (byte >> i) & 1;
        
        // Получение цвета пикселя
        QRgb pixel = image.pixel(x, y);
        int r = qRed(pixel);
        int g = qGreen(pixel);
        int b = qBlue(pixel);
        
        // Внедрение бита в младший бит цветового канала
        // Используем разные каналы по очереди
        switch (i % 3) {
            case 0: // Красный канал
                r = (r & 0xFE) | bit;
                break;
            case 1: // Зеленый канал
                g = (g & 0xFE) | bit;
                break;
            case 2: // Синий канал
                b = (b & 0xFE) | bit;
                break;
        }
        
        // Установка нового цвета пикселя
        image.setPixel(x, y, qRgb(r, g, b));
        
        // Переход к следующему пикселю
        x++;
        if (x >= image.width()) {
            x = 0;
            y++;
            if (y >= image.height()) {
                // Достигнут конец изображения
                return;
            }
        }
    }
}

unsigned char Steganography::extractByte(const QImage &image, int &x, int &y)
{
    unsigned char byte = 0;
    
    for (int i = 0; i < 8; i++) {
        // Проверка границ изображения
        if (x >= image.width() || y >= image.height()) {
            return 0;
        }
        
        // Получение цвета пикселя
        QRgb pixel = image.pixel(x, y);
        int r = qRed(pixel);
        int g = qGreen(pixel);
        int b = qBlue(pixel);
        
        // Извлечение бита из младшего бита цветового канала
        int bit = 0;
        switch (i % 3) {
            case 0: // Красный канал
                bit = r & 1;
                break;
            case 1: // Зеленый канал
                bit = g & 1;
                break;
            case 2: // Синий канал
                bit = b & 1;
                break;
        }
        
        // Установка бита в байт
        byte |= (bit << i);
        
        // Переход к следующему пикселю
        x++;
        if (x >= image.width()) {
            x = 0;
            y++;
        }
    }
    
    return byte;
}

bool Steganography::embedData(QImage &image, const QByteArray &data)
{
    // Проверка размера изображения
    int maxBytes = (image.width() * image.height() * 3) / 8 - HEADER_SIZE;
    if (data.size() > maxBytes) {
        m_lastError = QString("Данные слишком большие для этого изображения. Максимальный размер: %1 байт").arg(maxBytes);
        return false;
    }
    
    int x = 0, y = 0;
    
    // Внедрение сигнатуры
    for (int i = 0; i < SIGNATURE.size(); i++) {
        embedByte(image, x, y, SIGNATURE[i]);
    }
    
    // Внедрение размера данных (4 байта)
    int dataSize = data.size();
    embedByte(image, x, y, (dataSize >> 24) & 0xFF);
    embedByte(image, x, y, (dataSize >> 16) & 0xFF);
    embedByte(image, x, y, (dataSize >> 8) & 0xFF);
    embedByte(image, x, y, dataSize & 0xFF);
    
    // Внедрение данных
    for (int i = 0; i < data.size(); i++) {
        embedByte(image, x, y, data[i]);
    }
    
    return true;
}

QByteArray Steganography::extractData(const QImage &image)
{
    int x = 0, y = 0;
    
    // Извлечение сигнатуры
    QByteArray signature;
    for (int i = 0; i < SIGNATURE.size(); i++) {
        signature.append(extractByte(image, x, y));
    }
    
    // Проверка сигнатуры
    if (signature != SIGNATURE) {
        m_lastError = "Изображение не содержит внедренного сообщения или формат не поддерживается";
        return QByteArray();
    }
    
    // Извлечение размера данных (4 байта)
    int dataSize = 0;
    dataSize |= extractByte(image, x, y) << 24;
    dataSize |= extractByte(image, x, y) << 16;
    dataSize |= extractByte(image, x, y) << 8;
    dataSize |= extractByte(image, x, y);
    
    // Проверка размера данных
    int maxBytes = (image.width() * image.height() * 3) / 8 - HEADER_SIZE;
    if (dataSize <= 0 || dataSize > maxBytes) {
        m_lastError = "Неверный размер данных";
        return QByteArray();
    }
    
    // Извлечение данных
    QByteArray data;
    for (int i = 0; i < dataSize; i++) {
        data.append(extractByte(image, x, y));
    }
    
    return data;
}

