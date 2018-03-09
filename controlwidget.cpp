#include "controlwidget.h"
#include <QDebug>
#include "ui_controlwidget.h"

ControlWidget::ControlWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ControlWidget) {
  ui->setupUi(this);
}

ControlWidget::~ControlWidget() {
  delete ui;
}

void ControlWidget::closeEvent(QCloseEvent* event) {
  emit aboutToClose();
  event->accept();
}

Ui::ControlWidget* ControlWidget::getUi() const {
  return ui;
}
