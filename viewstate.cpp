#include "viewstate.h"

ViewState::ViewState(QObject *parent) : QObject(parent) {}

size_t ViewState::ccSteps() const { return m_ccSteps; }

void ViewState::setCcSteps(const int &ccSteps) {
  m_ccSteps = static_cast<size_t>(ccSteps);
  emit updated();
}

size_t ViewState::refinementLevel() const { return m_refinementLevel; }

void ViewState::setRefinementLevel(const int &refinementLevel) {
  m_refinementLevel = static_cast<size_t>(refinementLevel);
  emit refinementLevelChanged();
}

bool ViewState::showSurface() const { return m_showSurface; }

void ViewState::setShowSurface(bool showSurface) {
  m_showSurface = showSurface;
  emit updated();
}

bool ViewState::showPoints() const { return m_showPoints; }

void ViewState::setShowPoints(bool showPoints) {
  m_showPoints = showPoints;
  emit updated();
}

bool ViewState::showLines() const { return m_showLines; }

void ViewState::setShowLines(bool showLines) {
  m_showLines = showLines;
  emit updated();
}
