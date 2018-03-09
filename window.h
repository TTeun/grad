#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWindow>

#include "renderable.h"

class QOpenGLShaderProgram;

class Window : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

  // OpenGL Events
 public:
 protected slots:
  void cleanUp();

 private:
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();

  Renderable* m_renderable;
};

#endif  // WINDOW_H
