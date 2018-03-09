#ifndef SUBIV_H
#define SUBIV_H

#include "mesh.h"

class Subdiv {
 private:
  Subdiv() = delete;

 public:
  static void subdivideCatmullClark(Mesh const* oldMesh, Mesh* newMesh);
};

#endif  // SUBIV_H
