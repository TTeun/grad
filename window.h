#ifndef WINDOW_H
#define WINDOW_H

#include <QMouseEvent>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWindow>
#include <vector>

#include "renderable.h"

class Window : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

 public:
 protected slots:
  void cleanUp();
  void mousePressEvent(QMouseEvent* event);

 private:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

  size_t ccSteps = 2;
  size_t m_currentMeshIndex;
  std::vector<Mesh*> m_meshVector;

  Renderable* m_renderable;
};

#endif  // WINDOW_H
