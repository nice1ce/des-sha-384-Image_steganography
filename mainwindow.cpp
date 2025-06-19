/**
 * @file mainwindow.cpp
 * @brief Реализация MainWindow - главного окна GUI приложения с вкладками
 * @date 2025
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myclient.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QBuffer>
#include <QImageReader>
#include <QImageWriter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_steganography(new Steganography(this))
{
    ui->setupUi(this);
    setWindowTitle("Стеганография");
    
    // Показать последний ответ от сервера, если есть
    QString last = MyClient::getInstance()->getLastResponse();
    if (!last.isEmpty()) {
        appendToOutput("> Последний ответ:\n" + last);
    }
    
    // Подключаем сигналы к слотам
    connect(ui->btnEmbed, &QPushButton::clicked, this, &MainWindow::onEmbedMessageClicked);
    connect(ui->btnExtract, &QPushButton::clicked, this, &MainWindow::onExtractMessageClicked);
    connect(ui->btnSelectImageEmbed, &QPushButton::clicked, this, &MainWindow::onSelectImageForEmbedClicked);
    connect(ui->btnSelectImageExtract, &QPushButton::clicked, this, &MainWindow::onSelectImageForExtractClicked);
    connect(ui->btnSaveImage, &QPushButton::clicked, this, &MainWindow::onSaveImageClicked);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    
    // Настраиваем начальное состояние интерфейса
    ui->btnSaveImage->setEnabled(false);
    
    // Подключаем сигнал от клиента для получения ответов от сервера
    connect(MyClient::getInstance(), &MyClient::responseReceived, this, [this](const QString& text) {
        appendToOutput("> Ответ от сервера:\n" + text);
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onEmbedMessageClicked() {
    // Проверяем, выбрано ли изображение
    if (m_originalImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите изображение");
        return;
    }
    
    // Получаем сообщение
    QString message = ui->editEmbedMessage->text().trimmed();
    
    // Проверяем, что сообщение не пустое
    if (message.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите сообщение для внедрения");
        return;
    }
    
    // Внедряем сообщение в изображение
    m_stegoImage = m_steganography->embedMessage(m_originalImage, message);
    
    // Проверяем результат
    if (m_stegoImage.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось внедрить сообщение: " + m_steganography->lastError());
        return;
    }
    
    // Отображаем результат
    appendToOutput("> Сообщение успешно внедрено в изображение");
    appendToOutput("> Теперь вы можете сохранить изображение");
    
    // Отображаем изображение с внедренным сообщением
    displayImage(m_stegoImage);
    
    // Активируем кнопку сохранения
    ui->btnSaveImage->setEnabled(true);
    
    // Очищаем поле ввода сообщения
    ui->editEmbedMessage->clear();
}

void MainWindow::onExtractMessageClicked() {
    // Проверяем, выбрано ли изображение
    if (m_originalImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите изображение");
        return;
    }
    
    // Проверяем, содержит ли изображение внедренное сообщение
    if (!m_steganography->hasEmbeddedMessage(m_originalImage)) {
        QMessageBox::warning(this, "Ошибка", "Изображение не содержит внедренного сообщения");
        return;
    }
    
    // Извлекаем сообщение из изображения
    QString message = m_steganography->extractMessage(m_originalImage);
    
    // Проверяем результат
    if (message.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось извлечь сообщение: " + m_steganography->lastError());
        return;
    }
    
    // Отображаем результат
    appendToOutput("> Извлеченное сообщение:");
    appendToOutput(message);
}

void MainWindow::onSelectImageForEmbedClicked() {
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", 
                                                  QString(), "Изображения (*.png *.jpg *.jpeg *.bmp)");
    
    // Проверяем, выбран ли файл
    if (filePath.isEmpty()) {
        return;
    }
    
    // Загружаем изображение
    m_originalImage = QImage(filePath);
    
    // Проверяем, загружено ли изображение
    if (m_originalImage.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    
    // Сохраняем путь к изображению
    m_lastImagePath = filePath;
    
    // Отображаем информацию об изображении
    appendToOutput("> Изображение загружено:");
    appendToOutput(QString("Размер: %1x%2 пикселей").arg(m_originalImage.width()).arg(m_originalImage.height()));
    appendToOutput(QString("Формат: %1").arg(m_originalImage.format()));
    
    // Отображаем изображение
    displayImage(m_originalImage);
    
    // Сбрасываем состояние кнопки сохранения
    ui->btnSaveImage->setEnabled(false);
}

void MainWindow::onSelectImageForExtractClicked() {
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", 
                                                  QString(), "Изображения (*.png *.jpg *.jpeg *.bmp)");
    
    // Проверяем, выбран ли файл
    if (filePath.isEmpty()) {
        return;
    }
    
    // Загружаем изображение
    m_originalImage = QImage(filePath);
    
    // Проверяем, загружено ли изображение
    if (m_originalImage.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось загрузить изображение");
        return;
    }
    
    // Сохраняем путь к изображению
    m_lastImagePath = filePath;
    
    // Проверяем, содержит ли изображение внедренное сообщение
    bool hasMessage = m_steganography->hasEmbeddedMessage(m_originalImage);
    
    // Отображаем информацию об изображении
    appendToOutput("> Изображение загружено:");
    appendToOutput(QString("Размер: %1x%2 пикселей").arg(m_originalImage.width()).arg(m_originalImage.height()));
    appendToOutput(QString("Содержит внедренное сообщение: %1").arg(hasMessage ? "Да" : "Нет"));
    
    // Отображаем изображение
    displayImage(m_originalImage);
}

void MainWindow::onSaveImageClicked() {
    // Проверяем, есть ли изображение для сохранения
    if (m_stegoImage.isNull()) {
        QMessageBox::warning(this, "Ошибка", "Нет изображения для сохранения");
        return;
    }
    
    // Открываем диалог сохранения файла
    QString filePath = QFileDialog::getSaveFileName(this, "Сохранить изображение", 
                                                  QString(), "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");
    
    // Проверяем, выбран ли файл
    if (filePath.isEmpty()) {
        return;
    }
    
    // Сохраняем изображение
    if (!m_stegoImage.save(filePath)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось сохранить изображение");
        return;
    }
    
    // Отображаем информацию о сохранении
    appendToOutput("> Изображение успешно сохранено:");
    appendToOutput(filePath);
}

void MainWindow::onTabChanged(int index) {
    // Обработка изменения вкладки
    if (index == 0) {
        // Вкладка "Стеганография"
        if (!m_originalImage.isNull()) {
            displayImage(m_originalImage);
        }
    }
}

void MainWindow::displayImage(const QImage &image) {
    if (image.isNull()) {
        return;
    }
    
    // Создаем QPixmap из QImage
    QPixmap pixmap = QPixmap::fromImage(image);
    
    // Масштабируем изображение, чтобы оно поместилось в метку
    pixmap = pixmap.scaled(ui->labelImage->width(), ui->labelImage->height(), 
                          Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Отображаем изображение в метке
    ui->labelImage->setPixmap(pixmap);
}

void MainWindow::appendToOutput(const QString &message) {
    ui->textOutput->append(message);
}

