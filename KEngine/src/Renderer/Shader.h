#pragma once
#include "Core.h"
#include "glad/glad.h"
#include "glm.hpp"

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
		void SetUniformMatrix4fv(const glm::mat4 matrix, const std::string name);
	};
}
