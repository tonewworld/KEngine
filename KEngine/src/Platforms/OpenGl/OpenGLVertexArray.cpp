#include "kepch.h"
#include "OpenGLVertexArray.h"

#include <glad/glad.h>

namespace KEngine {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case KEngine::ShaderDataType::Float:    return GL_FLOAT;
		case KEngine::ShaderDataType::Float2:   return GL_FLOAT;
		case KEngine::ShaderDataType::Float3:   return GL_FLOAT;
		case KEngine::ShaderDataType::Float4:   return GL_FLOAT;
		case KEngine::ShaderDataType::Mat3:     return GL_FLOAT;
		case KEngine::ShaderDataType::Mat4:     return GL_FLOAT;
		case KEngine::ShaderDataType::Int:      return GL_INT;
		case KEngine::ShaderDataType::Int2:     return GL_INT;
		case KEngine::ShaderDataType::Int3:     return GL_INT;
		case KEngine::ShaderDataType::Int4:     return GL_INT;
		case KEngine::ShaderDataType::Bool:     return GL_BOOL;
		}

		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
	{
		

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		
		const auto& layout = vertexBuffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(m_VertexBufferIndex);
			glVertexAttribPointer(m_VertexBufferIndex,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.type),
				element.normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.offset);
			m_VertexBufferIndex++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

}