#include "CalculatorWidget.h"

#include <QGridLayout>

#include <QLCDNumber>
#include <QPushButton>

#ifdef QT_DEBUG
#include <QDebug>
#endif

CalculatorWidget::CalculatorWidget(QWidget *parent)
    : QWidget{parent},
      lcd_number_(new QLCDNumber(10)),
      addition_button_(new QPushButton("+")),
      subtraction_button_(new QPushButton("-")),
      equal_button_(new QPushButton("=")),
      reset_button_(new QPushButton("C"))
{
  initUI();

  QObject::connect(addition_button_, &QPushButton::pressed,
                   this, [&]() {
                     buffer_ = lcd_number_->value();
                     operation_ = Operation::kAddition;
                   });

  QObject::connect(subtraction_button_, &QPushButton::pressed,
                   this, [&]() {
                     buffer_ = lcd_number_->value();
                     operation_ = Operation::kSubstraction;
                   });

  QObject::connect(equal_button_, &QPushButton::pressed,
                   this, &CalculatorWidget::calculate);

  QObject::connect(reset_button_, &QPushButton::pressed,
                   this, [&]() {
                     buffer_ = 0.0;
                     operation_ = Operation::kNone;

                     emit equal(0.0);
                   });
}

void CalculatorWidget::initUI()
{
  this->setMaximumWidth(256);
  lcd_number_->setFixedHeight(100);
  lcd_number_->setSmallDecimalPoint(true);
  lcd_number_->setSegmentStyle(QLCDNumber::SegmentStyle::Flat);

  QGridLayout* layout = new QGridLayout();

  // Row 0
  layout->addWidget(lcd_number_, 0, 0, 1, 4);

  // Row 1
  layout->addWidget(addition_button_, 1, 0);
  layout->addWidget(subtraction_button_, 1, 1);
  layout->addWidget(equal_button_, 1, 2);
  layout->addWidget(reset_button_, 1, 3);

  layout->setRowStretch(2, 1);

  setLayout(layout);
}

void CalculatorWidget::setNumber(double number)
{
  lcd_number_->display(number);
}

void CalculatorWidget::calculate()
{
  double result = 0.0;
  switch (operation_) {
    case Operation::kAddition: {
      result = buffer_ + lcd_number_->value();
      break;
    }
    case Operation::kSubstraction: {
      result = buffer_ - lcd_number_->value();
      break;
    }
    default: break;
  }

  operation_ = Operation::kNone;

  //! Отрицательные числа и больше 100.000 отбрасываются
  if (result >= 0 && result < 100000)
    emit equal(result);
}
