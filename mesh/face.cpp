#include "face.h"
#include "halfedge.h"

HalfEdge* Face::side() const { return m_side; }

unsigned short Face::val() const { return m_val; }

unsigned int Face::index() const { return m_index; }

QVector2D Face::center() const {
  QVector2D c;
  auto      currentEdge = m_side;
  for (size_t i = 0; i != m_val; ++i) {
    c += currentEdge->target()->coords();
    currentEdge = currentEdge->next();
  }
  c /= m_val;
  return c;
}

QVector3D Face::centerColour() const {
  QVector3D c;
  auto      currentEdge = m_side;
  for (size_t i = 0; i != m_val; ++i) {
    c += currentEdge->colour();
    currentEdge = currentEdge->next();
  }
  c /= m_val;
  return c;
}

bool Face::containsPoint(const QVector2D& point) {
  std::vector<QVector2D> points;
  HalfEdge*              currentEdge = m_side;
  for (size_t i = 0; i != m_val; ++i) {
    points.push_back(currentEdge->target()->coords());
    currentEdge = currentEdge->next();
  }

  bool pointIsInCenter = false;

  for (size_t i = 0, j = m_val - 1; i < m_val; j = i++) {
    if (((points[i].y() >= point.y()) != (points[j].y() >= point.y())) &&
        (point.x() <= (points[j].x() - points[i].x()) * (point.y() - points[i].y()) /
                              (points[j].y() - points[i].y()) +
                          points[i].x()))
      pointIsInCenter = !pointIsInCenter;
  }

  return pointIsInCenter;
}

void Face::setSide(HalfEdge* side) { m_side = side; }
