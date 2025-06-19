#include "registerdialog.h"
#include "ui_dialog.h"
#include "myclient.h"

RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setWindowTitle("Регистрация");
    ui->textEdit_3->setReadOnly(true);

    // Кнопка "Зарегистрироваться"
    connect(ui->pushButton, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);

    // Обработка ответа от сервера
    connect(MyClient::getInstance(), &MyClient::responseReceived, this, [this](const QString& response) {
        ui->textEdit_3->setText(response);

        if (response.contains("Регистрация успешна", Qt::CaseInsensitive)) {
            this->accept();  // Закрыть окно при успехе
        }
    });
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::onRegisterClicked()
{
    QString login = ui->lineEdit->text().trimmed();
    QString password = ui->lineEdit_2->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        ui->textEdit_3->setText("Введите логин и пароль.");
        return;
    }

    QString command = QString("reg %1:%2").arg(login, password);
    MyClient::getInstance()->sendCommand(command);
}
