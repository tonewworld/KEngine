#include "kepch.h"
#include "Renderer.h"

namespace KEngine{
	void Renderer::Init(unsigned int &frameBuffer1,unsigned int &textureID)
	{
		RenderCommand::Init();
		/*glGenFramebuffers(1, &frameBuffer1);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer1);

		
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 1600.0f, 900.f, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);*/

		//GLuint rbo;
		//glGenRenderbuffers(1, &rbo);
		//glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600.f, 900.f); // Use a single renderbuffer object for both a depth AND stencil buffer.
		//glBindRenderbuffer(GL_RENDERBUFFER, 0);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // Now actually attach it
		//// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//	std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void Renderer::BeginScene()
	{
		RenderCommand::SetClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		RenderCommand::Clear();
		//Éî¶È²âÊÔ
		RenderCommand::SetDepthOpenOrClose(true);
		//Ä£°å²âÊÔ
		RenderCommand::SetStencilOpenOrClose(true);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		RenderCommand::SetStencilMask(0x00);
		

		//ÃæÌÞ³ý
		//glEnable(GL_CULL_FACE);
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

	void Renderer::Test(unsigned int textureID)
	{
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
	
	
}