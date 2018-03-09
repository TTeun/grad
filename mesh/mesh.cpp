#include "mesh.h"
#include <math.h>
#include <cassert>
#include "subdiv.h"

Mesh::Mesh() {}

Mesh::Mesh(const OBJFile* const loadedOBJFile) {
  // Convert loaded OBJ file to HalfEdge mesh
  size_t numVertices, numHalfEdges, numFaces;
  size_t k, m, n;

  numVertices  = loadedOBJFile->vertexCoords.size();
  numHalfEdges = 0;

  for (k = 0; k < (size_t)loadedOBJFile->faceValences.size(); k++)
    numHalfEdges += loadedOBJFile->faceValences[k];

  numFaces = loadedOBJFile->faceValences.size();

  // Note - resize() invokes the Vertex() constructor, reserve() does not.
  m_vertices.reserve(numVertices);
  // If boundaries are present, reserve twice as much = worst case scenario
  m_halfEdges.reserve(2 * numHalfEdges);
  m_faces.reserve(numFaces);

  // Add Vertices

  for (k = 0; k < numVertices; k++) {
    // Coords (x,y,z), Out, Valence, Index
    m_vertices.append(Vertex(loadedOBJFile->vertexCoords[k], nullptr, 0, k, 0));
    // Out and valence are unknown at this point.
  }

  qDebug() << "   # Vertices" << m_vertices.capacity() << m_vertices.size();

  size_t indexH       = 0;
  size_t currentIndex = 0;

  // Initialize every entry of PotentialTwins with an empty QVector (using
  // resize() )
  PotentialTwins.resize(loadedOBJFile->vertexCoords.size());

  // Add Faces and most of the HalfEdges

  for (m = 0; m < numFaces; m++) {
    // Side, Val, Index
    m_faces.append(Face(nullptr, loadedOBJFile->faceValences[m], m));

    for (n = 0; n < loadedOBJFile->faceValences[m]; n++) {
      // Target, Next, Prev, Twin, Poly, Index
      m_halfEdges.append(
          HalfEdge(&m_vertices[loadedOBJFile->faceCoordInd[currentIndex + n]],
                   QVector3D(1., 0.0, 0.0),
                   nullptr,
                   nullptr,
                   nullptr,
                   &m_faces[m],
                   indexH));

      // Next, Prev and Twin of the above HalfEdge have to be assigned later!
      // Starting below...

      if (n > 0) {
        m_halfEdges[indexH - 1].setNext(&m_halfEdges[indexH]);
        m_halfEdges[indexH].setPrev(&m_halfEdges[indexH - 1]);

        // Append index of HalfEdge to list of OutgoingHalfEdges of its
        // TailVertex.
        PotentialTwins[loadedOBJFile->faceCoordInd[currentIndex + n - 1]].append(indexH);
      }
      indexH++;
    }

    // HalfEdges[indexH-1] is the most recent addition.
    m_faces[m].setSide(&m_halfEdges[indexH - 1]);

    m_halfEdges[indexH - 1].setNext(&m_halfEdges[indexH - n]);
    m_halfEdges[indexH - n].setPrev(&m_halfEdges[indexH - 1]);

    PotentialTwins[loadedOBJFile->faceCoordInd[currentIndex + n - 1]].append(indexH - n);

    currentIndex += loadedOBJFile->faceValences[m];
  }

  qDebug() << "   # Faces" << m_faces.capacity() << m_faces.size();
  qDebug() << "   # HalfEdges" << m_halfEdges.capacity() << m_halfEdges.size();

  // Outs and Valences of vertices
  for (k = 0; k < (size_t)m_vertices.size(); k++) {
    if (PotentialTwins[k].size() == 0) {
      qWarning() << " ! Isolated Vertex? PotentialTwins empty for Index" << k;
      continue;
    }
    m_vertices[k].setOut(&m_halfEdges[PotentialTwins[k][0]]);
    // Not the correct valence when on the boundary! Fixed below.
    m_vertices[k].setVal(PotentialTwins[k].size());
  }

  setTwins(numHalfEdges, indexH);

  PotentialTwins.clear();
  PotentialTwins.squeeze();

  qDebug() << "   # Updated HalfEdges" << m_halfEdges.capacity() << m_halfEdges.size();

  HalfEdge* currentEdge;
  for (int i = 0; i < m_halfEdges.size(); ++i) {
    currentEdge = &m_halfEdges[i];
    currentEdge->setColGrad(0.33 * (currentEdge->target()->coords() -
                                    currentEdge->twin()->target()->coords()));
  }

  for (size_t k = 0; k != numVertices; ++k) {
    auto vertex      = m_vertices[k];
    auto currentEdge = vertex.out();
    for (size_t i = 0; i != vertex.val(); ++i) {
      currentEdge->prev()->setColour(loadedOBJFile->vertexColours[k]);
      currentEdge = currentEdge->twin()->next();
    }
  }
}

