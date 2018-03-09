#ifndef SELECTIONRENDERABLE_H
#define SELECTIONRENDERABLE_H

#include "renderable.h"

class HalfEdge;

class SelectionRenderable : public Renderable {
 public:
  SelectionRenderable();
  ~SelectionRenderable();

  void reset();
  void render() override;
  void update() override;
  void fillCoords(const HalfEdge* halfEdge);
};

#endif  // SELECTIONRENDERABLE_H
