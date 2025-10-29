#pragma once
#include "core.h"
#include "glad/glad.h"

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
		virtual std::string GetPath() = 0;
		virtual void LoadTexture() = 0;
		static Texture2D* Create();
		static Texture2D* Create(GLint type,unsigned int width,unsigned int height);
		static Texture2D* Create(const std::string& path);
		
	};
	
	class KE_API TextureCube :public Texture
	{
	public:

		TextureCube() = default;
		~TextureCube() = default;
		virtual void Bind() = 0;
		virtual void Unbind() = 0;
		static TextureCube* Create(std::vector<std::string>& faces);

	};

}

