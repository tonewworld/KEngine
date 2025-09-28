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

	void RenderCommand::SetStencilOpenOrClose(bool tag) {
		m_RendererAPI->SetStencilOpenOrClose(tag);
	}
	void RenderCommand::SetStencilFunc(GLenum func, GLint ref, GLuint mask)
	{
		m_RendererAPI->SetStencilFunc(func, ref, mask);
	}
	void RenderCommand::SetStencilMask(GLint tag) {
		m_RendererAPI->SetStencilMask(tag);
	}
	void RenderCommand::SetDepthOpenOrClose(bool tag) {
		m_RendererAPI->SetDepthOpenOrClose(tag);
	}
	void RenderCommand::SwitchFrameBuffer(unsigned int frameBuffer) {
		m_RendererAPI->SwitchFrameBuffer(frameBuffer);
	}
}
