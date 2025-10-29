#pragma once
#include "Core.h"
#include "Renderer/Texture.h"
namespace KEngine
{
	class KE_API OpenGLTexture2D :public Texture2D
	{
	private:
		unsigned int m_RendererID;
		std::string m_Path;
	public:
		OpenGLTexture2D();
		OpenGLTexture2D(GLint type,unsigned int width, unsigned int height);
		OpenGLTexture2D(const std::string& path);
		virtual ~OpenGLTexture2D();
		virtual void Bind() override;
		virtual void Unbind() override;
		inline virtual std::string GetPath() override { return m_Path; }
		virtual void LoadTexture() override;
		inline virtual unsigned int GetRendererID() const override { return m_RendererID; }
	};
	
	class KE_API OpenGLTextureCube :public TextureCube
	{
	private:
		unsigned int m_RendererID;
		std::vector<std::string> faces;
	public:
		OpenGLTextureCube(std::vector<std::string>& faces);
		virtual ~OpenGLTextureCube();
		virtual void Bind() override;
		virtual void Unbind() override;
		inline virtual unsigned int GetRendererID() const override { return m_RendererID; }
	};

}