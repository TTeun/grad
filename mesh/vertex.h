#ifndef VERTEX
#define VERTEX

#include <QDebug>
#include <QVector2D>
#include <QVector3D>

// Forward declaration
class HalfEdge;

class Vertex {
 public:
  Vertex();
  Vertex(QVector2D const& coords,
         HalfEdge*        out,
         unsigned short   val,
         unsigned int     index,
         float            sharpness = 0);

  Vertex(QVector2D const& vcoords);

  QVector2D coords() const;
  void      setCoords(const QVector2D& coords);

  HalfEdge* out() const;
  void      setOut(HalfEdge* out);

  unsigned short val() const;
  void           setVal(unsigned short val);

  unsigned int index() const;
  void         setIndex(unsigned int index);

  unsigned short sharpness() const;
  void           setSharpness(unsigned short sharpness);

  HalfEdge* getBoundaryEdge() const;

 private:
  QVector2D      m_coords;
  HalfEdge*      m_out;
  unsigned short m_val;
  unsigned int   m_index;
  unsigned short m_sharpness;
};

#endif  // VERTEX
