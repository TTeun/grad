#include "subdiv.h"
#include <cassert>

void setNext(HalfEdge* edge, HalfEdge* next) {
  edge->setNext(next);
  next->setPrev(edge);
}

void setTwin(HalfEdge* twin1, HalfEdge* twin2) {
  twin1->setTwin(twin2);
  twin2->setTwin(twin1);
}

void buildNewVertices(Mesh const* oldMesh, Mesh* newMesh) {
  const auto& oldFaces     = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();
  const auto& oldVertices  = oldMesh->vertices();

  const size_t numOldVertices = oldVertices.size();
  auto&        newVertices    = newMesh->vertices();

  for (auto const& vert : oldVertices) {
    newVertices.append(Vertex(vert.coords(), nullptr, vert.val(), vert.index()));
  }

  size_t vertIndex = numOldVertices;
  for (auto const& edge : oldHalfEdges) {
    newVertices.append(Vertex(edge.center(), nullptr, 0., vertIndex));
    ++vertIndex;
  }

  for (auto const& face : oldFaces) {
    newVertices.append(Vertex(face.center(), nullptr, 0., vertIndex));
    ++vertIndex;
  }
}

void buildNewHalfEdges(Mesh const* oldMesh, Mesh* newMesh) {
  const auto& oldFaces     = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();
  const auto& oldVertices  = oldMesh->vertices();

  const size_t numOldHalfEdges = oldHalfEdges.size();
  const size_t numOldVertices  = oldVertices.size();

  auto& newHalfEdges = newMesh->halfEdges();
  auto& newVertices  = newMesh->vertices();

  size_t numNewEdges = 2 * numOldHalfEdges;
  for (auto face : oldFaces) numNewEdges += 2 * face.val();
  newHalfEdges.resize(numNewEdges);

  size_t edgeIndex = 0;
  for (auto const& edge : oldHalfEdges) {
    newHalfEdges[edgeIndex] =
        HalfEdge(&newVertices[numOldVertices + edge.index()],
                 edge.centerColour(),
                 nullptr,
                 &newHalfEdges[edge.prev()->index() + numOldHalfEdges],
                 &newHalfEdges[edge.twin()->index() + numOldHalfEdges],
                 nullptr,
                 edge.index());

    newHalfEdges[edgeIndex + numOldHalfEdges] =
        HalfEdge(&newVertices[edge.target()->index()],
                 edge.endColour(),
                 &newHalfEdges[edge.next()->index()],
                 nullptr,
                 &newHalfEdges[edge.twin()->index()],
                 nullptr,
                 edge.index() + numOldHalfEdges);

    if (not edge.polygon()) {
      setNext(&newHalfEdges[edgeIndex], &newHalfEdges[edgeIndex + numOldHalfEdges]);
    }

    newVertices[newHalfEdges[edgeIndex].target()->index()].setOut(
        newHalfEdges[edgeIndex].twin());
    newVertices[newHalfEdges[edgeIndex + numOldHalfEdges].target()->index()].setOut(
        newHalfEdges[edgeIndex + numOldHalfEdges].twin());

    assert(edgeIndex == edge.index());
    ++edgeIndex;
  }

  edgeIndex = 2 * numOldHalfEdges;
  for (auto const& face : oldFaces) {
    auto currentEdge = face.side();
    for (size_t i = 0; i != face.val(); ++i) {
      newHalfEdges[edgeIndex] =
          HalfEdge(&newVertices[numOldVertices + numOldHalfEdges + face.index()],
                   face.centerColour(),
                   nullptr,
                   nullptr,
                   &newHalfEdges[edgeIndex + 1],
                   nullptr,
                   edgeIndex);

      newHalfEdges[edgeIndex + 1] =
          HalfEdge(&newVertices[numOldVertices + currentEdge->index()],
                   currentEdge->centerColour(),
                   nullptr,
                   nullptr,
                   &newHalfEdges[edgeIndex],
                   nullptr,
                   edgeIndex + 1);

      newVertices[newHalfEdges[edgeIndex].target()->index()].setOut(
          newHalfEdges[edgeIndex].twin());
      newVertices[newHalfEdges[edgeIndex + 1].target()->index()].setOut(
          newHalfEdges[edgeIndex + 1].twin());

      edgeIndex += 2;
      currentEdge = currentEdge->next();
    }
  }
}

