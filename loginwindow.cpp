/**
 * @file loginwindow.cpp
 * @brief Реализация окна входа в приложение
 * @date 2025
 */

#include "loginwindow.h"
#include "ui_login.h"
#include "registerdialog.h"
#include "myclient.h"
#include "mainwindow_tabs.h"

LoginWindow::LoginWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
    MyClient::getInstance()->connectToServer();
    setWindowTitle("Вход");
    ui->textEdit_3->setReadOnly(true);

    // обработка ответа от сервера
    connect(MyClient::getInstance(), &MyClient::responseReceived, this, [this](const QString& text) {
        ui->textEdit_3->append("> Ответ:\n" + text);

        if (text.contains("Авторизация успешна", Qt::CaseInsensitive)) {
            QString role = "user";
            if (text.contains("Роль: admin")) role = "admin";

            if (!mainWindow)
                mainWindow = new MainWindow(nullptr, role); // Передаём роль в MainWindow
            mainWindow->setRole(role);
            mainWindow->show();
            this->hide();
        }
    });

    // регистрация
    connect(ui->pushButton, &QPushButton::clicked, this, [this]() {
        RegisterDialog reg(this);
        reg.exec();
    });

    // кнопка OK
    connect(ui->pushButton_2, &QPushButton::clicked, this, &LoginWindow::onOkClicked);


    // выводим последний полученный ответ, если есть
    QString last = MyClient::getInstance()->getLastResponse();
    if (!last.isEmpty()) {
        ui->textEdit_3->append("> Последний ответ:\n" + last);
    }
}

LoginWindow::~LoginWindow() {
    delete ui;
}

void LoginWindow::handleServerResponse(const QString& response) {
    ui->textEdit_3->append("> Ответ:\n" + response);
}

void LoginWindow::onOkClicked() {
    QString login = ui->lineEdit->text().trimmed();
    QString password = ui->lineEdit_2->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        ui->textEdit_3->append("> Ошибка: введите логин и пароль.");
        return;
    }

    QString command = QString("auth %1:%2").arg(login, password);
    MyClient::getInstance()->sendCommand(command);
}

