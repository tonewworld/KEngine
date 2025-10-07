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
	TextureCube* TextureCube::Create() {
		switch (Renderer::GetAPI()) {
		case RendererAPI::API::None:
			//KE_CORE_ASSERT(false, "RendererAPI::None is currently not supported");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return new OpenGLTextureCube();
		}
		//KE_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}