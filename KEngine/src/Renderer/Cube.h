#pragma once
#include "Core.h"
#include "Mesh.h"
namespace KEngine {


	class KE_API Cube :public Mesh {
	public:
		Cube(float* m_Vertices, unsigned int vertexCount,
			BufferLayout layout,
			unsigned int* m_Indexes, unsigned int indexCount,
			const std::string& name = "Cube");
		virtual ~Cube() = default;

	private:


	};
}