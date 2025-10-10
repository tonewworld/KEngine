#include "kepch.h"
#include "OpenGLTexture.h"
#include "SOIL2/SOIL2.h"
namespace KEngine
{
	OpenGLTexture2D::OpenGLTexture2D()
	{

		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600.f, 900.f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Unbind();

	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
	}

	void OpenGLTexture2D::Bind()
	{
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void OpenGLTexture2D::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	

	OpenGLTextureCube::OpenGLTextureCube()
	{

	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
	}

	void OpenGLTextureCube::Bind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);
	}

	void OpenGLTextureCube::Unbind()
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}
	void OpenGLTextureCube::LoadCubemap(std::vector<std::string>& faces) {
		glActiveTexture(GL_TEXTURE0);//maybe as a parameter

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


	OpenGLTexture3D::OpenGLTexture3D()
	{
	}
	OpenGLTexture3D::~OpenGLTexture3D()
	{
	}
	void OpenGLTexture3D::Bind()
	{
	}
	void OpenGLTexture3D::Unbind()
	{
	}
}