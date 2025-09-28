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
		//…Ó∂»≤‚ ‘
		RenderCommand::SetDepthOpenOrClose(true);
		//ƒ£∞Â≤‚ ‘
		RenderCommand::SetStencilOpenOrClose(true);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		RenderCommand::SetStencilMask(0x00);
		

		//√ÊÃﬁ≥˝
		//glEnable(GL_CULL_FACE);
	}
	void Renderer::EndScene()
	{
		glStencilMask(0xFF);
	}
	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{

		shader->Bind();
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}
	void Renderer::SetStencilOpenOrClose(bool tag) {
		RenderCommand::SetStencilOpenOrClose(tag);
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
	void Renderer::SwitchFrameBuffer(unsigned int frameBuffer) {
		RenderCommand::SwitchFrameBuffer(frameBuffer);
	}

	void Renderer::Test(std::shared_ptr<FrameBuffer> frameBuffer)
	{
		frameBuffer->Unbind();
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void Renderer::Test2(std::shared_ptr<FrameBuffer> frameBuffer) {
		frameBuffer->Bind();
	}
	
}