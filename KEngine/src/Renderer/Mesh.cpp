#include "kepch.h"
#include "Mesh.h"
#include "glad/glad.h"
namespace KEngine
{
	Mesh::Mesh(float* m_Vertices,unsigned int vertexCount, BufferLayout& layout, unsigned int* m_Indexes,unsigned int indexCount)
		:layout(layout){

		vertices.resize(vertexCount);
		memcpy(vertices.data(), m_Vertices, vertexCount * sizeof(Vertex)); // 注意类型和大小

		indices.resize(indexCount);
		memcpy(indices.data(), m_Indexes, indexCount * sizeof(unsigned int));
		
		VAO.reset(VertexArray::Create());
		VAO->Bind();

		VBO.reset(VertexBuffer::Create(vertices));
		VBO->SetLayout(layout);
		VAO->AddVertexBuffer(VBO);

		
		IBO.reset(IndexBuffer::Create(indices));
		VAO->SetIndexBuffer(IBO);

		VAO->Unbind();

		
	}
	
}