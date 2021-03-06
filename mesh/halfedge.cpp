#include "halfedge.h"
#include "face.h"

#include <cassert>
#include <climits>

HalfEdge::HalfEdge()
    : m_target(nullptr),
      m_colour(QVector3D(1.0, 1.0, 1.0)),
      m_next(nullptr),
      m_prev(nullptr),
      m_twin(nullptr),
      m_polygon(nullptr),
      m_index(std::numeric_limits<unsigned int>::max()),
      m_sharpness(0),
      m_colGrad(QVector2D(0.0, 0.0)) {}

HalfEdge::HalfEdge(Vertex*          target,
                   const QVector3D& colour,
                   HalfEdge*        next,
                   HalfEdge*        prev,
                   HalfEdge*        twin,
                   Face*            polygon,
                   unsigned int     index,
                   float            sharpness)
    : m_target(target),
      m_colour(colour),
      m_next(next),
      m_prev(prev),
      m_twin(twin),
      m_polygon(polygon),
      m_index(index),
      m_sharpness(sharpness),
      m_colGrad(QVector2D(0.0, 0.0)) {}

HalfEdge::HalfEdge(Vertex*          target,
                   const QVector3D& colour,
                   HalfEdge*        next,
                   HalfEdge*        prev,
                   HalfEdge*        twin,
                   Face*            polygon,
                   unsigned int     index,
                   float            sharpness,
                   QVector2D const& colGrad)

    : m_target(target),
      m_colour(colour),
      m_next(next),
      m_prev(prev),
      m_twin(twin),
      m_polygon(polygon),
      m_index(index),
      m_sharpness(sharpness),
      m_colGrad(colGrad) {}

HalfEdge* HalfEdge::next() const { return m_next; }

void HalfEdge::setNext(HalfEdge* next) {
  m_next       = next;
  next->m_prev = this;
}

HalfEdge* HalfEdge::prev() const { return m_prev; }

void HalfEdge::setPrev(HalfEdge* prev) { m_prev = prev; }

HalfEdge* HalfEdge::twin() const { return m_twin; }

void HalfEdge::setTwin(HalfEdge* twin) {
  m_twin       = twin;
  twin->m_twin = this;
}

Face* HalfEdge::polygon() const { return m_polygon; }

void HalfEdge::setPolygon(Face* polygon) { m_polygon = polygon; }

float HalfEdge::sharpness() const { return m_sharpness; }

void HalfEdge::setSharpness(float sharpness) { m_sharpness = sharpness; }

QVector2D HalfEdge::colGrad() const { return m_colGrad; }

void HalfEdge::setColGrad(const QVector2D& colGrad) { m_colGrad = colGrad; }

QVector3D HalfEdge::colour() const { return m_colour; }

QVector2D HalfEdge::center() const {
  QVector2D c = m_target->coords();
  c += m_twin->target()->coords();
  c /= 2;
  return c;
}

QVector3D HalfEdge::centerColour() const {
  if ((!m_polygon) || (!m_twin->polygon())) return 0.5 * (m_colour + m_prev->colour());

  QVector3D c = m_colour + m_prev->colour();
  c += m_polygon->centerColour();
  c += m_twin->polygon()->centerColour();
  return 0.25 * c;
}

QVector3D HalfEdge::endColour() const {
  QVector3D c(0., 0., 0.);
  auto      currentEdge = this;
  assert(m_target->val() != 0);
  for (size_t i = 0; i != m_target->val(); ++i) {
    if (currentEdge->polygon()) c += currentEdge->polygon()->centerColour();

    //    qDebug() << currentEdge->prev()->colour();
    c += currentEdge->prev()->colour();
    currentEdge = currentEdge->next()->twin();
  }
  c /= m_target->val() * m_target->val();

  c += (m_target->val() - 2.) * currentEdge->colour() / m_target->val();
  return c;
}

double HalfEdge::distanceToPoint(const QVector2D& mousePoint) const {
  QVector2D point = mousePoint - start();
  QVector2D dir   = end() - start();

  float d = QVector2D::dotProduct(dir, point);
  if (d < 0 || d > dir.lengthSquared()) return 10000.0f;

  dir.normalize();
  point = point - d * dir;
  return point.length();
}

unsigned int HalfEdge::index() const { return m_index; }

void HalfEdge::setIndex(unsigned int index) { m_index = index; }

Vertex* HalfEdge::target() const { return m_target; }

void HalfEdge::setTarget(Vertex* target) { m_target = target; }

void HalfEdge::setColour(const QVector3D& colour) { m_colour = colour; }
