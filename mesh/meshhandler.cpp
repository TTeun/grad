#include "meshhandler.h"
#include <QColorDialog>

MeshHandler::MeshHandler(QObject* parent)
    : QObject(parent),
      m_meshRenderable(new MeshRenderable()),
      m_skeletonRenderable(new MeshRenderable()),
      m_selectionRenderable(new SelectionRenderable()) {
  m_ccSteps = 2;
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
  m_meshRenderable->setRenderMode(static_cast<int>(Renderable::RenderMode::SURFACE) |
                                  static_cast<int>(Renderable::RenderMode::POINTS));
  m_skeletonRenderable->init();
  m_selectionRenderable->init();
  m_skeletonRenderable->setRenderMode(static_cast<int>(Renderable::RenderMode::LINES));
  buildMeshes();
}

void MeshHandler::buildMeshes(size_t startIndex) {
  m_meshVector.resize(1);

  //  m_meshVector.push_back(m_meshVector.back()->ternarySubdiv());

  for (size_t i = startIndex; i < m_ccSteps; ++i) {
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

void MeshHandler::setColour(const QVector2D& mousePosition) {
  HalfEdge* edge;
  if ((edge = findClosest(mousePosition)) == nullptr) return;

  QColor color = QColorDialog::getColor(Qt::white);
  edge->setColour(QVector3D(color.redF(), color.greenF(), color.blueF()));

  buildMeshes(m_currentMeshIndex + 1);
  emit hasChanged();
}

HalfEdge* MeshHandler::findClosest(QVector2D const& mousePosition) {
  Face* selectedFace = nullptr;
  for (auto face : m_meshVector[m_currentMeshIndex]->faces()) {
    if (face.containsPoint(mousePosition)) {
      selectedFace = &face;
      break;
    }
  }
  if (not selectedFace) {
    qDebug() << "Outside polygon\n";
    return nullptr;
  }

  double minDistance = 10000.0;
  size_t edgeIndex   = std::numeric_limits<size_t>::max();
  auto*  currentEdge = selectedFace->side();
  for (size_t i = 0; i != selectedFace->val(); ++i) {
    if ((currentEdge->target()->coords() - mousePosition).lengthSquared() < minDistance) {
      minDistance = (currentEdge->target()->coords() - mousePosition).lengthSquared();
      edgeIndex   = currentEdge->index();
    }
    currentEdge = currentEdge->next();
  }
  if (edgeIndex != std::numeric_limits<size_t>::max())
    m_selectionRenderable->fillCoords(
        &m_meshVector[m_currentMeshIndex]->halfEdges().at(edgeIndex));

  return &m_meshVector[m_currentMeshIndex]->halfEdges()[edgeIndex];
}
