#include "window.h"
#include <QDebug>
#include <QOpenGLShaderProgram>
#include <QString>
#include <climits>
#include "vertex.h"

void Window::initializeGL() {
  initializeOpenGLFunctions();
  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(cleanUp()),
          Qt::DirectConnection);

  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(std::numeric_limits<unsigned int>::max());

  m_renderable = new Renderable();
  glClearColor(1.f, 1.f, 1.f, 1.0f);
}

void Window::resizeGL(int width, int height) {
  Q_UNUSED(width);
  Q_UNUSED(height);
}

void Window::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  m_renderable->render();
}

void Window::cleanUp() {
  delete m_renderable;
}
