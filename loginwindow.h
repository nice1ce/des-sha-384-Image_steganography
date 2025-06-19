/**
 * @file loginwindow.h
 * @brief Заголовочный файл окна входа в приложение
 * @date 2025
 */

#pragma once

#include <QDialog>
#include "mainwindow_tabs.h"

namespace Ui {
class LoginWindow;
}

/**
 * @brief Класс LoginWindow представляет окно входа в приложение
 */
class LoginWindow : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса LoginWindow
     * @param parent Родительский виджет
     */
    explicit LoginWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Деструктор класса LoginWindow
     */
    ~LoginWindow();

private slots:
    /**
     * @brief Обработчик ответа от сервера
     * @param response Ответ от сервера
     */
    void handleServerResponse(const QString& response);
    
    /**
     * @brief Обработчик нажатия кнопки "Войти"
     */
    void onOkClicked();

private:
    Ui::LoginWindow *ui; ///< Указатель на UI окна входа
    MainWindow *mainWindow = nullptr; ///< Указатель на главное окно
};

