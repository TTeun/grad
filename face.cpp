#include "face.h"
#include "halfedge.h"

HalfEdge* Face::side() const {
  return m_side;
}

unsigned short Face::val() const {
  return m_val;
}

unsigned int Face::index() const {
  return m_index;
}

QVector2D Face::center() const {
  QVector2D c;
  auto currentEdge = m_side;
  for (size_t i = 0; i != m_val; ++i) {
    c += currentEdge->target()->coords();
    currentEdge = currentEdge->next();
  }
  c /= m_val;
  return c;
}

QVector3D Face::centerColour() const {
  QVector3D c;
  auto currentEdge = m_side;
  for (size_t i = 0; i != m_val; ++i) {
    c += currentEdge->colour();
    currentEdge = currentEdge->next();
  }
  c /= m_val;
  return c;
}

void Face::setSide(HalfEdge* side) {
  m_side = side;
}
