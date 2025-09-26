#pragma once
#include "Core.h"
#include "RendererAPI.h"

namespace KEngine{
	class KE_API RenderCommand
	{
	public:
		static void Init();
		static void SetClearColor(float r, float g, float b, float a);
		static void Clear();
		static void DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray);
		static void SetStencilFunc(GLenum func, GLint ref, GLuint mask);
		static void SetStencilMask(GLint tag);
		static void SetDepthOpenOrClose(bool tag);

	private: 
		static RendererAPI* m_RendererAPI;

	};
}