void buildNewFaces(Mesh const* oldMesh, Mesh* newMesh) {
  const auto& oldFaces     = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();

  const size_t numOldHalfEdges = oldHalfEdges.size();

  auto& newFaces     = newMesh->faces();
  auto& newHalfEdges = newMesh->halfEdges();

  size_t numNewFaces = 0;

  for (auto face : oldFaces) numNewFaces += face.val();
  newFaces.resize(numNewFaces);

  size_t faceIndex = 0;
  size_t edgeIndex = 2 * numOldHalfEdges;
  for (auto const& face : oldFaces) {
    size_t faceIndexOffSet = 0;
    auto   currentEdge     = face.side();
    for (size_t i = 0; i != face.val(); ++i) {
      newFaces[faceIndex + faceIndexOffSet] =
          Face(&newHalfEdges[currentEdge->index()], 4, faceIndex + faceIndexOffSet);

      newHalfEdges[edgeIndex].setPolygon(&newFaces[faceIndex + faceIndexOffSet]);
      newHalfEdges[edgeIndex + 1].setPolygon(
          &newFaces[faceIndex + (faceIndexOffSet + 1) % face.val()]);

      if (faceIndexOffSet == 0)
        setNext(&newHalfEdges[edgeIndex], &newHalfEdges[edgeIndex + 2 * face.val() - 1]);
      else
        setNext(&newHalfEdges[edgeIndex], &newHalfEdges[edgeIndex - 1]);

      setNext(&newHalfEdges[edgeIndex + 1],
              &newHalfEdges[currentEdge->index() + numOldHalfEdges]);

      setNext(&newHalfEdges[currentEdge->index()], &newHalfEdges[edgeIndex]);

      currentEdge = currentEdge->next();
      ++faceIndexOffSet;
      edgeIndex += 2;
    }
    faceIndex += face.val();
  }
}

void Subdiv::subdivideCatmullClark(Mesh const* oldMesh, Mesh* newMesh) {
  buildNewVertices(oldMesh, newMesh);
  buildNewHalfEdges(oldMesh, newMesh);
  buildNewFaces(oldMesh, newMesh);

  assert(newMesh->checkMesh());
}

//#include "../auxilliary/geom.h"
//#include "../auxilliary/utility.h"
//#include "../mesh/mesh.h"
//#include "subdiv.h"

