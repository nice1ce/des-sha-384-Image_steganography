/**
 * @file steganography.h
 * @brief Заголовочный файл класса Steganography для внедрения и извлечения сообщений из изображений
 * @date 2025
 */

#pragma once

#include <QObject>
#include <QImage>
#include <QString>
#include <QByteArray>
#include <QBuffer>
#include <QDebug>

/**
 * @brief Класс Steganography предоставляет функции для внедрения и извлечения сообщений из изображений
 * 
 * Класс использует метод LSB (Least Significant Bit) для внедрения сообщений в изображения.
 */
class Steganography : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса Steganography
     * @param parent Родительский объект
     */
    explicit Steganography(QObject *parent = nullptr);
    
    /**
     * @brief Деструктор класса Steganography
     */
    ~Steganography();
    
    /**
     * @brief Внедряет сообщение в изображение
     * @param image Исходное изображение
     * @param message Сообщение для внедрения
     * @return Изображение с внедренным сообщением или пустое изображение в случае ошибки
     */
    QImage embedMessage(const QImage &image, const QString &message);
    
    /**
     * @brief Извлекает сообщение из изображения
     * @param image Изображение с внедренным сообщением
     * @return Извлеченное сообщение или пустая строка в случае ошибки
     */
    QString extractMessage(const QImage &image);
    
    /**
     * @brief Проверяет, содержит ли изображение внедренное сообщение
     * @param image Изображение для проверки
     * @return true, если изображение содержит внедренное сообщение, иначе false
     */
    bool hasEmbeddedMessage(const QImage &image);
    
    /**
     * @brief Возвращает последнюю ошибку
     * @return Строка с описанием последней ошибки
     */
    QString lastError() const;

private:
    /**
     * @brief Внедряет байт данных в пиксели изображения
     * @param image Изображение
     * @param x Координата X начального пикселя
     * @param y Координата Y начального пикселя
     * @param byte Байт для внедрения
     */
    void embedByte(QImage &image, int &x, int &y, unsigned char byte);
    
    /**
     * @brief Извлекает байт данных из пикселей изображения
     * @param image Изображение
     * @param x Координата X начального пикселя
     * @param y Координата Y начального пикселя
     * @return Извлеченный байт
     */
    unsigned char extractByte(const QImage &image, int &x, int &y);
    
    /**
     * @brief Внедряет данные в изображение
     * @param image Изображение
     * @param data Данные для внедрения
     * @return true в случае успеха, иначе false
     */
    bool embedData(QImage &image, const QByteArray &data);
    
    /**
     * @brief Извлекает данные из изображения
     * @param image Изображение
     * @return Извлеченные данные
     */
    QByteArray extractData(const QImage &image);
    
    QString m_lastError; ///< Последняя ошибка
    
    // Константы
    const QByteArray SIGNATURE = QByteArray::fromHex("5354454741"); ///< Сигнатура "STEGA" в HEX
    const int HEADER_SIZE = 9; ///< Размер заголовка (сигнатура + размер данных)
};

