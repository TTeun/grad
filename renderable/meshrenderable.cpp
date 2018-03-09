#include "meshrenderable.h"

MeshRenderable::MeshRenderable() : Renderable() {}

MeshRenderable::~MeshRenderable() {}

void MeshRenderable::fillCoords() {
  m_data->clear();
  m_indices->clear();
  unsigned int index = 0;
  auto const faces = m_mesh->faces();
  HalfEdge* currentEdge;
  for (auto face : faces) {
    currentEdge = face.side();

    for (size_t i = 0; i != face.val(); ++i) {
      m_data->push_back(currentEdge->target()->coords()[0]);
      m_data->push_back(currentEdge->target()->coords()[1]);
      m_data->push_back(currentEdge->colour()[0]);
      m_data->push_back(currentEdge->colour()[1]);
      m_data->push_back(currentEdge->colour()[2]);
      m_indices->push_back(index);
      ++index;
      currentEdge = currentEdge->next();
    }
    m_indices->push_back(std::numeric_limits<unsigned int>::max());
  }

  m_coordsNeedToBeFilled = false;
}

void MeshRenderable::setMesh(Mesh* mesh) {
  m_mesh = mesh;
}
