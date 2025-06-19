/**
 * @file main.cpp
 * @brief Точка входа в приложение
 * @date 2025
 */

#include "loginwindow.h"
#include <QApplication>

/**
 * @brief Главная функция приложения
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код завершения приложения
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаем и показываем окно входа
    LoginWindow w;
    w.show();

    return a.exec();
}

