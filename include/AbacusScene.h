#ifndef ABACUSSCENE_H
#define ABACUSSCENE_H

#include <queue>
#include <stack>

#include <QGraphicsScene>
#include <QSoundEffect>
#include <QGraphicsItemAnimation>

#include "BonesItem.h"

class AbacusScene : public QGraphicsScene
{
  Q_OBJECT
 public:
  AbacusScene();

 signals:
  /**
   * @brief clickAt
   *
   * Сигнал эммитится при нажатии на костяшку
   *
   * @param spoke     Индекс спицы
   * @param value     Значение костяшки
   * @param position  Положение костяшки
   */
  void clickAt(uint8_t spoke, uint8_t value, BonesItem::Position position);

  /**
   * @brief sendToLCD
   *
   * Сигнал отправки нового значения на семисегментник
   *
   * @param value Новое значение
   */
  void sendToLCD(double value);

 public slots:
  /**
   * @brief convertNumberToBones
   *
   * Слот конвертирует численное значение
   * в положение костяшек на счётах
   *
   * @param number
   */
  void convertNumberToBones(double number);

 private:
  /**
   * @brief init
   *
   * Метод инициализации сцены
   */
  void init();

  /**
   * @brief calculate
   *
   * Метод вычисляет численное значение из костяшек
   */
  void calculate();

 protected:
  /**
   * @brief mousePressEvent
   *
   * Событие клика мыши
   */
  virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

 private slots:
  /**
   * @brief moveBones
   *
   * Слот перемещения костяшек на спицах
   *
   * @param spoke     Индекс спицы
   * @param value     Значение костяшки
   * @param position  Положение костяшки
   */
  void moveBones(uint8_t spoke, uint8_t value, BonesItem::Position position);

  /**
   * @brief animationFinished
   *
   * Слот служит для воспроизведения звукового эффекта
   * и запуска анимации для других костяшек
   */
  void animationFinished();

 private:
  //! Звуковой эффект перемещения костяшек
  QSoundEffect effect;
  //! Группа элементов для перемещения по спице
  QGraphicsItemGroup* group_;
  //! Анимация перемещения
  QGraphicsItemAnimation* animation_;
  //! Временная шкала для управления анимацией
  QTimeLine* timeline_;
  //! Очередь анимации
  std::queue<std::tuple<int, int, BonesItem::Position>> animation_queue_;
  //! Модель счётов
  std::vector<std::array<std::stack<BonesItem *>, 2>> model_;

};

#endif // ABACUSSCENE_H
