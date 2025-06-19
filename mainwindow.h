/**
 * @file mainwindow.h
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
#include "steganography.h"

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
    explicit MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор класса MainWindow
     */
    ~MainWindow();

private slots:
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

private:
    Ui::MainWindow *ui; ///< Указатель на UI главного окна
    Steganography *m_steganography; ///< Указатель на объект класса Steganography
    QImage m_originalImage; ///< Исходное изображение
    QImage m_stegoImage; ///< Изображение с внедренным сообщением
    QString m_lastImagePath; ///< Путь к последнему выбранному изображению
};

