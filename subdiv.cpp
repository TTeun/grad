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
  const auto& oldFaces = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();
  const auto& oldVertices = oldMesh->vertices();

  const size_t numOldVertices = oldVertices.size();
  auto& newVertices = newMesh->vertices();

  for (auto const& vert : oldVertices) {
    newVertices.append(
        Vertex(vert.coords(), nullptr, vert.val(), vert.index()));
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
  const auto& oldFaces = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();
  const auto& oldVertices = oldMesh->vertices();

  const size_t numOldHalfEdges = oldHalfEdges.size();
  const size_t numOldVertices = oldVertices.size();

  auto& newHalfEdges = newMesh->halfEdges();
  auto& newVertices = newMesh->vertices();

  size_t numNewEdges = 2 * numOldHalfEdges;
  for (auto face : oldFaces)
    numNewEdges += 2 * face.val();
  newHalfEdges.resize(numNewEdges);

  size_t edgeIndex = 0;
  for (auto const& edge : oldHalfEdges) {
    newHalfEdges[edgeIndex] = HalfEdge(
        &newVertices[numOldVertices + edge.index()], edge.centerColour(),
        nullptr, &newHalfEdges[edge.prev()->index() + numOldHalfEdges],
        &newHalfEdges[edge.twin()->index() + numOldHalfEdges], nullptr,
        edge.index());

    newHalfEdges[edgeIndex + numOldHalfEdges] =
        HalfEdge(&newVertices[edge.target()->index()], edge.endColour(),
                 &newHalfEdges[edge.next()->index()], nullptr,
                 &newHalfEdges[edge.twin()->index()], nullptr,
                 edge.index() + numOldHalfEdges);

    if (not edge.polygon()) {
      setNext(&newHalfEdges[edgeIndex],
              &newHalfEdges[edgeIndex + numOldHalfEdges]);
    }

    newVertices[newHalfEdges[edgeIndex].target()->index()].setOut(
        newHalfEdges[edgeIndex].twin());
    newVertices[newHalfEdges[edgeIndex + numOldHalfEdges].target()->index()]
        .setOut(newHalfEdges[edgeIndex + numOldHalfEdges].twin());

    assert(edgeIndex == edge.index());
    ++edgeIndex;
  }

  edgeIndex = 2 * numOldHalfEdges;
  for (auto const& face : oldFaces) {
    auto currentEdge = face.side();
    for (size_t i = 0; i != face.val(); ++i) {
      newHalfEdges[edgeIndex] = HalfEdge(
          &newVertices[numOldVertices + numOldHalfEdges + face.index()],
          face.centerColour(), nullptr, nullptr, &newHalfEdges[edgeIndex + 1],
          nullptr, edgeIndex);

      newHalfEdges[edgeIndex + 1] =
          HalfEdge(&newVertices[numOldVertices + currentEdge->index()],
                   currentEdge->centerColour(), nullptr, nullptr,
                   &newHalfEdges[edgeIndex], nullptr, edgeIndex + 1);

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
  const auto& oldFaces = oldMesh->faces();
  const auto& oldHalfEdges = oldMesh->halfEdges();

  const size_t numOldHalfEdges = oldHalfEdges.size();

  auto& newFaces = newMesh->faces();
  auto& newHalfEdges = newMesh->halfEdges();

  size_t numNewFaces = 0;

  for (auto face : oldFaces)
    numNewFaces += face.val();
  newFaces.resize(numNewFaces);

  size_t faceIndex = 0;
  size_t edgeIndex = 2 * numOldHalfEdges;
  for (auto const& face : oldFaces) {
    size_t faceIndexOffSet = 0;
    auto currentEdge = face.side();
    for (size_t i = 0; i != face.val(); ++i) {
      newFaces[faceIndex + faceIndexOffSet] = Face(
          &newHalfEdges[currentEdge->index()], 4, faceIndex + faceIndexOffSet);

      newHalfEdges[edgeIndex].setPolygon(
          &newFaces[faceIndex + faceIndexOffSet]);
      newHalfEdges[edgeIndex + 1].setPolygon(
          &newFaces[faceIndex + (faceIndexOffSet + 1) % face.val()]);

      if (faceIndexOffSet == 0)
        setNext(&newHalfEdges[edgeIndex],
                &newHalfEdges[edgeIndex + 2 * face.val() - 1]);
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
