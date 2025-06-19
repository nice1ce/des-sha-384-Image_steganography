#ifndef CHORDMETHOD_H
#define CHORDMETHOD_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPointF>
#include <functional>

/**
 * @brief Класс ChordMethod реализует метод хорд для поиска корней уравнений
 */
class ChordMethod : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса ChordMethod
     * @param parent Родительский объект
     */
    explicit ChordMethod(QObject *parent = nullptr);

    /**
     * @brief Проверяет наличие корня на заданном интервале
     * @param function Функция f(x)
     * @param a Левая граница интервала
     * @param b Правая граница интервала
     * @return true, если на интервале есть корень, иначе false
     */
    bool hasRoot(const std::function<double(double)>& function, double a, double b);

    /**
     * @brief Решает уравнение f(x) = 0 методом хорд
     * @param function Функция f(x)
     * @param a Левая граница интервала
     * @param b Правая граница интервала
     * @param epsilon Точность вычислений
     * @param maxIterations Максимальное количество итераций
     * @return Найденный корень уравнения
     */
    double solve(const std::function<double(double)>& function, double a, double b, 
                double epsilon = 1e-6, int maxIterations = 100);

    /**
     * @brief Возвращает количество выполненных итераций
     * @return Количество итераций
     */
    int getIterationCount() const;

    /**
     * @brief Возвращает последнюю ошибку
     * @return Текст ошибки
     */
    QString lastError() const;

    /**
     * @brief Вычисляет точки функции для построения графика
     * @param function Функция f(x)
     * @param a Левая граница интервала
     * @param b Правая граница интервала
     * @param pointCount Количество точек
     * @return Вектор точек (x, f(x))
     */
    QVector<QPointF> calculateFunctionPoints(const std::function<double(double)>& function, 
                                           double a, double b, int pointCount = 100);

    /**
     * @brief Возвращает историю итераций метода хорд
     * @return Вектор точек (x, f(x)) для каждой итерации
     */
    QVector<QPointF> getIterationHistory() const;

private:
    QString m_lastError; ///< Последняя ошибка
    int m_iterationCount; ///< Количество выполненных итераций
    QVector<QPointF> m_iterationHistory; ///< История итераций
};

#endif // CHORDMETHOD_H