void Mesh::copy(Mesh const* mesh) {
  if (mesh == nullptr) {
    qDebug() << "Copying nullptr mesh";
    return;
  }
  if (mesh == this) {
    qDebug() << "Copying mesh into itself";
    return;
  }

  size_t numVerts = mesh->m_vertices.size();
  size_t numEdges = mesh->m_halfEdges.size();
  size_t numFaces = mesh->m_faces.size();

  m_vertices.clear();
  m_vertices.reserve(numVerts);
  m_halfEdges.clear();
  m_halfEdges.reserve(numEdges);
  m_faces.clear();
  m_faces.reserve(numFaces);

  Vertex const*   currentVertex;
  HalfEdge const* currentEdge;
  Face const*     currentFace;

  for (size_t vs = 0; vs < numVerts; ++vs) {
    currentVertex = &mesh->m_vertices[vs];
    //        (&coords, out, val, index, sharpness = 0, colour =
    //        QVector3D(1.0, 1.0, 0.0))

    m_vertices.append(Vertex(currentVertex->coords(),
                             nullptr,
                             currentVertex->val(),
                             currentVertex->index(),
                             currentVertex->sharpness()));
    if (currentVertex->index() != vs) qDebug() << "Vertex index mismatch";
  }

  for (size_t fs = 0; fs < numFaces; ++fs) {
    // (HalfEdge* fside, unsigned short fval, unsigned int findex) {
    currentFace = &mesh->m_faces[fs];
    m_faces.append(Face(nullptr, currentFace->val(), fs));
    if (currentFace->index() != fs) qDebug() << "Face index mismatch";
  }

  for (size_t es = 0; es < numEdges; ++es) {
    currentEdge = &mesh->m_halfEdges[es];
    m_halfEdges.append(HalfEdge(nullptr,
                                QVector3D(1., 0.0, 0.0),
                                nullptr,
                                nullptr,
                                nullptr,
                                nullptr,
                                currentEdge->index(),
                                currentEdge->sharpness(),
                                currentEdge->colGrad()));
    if (currentEdge->index() != es) qDebug() << "Edge index mismatch";
  }

  for (size_t vs = 0; vs < numVerts; ++vs)
    m_vertices[vs].setOut(&m_halfEdges[mesh->m_vertices[vs].out()->index()]);

  for (size_t fs = 0; fs < numFaces; ++fs)
    m_faces[fs].setSide(&m_halfEdges[mesh->m_faces[fs].side()->index()]);

  for (size_t es = 0; es < numEdges; ++es) {
    currentEdge = &mesh->m_halfEdges[es];
    m_halfEdges[es].setTarget(&m_vertices[currentEdge->target()->index()]);
    m_halfEdges[es].setNext(&m_halfEdges[currentEdge->next()->index()]);
    m_halfEdges[es].setPrev(&m_halfEdges[currentEdge->prev()->index()]);
    m_halfEdges[es].setTwin(&m_halfEdges[currentEdge->twin()->index()]);
    if (currentEdge->polygon())
      m_halfEdges[es].setPolygon(&m_faces[currentEdge->polygon()->index()]);
  }
}

const QVector<Vertex>& Mesh::vertices() const { return m_vertices; }

const QVector<Face>& Mesh::faces() const { return m_faces; }

const QVector<HalfEdge>& Mesh::halfEdges() const { return m_halfEdges; }

QVector<Vertex>& Mesh::vertices() { return m_vertices; }

QVector<Face>& Mesh::faces() { return m_faces; }

QVector<HalfEdge>& Mesh::halfEdges() { return m_halfEdges; }

bool Mesh::checkMesh() const {
  //  for (auto& edge : m_halfEdges) {
  //    assert(edge.next()->prev() == &edge);
  //    assert(edge.prev()->next() == &edge);
  //    assert(edge.twin()->twin() == &edge);
  //  }
  return true;
}

