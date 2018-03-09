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
  initMembers();
}

void Window::initMembers() {
  m_meshRenderable = new MeshRenderable();
  m_meshRenderable->setRenderMode(
      static_cast<int>(Renderable::RenderMode::SURFACE) |
      static_cast<int>(Renderable::RenderMode::POINTS) |
      static_cast<int>(Renderable::RenderMode::LINES));

  {
    OBJFile const* const objFile = new OBJFile("models/square.obj");
    m_meshVector.push_back(std::unique_ptr<Mesh>(new Mesh(objFile)));
    delete objFile;
  }

  for (size_t i = 0; i != ccSteps; ++i) {
    m_meshVector.push_back(m_meshVector.back()->ccSubdiv());
  }
  m_meshRenderable->setMesh(m_meshVector.back().get());
  m_meshRenderable->setCoordsNeedToBeFilled(true);
  m_currentMeshIndex = ccSteps;
}

void Window::resizeGL(int width, int height) {
  Q_UNUSED(width);
  Q_UNUSED(height);
}

void Window::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  m_meshRenderable->render();
}

void Window::cleanUp() {
  delete m_meshRenderable;
}

void Window::mousePressEvent(QMouseEvent* event) {
  QVector2D mousePt{static_cast<double>(event->x()) / width(),
                    static_cast<double>(event->y()) / height()};
  mousePt[0] = (mousePt[0] - 0.5) * 2.;
  mousePt[1] = -(mousePt[1] - 0.5) * 2.;
}
