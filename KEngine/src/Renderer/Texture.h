#pragma once
#include "core.h"
#include "glad/glad.h"
#include "Buffer.h"
namespace KEngine {

	class KE_API Texture {
	public:
		Texture() = default;
		virtual ~Texture() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual unsigned int GetRendererID() const = 0;
		static Texture* Create();
	};

	class KE_API Texture2D :public Texture
	{
	public:
		Texture2D() = default;
		virtual ~Texture2D()=default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		static Texture2D* Create();
	};
	class KE_API Texture3D :public Texture{
		public:
		Texture3D() = default;
		virtual ~Texture3D() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		static Texture3D* Create();
	};
	class KE_API TextureCube :public Texture
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

