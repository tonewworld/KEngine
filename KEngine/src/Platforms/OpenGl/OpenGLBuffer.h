#pragma once 
#include "Core.h"
#include "Renderer/Buffer.h"
namespace KEngine{
    class KE_API OpenGLVertexBuffer:public VertexBuffer{
    public:
        OpenGLVertexBuffer(float* vertices,unsigned int size);
        virtual ~OpenGLVertexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;
    private:
        unsigned int m_RendererID;

    };
    class KE_API OpenGLIndexBuffer:public IndexBuffer{
    public:
        OpenGLIndexBuffer(unsigned int* indices,unsigned int count);
        virtual ~OpenGLIndexBuffer();
        virtual void Bind() const override;
        virtual void Unbind() const override;
        virtual unsigned int GetCount()const {return m_Count;}
    private: 
        unsigned int m_RendererID;
        unsigned int m_Count;
    }; 
}