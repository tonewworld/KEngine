#pragma once 
#include "Core.h"
#include "Renderer/Buffer.h"
#include "glm.hpp"
namespace KEngine{
    class KE_API OpenGLVertexBuffer:public VertexBuffer{
    public:
        OpenGLVertexBuffer(const std::vector<float>& data);
        virtual ~OpenGLVertexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout layout) override { m_Layout = layout; }
    private:
        unsigned int m_RendererID;

		BufferLayout m_Layout;

    };
    class KE_API OpenGLIndexBuffer:public IndexBuffer{
    public:
        OpenGLIndexBuffer(std::vector<unsigned int> index);
        virtual ~OpenGLIndexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual unsigned int GetCount()const {return m_Count;}
    private: 
        unsigned int m_RendererID;
        unsigned int m_Count;
    }; 
	class KE_API OpenGLFrameBuffer :public FrameBuffer
    {

    private:
        unsigned int m_RendererID;
    public:
        OpenGLFrameBuffer();
        virtual void Bind() override ;
        virtual void Unbind() override ;
        static OpenGLFrameBuffer* Create();
        virtual void AddTexture(unsigned int textureID) override;
    };

    class KE_API OpenGLRenderBuffer :public RenderBuffer
    {

    private:
        unsigned int m_RendererID;
    public:
        OpenGLRenderBuffer();
        virtual void Bind() override;
        virtual void Unbind() override;
        static OpenGLRenderBuffer* Create();

    };

    class KE_API OpenGLUniformBuffer :public UniformBuffer
    {
    private:
		unsigned int m_RendererID;
    public:
        OpenGLUniformBuffer(unsigned int size,unsigned int bindingPoint);
        virtual ~OpenGLUniformBuffer();
        virtual void Bind() override;
        virtual void Unbind() override;
        
        virtual void AddVPMatrix(glm::mat4& view ,glm::mat4& proj,std::size_t offset = 0) override ;
		virtual void AddVec3(glm::vec3& vec, std::size_t offset) override;
		virtual void AddFloat(float& f, std::size_t offset);
        virtual void AddMaterial(MaterialUboData& material) ;
        virtual void AddPointLight(const std::vector<std::shared_ptr<PointLight>>& plList);
    };

}