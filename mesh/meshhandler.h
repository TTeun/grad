#ifndef MESHHANDLER_H
#define MESHHANDLER_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

#include "../renderable/meshrenderable.h"
#include "../renderable/selectionrenderable.h"
#include "viewstate.h"

class MeshRenderable;
class SelectionRenderable;

class MeshHandler : public QObject {
  Q_OBJECT

  enum class COLOUR_SET_MODE { BY_VERTEX, BY_EDGE };

 private:
  COLOUR_SET_MODE                      m_colourSetMode = COLOUR_SET_MODE::BY_VERTEX;
  ViewState*                           m_viewState;
  std::vector<std::unique_ptr<Mesh>>   m_meshVector;
  std::unique_ptr<MeshRenderable>      m_meshRenderable;
  std::unique_ptr<MeshRenderable>      m_skeletonRenderable;
  std::unique_ptr<SelectionRenderable> m_selectionRenderable;

 public:
  MeshHandler(ViewState* const viewState, QObject* parent = nullptr);

  void init(const QString& path);
  void render() const;

  ViewState* viewState() const;

 public slots:
  void refinementLevelChanged();
  void setColour(const QVector2D& mousePosition);
  void updateScene();

 signals:
  void hasChanged();

 private:
  void      buildMeshes(size_t startIndex = 0);
  HalfEdge* findClosest(const QVector2D& mousePosition);
};

#endif  // MESHHANDLER_H
