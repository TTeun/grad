#ifndef VIEWSTATE_H
#define VIEWSTATE_H

#include <QObject>

class ViewState : public QObject {
  Q_OBJECT

 private:
  size_t       m_refinementLevel = 0;
  size_t const m_ccSteps         = 4;
  bool         m_showSurface     = true;
  bool         m_showPoints      = true;
  bool         m_showLines       = true;

 public:
  explicit ViewState(QObject *parent = nullptr);

  size_t refinementLevel() const;

  bool showSurface() const;

  bool showPoints() const;
  void setShowPoints(bool showPoints);

  bool showLines() const;

  size_t ccSteps() const;

 signals:
  void refinementLevelChanged() const;
  void updated() const;

 public slots:
  void setCcSteps(const int &ccSteps);
  void setRefinementLevel(const int &refinementLevel);
  void setShowLines(bool showLines);
  void setShowSurface(bool showSurface);
};

#endif  // VIEWSTATE_H
