#ifndef MESHRENDERABLE_H
#define MESHRENDERABLE_H

#include "../mesh/mesh.h"
#include "renderable.h"

class MeshRenderable : public Renderable {
 public:
  MeshRenderable();
  ~MeshRenderable();

  void setMesh(Mesh* mesh);

 private:
  void fillCoords() override;
  Mesh* m_mesh;
};

#endif  // MESHRENDERABLE_H
