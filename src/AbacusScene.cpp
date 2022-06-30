#include "AbacusScene.h"

#include <array>

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QTimeLine>
#include <QRandomGenerator>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "BonesItem.h"

/**
 * @warning Так как изображение-подложка имеет фиксированный размер,
 * "захардкодил" область внутри рамки счёт
 */
constexpr QRectF inside_frame_rect = QRectF(QPointF(106, 178), QPointF(1124, 1678));

//! Количество спиц в счётах
constexpr int number_of_spokes = 10;

//! Вес самой малой единицы счёт
constexpr int less_weight = -4;

//! Ширина текстуры косточки
constexpr int bones_image_width = 55;

//! Длительность анимации перемещения косточек (мс)
constexpr int animation_duration = 350;

//! Анимация в 60 к/с
constexpr int animation_fps90 = static_cast<int>(1000.0 / 90);
//! Анимация в 30 к/с
constexpr int animation_fps60 = static_cast<int>(1000.0 / 60);
//! Анимация в 21 к/с
constexpr int animation_fps21 = static_cast<int>(1000.0 / 21);

AbacusScene::AbacusScene()
{
  init();

  QObject::connect(this, &AbacusScene::clickAt, &AbacusScene::moveBones);
}

void AbacusScene::init()
{
  // Инициализация объектов для анимации
  {
    animation_ = new QGraphicsItemAnimation;
    timeline_ = new QTimeLine(animation_duration);
    timeline_->setUpdateInterval(animation_fps60);

    QObject::connect(timeline_, &QTimeLine::finished,
                     this, &AbacusScene::animationFinished);

    animation_->setTimeLine(timeline_);
  }

  // Рамка счёт
  auto frame_item = new QGraphicsPixmapItem(QPixmap(":/image/frame.png"));
  frame_item->setFlag(QGraphicsItem::ItemIsSelectable, false);

  const QPixmap line_pixmap(":/image/line.png");
  const QPixmap lighter_bones(":/image/lighter.png");
  const QPixmap lighter_bones_2(":/image/lighter2.png");
  const QPixmap darker_bones(":/image/darker1.png");
  const QPixmap darker_bones_2(":/image/darker2.png");

  //
  const double y_padding = inside_frame_rect.top();
  const double height = inside_frame_rect.height();
  const double x_offset = inside_frame_rect.left();

  const double y_step = height / 9;

  double y_offset = y_padding + y_step;
  for (int i = 0; i < 10; i++) {
    std::array<std::stack<BonesItem *>, 2> array;

    auto line_item = new QGraphicsPixmapItem(line_pixmap);
    line_item->setFlag(QGraphicsItem::ItemIsSelectable, false);
    line_item->setPos( { x_offset, y_offset } );

    const double y_center_line = y_offset + line_pixmap.height()/2;

    addItem(line_item);

    double bones_x = inside_frame_rect.right();
    int bones = 10;
    if (i == 5)
      bones = 4;

    for (int j = 0; j < bones; j++) {
      auto bones_item = new BonesItem(i, bones-j);

      if ((bones == 10 && (j == 4 || j == 5))
          || (bones == 4 && (j == 1 || j == 2))) {
        bones_item->setPixmap(darker_bones_2);
      } else {
        const auto type = QRandomGenerator::global()->bounded(0, 3);
        switch (type) {
          case 0: bones_item->setPixmap(lighter_bones); break;
          case 1: bones_item->setPixmap(lighter_bones_2); break;
          case 2: bones_item->setPixmap(darker_bones); break;
        }
      }

      bones_x -= bones_image_width;

      bones_item->setPos( { bones_x, y_center_line - darker_bones.height()/2 } );

      addItem(bones_item);
      array.at(1).push(bones_item);
    }

    model_.push_back(array);

    y_offset += y_step - line_pixmap.height() / 2;
  }


  addItem(frame_item);

  group_ = new QGraphicsItemGroup;
  animation_->setItem(group_);
  animation_->setPosAt(0.0, { 0.0, 0.0 });

  addItem(group_);
}

void AbacusScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
  if (!views().isEmpty()) {
    auto item = itemAt(mouseEvent->scenePos(), views().at(0)->transform());
    if (auto bones = dynamic_cast<BonesItem *>(item)) {
      emit clickAt(bones->spokeIndex(), bones->value(), bones->position());
    }
  }

  QGraphicsScene::mousePressEvent(mouseEvent);
}

