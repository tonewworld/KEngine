#include "kepch.h"
#include "Light.h"
#include "glad/glad.h"

namespace KEngine
{

    Light::Light(float* m_Vertices, unsigned int floatCount,
        BufferLayout& layout,
        unsigned int* m_Indexes, unsigned int indexCount,
        const std::string& name)
        :Object(name)
    {
        this->m_layout = layout;
        std::vector<float> vertices(m_Vertices, m_Vertices + floatCount);

        std::vector<unsigned int> indices(m_Indexes, m_Indexes + indexCount);

        VAO.reset(VertexArray::Create());
        VAO->Bind();

        VBO.reset(VertexBuffer::Create(vertices));
        VBO->SetLayout(m_layout);
        VAO->AddVertexBuffer(VBO);

        IBO.reset(IndexBuffer::Create(indices));
        VAO->SetIndexBuffer(IBO);

        VAO->Unbind();
    }
}