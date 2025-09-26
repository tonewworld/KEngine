#include "kepch.h"
#include "Renderer.h"

namespace KEngine{
	void Renderer::Init()
	{
		RenderCommand::Init();
	}
	void Renderer::BeginScene()
	{
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();
	}
	void Renderer::EndScene()
	{
		glStencilMask(0xFF);
		glEnable(GL_DEPTH_TEST);
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{

		shader->Bind();
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::SetStencilFunc(GLenum func, GLint ref, GLuint mask)
	{
		RenderCommand::SetStencilFunc(func, ref, mask);
	}
	void Renderer::SetStencilMask(GLint tag) {
		RenderCommand::SetStencilMask(tag);
	}
	void Renderer::SetDepthOpenOrClose(bool tag) {
		RenderCommand::SetDepthOpenOrClose(tag);
	}
	
	
}