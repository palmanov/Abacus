#include "MainWindow.h"

#include <QGraphicsView>

#include "AbacusScene.h"
#include "CalculatorWidget.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      scene_(new AbacusScene),
      view_(new QGraphicsView(scene_, this)),
      calculator_(new CalculatorWidget)
{
  initUI();

  QObject::connect(scene_, &AbacusScene::sendToLCD,
                   calculator_, &CalculatorWidget::setNumber);

  QObject::connect(calculator_, &CalculatorWidget::equal,
                   scene_, &AbacusScene::convertNumberToBones);
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
  setWindowTitle(tr("Русские счёты"));

  addCalculatorDockbar();

  setCentralWidget(view_);
}

void MainWindow::addCalculatorDockbar()
{
  QDockWidget* calculator_dockwidget_ = new QDockWidget(tr("Калькулятор"));
  calculator_dockwidget_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  calculator_dockwidget_->setWidget(calculator_);

  addDockWidget(Qt::RightDockWidgetArea, calculator_dockwidget_);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
  QMainWindow::resizeEvent(e);

  view_->fitInView(view_->scene()->sceneRect(), Qt::KeepAspectRatio);
}
