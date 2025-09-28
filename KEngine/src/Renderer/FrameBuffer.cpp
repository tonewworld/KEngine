#include "kepch.h"
#include "FrameBuffer.h"

namespace KEngine {

	FrameBuffer::FrameBuffer()
	{
		glCreateFramebuffers(1, &m_RendererID);
	}

	void FrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	}

	void FrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	FrameBuffer* FrameBuffer::Create() {
		return new FrameBuffer();
	}
}
