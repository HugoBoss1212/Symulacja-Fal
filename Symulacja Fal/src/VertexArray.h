#pragma once

#include "VertexBuffer.h"

class VertexBufferlayout;

class VertexArray {
private:
	unsigned int m_RendererID;
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferlayout& layout);
	void Bind() const;
	void Unbind() const;
};