#pragma once
#include "Core.h"
#include "Renderer/VertexArray.h"
#include "Texture.h"
namespace KEngine {
	

	class KE_API Mesh {
	public:
		Mesh(float* m_Vertices, unsigned int vertexCount,
			BufferLayout& layout, 
			unsigned int* m_Indexes, unsigned int indexCount);
		virtual ~Mesh() = default;
		
		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;

	private:
		
		BufferLayout layout;
		
		
		
	};
}