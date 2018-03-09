#ifndef WINDOW_H
#define WINDOW_H

#include <QMouseEvent>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWindow>
#include <vector>

#include "controlwidget.h"
#include "mesh/meshhandler.h"
#include "renderable/meshrenderable.h"

class MainWindow : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

 public:
  MainWindow();
  ~MainWindow();

 protected slots:
  void cleanUp();
  void mousePressEvent(QMouseEvent* event);

 signals:
  void aboutToClose();

 private:
  void initializeGL();
  void initMembers();
  void resizeGL(int width, int height);
  void paintGL();

  std::unique_ptr<MeshHandler> m_meshHandler;
  std::unique_ptr<ControlWidget> m_controlWidget;
};

#endif  // WINDOW_H
