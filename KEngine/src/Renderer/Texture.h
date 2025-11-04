#pragma once
#include "core.h"
#include "glad/glad.h"

namespace KEngine {

	class KE_API Texture {
	public:
		Texture() = default;
		virtual ~Texture() = default;
		virtual void Bind() = 0;
		virtual void Bind(unsigned int texSlot) = 0;
		virtual void Unbind() = 0;

		virtual unsigned int GetRendererID() const = 0;
		static Texture* Create();
		virtual std::string GetPath() = 0;
		virtual void LoadTexture() = 0;

		inline unsigned int GetTexSlot() { return texSlot; }
		inline void SetTexSlot(unsigned int ts) { texSlot = ts; }
	protected:
		unsigned int texSlot = 0;
	};

	class KE_API Texture2D :public Texture
	{
	public:
		Texture2D() = default;
		virtual ~Texture2D()=default;
		virtual void Bind() = 0;
		virtual void Bind(unsigned int slot) = 0;
		virtual void Unbind() = 0;
		virtual std::string GetPath() = 0;
		virtual void LoadTexture() = 0;

		static Texture2D* Create();
		static Texture2D* Create(GLint type,unsigned int width,unsigned int height);
		static Texture2D* Create(const std::string& path);
	private:
	};
	
	class KE_API TextureCube :public Texture
	{
	public:

		TextureCube() = default;
		~TextureCube() = default;
		virtual void Bind() = 0;
		virtual void Bind(unsigned int slot) = 0;
		virtual void Unbind() = 0;
		virtual std::string GetPath() = 0;
		virtual void LoadTexture() = 0;
		static TextureCube* Create(std::vector<std::string>& faces);
		static TextureCube* Create(GLint type, unsigned int width, unsigned int height);
	};

}

