#include "vertex.h"
#include "halfedge.h"

Vertex::Vertex() : m_coords(), m_out(nullptr), m_val(0), m_index(0), m_sharpness(0) {}

Vertex::Vertex(QVector2D const& coords,
               HalfEdge*        out,
               unsigned short   val,
               unsigned int     index,
               float            sharpness)
    : m_coords(std::move(coords)),
      m_out(out),
      m_val(val),
      m_index(index),
      m_sharpness(sharpness) {}

Vertex::Vertex(const QVector2D& coords)
    : m_coords(coords), m_out(nullptr), m_val(0), m_index(0), m_sharpness(0) {}

QVector2D Vertex::coords() const { return m_coords; }

void Vertex::setCoords(const QVector2D& coords) { m_coords = coords; }

HalfEdge* Vertex::out() const { return m_out; }

void Vertex::setOut(HalfEdge* out) { m_out = out; }

unsigned short Vertex::val() const { return m_val; }

void Vertex::setVal(unsigned short val) { m_val = val; }

unsigned int Vertex::index() const { return m_index; }

void Vertex::setIndex(unsigned int index) { m_index = index; }

unsigned short Vertex::sharpness() const { return m_sharpness; }

void Vertex::setSharpness(unsigned short sharpness) { m_sharpness = sharpness; }

HalfEdge* Vertex::getBoundaryEdge() const {
  auto currentEdge = m_out;
  for (size_t i = 0; i != m_val; ++i) {
    if (currentEdge->polygon() == nullptr) return currentEdge;
    currentEdge = currentEdge->prev()->twin();
  }
  return nullptr;
}
