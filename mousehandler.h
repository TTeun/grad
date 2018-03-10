#ifndef MOUSEHANDLER_H
#define MOUSEHANDLER_H

#include <QMouseEvent>
#include <QObject>
#include <QVector2D>

class MouseHandler : public QObject {
  Q_OBJECT
 private:
  QVector2D m_mousePosition;
  size_t    m_width;
  size_t    m_height;

 public:
  explicit MouseHandler(QObject* parent = nullptr);

 public slots:
  void setWidth(const size_t& width);
  void setHeight(const size_t& height);

 public:
  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);

 signals:
  void clicked(QVector2D const& pos);
  void rightClicked(QVector2D const& pos);
  void leftClicked(QVector2D const& pos);

 public slots:
};

#endif  // MOUSEHANDLER_H
