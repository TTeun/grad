#include "controlwidget.h"
#include "ui_controlwidget.h"

#include <QDebug>
#include <cassert>

ControlWidget::ControlWidget(QWidget* parent)
    : QWidget(parent), ui(new Ui::ControlWidget), m_viewState(new ViewState()) {
  ui->setupUi(this);

  assert(QObject::connect(ui->meshIndexSpinBox,
                          SIGNAL(valueChanged(int)),
                          m_viewState.get(),
                          SLOT(setRefinementLevel(int))));

  assert(QObject::connect(ui->showSurfaceCheckBox,
                          &QCheckBox::clicked,
                          m_viewState.get(),
                          &ViewState::setShowSurface));

  assert(QObject::connect(ui->showSkeletonCheckBox,
                          &QCheckBox::clicked,
                          m_viewState.get(),
                          &ViewState::setShowLines));

  assert(QObject::connect(ui->ccLevelSpinbox,
                          SIGNAL(valueChanged(int)),
                          m_viewState.get(),
                          SLOT(setCcSteps(int))));
}

ControlWidget::~ControlWidget() { delete ui; }

void ControlWidget::closeEvent(QCloseEvent* event) {
  emit aboutToClose();
  event->accept();
}

Ui::ControlWidget* ControlWidget::getUi() const { return ui; }

ViewState* ControlWidget::viewState() const { return m_viewState.get(); }
