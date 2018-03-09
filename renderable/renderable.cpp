#include "renderable.h"
#include "subdiv.h"

Renderable::Renderable()
    : m_data(new std::vector<float>),
      m_indices(new std::vector<unsigned int>),
      m_colourShader(new QOpenGLShaderProgram()),
      m_blackShader(new QOpenGLShaderProgram()),
      m_whiteShader(new QOpenGLShaderProgram()),
      m_vertexBuffer(
          new QOpenGLBuffer(QOpenGLBuffer(QOpenGLBuffer::VertexBuffer))),
      m_indexBuffer(
          new QOpenGLBuffer(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))) {
  setRenderMode(static_cast<int>(RenderMode::SURFACE));
}

Renderable::~Renderable() {
  m_vao.destroy();
  m_vertexBuffer->destroy();
  m_indexBuffer->destroy();
}

void Renderable::init() {
  m_colourShader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/vert.glsl");
  m_colourShader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/frag.glsl");
  m_blackShader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/shaders/vert_black.glsl");
  m_blackShader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/shaders/frag.glsl");
  m_whiteShader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/shaders/vert_white.glsl");
  m_whiteShader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/shaders/frag.glsl");

  m_colourShader->link();
  m_blackShader->link();
  m_whiteShader->link();
  m_colourShader->bind();

  m_vertexBuffer->create();
  m_vertexBuffer->bind();
  m_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_vertexBuffer->allocate(m_data->data(), m_data->size() * sizeof(float));

  m_indexBuffer->create();
  m_indexBuffer->bind();
  m_indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_indexBuffer->allocate(m_indices->data(),
                          m_indices->size() * sizeof(unsigned int));

  m_vao.create();
  m_vao.bind();
  m_colourShader->enableAttributeArray(0);
  m_colourShader->enableAttributeArray(1);
  m_colourShader->setAttributeBuffer(0, GL_FLOAT, 0, 2, 5 * sizeof(float));
  m_colourShader->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 3,
                                     5 * sizeof(float));

  m_blackShader->enableAttributeArray(0);
  m_blackShader->enableAttributeArray(1);
  m_blackShader->setAttributeBuffer(0, GL_FLOAT, 0, 2, 5 * sizeof(float));
  m_blackShader->setAttributeBuffer(1, GL_FLOAT, 2 * sizeof(float), 3,
                                    5 * sizeof(float));

  m_vao.release();
  m_vertexBuffer->release();
  m_indexBuffer->release();
  m_colourShader->release();
}

void Renderable::render() {
  update();
  m_vao.bind();
  m_vertexBuffer->bind();
  m_indexBuffer->bind();

  if (m_renderMode & static_cast<int>(RenderMode::SURFACE)) {
    m_colourShader->bind();
    glDrawElements(GL_TRIANGLE_FAN, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_colourShader->release();
  }

  if (m_renderMode & static_cast<int>(RenderMode::LINES)) {
    m_blackShader->bind();
    glDrawElements(GL_LINE_LOOP, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_blackShader->release();
  }

  if (m_renderMode & static_cast<int>(RenderMode::POINTS)) {
    {
      glPointSize(10);
      m_blackShader->bind();
      glDrawElements(GL_POINTS, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
      m_blackShader->release();
    }
    {
      glPointSize(8);
      m_colourShader->bind();
      glDrawElements(GL_POINTS, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
      m_colourShader->release();
    }
  }

  m_indexBuffer->release();
  m_vertexBuffer->release();
  m_vao.release();
}

void Renderable::update() {
  if (m_coordsNeedToBeFilled) {
    fillCoords();
    m_vao.bind();
    m_vertexBuffer->bind();
    m_indexBuffer->bind();
    m_vertexBuffer->allocate(m_data->data(), m_data->size() * sizeof(float));
    m_indexBuffer->allocate(m_indices->data(),
                            m_indices->size() * sizeof(unsigned int));
    m_vao.release();
  }
}

bool Renderable::coordsNeedToBeFilled() const {
  return m_coordsNeedToBeFilled;
}

void Renderable::setCoordsNeedToBeFilled(bool coordsNeedToBeFilled) {
  m_coordsNeedToBeFilled = coordsNeedToBeFilled;
}

void Renderable::setRenderMode(const int& renderMode) {
  m_renderMode = renderMode;
}
