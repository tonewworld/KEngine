#include "kepch.h"
#include "OpenGLTexture.h"
#include "SOIL2/SOIL2.h"
namespace KEngine
{
	OpenGLTexture2D::OpenGLTexture2D()
	{
		glGenTextures(1, &m_RendererID);
		Bind();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1600, 900, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Unbind();

	}

	OpenGLTexture2D::OpenGLTexture2D(GLint internalFormat, unsigned int width, unsigned int height,void*data)
	{
		glGenTextures(1, &m_RendererID);
		Bind();

		// 选择外部 format 和 data type，保证与 internalFormat 匹配
		GLenum externalFormat = GL_RGB;
		GLenum dataType = GL_UNSIGNED_BYTE;

		if (internalFormat == GL_R16F || internalFormat == GL_R32F) {
			externalFormat = GL_RED;
			dataType = GL_FLOAT;
		}
		else if (internalFormat == GL_RGB16F || internalFormat == GL_RGB32F) {
			externalFormat = GL_RGB;
			dataType = GL_FLOAT;
		}
		else if (internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F) {
			externalFormat = GL_RGBA;
			dataType = GL_FLOAT;
		}
		else if (internalFormat == GL_DEPTH_COMPONENT || internalFormat == GL_DEPTH_COMPONENT16 || internalFormat == GL_DEPTH_COMPONENT24 || internalFormat == GL_DEPTH_COMPONENT32) {
			externalFormat = GL_DEPTH_COMPONENT;
			dataType = GL_FLOAT;
		}
		else {
			// 其他常见格式按 RGB/UNSIGNED_BYTE 处理
			externalFormat = (internalFormat == GL_RGBA ? GL_RGBA : GL_RGB);
			dataType = GL_UNSIGNED_BYTE;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, externalFormat, dataType, data);

		// 对浮点纹理使用线性过滤，一般不使用 mipmap（如果需要请生成 mipmap 并设置 MIN_FILTER）
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (dataType == GL_FLOAT) ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

	

	OpenGLTextureCube::OpenGLTextureCube(GLint internalFormat, unsigned int width, unsigned int height)
	{
		glGenTextures(1, &m_RendererID);
		
		Bind();

		// 选择外部 format 和 data type 与 internalFormat 匹配
		GLenum externalFormat = GL_RGB;
		GLenum dataType = GL_UNSIGNED_BYTE;
		if (internalFormat == GL_DEPTH_COMPONENT || internalFormat == GL_DEPTH_COMPONENT24 || internalFormat == GL_DEPTH_COMPONENT32) {
			externalFormat = GL_DEPTH_COMPONENT;
			dataType = GL_FLOAT;
		}
		else if (internalFormat == GL_RGBA || internalFormat == GL_RGBA16F || internalFormat == GL_RGBA32F) {
			externalFormat = GL_RGBA;
			dataType = (internalFormat == GL_RGBA ? GL_UNSIGNED_BYTE : GL_FLOAT);
		}
		else {
			externalFormat = GL_RGB;
			dataType = (internalFormat == GL_RGB ? GL_UNSIGNED_BYTE : GL_FLOAT);
		}

		for (GLint i = 0; i < 6; ++i) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat,
				width, height, 0, externalFormat, dataType, NULL);
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

	OpenGLMultiSampleTexture::OpenGLMultiSampleTexture()
	{
		
	}

	OpenGLMultiSampleTexture::OpenGLMultiSampleTexture(GLint type, unsigned int width, unsigned int height, int samples)
	{
		glGenTextures(1, &m_RendererID);
		Bind();
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, type, width, height, GL_TRUE);
	}

	OpenGLMultiSampleTexture::~OpenGLMultiSampleTexture()
	{
	}

	void OpenGLMultiSampleTexture::Bind()
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_RendererID);
	}

	void OpenGLMultiSampleTexture::Bind(unsigned int slot)
	{
	}

	void OpenGLMultiSampleTexture::Unbind()
	{
	}

	void OpenGLMultiSampleTexture::LoadTexture()
	{
	}

}