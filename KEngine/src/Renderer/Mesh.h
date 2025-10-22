#pragma once
#include "Core.h"
#include "Object.h"
#include "Renderer/VertexArray.h"
#include "Texture.h"
namespace KEngine {
	

	class KE_API Mesh:public Object {
	public:
		Mesh(float* m_Vertices, unsigned int vertexCount,
			BufferLayout& layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Mesh");
		virtual ~Mesh() = default;
		
		std::shared_ptr<VertexArray> VAO;
		std::shared_ptr<VertexBuffer> VBO;
		std::shared_ptr<IndexBuffer> IBO;


	private:
		
		BufferLayout m_layout;
		
		
		
	};
}