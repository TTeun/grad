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

  auto& newVertices = newMesh->vertices();

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

  //  assert(newMesh->checkMesh());
}

void Subdiv::subdivideTernary(const Mesh* oldMesh, Mesh* newMesh) {
  auto const& oldFaces     = oldMesh->faces();
  auto const& oldHalfEdges = oldMesh->halfEdges();
  auto const& oldVertices  = oldMesh->vertices();

  size_t const numOldFaces     = oldFaces.size();
  size_t const numOldHalfEdges = oldHalfEdges.size();
  size_t const numOldVertices  = oldVertices.size();

  auto& newFaces     = newMesh->faces();
  auto& newHalfEdges = newMesh->halfEdges();
  auto& newVertices  = newMesh->vertices();

  size_t sumFaceVal  = 0;
  size_t numNewFaces = 0;

  for (size_t k = 0; k < numOldFaces; ++k) {
    sumFaceVal += oldFaces[k].val();
    numNewFaces += 1 + 2 * oldFaces[k].val();
  }

  size_t newnumOldVertices = numOldVertices + numOldHalfEdges + sumFaceVal;
  newVertices.clear();
  newVertices.reserve(newnumOldVertices);

  for (size_t k = 0; k < numOldVertices; ++k)
    newVertices.push_back(oldVertices[k]);  // out pointer is nonsense at this point

  HalfEdge* twinEdge;
  for (HalfEdge const& edge : oldHalfEdges) {
    twinEdge = edge.twin();
    newVertices.push_back(Vertex(twinEdge->target()->coords() + edge.colGrad(),
                                 nullptr,
                                 4,
                                 numOldVertices + edge.index(),
                                 0));
  }

  float     d1, d2, N1, N2, V01, V02;  // See Kosinka et. al. paper on grad mesh for these
  QVector2D V0, V1, V2, D1, D2, C;     // V1 ---- V0 ---- V2
  HalfEdge const* currentEdge;
  QVector2D       newPosition;
  size_t          vertIndex = numOldVertices + numOldHalfEdges;

  for (Face const& face : oldFaces) {
    currentEdge = face.side();
    C           = face.center();
    for (size_t l = 0; l < face.val(); ++l) {
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

      newVertices.push_back(Vertex(newPosition, nullptr, 4, vertIndex, 0));
      currentEdge = currentEdge->next();
      ++vertIndex;
    }
  }

  size_t newnumOldHalfEdges = 3 * numOldHalfEdges + 6 * sumFaceVal;
  newHalfEdges.clear();
  newHalfEdges.reserve(newnumOldHalfEdges);

  for (size_t k = 0; k < numOldHalfEdges; ++k) {
    currentEdge = &oldHalfEdges[k];
    newHalfEdges.push_back(HalfEdge(&newVertices[numOldVertices + currentEdge->index()],
                                    currentEdge->prev()->colour(),
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    nullptr,
                                    3 * k));
    newHalfEdges.push_back(
        HalfEdge(&newVertices[numOldVertices + currentEdge->twin()->index()],
                 currentEdge->colour(),
                 nullptr,
                 nullptr,
                 nullptr,
                 nullptr,
                 3 * k + 1));
    newHalfEdges.push_back(HalfEdge(&newVertices[currentEdge->target()->index()],
                                    currentEdge->colour(),
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
  for (size_t k = 0; k < numOldHalfEdges; ++k) {
    currentEdge = &oldHalfEdges[k];
    twinIndex   = currentEdge->twin()->index();
    newHalfEdges[3 * k].setTwin(&newHalfEdges[3 * twinIndex + 2]);
    newHalfEdges[3 * k + 1].setTwin(&newHalfEdges[3 * twinIndex + 1]);
    newHalfEdges[3 * k + 2].setTwin(&newHalfEdges[3 * twinIndex]);

    newHalfEdges[3 * k].setPrev(&newHalfEdges[3 * currentEdge->prev()->index() + 2]);
    newHalfEdges[3 * k + 2].setNext(&newHalfEdges[3 * currentEdge->next()->index()]);

    if (not currentEdge->polygon()) {
      newHalfEdges[3 * k].setNext(&newHalfEdges[3 * k + 1]);
      newHalfEdges[3 * k + 1].setNext(&newHalfEdges[3 * k + 2]);
    }

    twinEdge = currentEdge->twin();
    if (twinEdge->index() < currentEdge->index()) {
      newVertices[twinEdge->target()->index()].setOut(&newHalfEdges[3 * k]);
      newVertices[numOldVertices + currentEdge->index()].setOut(&newHalfEdges[3 * k + 1]);
      newVertices[numOldVertices + twinIndex].setOut(&newHalfEdges[3 * k + 2]);
    }
  }

  size_t n;
  size_t edgeIndex = 3 * numOldHalfEdges;
  vertIndex        = numOldVertices + numOldHalfEdges;
  size_t currentIndex;
  size_t nextIndex;
  size_t edgesAdded;
  size_t faceIndex = 0;

  newFaces.clear();
  newFaces.reserve(numNewFaces);

  for (Face const& face : oldFaces) {
    n           = face.val();
    currentEdge = face.side();

    // Now begins the bookkeeping nightmare of the ternary subdivision step that creates
    // 2n + 1 new faces per old face
    for (size_t l = 0; l < n; ++l) {
      currentIndex = currentEdge->index();
      nextIndex    = currentEdge->next()->index();
      twinIndex    = currentEdge->twin()->index();

      newHalfEdges.push_back(HalfEdge(&newVertices[vertIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex));
      newHalfEdges.push_back(HalfEdge(&newVertices[vertIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex + 1));
      newHalfEdges.push_back(HalfEdge(&newVertices[numOldVertices + nextIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex + 2));
      newHalfEdges.push_back(HalfEdge(&newVertices[vertIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex + 3));
      newHalfEdges.push_back(HalfEdge(&newVertices[numOldVertices + twinIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex + 4));
      newHalfEdges.push_back(HalfEdge(&newVertices[vertIndex],
                                      currentEdge->colour(),
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      edgeIndex + 5));

      newHalfEdges[edgeIndex + 1].setNext(&newHalfEdges[edgeIndex + 2]);
      newHalfEdges[3 * nextIndex].setNext(&newHalfEdges[edgeIndex + 3]);
      newHalfEdges[edgeIndex + 3].setNext(&newHalfEdges[edgeIndex + 4]);
      newHalfEdges[3 * currentIndex + 1].setNext(&newHalfEdges[edgeIndex + 5]);
      newHalfEdges[edgeIndex + 2].setNext(&newHalfEdges[3 * nextIndex + 1]);
      newHalfEdges[edgeIndex + 4].setNext(&newHalfEdges[3 * currentIndex + 2]);

      newHalfEdges[edgeIndex + 2].setTwin(&newHalfEdges[edgeIndex + 3]);
      newHalfEdges[edgeIndex + 4].setTwin(&newHalfEdges[edgeIndex + 5]);

      newVertices[vertIndex].setOut(&newHalfEdges[edgeIndex + 2]);

      newFaces.push_back(Face(&newHalfEdges[edgeIndex + 1], 4, faceIndex));
      newFaces.push_back(Face(&newHalfEdges[edgeIndex + 4], 4, faceIndex + 1));

      faceIndex += 2;
      ++vertIndex;
      edgeIndex += 6;
      currentEdge = currentEdge->next();
    }

    edgesAdded = 6 * n;
    edgeIndex -= edgesAdded;

    newFaces.push_back(Face(&newHalfEdges[edgeIndex], n, faceIndex));
    ++faceIndex;

    newHalfEdges[edgeIndex].setTwin(&newHalfEdges[edgeIndex + 6 * (n - 1) + 1]);
    newHalfEdges[edgeIndex + 5].setNext(&newHalfEdges[edgeIndex + 6 * (n - 1) + 1]);
    newHalfEdges[edgeIndex + 6 * (n - 1)].setNext(&newHalfEdges[edgeIndex]);

    for (size_t l = 0; l < n - 1; ++l) {
      newHalfEdges[edgeIndex + 1].setTwin(&newHalfEdges[edgeIndex + 6]);
      newHalfEdges[edgeIndex + 11].setNext(&newHalfEdges[edgeIndex + 1]);
      newHalfEdges[edgeIndex].setNext(&newHalfEdges[edgeIndex + 6]);
      edgeIndex += 6;
    }
    edgeIndex += 6;
  }

  HalfEdge* mutableEdge;
  for (size_t i = 0; i < numNewFaces; ++i) {
    n           = newFaces[i].val();
    mutableEdge = newFaces[i].side();
    for (size_t j = 0; j < n; ++j) {
      mutableEdge->setPolygon(&newFaces[i]);
      mutableEdge = mutableEdge->next();
    }
  }
  //  assert(newMesh->checkMesh());
}
