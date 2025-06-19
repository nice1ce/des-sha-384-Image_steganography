#ifndef SHA384HASHER_H
#define SHA384HASHER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QFile>
#include "sha384.h"

/**
 * @brief Класс SHA384Hasher предоставляет удобный интерфейс для работы с SHA-384 хешированием
 */
class SHA384Hasher : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса SHA384Hasher
     * @param parent Родительский объект
     */
    explicit SHA384Hasher(QObject *parent = nullptr);

    /**
     * @brief Вычисляет хеш SHA-384 для заданных данных
     * @param data Данные для хеширования
     * @return Хеш SHA-384 в виде массива байт
     */
    QByteArray hash(const QByteArray &data);

    /**
     * @brief Вычисляет хеш SHA-384 для текста
     * @param text Текст для хеширования
     * @return Хеш SHA-384 в виде массива байт
     */
    QByteArray hashText(const QString &text);

    /**
     * @brief Вычисляет хеш SHA-384 для файла
     * @param filePath Путь к файлу
     * @return Хеш SHA-384 в виде массива байт
     */
    QByteArray hashFile(const QString &filePath);

    /**
     * @brief Вычисляет хеш SHA-384 для данных и возвращает его в шестнадцатеричном формате
     * @param data Данные для хеширования
     * @return Хеш SHA-384 в шестнадцатеричном формате
     */
    QString hashToHex(const QByteArray &data);

    /**
     * @brief Вычисляет хеш SHA-384 для текста и возвращает его в шестнадцатеричном формате
     * @param text Текст для хеширования
     * @return Хеш SHA-384 в шестнадцатеричном формате
     */
    QString hashTextToHex(const QString &text);

    /**
     * @brief Вычисляет хеш SHA-384 для файла и возвращает его в шестнадцатеричном формате
     * @param filePath Путь к файлу
     * @return Хеш SHA-384 в шестнадцатеричном формате
     */
    QString hashFileToHex(const QString &filePath);

    /**
     * @brief Вычисляет хеш SHA-384 для данных и возвращает его в формате Base64
     * @param data Данные для хеширования
     * @return Хеш SHA-384 в формате Base64
     */
    QString hashToBase64(const QByteArray &data);

    /**
     * @brief Вычисляет хеш SHA-384 для текста и возвращает его в формате Base64
     * @param text Текст для хеширования
     * @return Хеш SHA-384 в формате Base64
     */
    QString hashTextToBase64(const QString &text);

    /**
     * @brief Вычисляет хеш SHA-384 для файла и возвращает его в формате Base64
     * @param filePath Путь к файлу
     * @return Хеш SHA-384 в формате Base64
     */
    QString hashFileToBase64(const QString &filePath);

    /**
     * @brief Проверяет, соответствует ли хеш SHA-384 заданным данным
     * @param data Данные для проверки
     * @param expectedHash Ожидаемый хеш в виде массива байт
     * @return true, если хеш соответствует данным, иначе false
     */
    bool verify(const QByteArray &data, const QByteArray &expectedHash);

    /**
     * @brief Проверяет, соответствует ли хеш SHA-384 заданному тексту
     * @param text Текст для проверки
     * @param expectedHash Ожидаемый хеш в шестнадцатеричном формате
     * @return true, если хеш соответствует тексту, иначе false
     */
    bool verifyText(const QString &text, const QString &expectedHash);

    /**
     * @brief Проверяет, соответствует ли хеш SHA-384 заданному файлу
     * @param filePath Путь к файлу
     * @param expectedHash Ожидаемый хеш в шестнадцатеричном формате
     * @return true, если хеш соответствует файлу, иначе false
     */
    bool verifyFile(const QString &filePath, const QString &expectedHash);

    /**
     * @brief Возвращает последнюю ошибку
     * @return Текст ошибки
     */
    QString lastError() const;

private:
    SHA384 m_sha384;
    QString m_lastError;
};

#endif // SHA384HASHER_H

