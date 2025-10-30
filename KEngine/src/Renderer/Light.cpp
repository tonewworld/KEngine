#include "kepch.h"
#include "Light.h"
#include "glad/glad.h"

namespace KEngine
{

    Light::Light(float* m_Vertices, unsigned int floatCount,
        BufferLayout layout,
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
    
    glm::mat4 ParallelLight::CalculateLightSpace()
    {
        // 使用正交投影
        float orthoSize = 15.0f; // 覆盖范围，根据场景调整
        float nearPlane = 1.0f;
        float farPlane = 50.0f;

        glm::mat4 lightProjection = glm::ortho(
            -orthoSize, orthoSize,     // 左右边界
            -orthoSize, orthoSize,     // 上下边界  
            nearPlane, farPlane        // 近远平面
        );

        // 光源位置 - 在场景上方，沿着光照方向的反方向
        glm::vec3 lightDir = this->GetLightAttributes().direct;
        

       
        // 光源视角矩阵 - 看向场景中心
        glm::mat4 lightView = glm::lookAt(
            this->GetPosition(),                    // 光源位置
            glm::vec3(0.0f, 0.0f, 0.0f), // 看向场景中心
            glm::vec3(0.0f, 1.0f, 0.0f)  // 上方向
        );

        return lightProjection * lightView;
    }

}