void Subdiv::subdivideTernary(const Mesh* oldMesh, Mesh* newMesh) {
  size_t k, l;
  size_t vertCount = oldMesh->vertices().size();
  size_t edgeCount = oldMesh->halfEdges().size();
  size_t faceCount = oldMesh->faces().size();

  size_t sumFaceVal   = 0;
  size_t newFaceCount = 0;

  for (k = 0; k < faceCount; ++k) {
    sumFaceVal += oldMesh->faces()[k].val();
    newFaceCount += 1 + 2 * oldMesh->faces()[k].val();
  }

  size_t newVertCount = vertCount + edgeCount + sumFaceVal;
  newMesh->vertices().clear();
  newMesh->vertices().reserve(newVertCount);

  for (k = 0; k < vertCount; ++k)
    newMesh->vertices().push_back(
        oldMesh->vertices()[k]);  // out pointer is nonsense at this point

  HalfEdge* twinEdge;
  for (HalfEdge const& edge : oldMesh->halfEdges()) {
    twinEdge = edge.twin();
    newMesh->vertices().push_back(Vertex(twinEdge->target()->coords() + edge.colGrad(),
                                         nullptr,
                                         4,
                                         vertCount + edge.index(),
                                         0));
  }

  float     d1, d2, N1, N2, V01, V02;  // See Kosinka et. al. paper on grad mesh for these
  QVector2D V0, V1, V2, D1, D2, C;     // V1 ---- V0 ---- V2
  HalfEdge const* currentEdge;
  QVector2D       newPosition;
  size_t          vertIndex = vertCount + edgeCount;

  for (Face const& face : oldMesh->faces()) {
    currentEdge = face.side();
    C           = face.center();
    for (l = 0; l < face.val(); ++l) {
      V1 = currentEdge->prev()->target()->coords();
      V0 = currentEdge->target()->coords();
      V2 = currentEdge->next()->target()->coords();
      D1 = currentEdge->twin()->colGrad();
      D2 = currentEdge->next()->colGrad();
      N1 = D1.length();
      N2 = D2.length();
      if (N1 == 0.0 || N2 == 0.0) qDebug() << "Zero length colour gradient";

      V01 = QVector2D(V0 - V1).length();
      V02 = QVector2D(V0 - V2).length();

      d1 = 2 * N1 / V01;
      d2 = 2 * N2 / V02;

      newPosition = (1 - d1) * (1 - d2) * V0 + d1 * d2 * C +
                    d1 * (1 - d2) * (V0 + V01 * D1 / (2 * N1)) +
                    d2 * (1 - d1) * (V0 + V02 * D2 / (2 * N2));

      newMesh->vertices().push_back(Vertex(newPosition, nullptr, 4, vertIndex, 0));
      currentEdge = currentEdge->next();
      ++vertIndex;
    }
  }

  size_t newEdgeCount = 3 * edgeCount + 6 * sumFaceVal;
  newMesh->halfEdges().clear();
  newMesh->halfEdges().reserve(newEdgeCount);

  for (k = 0; k < edgeCount; ++k) {
    currentEdge = &oldMesh->halfEdges()[k];
    newMesh->halfEdges().push_back(
        HalfEdge(&newMesh->vertices()[vertCount + currentEdge->index()],
                 QVector3D(0., 0., 0.),
                 nullptr,
                 nullptr,
                 nullptr,
                 nullptr,
                 3 * k));
    newMesh->halfEdges().push_back(
        HalfEdge(&newMesh->vertices()[vertCount + currentEdge->twin()->index()],
                 QVector3D(0., 0., 0.),
                 nullptr,
                 nullptr,
                 nullptr,
                 nullptr,
                 3 * k + 1));
    newMesh->halfEdges().push_back(
        HalfEdge(&newMesh->vertices()[currentEdge->target()->index()],
                 QVector3D(0., 0., 0.),
                 nullptr,
                 nullptr,
                 nullptr,
                 nullptr,
                 3 * k + 2));

    // /|    /|
    //  |     |  2
    //  |     |
    //  |    /|
    //  | ->  |  1
    //  |     |
    //  |    /|
    //  |     |  0
    //  |     |
  }

  size_t twinIndex;
  for (k = 0; k < edgeCount; ++k) {
    currentEdge = &oldMesh->halfEdges()[k];
    twinIndex   = currentEdge->twin()->index();
    newMesh->halfEdges()[3 * k].setTwin(&newMesh->halfEdges()[3 * twinIndex + 2]);
    newMesh->halfEdges()[3 * k + 1].setTwin(&newMesh->halfEdges()[3 * twinIndex + 1]);
    newMesh->halfEdges()[3 * k + 2].setTwin(&newMesh->halfEdges()[3 * twinIndex]);

    newMesh->halfEdges()[3 * k].setPrev(
        &newMesh->halfEdges()[3 * currentEdge->prev()->index() + 2]);
    newMesh->halfEdges()[3 * k + 2].setNext(
        &newMesh->halfEdges()[3 * currentEdge->next()->index()]);

    if (not currentEdge->polygon()) {
      newMesh->halfEdges()[3 * k].setNext(&newMesh->halfEdges()[3 * k + 1]);
      newMesh->halfEdges()[3 * k + 1].setNext(&newMesh->halfEdges()[3 * k + 2]);
    }

    twinEdge = currentEdge->twin();
    if (twinEdge->index() < currentEdge->index()) {
      newMesh->vertices()[twinEdge->target()->index()].setOut(
          &newMesh->halfEdges()[3 * k]);
      newMesh->vertices()[vertCount + currentEdge->index()].setOut(
          &newMesh->halfEdges()[3 * k + 1]);
      newMesh->vertices()[vertCount + twinIndex].setOut(&newMesh->halfEdges()[3 * k + 2]);
    }
  }

  size_t n;
  size_t edgeIndex = 3 * edgeCount;
  vertIndex        = vertCount + edgeCount;
  size_t currentIndex;
  size_t nextIndex;
  size_t edgesAdded;
  size_t faceIndex = 0;

  newMesh->faces().clear();
  ;
  newMesh->faces().reserve(newFaceCount);

  for (Face const& face : oldMesh->faces()) {
    n           = face.val();
    currentEdge = face.side();

    // Now begins the bookkeeping nightmare of the ternary subdivision step that creates
    // 2n + 1 new faces per old face
    for (l = 0; l < n; ++l) {
      currentIndex = currentEdge->index();
      nextIndex    = currentEdge->next()->index();
      twinIndex    = currentEdge->twin()->index();

      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex));
      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex + 1));
      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertCount + nextIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex + 2));
      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex + 3));
      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertCount + twinIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex + 4));
      newMesh->halfEdges().push_back(HalfEdge(&newMesh->vertices()[vertIndex],
                                              QVector3D(0., 0., 0.),
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              nullptr,
                                              edgeIndex + 5));

      newMesh->halfEdges()[edgeIndex + 1].setNext(&newMesh->halfEdges()[edgeIndex + 2]);
      newMesh->halfEdges()[3 * nextIndex].setNext(&newMesh->halfEdges()[edgeIndex + 3]);
      newMesh->halfEdges()[edgeIndex + 3].setNext(&newMesh->halfEdges()[edgeIndex + 4]);
      newMesh->halfEdges()[3 * currentIndex + 1].setNext(
          &newMesh->halfEdges()[edgeIndex + 5]);
      newMesh->halfEdges()[edgeIndex + 2].setNext(
          &newMesh->halfEdges()[3 * nextIndex + 1]);
      newMesh->halfEdges()[edgeIndex + 4].setNext(
          &newMesh->halfEdges()[3 * currentIndex + 2]);

      newMesh->halfEdges()[edgeIndex + 2].setTwin(&newMesh->halfEdges()[edgeIndex + 3]);
      newMesh->halfEdges()[edgeIndex + 4].setTwin(&newMesh->halfEdges()[edgeIndex + 5]);

      newMesh->vertices()[vertIndex].setOut(&newMesh->halfEdges()[edgeIndex + 2]);

      newMesh->faces().push_back(
          Face(&newMesh->halfEdges()[edgeIndex + 1], 4, faceIndex));
      newMesh->faces().push_back(
          Face(&newMesh->halfEdges()[edgeIndex + 4], 4, faceIndex + 1));

      faceIndex += 2;
      ++vertIndex;
      edgeIndex += 6;
      currentEdge = currentEdge->next();
    }

    edgesAdded = 6 * n;
    edgeIndex -= edgesAdded;

    newMesh->faces().push_back(Face(&newMesh->halfEdges()[edgeIndex], n, faceIndex));
    ++faceIndex;

    newMesh->halfEdges()[edgeIndex].setTwin(
        &newMesh->halfEdges()[edgeIndex + 6 * (n - 1) + 1]);
    newMesh->halfEdges()[edgeIndex + 5].setNext(
        &newMesh->halfEdges()[edgeIndex + 6 * (n - 1) + 1]);
    newMesh->halfEdges()[edgeIndex + 6 * (n - 1)].setNext(
        &newMesh->halfEdges()[edgeIndex]);

    for (l = 0; l < n - 1; ++l) {
      newMesh->halfEdges()[edgeIndex + 1].setTwin(&newMesh->halfEdges()[edgeIndex + 6]);
      newMesh->halfEdges()[edgeIndex + 11].setNext(&newMesh->halfEdges()[edgeIndex + 1]);
      newMesh->halfEdges()[edgeIndex].setNext(&newMesh->halfEdges()[edgeIndex + 6]);
      edgeIndex += 6;
    }
    edgeIndex += 6;
  }

  HalfEdge* mutableEdge;
  for (size_t i = 0; i < newFaceCount; ++i) {
    n           = newMesh->faces()[i].val();
    mutableEdge = newMesh->faces()[i].side();
    for (size_t j = 0; j < n; ++j) {
      mutableEdge->setPolygon(&newMesh->faces()[i]);
      mutableEdge = mutableEdge->next();
    }
  }
}
