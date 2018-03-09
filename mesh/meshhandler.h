#ifndef MESHHANDLER_H
#define MESHHANDLER_H

#include <QObject>
#include <QString>
#include <memory>
#include <vector>

#include "../renderable/meshrenderable.h"
#include "mesh.h"

class MeshHandler : public QObject {
  Q_OBJECT
 public:
  MeshHandler(QObject* parent = nullptr);

  void init(const QString& path);
  void buildMeshes();
  void render() const;

  size_t ccSteps() const;
  void setCcSteps(const size_t& ccSteps);

 public slots:
  void currentMeshIndexChanged(int newIndex);

 signals:
  void hasChanged();

 private:
  size_t m_ccSteps = 3;
  size_t m_currentMeshIndex = 0;
  std::vector<std::unique_ptr<Mesh>> m_meshVector;

  std::unique_ptr<MeshRenderable> m_meshRenderable;
  std::unique_ptr<MeshRenderable> m_skeletonRenderable;
};

#endif  // MESHHANDLER_H
