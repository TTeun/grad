#include "renderable.h"
#include "subdiv.h"

Renderable::Renderable()
    : m_mesh(new Mesh(new OBJFile("models/square.obj"))),
      m_data(new std::vector<float>),
      m_indices(new std::vector<unsigned int>),
      m_colourShader(new QOpenGLShaderProgram()),
      m_blackShader(new QOpenGLShaderProgram()),
      m_indexBuffer(
          new QOpenGLBuffer(QOpenGLBuffer(QOpenGLBuffer::IndexBuffer))) {
  {
    Mesh* mesh = new Mesh();
    Subdiv::subdivideCatmullClark(m_mesh, mesh);
    m_mesh = mesh;
  }
  {
    Mesh* mesh = new Mesh();
    Subdiv::subdivideCatmullClark(m_mesh, mesh);
    m_mesh = mesh;
  }
  m_colourShader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                          ":/shaders/vert.glsl");
  m_colourShader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                          ":/shaders/frag.glsl");
  m_blackShader->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                         ":/shaders/vert_black.glsl");
  m_blackShader->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                         ":/shaders/frag.glsl");

  m_colourShader->link();
  m_colourShader->bind();

  m_buffer.create();
  m_buffer.bind();
  m_buffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
  m_buffer.allocate(m_data->data(), m_data->size() * sizeof(float));

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
  m_buffer.release();
  m_indexBuffer->release();
  m_colourShader->release();
}

Renderable::~Renderable() {
  m_vao.destroy();
  m_buffer.destroy();
  m_indexBuffer->destroy();
}

void Renderable::fillCoords() {
  unsigned int index = 0;
  auto const faces = m_mesh->faces();
  HalfEdge* currentEdge;
  for (auto face : faces) {
    currentEdge = face.side();

    for (size_t i = 0; i != face.val(); ++i) {
      m_data->push_back(currentEdge->target()->coords()[0]);
      m_data->push_back(currentEdge->target()->coords()[1]);
      m_data->push_back(currentEdge->colour()[0]);
      m_data->push_back(currentEdge->colour()[1]);
      m_data->push_back(currentEdge->colour()[2]);
      m_indices->push_back(index);
      ++index;
      currentEdge = currentEdge->next();
    }
    m_indices->push_back(std::numeric_limits<unsigned int>::max());
  }

  m_coordsNeedToBeFilled = false;
}

void Renderable::render() {
  update();
  m_vao.bind();
  m_buffer.bind();
  m_indexBuffer->bind();

  {
    m_colourShader->bind();
    glDrawElements(GL_TRIANGLE_FAN, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_colourShader->release();
  }

  {
    m_blackShader->bind();
    glDrawElements(GL_LINES, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_blackShader->release();
  }

  {
    glPointSize(10);
    m_colourShader->bind();
    glDrawElements(GL_POINTS, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_colourShader->release();
  }

  m_indexBuffer->release();
  m_buffer.release();
  m_vao.release();
}

void Renderable::update() {
  if (m_coordsNeedToBeFilled) {
    fillCoords();
    m_vao.bind();
    m_buffer.bind();
    m_indexBuffer->bind();
    m_buffer.allocate(m_data->data(), m_data->size() * sizeof(float));
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
