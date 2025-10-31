#include "kepch.h"
#include "OpenGLTexture.h"
#include "SOIL2/SOIL2.h"
namespace KEngine
{
	OpenGLTexture2D::OpenGLTexture2D()
	{
		glGenTextures(1, &m_RendererID);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Unbind();

	}

	OpenGLTexture2D::OpenGLTexture2D(GLint type,unsigned int width, unsigned int height)
	{
		glGenTextures(1, &m_RendererID);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		Unbind();
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
		:m_Path(path)
	{
		std::cout << "[Texture] loading " << path << std::endl;
		glGenTextures(1, &m_RendererID);
		
		int width, height;
		unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);
		SOIL_free_image_data(image);
		Unbind();
		std::cout << "[GL] Texture uploaded: " << m_RendererID
			<< " (" << width << "x" << height << ")" << std::endl;
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
	}

	void OpenGLTexture2D::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLTexture2D::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLTexture2D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture2D::LoadTexture()
	{
	}

	

	OpenGLTextureCube::OpenGLTextureCube(GLint type, unsigned int width, unsigned int height)
	{
		glGenTextures(1, &m_RendererID);
		
		Bind();

		for (GLint i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, type,
				width, height, 0, type, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		Unbind();

	}

	OpenGLTextureCube::OpenGLTextureCube(std::vector<std::string>& faces) :faces(faces)
	{
		glGenTextures(1, &m_RendererID);
		
		int width, height;
		unsigned char* image;

		Bind();
		for (GLuint i = 0; i < faces.size(); i++)
		{
			image = SOIL_load_image(faces[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
				GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		Unbind();
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
	}

	void OpenGLTextureCube::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLTextureCube::Bind(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLTextureCube::Unbind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

}