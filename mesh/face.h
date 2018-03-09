#ifndef FACE
#define FACE

#include <QVector2D>

// Forward declaration
class HalfEdge;

class Face {
 public:
  Face() : m_side(nullptr), m_val(0), m_index(0) {}

  Face(HalfEdge* side, unsigned short val, unsigned int index)
      : m_side(side), m_val(val), m_index(index) {}

  HalfEdge*      side() const;
  void           setSide(HalfEdge* side);
  unsigned short val() const;
  unsigned int   index() const;

  QVector2D center() const;
  QVector3D centerColour() const;

  bool containsPoint(QVector2D const& point);

 private:
  HalfEdge*      m_side;
  unsigned short m_val;
  unsigned int   m_index;
};

#endif  // FACE
