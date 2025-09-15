#pragma once
#include "Core.h"
#include "glad/glad.h"

namespace KEngine
{
	class KE_API Shader
	{
	public:
		unsigned int m_RendererID;
		Shader(const char* vertexShader,const char* fragmentShader);
		~Shader();

		void Bind() const;
		void Unbind() const;
	};
}
