#include "kepch.h"
#include "Renderer.h"

namespace KEngine{
	void Renderer::Init()
	{
		RenderCommand::Init();
	}
	void Renderer::BeginScene()
	{
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1);
		RenderCommand::Clear();
	}
	void Renderer::EndScene()
	{
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	
}