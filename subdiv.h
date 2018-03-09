#ifndef SUBIV_H
#define SUBIV_H

#include "mesh/mesh.h"

class Subdiv {
 private:
  Subdiv() = delete;

 public:
  static void subdivideCatmullClark(Mesh const* oldMesh, Mesh* newMesh);
  static void subdivideTernary(Mesh const* oldMesh, Mesh* newMesh);
};

#endif  // SUBIV_H
