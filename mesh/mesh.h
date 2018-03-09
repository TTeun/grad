#ifndef MESH_H
#define MESH_H

#include <memory>
#include "face.h"
#include "halfedge.h"
#include "objfile.h"
#include "vertex.h"

class Mesh {
 public:
  Mesh();
  Mesh(OBJFile const* const loadedOBJFile);
  ~Mesh();

  void copy(Mesh const* mesh);

  const QVector<Vertex>&   vertices() const;
  const QVector<Face>&     faces() const;
  const QVector<HalfEdge>& halfEdges() const;

  QVector<Vertex>&   vertices();
  QVector<Face>&     faces();
  QVector<HalfEdge>& halfEdges();

  bool checkMesh() const;

  std::unique_ptr<Mesh> ccSubdiv() const;
  std::unique_ptr<Mesh> ternarySubdiv() const;

 private:
  QVector<Vertex>          m_vertices;
  QVector<Face>            m_faces;
  QVector<HalfEdge>        m_halfEdges;
  QVector<QVector<size_t>> PotentialTwins;

  void setTwins(size_t numHalfEdges, size_t indexH);
};

#endif  // MESH_H
