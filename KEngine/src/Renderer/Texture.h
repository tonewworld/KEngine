#pragma once
#include "core.h"
#include "glad/glad.h"
#include "FrameBuffer.h"
namespace KEngine {

	class KE_API Texture
	{
	protected:
		unsigned int m_RendererID;
	public:
		Texture();
		virtual ~Texture() = default;

		virtual void Bind()=0;
		virtual void Unbind()=0;

		virtual void AddToFrameBuffer(std::shared_ptr<FrameBuffer>) {}
		virtual void LoadCubemap(std::vector<const GLchar*>faces) {}

		static Texture* Create(char* type);
	};
}

