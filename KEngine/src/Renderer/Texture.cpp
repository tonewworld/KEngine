#include "kepch.h"
#include "Texture.h"
namespace KEngine {
	Texture::Texture() {
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_RendererID, 0);

	}
	void Texture::Bind() {
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}
	void Texture::Unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	Texture* Texture::Create() {
		return new Texture();
	}
}