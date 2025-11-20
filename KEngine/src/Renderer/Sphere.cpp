#include "kepch.h"
#include "Sphere.h"

namespace KEngine
{
    Sphere::Sphere(float* m_Vertices, unsigned int floatCount,
        BufferLayout layout,
        unsigned int* m_Indexes, unsigned int indexCount,
        const std::string& name)
        :Mesh(m_Vertices, floatCount, layout, m_Indexes, indexCount, name)
    {
       
    }

}
