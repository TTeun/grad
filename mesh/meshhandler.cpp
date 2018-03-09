#include "meshhandler.h"

MeshHandler::MeshHandler(QObject* parent)
    : QObject(parent),
      m_meshRenderable(new MeshRenderable()),
      m_skeletonRenderable(new MeshRenderable()),
      m_selectionRenderable(new SelectionRenderable()) {
  qDebug() << "MeshHandler constructed";
}

void MeshHandler::init(const QString& path) {
  m_meshVector.clear();
  {
    OBJFile const* const objFile = new OBJFile(path);
    m_meshVector.push_back(std::unique_ptr<Mesh>(new Mesh(objFile)));
    delete objFile;
  }

  m_meshRenderable->init();
  m_skeletonRenderable->init();
  m_selectionRenderable->init();
  m_skeletonRenderable->setRenderMode(static_cast<int>(Renderable::RenderMode::LINES));
  buildMeshes();
}

void MeshHandler::buildMeshes() {
  m_meshVector.resize(1);
  for (size_t i = 0; i != m_ccSteps; ++i) {
    m_meshVector.push_back(m_meshVector.back()->ccSubdiv());
  }
  m_skeletonRenderable->setMesh(m_meshVector[0].get());
  m_skeletonRenderable->setCoordsNeedToBeFilled(true);
  m_meshRenderable->setMesh(m_meshVector.back().get());
  m_meshRenderable->setCoordsNeedToBeFilled(true);
}

void MeshHandler::render() const {
  m_meshRenderable->render();
  m_skeletonRenderable->render();
  m_selectionRenderable->render();
}

size_t MeshHandler::ccSteps() const { return m_ccSteps; }

void MeshHandler::setCcSteps(const size_t& ccSteps) { m_ccSteps = ccSteps; }

void MeshHandler::currentMeshIndexChanged(int newIndex) {
  m_currentMeshIndex = newIndex;
  if (m_currentMeshIndex > ccSteps()) {
    m_ccSteps = m_currentMeshIndex;
    buildMeshes();
  }

  m_skeletonRenderable->setMesh(m_meshVector[m_currentMeshIndex].get());
  m_skeletonRenderable->setCoordsNeedToBeFilled(true);
  emit hasChanged();
}

void MeshHandler::findClosest(QVector2D& mousePosition) {
  double    minDistance  = 10000.;
  HalfEdge* selectedEdge = nullptr;
  for (auto edge : m_meshVector[m_currentMeshIndex]->halfEdges()) {
    if (edge.distanceToPoint(mousePosition) < minDistance) {
      minDistance  = edge.distanceToPoint(mousePosition) < minDistance;
      selectedEdge = &edge;
    }
  }
  //  selectedEdge = &m_meshVector[m_currentMeshIndex]->halfEdges().front();
  m_selectionRenderable->fillCoords(selectedEdge);
  qDebug() << selectedEdge->index();

  emit hasChanged();
}