void AbacusScene::moveBones(uint8_t spoke, uint8_t value, BonesItem::Position position)
{
  if (!animation_queue_.empty())
    animation_queue_.pop();

  if (timeline_->state() == QTimeLine::Running
      || spoke >= number_of_spokes)
    return;

  for (auto& item : group_->childItems())
    group_->removeFromGroup(item);

  group_->setPos(0,0);

  auto& spoke_v = model_.at(spoke);

  assert(spoke_v.size() == 2);

  // Левый стек
  auto& l_stack = spoke_v.at(0);
  // Правый стек
  auto& r_stack = spoke_v.at(1);

  QPointF start_point, end_point;

  if (position == BonesItem::Position::kLeft) {
    if (l_stack.empty()) {
      timeline_->start();
      return;
    }

    // Рассчитаем кол-во костяшек для перемещения
    int diff = l_stack.top()->value() - value + 1;

    // Зададим начальную точку анимации
    start_point = l_stack.top()->scenePos();

    // Зададим конечную точку анимации
    if (r_stack.empty()) {
      end_point = QPointF(inside_frame_rect.right() - bones_image_width, start_point.y());
    } else {
      end_point = r_stack.top()->scenePos() - QPointF(bones_image_width, 0.0);
    }

    // Перемещаем костяшки из левой части в правую
    for (int i = 0; i < diff; i++) {
      r_stack.push(l_stack.top());
      r_stack.top()->setPosition(BonesItem::Position::kRight);
      l_stack.pop();

      // Добавим в группу для перемещения
      group_->addToGroup(r_stack.top());
    }
  } else {
    if (r_stack.empty()) {
      timeline_->start();
      return;
    }

    // Рассчитаем кол-во костяшек для перемещения
    int diff = value - r_stack.top()->value() + 1;

    // Зададим начальную точку анимации
    start_point = r_stack.top()->scenePos();

    // Зададим конечную точку анимации
    if (l_stack.empty()) {
      end_point = QPointF(inside_frame_rect.left(), start_point.y());
    } else {
      end_point = l_stack.top()->scenePos() + QPointF(bones_image_width, 0.0);
    }

    // Перемещаем костяшки из правой части в левую
    for (int i = 0; i < diff; i++) {
      l_stack.push(r_stack.top());
      l_stack.top()->setPosition(BonesItem::Position::kLeft);
      r_stack.pop();

      // Добавим в группу для перемещения
      group_->addToGroup(l_stack.top());
    }
  }

  // Передадим анимации конечную точку
  animation_->setPosAt(1.0, end_point - start_point);

  // Запуск анимации
  timeline_->start();

  // Вычислим полученное значение из костяшек
  calculate();
}

void AbacusScene::animationFinished()
{
  const auto type = static_cast<int>(QRandomGenerator::global()->bounded(1, 6));
  const QUrl sound_source = QUrl::fromLocalFile(QString(":/audio/effect%1.wav").arg(type));

  effect.setSource(sound_source);
  effect.setLoopCount(1);
  effect.setVolume(0.25f);
  effect.play();

  if (!animation_queue_.empty()) {
    const auto& [spoke, value, position] = animation_queue_.front();
    moveBones(spoke, value, position);
  }
}

void AbacusScene::calculate()
{
  double result = 0.0;
  int weight = less_weight;
  for (int i = number_of_spokes-1; i >= 0; i--) {
    const auto& left_stack = model_.at(i).at(0);

    const auto size = left_stack.size();
    if (size > 0) {
      if (i != 5) {
        result += size * std::pow(10, weight);
      } else {
        result += size * 0.25;
      }
    }

    if (i != 5)
      weight++;
  }

  emit sendToLCD(result);
}

void AbacusScene::convertNumberToBones(double number)
{
  int weight = less_weight;
  for (int i = number_of_spokes-1; i >= 0; i--) {
    if (i == 5) {
      animation_queue_.push( { i, 1, BonesItem::Position::kLeft } );
      continue;
    }

    double t = number / std::pow(10, weight);
    int new_bones_size = static_cast<int>(t) % 10;

    BonesItem::Position is_left = BonesItem::Position::kRight;
    if (model_.at(i).at(0).size() == new_bones_size) {
      weight++;
      continue;
    }

    if (model_.at(i).at(0).size() > new_bones_size) {
      is_left = BonesItem::Position::kLeft;
    }

    if (is_left == BonesItem::Position::kLeft)
      animation_queue_.push( { i, new_bones_size+1, is_left } );
    else
      animation_queue_.push( { i, new_bones_size, is_left } );

    weight++;
  }

  if (!animation_queue_.empty()) {
    const auto& [spoke, value, left] = animation_queue_.front();
    moveBones(spoke, value, left);
  }
}
