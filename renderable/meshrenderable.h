#ifndef MESHRENDERABLE_H
#define MESHRENDERABLE_H

#include "../mesh/mesh.h"
#include "renderable.h"

class MeshRenderable : public Renderable {
 private:
  Mesh* m_mesh;

 public:
  MeshRenderable();
  ~MeshRenderable();

  void setMesh(Mesh* mesh);

 private:
  void fillCoords() override;
};

#endif  // MESHRENDERABLE_H
