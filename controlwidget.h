#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QCloseEvent>
#include <QWidget>
#include "mesh/meshhandler.h"

namespace Ui {
class ControlWidget;
}

class ControlWidget : public QWidget {
  Q_OBJECT

 public:
  explicit ControlWidget(QWidget* parent = 0);
  ~ControlWidget();

  void closeEvent(QCloseEvent* event);

  Ui::ControlWidget* getUi() const;

 signals:
  void aboutToClose();

 private:
  Ui::ControlWidget* ui;
};

#endif  // CONTROLWIDGET_H
