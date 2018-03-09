#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <memory>
#include <vector>

class Renderable {
 public:
  enum class RenderMode : int { POINTS = 1 << 0, LINES = 1 << 1, SURFACE = 1 << 2, ALL = 7 };

  Renderable();
  virtual ~Renderable();

  virtual void fillCoords();
  void         init();
  virtual void render();
  virtual void update();

  bool coordsNeedToBeFilled() const;
  void setCoordsNeedToBeFilled(bool coordsNeedToBeFilled);
  void setRenderMode(const int& renderMode);

 protected:
  int  m_renderMode           = static_cast<int>(RenderMode::ALL);
  bool m_coordsNeedToBeFilled = true;

  std::unique_ptr<std::vector<float>>        m_data;
  std::unique_ptr<std::vector<unsigned int>> m_indices;
  std::unique_ptr<QOpenGLShaderProgram>      m_colourShader;
  std::unique_ptr<QOpenGLShaderProgram>      m_blackShader;
  std::unique_ptr<QOpenGLShaderProgram>      m_whiteShader;
  std::unique_ptr<QOpenGLBuffer>             m_vertexBuffer;
  std::unique_ptr<QOpenGLBuffer>             m_indexBuffer;
  QOpenGLVertexArrayObject                   m_vao;
};

#endif  // RENDERABLE_H
