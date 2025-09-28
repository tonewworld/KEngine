#include "kepch.h"
#include "RenderBuffer.h"
#include "Renderer.h"
namespace KEngine {

	RenderBuffer::RenderBuffer()
	{
		glGenRenderbuffers(1, &m_RendererID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1600.f, 900.f); // Use a single renderbuffer object for both a depth AND stencil buffer.
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RendererID); // Now actually attach it
		// Now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void RenderBuffer::Bind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
	}

	void RenderBuffer::Unbind()
	{
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}
	RenderBuffer* RenderBuffer::Create() {
		return new RenderBuffer();
	}
}
