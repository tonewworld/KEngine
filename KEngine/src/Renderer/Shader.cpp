#include "kepch.h"
#include "Shader.h"
#include "log.h"

namespace KEngine
{
	Shader::Shader(const char* vertexSrc, const char* fragmentSrc)
	{
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSrc, NULL);
		glCompileShader(vertexShader);
		{
			GLint success;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
				KE_CORE_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{0}", infoLog);
			}
		}
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
		glCompileShader(fragmentShader);
		{
			GLint success;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
				KE_CORE_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{0}", infoLog);
			}
		}
		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glLinkProgram(m_RendererID);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	Shader::Shader(const char* vertexSrc, const char* geometrySrc, const char* fragmentSrc)
	{
		unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSrc, NULL);
		glCompileShader(vertexShader);
		{
			GLint success;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
				KE_CORE_ERROR("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{0}", infoLog);
			}
		}
		unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
		glCompileShader(fragmentShader);
		{
			GLint success;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog);
				KE_CORE_ERROR("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{0}", infoLog);
			}
		}
		unsigned int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometrySrc, NULL);
		glCompileShader(geometryShader);
		{
			GLint success;
			glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				GLchar infoLog[1024];
				glGetShaderInfoLog(geometryShader, 1024, NULL, infoLog);
				KE_CORE_ERROR("ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n{0}", infoLog);
			}
		}

		m_RendererID = glCreateProgram();
		glAttachShader(m_RendererID, vertexShader);
		glAttachShader(m_RendererID, fragmentShader);
		glAttachShader(m_RendererID, geometryShader);
		glLinkProgram(m_RendererID);

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteShader(geometryShader);	
	}
	Shader::~Shader()
	{
		glDeleteProgram(m_RendererID);
	}
	void Shader::Bind() const
	{
		glUseProgram(m_RendererID);
	}
	void Shader::Unbind() const
	{
		glUseProgram(0);
	}
	void Shader::SetUniformMatrix4fv(const glm::mat4 matrix, const char* name)
	{
		this->Bind();
		unsigned int location = glGetUniformLocation(m_RendererID, name);
		glUniformMatrix4fv(location, 1, 0, &matrix[0][0]);
	}
	void Shader::SetUniform3f(const glm::vec3 vec, const char* name)
	{
		this->Bind();
		unsigned int location = glGetUniformLocation(m_RendererID, name);
		glUniform3f(location, vec.x,vec.y,vec.z);
	}

	void Shader::SetUniform1i(int value, const char* name)
	{
		this->Bind();
		unsigned int location = glGetUniformLocation(m_RendererID, name);
		glUniform1i(location, value);
	}

	void Shader::SetUniform1f(float value, const char* name)
	{
		this->Bind();
		unsigned int location = glGetUniformLocation(m_RendererID, name);
		glUniform1f(location, value);
	}

	void Shader::SetUniform1b(bool tag, const char* name)
	{
		this->Bind();
		unsigned int location = glGetUniformLocation(m_RendererID, name);
		glUniform1i(location, tag);
	}
	
	void Shader::BindUniformBufferPoint(const char* name,unsigned int number)
	{
		this->Bind();
		unsigned int location = glGetUniformBlockIndex(m_RendererID, name);
		glUniformBlockBinding(m_RendererID,location,number);
	}
}

