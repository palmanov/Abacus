#ifndef BONESITEM_H
#define BONESITEM_H

#include <QGraphicsPixmapItem>

class BonesItem : public QGraphicsPixmapItem
{
 public:
  //! Положение костяшки
  enum class Position {
    kLeft,  ///< Слева
    kRight  ///< Справа
  };

 public:
  BonesItem(uint8_t spoke_index, uint8_t value)
      : spoke_index_(spoke_index), value_(value) {

  }

  /**
   * @brief spokeIndex
   *
   * Метод возвращает индекс спицы,
   * на которой находится костяшка
   *
   * @return Индекс спицы
   */
  inline uint8_t spokeIndex() const { return spoke_index_; }

  /**
   * @brief value
   *
   * Метод возращает значение костяшки.
   * Значение находится в пределах 1-10
   *
   * @return Значение костяшки
   */
  inline uint8_t value() const { return value_; }

  /**
   * @brief setPosition
   *
   * Метод задаёт положение костяшки
   *
   * @param new_position Новое положение костяшки
   */
  void setPosition(Position new_position) { position_ = new_position; }

  /**
   * @brief position
   *
   * Метод возвращает положение костяшки
   *
   * @return Положение костяшки
   */
  inline Position position() const { return position_; }

 private:
  //! Индекс спицы
  uint8_t spoke_index_;
  //! Значение костяшки
  uint8_t value_;
  //! Положение костяшки (по умолчанию - справа)
  Position position_ = Position::kRight;
};

#endif // BONESITEM_H
