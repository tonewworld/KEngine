#include "kepch.h"
#include "Texture2D.h"
namespace KEngine {

	Texture2D::Texture2D()
	{
		
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE,NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Unbind();
		
	}

	Texture2D::~Texture2D()
	{
	}

	void Texture2D::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture2D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	
	void Texture2D::AddToFrameBuffer(std::shared_ptr<FrameBuffer>fbo) {
		fbo->Bind();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RendererID, 0);
	}
}
