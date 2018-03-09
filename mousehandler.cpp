#include "mousehandler.h"
#include <QDebug>

MouseHandler::MouseHandler(QObject* parent) : QObject(parent) {}

void MouseHandler::mousePressEvent(QMouseEvent* event) {
  QVector2D m_mousePosition{static_cast<double>(event->x()) / m_width,
                            static_cast<double>(event->y()) / m_height};
  m_mousePosition[0] = (m_mousePosition[0] - 0.5) * 2.;
  m_mousePosition[1] = -(m_mousePosition[1] - 0.5) * 2.;

  emit clicked(m_mousePosition);
  if (event->button() == Qt::MouseButton::RightButton)
    emit rightClicked(m_mousePosition);
  if (event->button() == Qt::MouseButton::RightButton)
    emit leftClicked(m_mousePosition);
}

void MouseHandler::mouseReleaseEvent(QMouseEvent* event) {}

void MouseHandler::mouseMoveEvent(QMouseEvent* event) {}

void MouseHandler::setHeight(const size_t& height) {
  m_height = height;
}

void MouseHandler::setWidth(const size_t& width) {
  m_width = width;
}