std::unique_ptr<Mesh> Mesh::ccSubdiv() const {
  Mesh* ccMesh = new Mesh;
  Subdiv::subdivideCatmullClark(static_cast<Mesh const*>(this), ccMesh);
  return std::move(std::unique_ptr<Mesh>(ccMesh));
}

std::unique_ptr<Mesh> Mesh::ternarySubdiv() const {
  Mesh* ternaryMesh = new Mesh;
  Subdiv::subdivideTernary(static_cast<Mesh const*>(this), ternaryMesh);
  return std::move(std::unique_ptr<Mesh>(ternaryMesh));
}

Mesh::~Mesh() {
  m_vertices.clear();
  m_vertices.squeeze();
  m_halfEdges.clear();
  m_halfEdges.squeeze();
  m_faces.clear();
  m_faces.squeeze();
  PotentialTwins.clear();
  PotentialTwins.squeeze();
}

void Mesh::setTwins(size_t numHalfEdges, size_t indexH) {
  size_t       m, n;
  size_t       hTail, hHead, len;
  QSet<size_t> Twinless;

  // Assign Twins
  for (m = 0; m < numHalfEdges; m++) {
    if (m_halfEdges[m].twin() == nullptr) {
      hTail = m_halfEdges[m].prev()->target()->index();
      hHead = m_halfEdges[m].target()->index();
      len   = m_halfEdges[m].target()->val();
      for (n = 0; n < len; n++) {
        if (m_halfEdges[PotentialTwins[hHead][n]].target()->index() == hTail) {
          m_halfEdges[m].setTwin(&m_halfEdges[PotentialTwins[hHead][n]]);
          m_halfEdges[PotentialTwins[hHead][n]].setTwin(&m_halfEdges[m]);
          break;
        }
      }
      if (n == len) {
        // Twin not found...
        Twinless.insert(m);
      }
    }
  }

  if (Twinless.size() > 0) {
    HalfEdge* initialEdge;
    HalfEdge* currentEdge;
    size_t    startBoundaryLoop;

    while (Twinless.size() > 0) {
      // Select a HalfEdge without Twin. The Twin that we will create is part of
      // a boundary edge loop
      qDebug() << " → Processing new Boundary Edge Loop";

      initialEdge = &m_halfEdges[*Twinless.begin()];
      Twinless.remove(initialEdge->index());

      // Target, Next, Prev, Twin, Poly, Index
      m_halfEdges.append(HalfEdge(initialEdge->prev()->target(),
                                  QVector3D(1., 0.0, 0.0),
                                  nullptr,
                                  nullptr,
                                  initialEdge,
                                  nullptr,
                                  indexH));
      startBoundaryLoop = indexH;
      // Twin of initialEdge should be assigned AFTER the central while loop!
      indexH++;

      // Use a sketch to properly understand these steps (assume
      // counter-clockwise HalfEdges) :)
      currentEdge = initialEdge->prev();
      while (currentEdge->twin() != nullptr) {
        currentEdge = currentEdge->twin()->prev();
      }

      // Trace the current boundary loop
      while (currentEdge != initialEdge) {
        Twinless.remove(currentEdge->index());

        // Target, Next, Prev, Twin, Poly, Index
        m_halfEdges.append(HalfEdge(currentEdge->prev()->target(),
                                    QVector3D(1., 0.0, 0.0),
                                    nullptr,
                                    &m_halfEdges[indexH - 1],
                                    currentEdge,
                                    nullptr,
                                    indexH));
        m_halfEdges[indexH - 1].setNext(&m_halfEdges[indexH]);

        currentEdge->target()->setVal(currentEdge->target()->val() + 1);
        currentEdge->setTwin(&m_halfEdges[indexH]);
        indexH++;

        currentEdge = currentEdge->prev();
        while (currentEdge->twin() != nullptr) {
          currentEdge = currentEdge->twin()->prev();
        }
      }

      m_halfEdges[startBoundaryLoop].setPrev(&m_halfEdges[indexH - 1]);
      m_halfEdges[indexH - 1].setNext(&m_halfEdges[startBoundaryLoop]);

      initialEdge->target()->setVal(initialEdge->target()->val() + 1);
      // Set Twin of initialEdge!
      initialEdge->setTwin(&m_halfEdges[startBoundaryLoop]);
    }
  }
}
