/**
 * @file steganography_test.cpp
 * @brief Тестирование функций класса Steganography
 * @date 2025
 */

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include "steganography.h"

/**
 * @brief Функция для тестирования внедрения и извлечения сообщения
 * @param imagePath Путь к тестовому изображению
 * @param message Сообщение для внедрения
 * @param password Пароль для шифрования
 * @return true, если тест пройден успешно, иначе false
 */
bool testEmbedAndExtract(const QString &imagePath, const QString &message, const QString &password) {
    qDebug() << "Тестирование изображения:" << imagePath;
    qDebug() << "Сообщение:" << message;
    qDebug() << "Пароль:" << password;
    
    // Создаем объект класса Steganography
    Steganography stego;
    
    // Загружаем изображение
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Ошибка: Не удалось загрузить изображение";
        return false;
    }
    
    qDebug() << "Размер изображения:" << image.width() << "x" << image.height();
    qDebug() << "Формат изображения:" << image.format();
    
    // Внедряем сообщение в изображение
    QImage stegoImage = stego.embedMessage(image, message, password);
    if (stegoImage.isNull()) {
        qDebug() << "Ошибка при внедрении сообщения:" << stego.lastError();
        return false;
    }
    
    // Сохраняем изображение с внедренным сообщением
    QString stegoImagePath = QFileInfo(imagePath).path() + "/stego_" + QFileInfo(imagePath).fileName();
    if (!stegoImage.save(stegoImagePath)) {
        qDebug() << "Ошибка при сохранении изображения с внедренным сообщением";
        return false;
    }
    
    qDebug() << "Изображение с внедренным сообщением сохранено:" << stegoImagePath;
    
    // Проверяем, содержит ли изображение внедренное сообщение
    if (!stego.hasEmbeddedMessage(stegoImage)) {
        qDebug() << "Ошибка: Изображение не содержит внедренного сообщения";
        return false;
    }
    
    // Извлекаем сообщение из изображения
    QString extractedMessage = stego.extractMessage(stegoImage, password);
    if (extractedMessage.isEmpty()) {
        qDebug() << "Ошибка при извлечении сообщения:" << stego.lastError();
        return false;
    }
    
    qDebug() << "Извлеченное сообщение:" << extractedMessage;
    
    // Проверяем, совпадает ли извлеченное сообщение с исходным
    if (extractedMessage != message) {
        qDebug() << "Ошибка: Извлеченное сообщение не совпадает с исходным";
        return false;
    }
    
    qDebug() << "Тест пройден успешно!";
    return true;
}

/**
 * @brief Функция для тестирования извлечения сообщения с неверным паролем
 * @param imagePath Путь к изображению с внедренным сообщением
 * @param correctPassword Правильный пароль
 * @param wrongPassword Неверный пароль
 * @return true, если тест пройден успешно, иначе false
 */
bool testWrongPassword(const QString &imagePath, const QString &correctPassword, const QString &wrongPassword) {
    qDebug() << "Тестирование извлечения с неверным паролем:" << imagePath;
    
    // Создаем объект класса Steganography
    Steganography stego;
    
    // Загружаем изображение
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Ошибка: Не удалось загрузить изображение";
        return false;
    }
    
    // Проверяем, содержит ли изображение внедренное сообщение
    if (!stego.hasEmbeddedMessage(image)) {
        qDebug() << "Ошибка: Изображение не содержит внедренного сообщения";
        return false;
    }
    
    // Извлекаем сообщение с неверным паролем
    QString extractedMessage = stego.extractMessage(image, wrongPassword);
    
    // Проверяем, что сообщение не извлечено или извлечено неверно
    if (!extractedMessage.isEmpty()) {
        // Извлекаем сообщение с правильным паролем для сравнения
        QString correctMessage = stego.extractMessage(image, correctPassword);
        
        if (extractedMessage == correctMessage) {
            qDebug() << "Ошибка: Сообщение извлечено с неверным паролем";
            return false;
        }
    }
    
    qDebug() << "Тест пройден успешно! Сообщение не извлечено с неверным паролем";
    return true;
}

/**
 * @brief Функция для тестирования максимального размера сообщения
 * @param imagePath Путь к тестовому изображению
 * @param password Пароль для шифрования
 * @return true, если тест пройден успешно, иначе false
 */
