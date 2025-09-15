#include "kepch.h"
#include "OpenGLBuffer.h"
#include "glad/glad.h"
namespace KEngine{
    OpenGLVertexBuffer::OpenGLVertexBuffer(float* vertices,unsigned int size){
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
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
    OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int* indexes,unsigned int count)
    :m_Count(count)
    {
        glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);
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
}