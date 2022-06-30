#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QGraphicsView;
QT_END_NAMESPACE

class CalculatorWidget;
class AbacusScene;

#include <QDockWidget>

class MainWindow : public QMainWindow
{
  Q_OBJECT
 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  /**
   * @brief initUI
   *
   * Инициализация интерфейса
   */
  void initUI();

 protected:
  /**
   * @brief resizeEvent
   *
   * Событие изменения размера окна
   */
  virtual void resizeEvent(QResizeEvent* e) override;

 private:
  /**
   * @brief addCalculatorDockbar
   *
   * Метод добавляет виджет "Калькулятор" в докбар
   */
  void addCalculatorDockbar();

 private:
  //! Виджет "Калькулятор"
  CalculatorWidget* calculator_;
  //! Графическая сцена
  AbacusScene* scene_;
  //! Виджет отрисовки сцены
  QGraphicsView* view_;

};
#endif // MAINWINDOW_H
