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
		//深度测试
		RenderCommand::SetDepthOpenOrClose(true);
		//模板测试
		RenderCommand::SetStencilOpenOrClose(true);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		RenderCommand::SetStencilMask(0x00);
		
		glEnable(GL_PROGRAM_POINT_SIZE);

		//面剔除
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

	void Renderer::Test()
	{
		glEnable(GL_PROGRAM_POINT_SIZE);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_DEPTH_TEST);

		float pointVertices[] = {
			0.f,0.f,0.f
		};
		unsigned int VAO, VBO, IBO;
		glCreateVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pointVertices), pointVertices, GL_STREAM_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// 绘制点（注意：你没有 EBO，所以用 glDrawArrays 而不是 glDrawElements）
		glDrawArrays(GL_POINTS, 0, 1);

		glEnable(GL_STENCIL_TEST);
		glEnable(GL_DEPTH_TEST);

		
	}
	
	
}