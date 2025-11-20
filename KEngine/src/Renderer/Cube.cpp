#include "kepch.h"
#include "Cube.h"
namespace KEngine
{
	Cube::Cube(float* m_Vertices, unsigned int floatCount,
		BufferLayout layout,
		unsigned int* m_Indexes, unsigned int indexCount,
		const std::string& name)
		:Mesh(m_Vertices, floatCount, layout, m_Indexes, indexCount, name)
	{
	}

}