#pragma once
#include "core.h"
#include "glad/glad.h"
#include "Buffer.h"
namespace KEngine {

	class KE_API Texture2D 
	{
	public:
		Texture2D() = default;
		virtual ~Texture2D()=default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void AddToFrameBuffer(std::shared_ptr<FrameBuffer>fbo) = 0;
		static Texture2D* Create();
	};
	class KE_API TextureCube 
	{
	public:
		TextureCube() = default;
		~TextureCube() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		virtual void LoadCubemap(std::vector<std::string>& faces) = 0;
		static TextureCube* Create();
	};
}

