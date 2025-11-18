#include "kepch.h"
#include "Texture.h"
#include "Renderer.h"
#include "Platforms/OpenGL/OpenGLTexture.h"
namespace KEngine {
	Texture2D* Texture2D::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTexture2D();
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Texture2D* Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTexture2D(path);
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	Texture2D* Texture2D::Create(GLint type,unsigned int width, unsigned int height,void*data)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTexture2D(type,width,height,data);
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	MultiSampleTexture* MultiSampleTexture::Create(GLint type, unsigned int width, unsigned int height, int samples)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLMultiSampleTexture(type, width, height, samples);
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	
	TextureCube* TextureCube::Create(std::vector<std::string>& faces) {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTextureCube(faces);
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	TextureCube* TextureCube::Create(GLint type, unsigned int width, unsigned int height)
	{
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTextureCube(type, width, height);
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}