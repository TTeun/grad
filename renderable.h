#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <memory>
#include <vector>

#include "mesh.h"
#include "vertex.h"

class Renderable {
 public:
  enum RenderMode {
    POINTS = 1 << 0,
    LINES = 1 << 1,
    SURFACE = 1 << 2,
    ALL = 7
  };

  Renderable();
  ~Renderable();

  void fillCoords();
  void render();
  void update();
  bool coordsNeedToBeFilled() const;
  void setCoordsNeedToBeFilled(bool coordsNeedToBeFilled);
  RenderMode renderMode();

 private:
  RenderMode m_renderMode = RenderMode::ALL;
  Mesh* m_mesh;
  bool m_coordsNeedToBeFilled = true;
  std::unique_ptr<std::vector<float>> m_data;

  std::unique_ptr<std::vector<unsigned int>> m_indices;

  std::unique_ptr<QOpenGLShaderProgram> m_colourShader;
  std::unique_ptr<QOpenGLShaderProgram> m_blackShader;

  QOpenGLBuffer m_buffer;
  std::unique_ptr<QOpenGLBuffer> m_indexBuffer;
  QOpenGLVertexArrayObject m_vao;
};

#endif  // RENDERABLE_H
