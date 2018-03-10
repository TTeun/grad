#ifndef WINDOW_H
#define WINDOW_H

#include <QMouseEvent>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWindow>
#include <vector>

#include "controlwidget.h"
#include "mesh/meshhandler.h"
#include "mousehandler.h"
#include "renderable/meshrenderable.h"

class MainWindow : public QOpenGLWindow, protected QOpenGLFunctions_4_1_Core {
  Q_OBJECT

 private:
  std::unique_ptr<MouseHandler>  m_mouseHandler;
  std::unique_ptr<ControlWidget> m_controlWidget;
  std::unique_ptr<MeshHandler>   m_meshHandler;

 public:
  MainWindow();
  ~MainWindow();

 protected slots:
  void cleanUp();

 signals:
  void aboutToClose();

 private:
  void initializeGL();
  void initMembers();
  void connectMembers();
  void resizeGL(int width, int height);
  void paintGL();

  void mousePressEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
};

#endif  // WINDOW_H
