#include "kepch.h"
#include "Mesh.h"

namespace KEngine
{
	Mesh::Mesh(Vertex v,Index i) :vertex(v),index(i) {
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;

		VAO.reset(VertexArray::Create());
		VAO->Bind();

		
		VBO.reset(VertexBuffer::Create(vertex));
		VBO->SetLayout(vertex.layout);
		VAO->AddVertexBuffer(VBO);

		
		IBO.reset(IndexBuffer::Create(index));
		VAO->SetIndexBuffer(IBO);

	}
}