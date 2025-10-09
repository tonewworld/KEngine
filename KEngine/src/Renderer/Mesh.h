#pragma once
#include "Core.h"
#include "Renderer/VertexArray.h"

namespace KEngine {
	
	class KE_API Mesh {
	public:
		Mesh(Vertex v,Index i);
		virtual ~Mesh() = default;
		
		std::shared_ptr<VertexArray> VAO;
		
	private:
		Vertex vertex;
		Index index;

		

	};
}