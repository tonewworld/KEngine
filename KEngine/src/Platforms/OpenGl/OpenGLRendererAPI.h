#pragma once

#include "Renderer/RendererAPI.h"
namespace KEngine
{
	class KE_API OpenGLRendererAPI :public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(float r, float g, float b, float a) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray) override;

		virtual void SetStencilOpenOrClose(bool tag);
		virtual void SetStencilFunc(GLenum func, GLint ref, GLint mask);
		virtual void SetStencilMask(GLint tag) override ;

		virtual void SetDepthOpenOrClose(bool tag) override ;

		virtual void SwitchFrameBuffer(unsigned int frameBuffer) override ;
	};
}

