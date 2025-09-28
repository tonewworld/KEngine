#pragma once
#include "core.h"
#include "glad/glad.h"
namespace KEngine {

	class KE_API Texture
	{
	private:
		unsigned int m_RendererID;
	public:
		Texture();
		void Bind();
		void Unbind();
		static Texture* Create();
	};
}

