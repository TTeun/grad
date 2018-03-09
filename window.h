#ifndef WINDOW_H
#define WINDOW_H

#include <QMouseEvent>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWindow>
#include <vector>

#include "meshrenderable.h"

class Window : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

 public:
  Window();

 protected slots:
  void cleanUp();
  void mousePressEvent(QMouseEvent* event);

 private:
  void initializeGL();
  void initMembers();
  void resizeGL(int width, int height);
  void paintGL();
  void buildMeshes();

  size_t ccSteps = 2;
  size_t m_currentMeshIndex;
  std::vector<std::unique_ptr<Mesh>> m_meshVector;
  std::unique_ptr<MeshRenderable> m_meshRenderable;
};

#endif  // WINDOW_H
