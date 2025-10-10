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
		virtual unsigned int GetRendererID() const override { return m_RendererID; }
	};
	class KE_API OpenGLTexture3D :public Texture3D
	{	
	private:
		unsigned int m_RendererID;
	public:
		OpenGLTexture3D();
		virtual ~OpenGLTexture3D();
		virtual void Bind() override;
		virtual void Unbind() override;
		virtual unsigned int GetRendererID() const override { return m_RendererID; }
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
		virtual unsigned int GetRendererID() const override { return m_RendererID; }
	};

}