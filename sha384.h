#ifndef SHA384_H
#define SHA384_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QVector>

/**
 * @brief Класс SHA384 реализует алгоритм хеширования SHA-384
 * 
 * SHA-384 - это хеш-функция из семейства SHA-2, которая генерирует 384-битный хеш.
 * Она основана на SHA-512, но использует другие начальные значения и усекает выходной хеш до 384 бит.
 */
class SHA384 : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса SHA384
     * @param parent Родительский объект
     */
    explicit SHA384(QObject *parent = nullptr);

    /**
     * @brief Вычисляет хеш SHA-384 для заданных данных
     * @param data Данные для хеширования
     * @return Хеш SHA-384 в виде массива байтов
     */
    QByteArray hash(const QByteArray &data);
    
    /**
     * @brief Вычисляет хеш SHA-384 для заданных данных
     * @param data Указатель на данные для хеширования
     * @param length Длина данных в байтах
     * @param output Указатель на массив для записи результата (должен быть не менее 48 байт)
     */
    void hash(const uint8_t* data, size_t length, uint8_t* output);

    /**
     * @brief Обновляет контекст хеширования новыми данными
     * @param data Указатель на данные для хеширования
     * @param length Длина данных в байтах
     */
    void update(const uint8_t* data, size_t length);
    
    /**
     * @brief Завершает процесс хеширования и возвращает результат
     * @param output Указатель на массив для записи результата (должен быть не менее 48 байт)
     */
    void finalize(uint8_t* output);

    /**
     * @brief Вычисляет хеш SHA-384 для заданных данных и возвращает его в виде шестнадцатеричной строки
     * @param data Данные для хеширования
     * @return Хеш SHA-384 в виде шестнадцатеричной строки
     */
    QString hashToHex(const QByteArray &data);

    /**
     * @brief Возвращает последнюю ошибку
     * @return Текст ошибки
     */
    QString lastError() const;

private:
    // Константы
    static const int BLOCK_SIZE = 128; // 1024 бит
    static const int HASH_SIZE = 48;   // 384 бит
    
    // Начальные значения хеша (H0-H7)
    static const quint64 H0[8];
    
    // Константы раунда (K0-K79)
    static const quint64 K[80];
    
    // Состояние хеша
    quint64 state[8];
    
    // Счетчик обработанных бит
    quint64 bitCount;
    
    // Буфер для накопления данных
    QByteArray buffer;
    
    QString m_lastError;

    // Вспомогательные функции
    void initialize();
    void processBlock(const quint8 *block);
    void finalize();
    
    // Функции для работы с 64-битными словами
    quint64 ch(quint64 x, quint64 y, quint64 z);
    quint64 maj(quint64 x, quint64 y, quint64 z);
    quint64 sigma0(quint64 x);
    quint64 sigma1(quint64 x);
    quint64 gamma0(quint64 x);
    quint64 gamma1(quint64 x);
    
    // Функции для преобразования данных
    void bytesToWords(const quint8 *bytes, quint64 *words, int size);
    void wordsToBytes(const quint64 *words, quint8 *bytes, int size);
    
    // Функции для работы с блоками
    QByteArray padData(const QByteArray &data);
};

#endif // SHA384_H

