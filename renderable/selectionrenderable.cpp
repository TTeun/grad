#include "selectionrenderable.h"
#include "../mesh/halfedge.h"

SelectionRenderable::SelectionRenderable() : Renderable() {}

SelectionRenderable::~SelectionRenderable() {}

void SelectionRenderable::reset() {
  m_data->clear();
  m_indices->clear();
}

void SelectionRenderable::fillCoords(const HalfEdge* edge) {
  reset();
  m_data->push_back(edge->target()->coords()[0]);
  m_data->push_back(edge->target()->coords()[1]);
  m_data->push_back(edge->colour()[0]);
  m_data->push_back(edge->colour()[1]);
  m_data->push_back(edge->colour()[2]);

  m_data->push_back(edge->twin()->target()->coords()[0]);
  m_data->push_back(edge->twin()->target()->coords()[1]);
  m_data->push_back(edge->twin()->colour()[0]);
  m_data->push_back(edge->twin()->colour()[1]);
  m_data->push_back(edge->twin()->colour()[2]);

  m_indices->push_back(0);
  m_indices->push_back(1);
  m_indices->push_back(std::numeric_limits<unsigned int>::max());
  update();
}

void SelectionRenderable::render() {
  update();
  m_vao.bind();
  m_vertexBuffer->bind();
  m_indexBuffer->bind();

  if (m_renderMode & static_cast<int>(RenderMode::LINES) || true) {
    m_whiteShader->bind();
    glDrawElements(GL_LINE_LOOP, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
    m_whiteShader->release();
  }

  //  if (m_renderMode & static_cast<int>(RenderMode::POINTS)) {
  //    {
  //      glPointSize(10);
  //      m_blackShader->bind();
  //      glDrawElements(GL_POINTS, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
  //      m_blackShader->release();
  //    }
  //    {
  //      glPointSize(8);
  //      m_colourShader->bind();
  //      glDrawElements(GL_POINTS, m_indexBuffer->size(), GL_UNSIGNED_INT, 0);
  //      m_colourShader->release();
  //    }
  //  }

  m_indexBuffer->release();
  m_vertexBuffer->release();
  m_vao.release();
}

void SelectionRenderable::update() {
  m_vao.bind();
  m_vertexBuffer->bind();
  m_indexBuffer->bind();
  m_vertexBuffer->allocate(m_data->data(), m_data->size() * sizeof(float));
  m_indexBuffer->allocate(m_indices->data(), m_indices->size() * sizeof(unsigned int));
  m_vao.release();
}
