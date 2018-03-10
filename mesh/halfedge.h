#ifndef HALFEDGE
#define HALFEDGE

#include <QVector2D>

#include "vertex.h"

// Forward declarations
class Face;

class HalfEdge {
 public:
  HalfEdge();
  HalfEdge(Vertex*          target,
           QVector3D const& colour,
           HalfEdge*        next,
           HalfEdge*        prev,
           HalfEdge*        twin,
           Face*            polygon,
           unsigned int     index,
           float            sharpness = 0);

  HalfEdge(Vertex*          target,
           QVector3D const& colour,
           HalfEdge*        next,
           HalfEdge*        prev,
           HalfEdge*        twin,
           Face*            polygon,
           unsigned int     index,
           float            sharpness,
           QVector2D const& colGrad);

  void setNext(HalfEdge* next);
  void setPrev(HalfEdge* prev);
  void setTwin(HalfEdge* twin);
  void setPolygon(Face* polygon);
  void setSharpness(float sharpness);
  void setColGrad(const QVector2D& colGrad);
  void setColour(const QVector3D& colour);

  HalfEdge* next() const;
  HalfEdge* prev() const;
  HalfEdge* twin() const;
  Face*     polygon() const;
  float     sharpness() const;
  QVector2D colGrad() const;
  QVector3D colour() const;

  QVector2D center() const;
  QVector3D centerColour() const;
  QVector3D endColour() const;

  double    distanceToPoint(const QVector2D& mousePoint) const;
  QVector2D start() const { return m_twin->m_target->coords(); }
  QVector2D end() const { return m_target->coords(); }
  float     length() const { return (start() - end()).length(); }

  unsigned int index() const;
  void         setIndex(unsigned int index);

  Vertex* target() const;
  void    setTarget(Vertex* target);

 private:
  Vertex*      m_target;
  QVector3D    m_colour;
  HalfEdge*    m_next;
  HalfEdge*    m_prev;
  HalfEdge*    m_twin;
  Face*        m_polygon;
  unsigned int m_index;
  float        m_sharpness;
  QVector2D    m_colGrad;
};

#endif  // HALFEDGE
