/**
 * @file mainwindow_tabs.h
 * @brief Заголовочный файл главного окна приложения с вкладками
 * @date 2025
 */

#pragma once

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QTabWidget>
#include <QLabel>
#include <QVector>
#include <QPointF>
#include "steganography.h"
#include "chordmethod.h"
#include "descipher.h"
#include "sha384hasher.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Класс MainWindow представляет главное окно приложения с вкладками
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса MainWindow
     * @param parent Родительский виджет
     */
    explicit MainWindow(QWidget *parent = nullptr, const QString& userRole = "user");
    
    /**
     * @brief Деструктор класса MainWindow
     */
    ~MainWindow();

    void setRole(const QString& role);
    void configureAdminAccess();

private slots:
    // Слоты для вкладки "Стеганография"
    /**
     * @brief Обработчик нажатия кнопки "Внедрить" для внедрения сообщения в изображение
     */
    void onEmbedMessageClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Извлечь" для извлечения сообщения из изображения
     */
    void onExtractMessageClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Выбрать изображение" для внедрения сообщения
     */
    void onSelectImageForEmbedClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Выбрать изображение" для извлечения сообщения
     */
    void onSelectImageForExtractClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Сохранить изображение" после внедрения сообщения
     */
    void onSaveImageClicked();
    
    /**
     * @brief Обработчик изменения вкладки
     * @param index Индекс выбранной вкладки
     */
    void onTabChanged(int index);

    // Слоты для вкладки "Метод хорд"
    /**
     * @brief Обработчик нажатия кнопки "Найти корень" для решения уравнения методом хорд
     */
    void onSolveClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Очистить" для очистки результатов метода хорд
     */
    void onClearChordClicked();

    // Слоты для вкладки "AES"
    /**
     * @brief Обработчик нажатия кнопки "Зашифровать" для шифрования текста
     */
    void onEncryptClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Дешифровать" для дешифрования текста
     */
    void onDecryptClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Очистить" для очистки полей AES
     */
    void onClearAESClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Копировать результат" для копирования результата шифрования/дешифрования
     */
    void onCopyAESResultClicked();
    
    /**
     * @brief Обработчик изменения размера ключа AES
     * @param index Индекс выбранного размера ключа
     */
    void onKeySizeChanged(int index);

    // Слоты для вкладки "SHA-384"
    /**
     * @brief Обработчик нажатия кнопки "Вычислить хеш" для вычисления хеша текста
     */
    void onCalculateHashClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Очистить" для очистки полей SHA-384
     */
    void onClearSHA384Clicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Копировать хеш" для копирования результата хеширования
     */
    void onCopyHashClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Выбрать файл" для выбора файла для хеширования
     */
    void onSelectFileClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Вычислить хеш файла" для вычисления хеша файла
     */
    void onCalculateFileHashClicked();
    
    /**
     * @brief Обработчик нажатия кнопки "Проверить хеш" для проверки хеша
     */
    void onVerifyHashClicked();

    // Слоты для вкладки "Admin"
    void onUsersClicked();
    void appendToAdmin(const QString &message);

private:
    /**
     * @brief Отображает изображение в метке
     * @param image Изображение для отображения
     */
    void displayImage(const QImage &image);
    
    /**
     * @brief Добавляет сообщение в текстовое поле вывода
     * @param message Сообщение для добавления
     */
    void appendToOutput(const QString &message);

    /**
     * @brief Добавляет сообщение в текстовое поле вывода метода хорд
     * @param message Сообщение для добавления
     */
    void appendToChordOutput(const QString &message);
    
    /**
     * @brief Получает функцию для метода хорд по индексу
     * @param index Индекс функции
     * @return Функция для метода хорд
     */
    std::function<double(double)> getFunction(int index);
    
    /**
     * @brief Отображает график функции
     * @param points Точки функции
     * @param root Корень уравнения
     */
    void displayPlot(const QVector<QPointF>& points, double root);

private:
    Ui::MainWindow *ui; ///< Указатель на UI главного окна
    Steganography *m_steganography; ///< Указатель на объект класса Steganography
    ChordMethod *m_chordMethod; ///< Указатель на объект класса ChordMethod
    DESCipher *m_desCipher; ///< Указатель на объект класса DESCipher
    SHA384Hasher *m_sha384Hasher; ///< Указатель на объект класса SHA384Hasher
    QImage m_originalImage; ///< Исходное изображение
    QImage m_stegoImage; ///< Изображение с внедренным сообщением
    QString m_lastImagePath; ///< Путь к последнему выбранному изображению
    QString m_selectedFilePath; ///< Путь к выбранному файлу для хеширования
    QString currentUserRole;
};