bool testMaxMessageSize(const QString &imagePath, const QString &password) {
    qDebug() << "Тестирование максимального размера сообщения:" << imagePath;
    
    // Создаем объект класса Steganography
    Steganography stego;
    
    // Загружаем изображение
    QImage image(imagePath);
    if (image.isNull()) {
        qDebug() << "Ошибка: Не удалось загрузить изображение";
        return false;
    }
    
    // Вычисляем максимальный размер сообщения
    int maxBytes = (image.width() * image.height() * 3) / 8 - 16; // 16 - размер заголовка
    
    qDebug() << "Максимальный размер сообщения:" << maxBytes << "байт";
    
    // Создаем сообщение максимального размера
    QString message;
    for (int i = 0; i < maxBytes / 10; i++) {
        message += "0123456789";
    }
    
    // Добавляем оставшиеся символы
    for (int i = 0; i < maxBytes % 10; i++) {
        message += QString::number(i);
    }
    
    qDebug() << "Размер сообщения:" << message.toUtf8().size() << "байт";
    
    // Внедряем сообщение в изображение
    QImage stegoImage = stego.embedMessage(image, message, password);
    if (stegoImage.isNull()) {
        qDebug() << "Ошибка при внедрении сообщения:" << stego.lastError();
        return false;
    }
    
    // Сохраняем изображение с внедренным сообщением
    QString stegoImagePath = QFileInfo(imagePath).path() + "/stego_max_" + QFileInfo(imagePath).fileName();
    if (!stegoImage.save(stegoImagePath)) {
        qDebug() << "Ошибка при сохранении изображения с внедренным сообщением";
        return false;
    }
    
    qDebug() << "Изображение с внедренным сообщением сохранено:" << stegoImagePath;
    
    // Извлекаем сообщение из изображения
    QString extractedMessage = stego.extractMessage(stegoImage, password);
    if (extractedMessage.isEmpty()) {
        qDebug() << "Ошибка при извлечении сообщения:" << stego.lastError();
        return false;
    }
    
    // Проверяем, совпадает ли извлеченное сообщение с исходным
    if (extractedMessage != message) {
        qDebug() << "Ошибка: Извлеченное сообщение не совпадает с исходным";
        return false;
    }
    
    qDebug() << "Тест пройден успешно!";
    return true;
}

/**
 * @brief Главная функция для запуска тестов
 * @param argc Количество аргументов командной строки
 * @param argv Аргументы командной строки
 * @return Код завершения программы
 */
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    
    qDebug() << "Начало тестирования функций стеганографии";
    qDebug() << "Текущее время:" << QDateTime::currentDateTime().toString();
    
    // Путь к тестовым изображениям
    QString testImagePath = "test_images/test.png";
    
    // Создаем директорию для тестовых изображений, если она не существует
    QDir().mkpath("test_images");
    
    // Создаем тестовое изображение, если оно не существует
    if (!QFileInfo::exists(testImagePath)) {
        QImage testImage(512, 512, QImage::Format_RGB32);
        testImage.fill(Qt::white);
        
        // Рисуем что-нибудь на изображении
        for (int y = 0; y < testImage.height(); y++) {
            for (int x = 0; x < testImage.width(); x++) {
                int r = (x * 255) / testImage.width();
                int g = (y * 255) / testImage.height();
                int b = ((x + y) * 255) / (testImage.width() + testImage.height());
                testImage.setPixel(x, y, qRgb(r, g, b));
            }
        }
        
        testImage.save(testImagePath);
        qDebug() << "Создано тестовое изображение:" << testImagePath;
    }
    
    // Тестирование внедрения и извлечения сообщения
    bool test1 = testEmbedAndExtract(testImagePath, "Это тестовое сообщение для проверки функций стеганографии", "password123");
    
    // Тестирование извлечения сообщения с неверным паролем
    bool test2 = testWrongPassword("test_images/stego_test.png", "password123", "wrongpassword");
    
    // Тестирование максимального размера сообщения
    bool test3 = testMaxMessageSize(testImagePath, "password123");
    
    // Вывод результатов тестирования
    qDebug() << "Результаты тестирования:";
    qDebug() << "Тест 1 (внедрение и извлечение):" << (test1 ? "Пройден" : "Не пройден");
    qDebug() << "Тест 2 (неверный пароль):" << (test2 ? "Пройден" : "Не пройден");
    qDebug() << "Тест 3 (максимальный размер):" << (test3 ? "Пройден" : "Не пройден");
    
    return 0;
}

