#include "meshhandler.h"

#include <QColorDialog>
#include <cassert>

MeshHandler::MeshHandler(ViewState* const viewState, QObject* parent)
    : QObject(parent),
      m_viewState(viewState),
      m_meshRenderable(new MeshRenderable()),
      m_skeletonRenderable(new MeshRenderable()),
      m_selectionRenderable(new SelectionRenderable()) {
  assert(QObject::connect(m_viewState,
                          &ViewState::refinementLevelChanged,
                          this,
                          &MeshHandler::refinementLevelChanged));

  assert(QObject::connect(
      m_viewState, &ViewState::updated, this, &MeshHandler::updateScene));

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
  m_meshRenderable->setRenderMode(static_cast<int>(Renderable::RenderMode::SURFACE));
  m_skeletonRenderable->init();
  m_selectionRenderable->init();
  m_skeletonRenderable->setRenderMode(static_cast<int>(Renderable::RenderMode::LINES));
  buildMeshes();
}

void MeshHandler::buildMeshes(size_t startIndex) {
  m_meshVector.resize(1);

  m_meshVector.push_back(m_meshVector.back()->ternarySubdiv());
  for (size_t i = 0; i < m_viewState->ccSteps(); ++i) {
    m_meshVector.push_back(m_meshVector.back()->ccSubdiv());
  }

  m_skeletonRenderable->setMesh(m_meshVector[m_viewState->refinementLevel()].get());
  m_skeletonRenderable->setCoordsNeedToBeFilled(true);
  m_meshRenderable->setMesh(m_meshVector.back().get());
  m_meshRenderable->setCoordsNeedToBeFilled(true);
}

void MeshHandler::render() const {
  if (m_viewState->showSurface()) m_meshRenderable->render();
  if (m_viewState->showLines()) m_skeletonRenderable->render();

  m_selectionRenderable->render();
}

void MeshHandler::updateScene() { emit hasChanged(); }

void MeshHandler::refinementLevelChanged() {
  buildMeshes();

  m_skeletonRenderable->setMesh(m_meshVector[m_viewState->refinementLevel()].get());
  m_skeletonRenderable->setCoordsNeedToBeFilled(true);
  emit hasChanged();
}

void MeshHandler::setColour(const QVector2D& mousePosition) {
  HalfEdge* edge;
  if ((edge = findClosest(mousePosition)) == nullptr) return;

  QColor color = QColorDialog::getColor(Qt::white);
  if (m_colourSetMode == COLOUR_SET_MODE::BY_EDGE)
    edge->setColour(QVector3D(color.redF(), color.greenF(), color.blueF()));
  else if (m_colourSetMode == COLOUR_SET_MODE::BY_VERTEX) {
    for (size_t i = 0; i != edge->target()->val(); ++i) {
      edge->setColour(QVector3D(color.redF(), color.greenF(), color.blueF()));
      edge = edge->next()->twin();
    }
  }

  buildMeshes(m_viewState->refinementLevel() + 1);
  emit hasChanged();
}

HalfEdge* MeshHandler::findClosest(QVector2D const& mousePosition) {
  Face* selectedFace = nullptr;
  for (auto face : m_meshVector[m_viewState->refinementLevel()]->faces()) {
    if (face.containsPoint(mousePosition)) {
      selectedFace = &face;
      break;
    }
  }
  if (not selectedFace) {
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
  //  if (edgeIndex != std::numeric_limits<size_t>::max())
  //    m_selectionRenderable->fillCoords(
  //        &m_meshVector[m_viewState->refinementLevel()]->halfEdges().at(edgeIndex));

  return &m_meshVector[m_viewState->refinementLevel()]->halfEdges()[edgeIndex];
}

ViewState* MeshHandler::viewState() const { return m_viewState; }
