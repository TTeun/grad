#ifndef MESHHANDLER_H
#define MESHHANDLER_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

#include "../renderable/meshrenderable.h"
#include "../renderable/selectionrenderable.h"

class MeshRenderable;
class SelectionRenderable;

class MeshHandler : public QObject {
  Q_OBJECT
 public:
  MeshHandler(QObject* parent = nullptr);

  void init(const QString& path);
  void buildMeshes(size_t startIndex = 0);
  void render() const;

  size_t ccSteps() const;
  void   setCcSteps(const size_t& ccSteps);

 public slots:
  void currentMeshIndexChanged(int newIndex);
  void setColour(const QVector2D& mousePosition);

 signals:
  void hasChanged();

 private:
  HalfEdge* findClosest(const QVector2D& mousePosition);

  size_t m_ccSteps          = 0;
  size_t m_currentMeshIndex = 0;

  std::vector<std::unique_ptr<Mesh>>   m_meshVector;
  std::unique_ptr<MeshRenderable>      m_meshRenderable;
  std::unique_ptr<MeshRenderable>      m_skeletonRenderable;
  std::unique_ptr<SelectionRenderable> m_selectionRenderable;
};

#endif  // MESHHANDLER_H
