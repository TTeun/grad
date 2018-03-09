#include "window.h"
#include <subdiv.h>
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

  glClearColor(1.f, 1.f, 1.f, 1.0f);

  m_renderable = new Renderable();
  m_renderable->setRenderMode(
      static_cast<int>(Renderable::RenderMode::SURFACE) |
      static_cast<int>(Renderable::RenderMode::POINTS) |
      static_cast<int>(Renderable::RenderMode::LINES));
  m_meshVector.resize(ccSteps + 1);

  m_meshVector[0] = new Mesh(new OBJFile("models/square.obj"));
  for (size_t i = 0; i != ccSteps; ++i) {
    m_meshVector[i + 1] = new Mesh();
    Subdiv::subdivideCatmullClark(m_meshVector[i], m_meshVector[i + 1]);
  }
  m_renderable->setMesh(m_meshVector.back());
  m_renderable->setCoordsNeedToBeFilled(true);
  m_currentMeshIndex = ccSteps;
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

void Window::mousePressEvent(QMouseEvent* event) {
  QVector2D mousePt{static_cast<double>(event->x()) / width(),
                    static_cast<double>(event->y()) / height()};
  mousePt[0] = (mousePt[0] - 0.5) * 2.;
  mousePt[1] = -(mousePt[1] - 0.5) * 2.;
}
