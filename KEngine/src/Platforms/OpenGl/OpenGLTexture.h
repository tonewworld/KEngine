#pragma once
#include "Core.h"
#include "Renderer/Texture.h"
namespace KEngine
{
	class KE_API OpenGLTexture2D :public Texture2D
	{
	private:
		unsigned int m_RendererID;
	public:
		OpenGLTexture2D();
		virtual ~OpenGLTexture2D();
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void AddToFrameBuffer(std::shared_ptr<FrameBuffer>fbo) override ;
	};
	class KE_API OpenGLTextureCube :public TextureCube
	{
	private:
		unsigned int m_RendererID;
	public:
		OpenGLTextureCube();
		virtual ~OpenGLTextureCube();
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void LoadCubemap(std::vector<std::string>& faces) override ;
	};

}