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

		//Ä£°å²âÊÔ
		static void SetStencilOpenOrClose(bool tag);
		static void SetStencilFunc(GLenum func, GLint ref, GLuint mask);
		static void SetStencilMask(GLint tag);

		//Éî¶È²âÊÔ
		static void SetDepthOpenOrClose(bool tag);

		//×ª»»Ö¡»º³å
		static void SwitchFrameBuffer(unsigned int frameBuffer);

	private: 
		static RendererAPI* m_RendererAPI;

	};
}

