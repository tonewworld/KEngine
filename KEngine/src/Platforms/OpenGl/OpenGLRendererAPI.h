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
		
	};
}

