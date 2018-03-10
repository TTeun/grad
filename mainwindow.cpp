#include "mainwindow.h"
#include "subdiv.h"
#include "ui_controlwidget.h"

#include <QDebug>
#include <cassert>

MainWindow::MainWindow()
    : m_mouseHandler(new MouseHandler(this)),
      m_meshHandler(new MeshHandler(this)),
      m_controlWidget(new ControlWidget()) {
  m_mouseHandler->setHeight(height());
  m_mouseHandler->setWidth(width());
  m_controlWidget->show();
  qDebug() << "MainWindow constructed";
}

MainWindow::~MainWindow() {}

void MainWindow::initializeGL() {
  initializeOpenGLFunctions();

  assert(QObject::connect(
      this, SIGNAL(visibleChanged(bool)), m_controlWidget.get(), SLOT(close())));
  assert(QObject::connect(
      m_controlWidget.get(), SIGNAL(aboutToClose()), this, SLOT(close())));

  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(std::numeric_limits<unsigned int>::max());

  glClearColor(1.f, 1.f, 1.f, 1.0f);
  initMembers();
  connectMembers();
}

void MainWindow::initMembers() { m_meshHandler->init(QString("models/square.obj")); }

void MainWindow::connectMembers() {
  assert(QObject::connect(m_controlWidget.get()->getUi()->meshIndexSpinBox,
                          SIGNAL(valueChanged(int)),
                          this->m_meshHandler.get(),
                          SLOT(currentMeshIndexChanged(int))));

  assert(
      QObject::connect(m_meshHandler.get(), SIGNAL(hasChanged()), this, SLOT(update())));

  assert(QObject::connect(m_mouseHandler.get(),
                          SIGNAL(rightClicked(QVector2D const&)),
                          m_meshHandler.get(),
                          SLOT(setColour(QVector2D const&))));
}

void MainWindow::resizeGL(int width, int height) {
  m_mouseHandler->setHeight(height);
  m_mouseHandler->setWidth(width);
}

void MainWindow::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  m_meshHandler->render();
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
  m_mouseHandler->mousePressEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent* event) {
  m_mouseHandler->mouseReleaseEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent* event) {
  m_mouseHandler->mouseMoveEvent(event);
}

void MainWindow::cleanUp() { qDebug() << "Clean up\n"; }
