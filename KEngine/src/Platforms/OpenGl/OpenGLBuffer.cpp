#include "kepch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"
#include <gtc/type_ptr.hpp>
#include "Renderer/Light.h"
namespace KEngine{
    
	//VertexBuffer
    OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<float>& data){
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
    }
    OpenGLVertexBuffer::~OpenGLVertexBuffer(){
        glDeleteBuffers(1,&m_RendererID);
    }
    void OpenGLVertexBuffer::Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);//感觉这里不用写bind函数了
    }
    void OpenGLVertexBuffer::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
	//IndexBuffer
    OpenGLIndexBuffer::OpenGLIndexBuffer(std::vector<unsigned int> indices)
    :m_Count(indices.size())
    {
        glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
    OpenGLIndexBuffer::~OpenGLIndexBuffer(){
        glDeleteBuffers(1,&m_RendererID);
    }
    void OpenGLIndexBuffer::Bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);//感觉这里不用写bind函数了
    }
    void OpenGLIndexBuffer::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
    }
	//FrameBuffer
    OpenGLFrameBuffer::OpenGLFrameBuffer()
    {
        glCreateFramebuffers(1, &m_RendererID);
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    }

    void OpenGLFrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void OpenGLFrameBuffer::Add2DTexture(GLint type, unsigned int textureID, GLboolean drawable, GLboolean readable)
    {
		this->Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, type, GL_TEXTURE_2D, textureID, 0);
        glDrawBuffer(drawable);
        glReadBuffer(readable);
       /* GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[ERROR] depthFBO not complete! 0x" << std::hex << status << std::endl;
        }
        else {
            std::cout << "[OK] depthFBO is complete!" << std::endl;
        }*/

    }
    void OpenGLFrameBuffer::AddTexture(GLint type, GLuint textureID, GLboolean drawable, GLboolean readable)
    {
        this->Bind();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureID, 0);
        glDrawBuffer(drawable);
        glReadBuffer(readable);
       /* GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::cout << "[ERROR] depthCubeFBO not complete! 0x" << std::hex << status << std::endl;
        }
        else {
            std::cout << "[OK] depthCubeFBO is complete!" << std::endl;
        }*/
        this->Unbind();
    }
	//RenderBuffer
    OpenGLRenderBuffer::OpenGLRenderBuffer()
    {
        glGenRenderbuffers(1, &m_RendererID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
		//深度和模板缓冲
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600.f, 900.f); // Use a single renderbuffer object for both a depth AND stencil buffer.
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendererID); // Now actually attach it
        // Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLRenderBuffer::Bind()
    {
        glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
    }

    void OpenGLRenderBuffer::Unbind()
    {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }


    OpenGLUniformBuffer::OpenGLUniformBuffer(unsigned int size,unsigned int bindingPoint)
    {
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, m_RendererID, 0, size);
		
    }

    OpenGLUniformBuffer::~OpenGLUniformBuffer()
    {
    }

    void OpenGLUniformBuffer::Bind()
    {
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
    }

    void OpenGLUniformBuffer::Unbind()
    {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
	
    void OpenGLUniformBuffer::AddVPMatrix(glm::mat4& view,glm::mat4& proj,std::size_t offset)
    {
        
        Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::mat4), glm::value_ptr(view));
		glBufferSubData(GL_UNIFORM_BUFFER, offset + sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(proj));
        Unbind();
    }

    void OpenGLUniformBuffer::AddVec3(glm::vec3& vec, std::size_t offset)
    {
        Bind();
        glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(glm::vec3), glm::value_ptr(vec));
        Unbind();
    }

    void OpenGLUniformBuffer::AddFloat(float& f, std::size_t offset)
    {
        Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, offset, sizeof(float), &f);
        Unbind();
    }

    void OpenGLUniformBuffer::AddMaterial(MaterialUboData& material)
    {
        Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(MaterialUboData), &material);
        Unbind();
    }

    void OpenGLUniformBuffer::AddPointLight(const std::vector<std::shared_ptr<PointLight>>& pointLightList) {
        Bind();

        size_t lightBytes = pointLightList.size() * sizeof(PointLightUboData);
        for (size_t i = 0; i < pointLightList.size(); ++i) {
            PointLightUboData ubo(pointLightList[i]->GetPosition(), pointLightList[i]->GetLightAttributes());
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(PointLightUboData), sizeof(ubo), &ubo);
        }

        int count = static_cast<int>(pointLightList.size());
        glBufferSubData(GL_UNIFORM_BUFFER, 10 * sizeof(PointLightUboData), sizeof(count), &count);

        Unbind();
    }

    void OpenGLUniformBuffer::AddParallelLight(const std::vector<std::shared_ptr<ParallelLight>>& parallelLightList) {
        Bind();

        size_t lightBytes = parallelLightList.size() * sizeof(ParallelLightUboData);
        for (size_t i = 0; i < parallelLightList.size(); ++i) {
            ParallelLightUboData ubo(parallelLightList[i]->GetLightAttributes());
            glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(ParallelLightUboData), sizeof(ubo), &ubo);
        }
        int count = static_cast<int>(parallelLightList.size());
        glBufferSubData(GL_UNIFORM_BUFFER, 10 * sizeof(ParallelLightUboData), sizeof(count), &count);

        Unbind();
    }
}