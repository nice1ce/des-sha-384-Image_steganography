#include "chordmethod.h"
#include <cmath>

ChordMethod::ChordMethod(QObject *parent) : QObject(parent), m_iterationCount(0)
{
}

bool ChordMethod::hasRoot(const std::function<double(double)>& function, double a, double b)
{
    // Проверяем, что функция меняет знак на интервале [a, b]
    double fa = function(a);
    double fb = function(b);
    
    // Если одно из значений равно нулю, то это корень
    if (std::abs(fa) < 1e-10 || std::abs(fb) < 1e-10) {
        return true;
    }
    
    // Если функция меняет знак на интервале, то на нем есть корень
    if (fa * fb < 0) {
        return true;
    }
    
    m_lastError = "На заданном интервале нет корня или их несколько";
    return false;
}

double ChordMethod::solve(const std::function<double(double)>& function, double a, double b, 
                        double epsilon, int maxIterations)
{
    // Сбрасываем счетчик итераций, историю итераций и ошибку
    m_iterationCount = 0;
    m_iterationHistory.clear();
    m_lastError.clear();
    
    // Проверяем корректность интервала
    if (a >= b) {
        m_lastError = "Левая граница должна быть меньше правой";
        return 0;
    }
    
    // Проверяем наличие корня на интервале
    if (!hasRoot(function, a, b)) {
        return 0;
    }
    
    // Вычисляем значения функции на границах интервала
    double fa = function(a);
    double fb = function(b);
    
    // Если одно из значений равно нулю, то это корень
    if (std::abs(fa) < epsilon) {
        m_iterationHistory.append(QPointF(a, fa));
        return a;
    }
    if (std::abs(fb) < epsilon) {
        m_iterationHistory.append(QPointF(b, fb));
        return b;
    }
    
    // Выполняем итерации метода хорд
    double c = a;
    double fc;
    
    while (m_iterationCount < maxIterations) {
        // Вычисляем новое приближение корня
        c = a - fa * (b - a) / (fb - fa);
        fc = function(c);
        
        // Сохраняем текущую итерацию
        m_iterationHistory.append(QPointF(c, fc));
        
        // Увеличиваем счетчик итераций
        m_iterationCount++;
        
        // Проверяем условие сходимости
        if (std::abs(fc) < epsilon) {
            return c;
        }
        
        // Обновляем границы интервала
        if (fa * fc < 0) {
            b = c;
            fb = fc;
        } else {
            a = c;
            fa = fc;
        }
        
        // Проверяем условие сходимости по аргументу
        if (std::abs(b - a) < epsilon) {
            return c;
        }
    }
    
    // Если достигнуто максимальное количество итераций
    m_lastError = "Превышено максимальное количество итераций";
    return c;
}

int ChordMethod::getIterationCount() const
{
    return m_iterationCount;
}

QString ChordMethod::lastError() const
{
    return m_lastError;
}

QVector<QPointF> ChordMethod::calculateFunctionPoints(const std::function<double(double)>& function, 
                                                   double a, double b, int pointCount)
{
    QVector<QPointF> points;
    
    // Проверяем корректность интервала
    if (a >= b || pointCount <= 0) {
        return points;
    }
    
    // Вычисляем шаг
    double step = (b - a) / (pointCount - 1);
    
    // Вычисляем точки функции
    for (int i = 0; i < pointCount; ++i) {
        double x = a + i * step;
        double y = function(x);
        
        // Ограничиваем значения функции для корректного отображения
        if (std::isfinite(y) && std::abs(y) < 1000) {
            points.append(QPointF(x, y));
        }
    }
    
    return points;
}

QVector<QPointF> ChordMethod::getIterationHistory() const
{
    return m_iterationHistory;
}

