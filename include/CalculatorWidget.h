#ifndef CALCULATORWIDGET_H
#define CALCULATORWIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLCDNumber;
class QPushButton;
QT_END_NAMESPACE

/** Виджет "Калькулятор" */
class CalculatorWidget : public QWidget
{
  Q_OBJECT

  /// Доступные операции калькулятора
  enum class Operation {
    kNone,          ///< Нет операции
    kAddition,      ///< Суммировать
    kSubstraction   ///< Вычесть
  };

 public:
  explicit CalculatorWidget(QWidget *parent = nullptr);

 signals:
  /**
   * @brief equal
   *
   * Сигнал эммитится после завершения операции над числами
   *
   * @param result Численный результат операции
   */
  void equal(double result);

 public slots:
  /**
   * @brief setNumber
   *
   * Метод задаёт новое значение
   *
   * @param number[in] Новое значение
   */
  void setNumber(double number);

 private slots:
  /**
   * @brief calculate
   *
   * Метод вычисления нового значения
   */
  void calculate();

 private:
  /**
   * @brief initUI
   *
   * Инициализация интерфейса
   */
  void initUI();

 private:
  //! Семисегментник
  QLCDNumber* lcd_number_;
  //! Кнопка "Плюс"
  QPushButton* addition_button_;
  //! Кнопка "Минус"
  QPushButton* subtraction_button_;
  //! Кнопка "Равно"
  QPushButton* equal_button_;
  //! Кнопка сброса
  QPushButton* reset_button_;
  //! Арифметическая операция (по умолчанию - None)
  Operation operation_ = Operation::kNone;
  //! Буфер
  double buffer_ = 0.0;
};

#endif // CALCULATORWIDGET_H
