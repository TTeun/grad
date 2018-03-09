#include "mainwindow.h"
#include "subdiv.h"
#include "ui_controlwidget.h"

#include <QDebug>
#include <cassert>

MainWindow::MainWindow()
    : m_meshHandler(new MeshHandler(this)),
      m_controlWidget(new ControlWidget()) {
  m_controlWidget->show();
  qDebug() << "MainWindow constructed";
}

MainWindow::~MainWindow() {}

void MainWindow::initializeGL() {
  initializeOpenGLFunctions();

  assert(QObject::connect(this, SIGNAL(visibleChanged(bool)),
                          m_controlWidget.get(), SLOT(close())));
  assert(QObject::connect(m_controlWidget.get(), SIGNAL(aboutToClose()), this,
                          SLOT(close())));

  assert(QObject::connect(m_controlWidget.get()->getUi()->meshIndexSpinBox,
                          SIGNAL(valueChanged(int)), this->m_meshHandler.get(),
                          SLOT(currentMeshIndexChanged(int))));

  assert(QObject::connect(m_meshHandler.get(), SIGNAL(hasChanged()), this,
                          SLOT(update())));

  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(std::numeric_limits<unsigned int>::max());

  glClearColor(1.f, 1.f, 1.f, 1.0f);
  initMembers();
}

void MainWindow::initMembers() {
  m_meshHandler->init(QString("models/tri1.obj"));
}

void MainWindow::resizeGL(int width, int height) {
  Q_UNUSED(width);
  Q_UNUSED(height);
}

void MainWindow::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT);

  m_meshHandler->render();
}

void MainWindow::cleanUp() {
  qDebug() << "Clean up\n";
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
  QVector2D mousePt{static_cast<double>(event->x()) / width(),
                    static_cast<double>(event->y()) / height()};
  mousePt[0] = (mousePt[0] - 0.5) * 2.;
  mousePt[1] = -(mousePt[1] - 0.5) * 2.;
}
