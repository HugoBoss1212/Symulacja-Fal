#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(unsigned int slot) {
	GLCall(glGenBuffers(1 + slot, &m_RendererID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
IndexBuffer::~IndexBuffer() {
	GLCall(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::Update(const unsigned int* data, unsigned int count) {
	m_Count = count;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}
void IndexBuffer::Bind() const {
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}
void IndexBuffer::Unbind() const{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}