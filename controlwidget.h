#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QCloseEvent>
#include <QWidget>
#include <memory>

#include "mesh/meshhandler.h"
#include "viewstate.h"

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

  ViewState* viewState() const;

 signals:
  void aboutToClose();

 private:
  Ui::ControlWidget*         ui;
  std::unique_ptr<ViewState> m_viewState;
};

#endif  // CONTROLWIDGET_H
