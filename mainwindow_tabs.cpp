/**
 * @file mainwindow_tabs.cpp
 * @brief Реализация MainWindow - главного окна GUI приложения с вкладками
 * @date 2025
 */

#include "mainwindow_tabs.h"
#include "ui_mainwindow_tabs.h"
#include "myclient.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QBuffer>
#include <QImageReader>
#include <QImageWriter>
#include <QClipboard>
#include <QPainter>
#include <QPen>
#include <QDateTime>
#include <QCryptographicHash>
#include <cmath>

MainWindow::MainWindow(QWidget *parent, const QString& userRole)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    currentUserRole(userRole),
    m_steganography(new Steganography(this)),
    m_chordMethod(new ChordMethod(this)),
    m_desCipher(new DESCipher(this)),
    m_sha384Hasher(new SHA384Hasher(this))
{
    ui->setupUi(this);
    setWindowTitle("Многофункциональное приложение");
    
    // Показать последний ответ от сервера, если есть
    QString last = MyClient::getInstance()->getLastResponse();
    if (!last.isEmpty()) {
        appendToOutput("> Последний ответ:\n" + last);
    }
    
    // Подключаем сигналы к слотам для вкладки "Стеганография"
    connect(ui->btnEmbed, &QPushButton::clicked, this, &MainWindow::onEmbedMessageClicked);
    connect(ui->btnExtract, &QPushButton::clicked, this, &MainWindow::onExtractMessageClicked);
    connect(ui->btnSelectImageEmbed, &QPushButton::clicked, this, &MainWindow::onSelectImageForEmbedClicked);
    connect(ui->btnSelectImageExtract, &QPushButton::clicked, this, &MainWindow::onSelectImageForExtractClicked);
    connect(ui->btnSaveImage, &QPushButton::clicked, this, &MainWindow::onSaveImageClicked);
    
    // Подключаем сигналы к слотам для вкладки "Метод хорд"
    connect(ui->btnSolve, &QPushButton::clicked, this, &MainWindow::onSolveClicked);
    connect(ui->btnClear, &QPushButton::clicked, this, &MainWindow::onClearChordClicked);
    
    // Подключаем сигналы к слотам для вкладки "AES"
    connect(ui->btnEncrypt, &QPushButton::clicked, this, &MainWindow::onEncryptClicked);
    connect(ui->btnDecrypt, &QPushButton::clicked, this, &MainWindow::onDecryptClicked);
    connect(ui->btnClearAES, &QPushButton::clicked, this, &MainWindow::onClearAESClicked);
    connect(ui->btnCopyAESResult, &QPushButton::clicked, this, &MainWindow::onCopyAESResultClicked);
    connect(ui->comboKeySizeAES, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onKeySizeChanged);
    
    // Подключаем сигналы к слотам для вкладки "SHA-384"
    connect(ui->btnCalculateHash, &QPushButton::clicked, this, &MainWindow::onCalculateHashClicked);
    connect(ui->btnClearSHA384, &QPushButton::clicked, this, &MainWindow::onClearSHA384Clicked);
    connect(ui->btnCopyHash, &QPushButton::clicked, this, &MainWindow::onCopyHashClicked);
    connect(ui->btnSelectFile, &QPushButton::clicked, this, &MainWindow::onSelectFileClicked);
    connect(ui->btnCalculateFileHash, &QPushButton::clicked, this, &MainWindow::onCalculateFileHashClicked);
    connect(ui->btnVerifyHash, &QPushButton::clicked, this, &MainWindow::onVerifyHashClicked);

    // Подключаем сигналы к слотам для вкладки "Admin"
    connect(ui->users, &QPushButton::clicked, this, &MainWindow::onUsersClicked);
    connect(MyClient::getInstance(), &MyClient::responseReceived, this, [this](const QString& text) {
        appendToAdmin("> Ответ от сервера:\n" + text);
    });

    // Подключаем сигнал изменения вкладки
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    
    // Настраиваем начальное состояние интерфейса
    ui->btnSaveImage->setEnabled(false);
    ui->labelSelectedFile->setText("Файл не выбран");
    ui->labelVerifyResult->setText("Результат проверки");
    
    // Обновляем информацию о размере ключа AES
    onKeySizeChanged(ui->comboKeySizeAES->currentIndex());
    
    // Подключаем сигнал от клиента для получения ответов от сервера
    connect(MyClient::getInstance(), &MyClient::responseReceived, this, [this](const QString& text) {
        appendToOutput("> Ответ от сервера:\n" + text);
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

// ===== Методы для вкладки "Стеганография" =====

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

// ===== Методы для вкладки "Метод хорд" =====

void MainWindow::onSolveClicked() {
    // Получаем параметры
    int functionIndex = ui->comboFunction->currentIndex();
    double a = ui->editA->text().toDouble();
    double b = ui->editB->text().toDouble();
    double epsilon = ui->editEpsilon->text().toDouble();
    int maxIter = ui->editMaxIter->text().toInt();
    
    // Проверяем корректность параметров
    if (a >= b) {
        QMessageBox::warning(this, "Ошибка", "Левая граница должна быть меньше правой");
        return;
    }
    
    if (epsilon <= 0) {
        QMessageBox::warning(this, "Ошибка", "Точность должна быть положительным числом");
        return;
    }
    
    if (maxIter <= 0) {
        QMessageBox::warning(this, "Ошибка", "Максимальное количество итераций должно быть положительным числом");
        return;
    }
    
    // Получаем функцию
    auto function = getFunction(functionIndex);
    
    // Проверяем наличие корня на интервале
    if (!m_chordMethod->hasRoot(function, a, b)) {
        QMessageBox::warning(this, "Ошибка", "На заданном интервале нет корня или их несколько");
        return;
    }
    
    // Решаем уравнение методом хорд
    double root = m_chordMethod->solve(function, a, b, epsilon, maxIter);
    
    // Проверяем результат
    if (!m_chordMethod->lastError().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", m_chordMethod->lastError());
        return;
    }
    
    // Вычисляем значение функции в корне
    double functionValue = function(root);
    
    // Отображаем результат
    ui->labelRoot->setText(QString::number(root, 'g', 10));
    ui->labelFunctionValue->setText(QString::number(functionValue, 'g', 10));
    ui->labelIterations->setText(QString::number(m_chordMethod->getIterationCount()));
    
    // Отображаем информацию о решении
    appendToChordOutput("> Решение методом хорд:");
    appendToChordOutput(QString("Функция: %1").arg(ui->comboFunction->currentText()));
    appendToChordOutput(QString("Интервал: [%1, %2]").arg(a).arg(b));
    appendToChordOutput(QString("Точность: %1").arg(epsilon));
    appendToChordOutput(QString("Корень: %1").arg(root));
    appendToChordOutput(QString("Значение функции: %1").arg(functionValue));
    appendToChordOutput(QString("Количество итераций: %1").arg(m_chordMethod->getIterationCount()));
    
    // Вычисляем точки функции для построения графика
    QVector<QPointF> points = m_chordMethod->calculateFunctionPoints(function, a, b);
    
    // Отображаем график функции
    displayPlot(points, root);
}

void MainWindow::onClearChordClicked() {
    // Очищаем поля ввода
    ui->editA->setText("0");
    ui->editB->setText("3");
    ui->editEpsilon->setText("0.0001");
    ui->editMaxIter->setText("100");
    
    // Очищаем результаты
    ui->labelRoot->setText("-");
    ui->labelFunctionValue->setText("-");
    ui->labelIterations->setText("-");
    
    // Очищаем текстовое поле вывода
    ui->textChordOutput->clear();
    
    // Очищаем график
    ui->labelPlot->setText("График функции");
    ui->labelPlot->setPixmap(QPixmap());
}

std::function<double(double)> MainWindow::getFunction(int index) {
    switch (index) {
        case 0: // x^2 - 4
            return [](double x) { return x * x - 4; };
        case 1: // sin(x) - 0.5
            return [](double x) { return std::sin(x) - 0.5; };
        case 2: // x^3 - 2*x - 5
            return [](double x) { return x * x * x - 2 * x - 5; };
        case 3: // exp(x) - 3
            return [](double x) { return std::exp(x) - 3; };
        case 4: // log(x) - 1
            return [](double x) { return std::log(x) - 1; };
        default:
            return [](double x) { return x * x - 4; };
    }
}

void MainWindow::displayPlot(const QVector<QPointF>& points, double root) {
    // Проверяем, есть ли точки для построения графика
    if (points.isEmpty()) {
        return;
    }
    
    // Создаем изображение для графика
    QImage image(ui->labelPlot->width(), ui->labelPlot->height(), QImage::Format_RGB32);
    image.fill(QColor(59, 66, 82)); // Цвет фона
    
    // Создаем объект QPainter для рисования на изображении
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Находим минимальные и максимальные значения x и y
    double minX = points.first().x();
    double maxX = points.first().x();
    double minY = points.first().y();
    double maxY = points.first().y();
    
    for (const QPointF& point : points) {
        minX = std::min(minX, point.x());
        maxX = std::max(maxX, point.x());
        minY = std::min(minY, point.y());
        maxY = std::max(maxY, point.y());
    }
    
    // Добавляем отступы
    double xMargin = (maxX - minX) * 0.05;
    double yMargin = (maxY - minY) * 0.05;
    minX -= xMargin;
    maxX += xMargin;
    minY -= yMargin;
    maxY += yMargin;
    
    // Вычисляем масштабные коэффициенты
    double xScale = image.width() / (maxX - minX);
    double yScale = image.height() / (maxY - minY);
    
    // Функция для преобразования координат
    auto transformX = [&](double x) { return (x - minX) * xScale; };
    auto transformY = [&](double y) { return image.height() - (y - minY) * yScale; };
    
    // Рисуем оси координат
    painter.setPen(QPen(QColor(216, 222, 233), 1)); // Цвет осей
    
    // Ось X
    if (minY <= 0 && maxY >= 0) {
        double y0 = transformY(0);
        painter.drawLine(0, y0, image.width(), y0);
    }
    
    // Ось Y
    if (minX <= 0 && maxX >= 0) {
        double x0 = transformX(0);
        painter.drawLine(x0, 0, x0, image.height());
    }
    
    // Рисуем график функции
    painter.setPen(QPen(QColor(136, 192, 208), 2)); // Цвет графика
    
    for (int i = 1; i < points.size(); ++i) {
        double x1 = transformX(points[i-1].x());
        double y1 = transformY(points[i-1].y());
        double x2 = transformX(points[i].x());
        double y2 = transformY(points[i].y());
        
        painter.drawLine(x1, y1, x2, y2);
    }
    
    // Рисуем корень
    painter.setPen(QPen(QColor(163, 190, 140), 3)); // Цвет корня
    double rootX = transformX(root);
    double rootY = transformY(0);
    painter.drawEllipse(QPointF(rootX, rootY), 5, 5);
    
    // Отображаем график
    ui->labelPlot->setPixmap(QPixmap::fromImage(image));
}

// ===== Методы для вкладки "AES" =====

void MainWindow::onEncryptClicked() {
    // Получаем ключ
    QString key = ui->editKeyAES->text();
    
    // Проверяем, что ключ не пустой
    if (key.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ключ шифрования");
        return;
    }
    
    // Получаем текст для шифрования
    QString plainText = ui->editPlainTextAES->toPlainText();
    
    // Проверяем, что текст не пустой
    if (plainText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для шифрования");
        return;
    }
    
    // Получаем размер ключа
    int keySize = 128; // По умолчанию 128 бит
    switch (ui->comboKeySizeAES->currentIndex()) {
        case 0: // 128 бит
            keySize = 128;
            break;
        case 1: // 192 бит
            keySize = 192;
            break;
        case 2: // 256 бит
            keySize = 256;
            break;
    }
    
    // Устанавливаем ключ
    if (!m_desCipher->setKey(key.toUtf8()))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось установить ключ шифрования");
        return;
    }
    // Шифруем текст
    QByteArray cipherText = m_desCipher->encrypt(plainText.toUtf8());

    // Проверяем результат
    if (cipherText.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось зашифровать текст");
        return;
    }
    
    // Преобразуем результат в нужный формат
    QString result;
    if (ui->comboOutputFormatAES->currentIndex() == 0) {
        // Hex
        result = cipherText.toHex();
    } else {
        // Base64
        result = cipherText.toBase64();
    }
    
    // Отображаем результат
    ui->editCipherTextAES->setPlainText(result);
    ui->labelAESStatus->setText("Статус: Текст успешно зашифрован");
}

void MainWindow::onDecryptClicked() {
    // Получаем ключ
    QString key = ui->editKeyAES->text();
    
    // Проверяем, что ключ не пустой
    if (key.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ключ шифрования");
        return;
    }
    
    // Получаем зашифрованный текст
    QString cipherTextStr = ui->editCipherTextAES->toPlainText().trimmed();
    
    // Проверяем, что текст не пустой
    if (cipherTextStr.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите зашифрованный текст");
        return;
    }
    
    // Преобразуем текст из нужного формата
    QByteArray cipherText;
    if (ui->comboOutputFormatAES->currentIndex() == 0) {
        // Hex
        cipherText = QByteArray::fromHex(cipherTextStr.toLatin1());
    } else {
        // Base64
        cipherText = QByteArray::fromBase64(cipherTextStr.toLatin1());
    }
    
    // Получаем размер ключа
    int keySize = 128; // По умолчанию 128 бит
    switch (ui->comboKeySizeAES->currentIndex()) {
        case 0: // 128 бит
            keySize = 128;
            break;
        case 1: // 192 бит
            keySize = 192;
            break;
        case 2: // 256 бит
            keySize = 256;
            break;
    }
    
    // Устанавливаем ключ
    if (!m_desCipher->setKey(key.toUtf8()))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось установить ключ шифрования");
        return;
    }
    
    // Дешифруем текст
    QByteArray plainText = m_desCipher->decrypt(cipherText);
    
    // Проверяем результат
    if (plainText.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось дешифровать текст");
        return;
    }
    
    // Отображаем результат
    ui->editPlainTextAES->setPlainText(QString::fromUtf8(plainText));
    ui->labelAESStatus->setText("Статус: Текст успешно дешифрован");
}

void MainWindow::onClearAESClicked() {
    // Очищаем поля ввода
    ui->editKeyAES->clear();
    ui->editPlainTextAES->clear();
    ui->editCipherTextAES->clear();
    ui->labelAESStatus->setText("Статус: Готов");
}

void MainWindow::onCopyAESResultClicked() {
    // Копируем результат в буфер обмена
    QString text = ui->editCipherTextAES->toPlainText();
    if (!text.isEmpty()) {
        QApplication::clipboard()->setText(text);
        ui->labelAESStatus->setText("Статус: Результат скопирован в буфер обмена");
    }
}

void MainWindow::onKeySizeChanged(int index) {
    ui->labelKeySize->setText("Ключ DES: 8 байт");
}

// ===== Методы для вкладки "SHA-384" =====

void MainWindow::onCalculateHashClicked() {
    // Получаем текст для хеширования
    QString text = ui->editInputTextSHA384->toPlainText();
    
    // Проверяем, что текст не пустой
    if (text.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст для хеширования");
        return;
    }
    
    // Вычисляем хеш
    QByteArray hash = m_sha384Hasher->hash(text.toUtf8());
    
    // Проверяем результат
    if (hash.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось вычислить хеш");
        return;
    }
    
    // Преобразуем результат в нужный формат
    QString result;
    if (ui->comboOutputFormatSHA384->currentIndex() == 0) {
        // Hex
        result = hash.toHex();
    } else {
        // Base64
        result = hash.toBase64();
    }
    
    // Отображаем результат
    ui->editHashResult->setText(result);
    ui->labelSHA384Status->setText("Статус: Хеш успешно вычислен");
}

void MainWindow::onClearSHA384Clicked() {
    // Очищаем поля ввода
    ui->editInputTextSHA384->clear();
    ui->editHashResult->clear();
    ui->editExpectedHash->clear();
    ui->labelSelectedFile->setText("Файл не выбран");
    ui->labelVerifyResult->setText("Результат проверки");
    ui->labelSHA384Status->setText("Статус: Готов");
    m_selectedFilePath.clear();
}

void MainWindow::onCopyHashClicked() {
    // Копируем хеш в буфер обмена
    QString hash = ui->editHashResult->text();
    if (!hash.isEmpty()) {
        QApplication::clipboard()->setText(hash);
        ui->labelSHA384Status->setText("Статус: Хеш скопирован в буфер обмена");
    }
}

void MainWindow::onSelectFileClicked() {
    // Открываем диалог выбора файла
    QString filePath = QFileDialog::getOpenFileName(this, "Выберите файл", QString(), "Все файлы (*.*)");
    
    // Проверяем, выбран ли файл
    if (filePath.isEmpty()) {
        return;
    }
    
    // Сохраняем путь к файлу
    m_selectedFilePath = filePath;
    
    // Отображаем информацию о файле
    QFileInfo fileInfo(filePath);
    ui->labelSelectedFile->setText(fileInfo.fileName());
    ui->labelSHA384Status->setText("Статус: Файл выбран");
}

void MainWindow::onCalculateFileHashClicked() {
    // Проверяем, выбран ли файл
    if (m_selectedFilePath.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала выберите файл");
        return;
    }
    
    // Открываем файл
    QFile file(m_selectedFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }
    
    // Вычисляем хеш файла
    QByteArray hash = m_sha384Hasher->hash(file.readAll());
    file.close();
    
    // Проверяем результат
    if (hash.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось вычислить хеш файла");
        return;
    }
    
    // Преобразуем результат в нужный формат
    QString result;
    if (ui->comboOutputFormatSHA384->currentIndex() == 0) {
        // Hex
        result = hash.toHex();
    } else {
        // Base64
        result = hash.toBase64();
    }
    
    // Отображаем результат
    ui->editHashResult->setText(result);
    ui->labelSHA384Status->setText("Статус: Хеш файла успешно вычислен");
}

void MainWindow::onVerifyHashClicked() {
    // Получаем вычисленный хеш
    QString computedHash = ui->editHashResult->text().trimmed();
    
    // Проверяем, что хеш не пустой
    if (computedHash.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Сначала вычислите хеш");
        return;
    }
    
    // Получаем ожидаемый хеш
    QString expectedHash = ui->editExpectedHash->text().trimmed();
    
    // Проверяем, что ожидаемый хеш не пустой
    if (expectedHash.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ожидаемый хеш для проверки");
        return;
    }
    
    // Сравниваем хеши
    bool match = (computedHash.compare(expectedHash, Qt::CaseInsensitive) == 0);
    
    // Отображаем результат
    if (match) {
        ui->labelVerifyResult->setText("Результат проверки: Хеши совпадают");
        ui->labelVerifyResult->setStyleSheet("QLabel { background-color: #3B4252; color: #A3BE8C; border: 1px solid #4C566A; border-radius: 4px; padding: 4px; }");
    } else {
        ui->labelVerifyResult->setText("Результат проверки: Хеши не совпадают");
        ui->labelVerifyResult->setStyleSheet("QLabel { background-color: #3B4252; color: #BF616A; border: 1px solid #4C566A; border-radius: 4px; padding: 4px; }");
    }
}

// ===== Методы для вкладки "Admin" =====

void MainWindow::onUsersClicked() {
    MyClient::getInstance()->sendCommand("list_users");  // команда серверу
}

void MainWindow::appendToAdmin(const QString &message) {
    ui->textAdminInfo->append(message);
}

void MainWindow::setRole(const QString& role) {
    ui->tabWidget->setCurrentIndex(0);

    if (role == "admin") {
        ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabAdmin), true);
    } else {
        ui->tabWidget->setTabEnabled(ui->tabWidget->indexOf(ui->tabAdmin), false);
        //QMessageBox::warning(this, "Ошибка доступа", "Недостаточно прав для доступа к вкладке Admin.");
    }
}

// ===== Общие методы =====

void MainWindow::onTabChanged(int index) {
    QWidget* currentTab = ui->tabWidget->widget(index);
    // Обработка изменения вкладки
    switch (index) {
        case 0: // Вкладка "Стеганография"
            if (!m_originalImage.isNull()) {
                displayImage(m_originalImage);
            }
            break;
        case 1: // Вкладка "Метод хорд"
            // Ничего не делаем
            break;
        case 2: // Вкладка "AES"
            // Ничего не делаем
            break;
        case 3: // Вкладка "SHA-384"
            // Ничего не делаем
            break;
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

void MainWindow::appendToChordOutput(const QString &message) {
    ui->textChordOutput->append(message);
}

