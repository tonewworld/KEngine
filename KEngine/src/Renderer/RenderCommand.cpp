#include "kepch.h"
#include "RenderCommand.h"
#include "Platforms/OpenGL/OpenGLRendererAPI.h"
namespace KEngine{

	RendererAPI* RenderCommand::m_RendererAPI = new OpenGLRendererAPI;

	void RenderCommand::Init() {
		m_RendererAPI->Init();
	}
	void RenderCommand::SetClearColor(float r, float g, float b, float a) {
		m_RendererAPI->SetClearColor(r, g, b, a);
	}
	void RenderCommand::Clear () {
		m_RendererAPI->Clear();
	}
	void RenderCommand::DrawIndexed(const std::shared_ptr<class VertexArray>& vertexArray) {
		m_RendererAPI->DrawIndexed(vertexArray);
	}
}
